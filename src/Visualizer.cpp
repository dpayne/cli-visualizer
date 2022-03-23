/*
 * Visualizer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Visualizer.h"
#include "Domain/VisConstants.h"
#include "Domain/VisException.h"
#include "Source/MpdAudioSource.h"
#include "Source/PortAudioSource.h"
#include "Source/PulseAudioSource.h"
#include "Source/ShmemAudioSource.h"
#include "Transformer/EllipseTransformer.h"
#include "Transformer/LorenzTransformer.h"
#include "Transformer/SpectrumCircleTransformer.h"
#include "Transformer/SpectrumTransformer.h"
#include "Utils/ConfigurationUtils.h"
#include "Utils/NcursesUtils.h"

#include <csignal>
#include <iostream>
#include <thread>

static vis::Visualizer *g_vis = nullptr;

namespace
{
const int16_t k_input_quit{'q'};
const int16_t k_input_reload{'r'};
const int16_t k_input_rotate_color{'c'};
const int16_t k_input_toggle_stereo{'s'};
const int16_t k_input_decrease_scaling{'-'};
const int16_t k_input_increase_scaling{'+'};

const double k_scaling_multiplier_interval{0.1};

sigset_t signal_set{};
bool is_received_reload_signal = false;
} // namespace

vis::Visualizer::Visualizer(const std::string &config_path,
                            const std::locale &loc)
    : m_settings{std::make_shared<vis::Settings>(config_path)},
      m_current_color_scheme_index{0}, m_current_transformer_index{0},
      m_shutdown{false}, m_signal_handlers_setup{false}, m_loc{loc},
      m_pcm_buffer{nullptr}
{
    vis::ConfigurationUtils::load_settings(m_settings, config_path, loc);
    g_vis = this;
}

// Used to catch CTRL-C signal to cause shutdown
static inline void shutdown_sig(int sig)
{
    std::cerr << "Received signal: " << sig << std::endl;

    if (g_vis != nullptr)
    {
        g_vis->shutdown();
    }
}

// Used to catch signal to cause config to be reloaded
static inline void reload_config_sig(int sig)
{
    std::cerr << "Received signal: " << sig << std::endl;

    if (g_vis != nullptr)
    {
        is_received_reload_signal = true;
    }
}

void vis::Visualizer::setup_signal_handlers()
{
    if (!m_signal_handlers_setup)
    {
        struct sigaction action;

        sigemptyset(&signal_set);
        sigaddset(&signal_set, SIGUSR1);

        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        action.sa_handler = shutdown_sig;

        sigaction(SIGTERM, &action, nullptr);
        sigaction(SIGINT, &action, nullptr);

        action.sa_handler = reload_config_sig;
        sigaction(SIGUSR1, &action, nullptr);
    }
}

void vis::Visualizer::allocate_buffer()
{
    if (m_pcm_buffer != nullptr)
    {
        free(m_pcm_buffer); // NOLINT
    }

    m_pcm_buffer = static_cast<pcm_stereo_sample *>(calloc(         // NOLINT
        m_settings->get_sample_size(), sizeof(pcm_stereo_sample))); // NOLINT
}

void vis::Visualizer::setup_audio_source()
{
    const auto audio_source = m_settings->get_audio_source();
    if (audio_source == VisConstants::k_mpd_audio_source_name)
    {
        m_audio_source = std::make_unique<vis::MpdAudioSource>(m_settings);
    }
    else if (audio_source == VisConstants::k_port_audio_source_name)
    {
        m_audio_source = std::make_unique<vis::PortAudioSource>(m_settings);
    }
    else if (audio_source == VisConstants::k_pulse_audio_source_name)
    {
        m_audio_source = std::make_unique<vis::PulseAudioSource>(m_settings);
    }
    else if (audio_source == VisConstants::k_shmem_audio_source_name)
    {
        m_audio_source = std::make_unique<vis::ShmemAudioSource>(m_settings);
    }
    else
    {
        // Throw an error if there are no audio sources
        throw vis::VisException{"No audio sources defined"};
    }
}

void vis::Visualizer::run()
{
    allocate_buffer();

    setup_audio_source();

    setup_transformers();

    m_writer = std::make_unique<NcursesWriter>();

    // color settings must be re-loaded after ncurses initialization
    vis::ConfigurationUtils::load_color_settings(m_settings);

    m_writer->setup_colors(m_settings->is_override_terminal_colors(),
                           m_settings->get_colors());

    int64_t last_rotation_timestamp =
        std::chrono::system_clock::now().time_since_epoch() /
        std::chrono::seconds(1);

    while (!should_shutdown())
    {
        // Process user controls
        process_user_input();

        auto transformer = get_current_transformer();

        if (m_audio_source->read(m_pcm_buffer, m_settings->get_sample_size()))
        {
            if (m_settings->is_stereo_enabled())
            {
                transformer->execute_stereo(m_pcm_buffer, m_writer.get());
            }
            else
            {
                transformer->execute_mono(m_pcm_buffer, m_writer.get());
            }
        }
        // There was not enough data from the audio stream, sleep for sometime
        // before trying again
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(
                VisConstants::k_silent_sleep_milliseconds));
        }

        rotate_transformer(m_settings->get_rotation_interval(),
                           &last_rotation_timestamp);

        // Only do this after at least one loop to prevent CTRL-C not killing
        // the process if audio cannot be read
        setup_signal_handlers();

        if (is_received_reload_signal)
        {
            is_received_reload_signal = false;
            reload_config();
        }
    }
}

void vis::Visualizer::rotate_transformer(const int64_t rotation_interval,
                                         int64_t *last_rotation_timestamp)
{
    if (rotation_interval > 0)
    {
        auto current_timestamp =
            std::chrono::system_clock::now().time_since_epoch() /
            std::chrono::seconds(1);

        if ((current_timestamp - *last_rotation_timestamp) >= rotation_interval)
        {
            m_current_transformer_index =
                (m_current_transformer_index + 1) % m_transformers.size();
            *last_rotation_timestamp = current_timestamp;
        }
    }
}

void vis::Visualizer::process_user_input()
{
    const int32_t user_input = NcursesUtils::get_user_input();
    switch (user_input)
    {
    case ' ':
        // switch visualizer
        m_current_transformer_index =
            (m_current_transformer_index + 1) % m_transformers.size();
        break;
    case k_input_quit:
        shutdown();
        break;
    case k_input_toggle_stereo:
        m_settings->set_is_stereo_enabled(!m_settings->is_stereo_enabled());
        break;
    case k_input_reload:
        reload_config();
        break;
    case k_input_rotate_color:
        rotate_color_scheme();
        break;
    case k_input_decrease_scaling:
        m_settings->set_scaling_multiplier(
            m_settings->get_scaling_multiplier() -
            (m_settings->get_scaling_multiplier() *
             k_scaling_multiplier_interval));
        break;
    case k_input_increase_scaling:
        m_settings->set_scaling_multiplier(
            m_settings->get_scaling_multiplier() +
            (m_settings->get_scaling_multiplier() *
             k_scaling_multiplier_interval));
        break;
    default:
        // do nothing
        break;
    }
}

void vis::Visualizer::rotate_color_scheme()
{
    if (m_settings->get_color_schemes().size() > 1)
    {
        m_current_color_scheme_index = (m_current_color_scheme_index + 1) %
                                       m_settings->get_color_schemes().size();

        vis::ConfigurationUtils::load_color_settings_from_color_scheme(
            m_settings->get_color_schemes()[m_current_color_scheme_index],
            m_settings);

        m_writer->setup_colors(m_settings->is_override_terminal_colors(),
                               m_settings->get_colors());

        for (const auto &transformer : m_transformers)
        {
            transformer->clear_colors();
        }
    }
}

void vis::Visualizer::reload_config()
{
    // Re-use the same config path to preserve the invocation with -c
    const auto config_path = m_settings->get_config_path();

    // keep the same scaling multiplier
    const auto scaling_multiplier = m_settings->get_scaling_multiplier();

    m_transformers.clear();

    // Note m_writer should not be reset, this is because it causes the screen
    // to flash this is also why it does not hold a pointer to settings
    m_settings.reset(new Settings{config_path});

    vis::ConfigurationUtils::load_settings(m_settings, config_path, m_loc);

    m_settings->set_scaling_multiplier(scaling_multiplier);

    setup_transformers();

    // reset the transformers position if the size has shrunk smaller than the
    // current position
    if (m_current_transformer_index >= m_transformers.size())
    {
        m_current_transformer_index = 0;
    }

    if (m_current_color_scheme_index >= m_settings->get_color_schemes().size())
    {
        m_current_color_scheme_index = 0;
    }

    // try to reload the same color scheme
    if (!m_settings->get_color_schemes().empty())
    {
        vis::ConfigurationUtils::load_color_settings_from_color_scheme(
            m_settings->get_color_schemes()[m_current_color_scheme_index],
            m_settings);
    }
    else
    {
        vis::ConfigurationUtils::load_color_settings(m_settings);
    }

    m_writer->setup_colors(m_settings->is_override_terminal_colors(),
                           m_settings->get_colors());
}

void vis::Visualizer::setup_transformers()
{
    for (const auto &visualizer : m_settings->get_visualizers())
    {
        if (visualizer == VisConstants::k_spectrum_visualizer_name)
        {
            m_transformers.emplace_back(
                std::make_unique<SpectrumTransformer>(m_settings, visualizer));
        }
        else if (visualizer == VisConstants::k_spectrum_circle_visualizer_name)
        {
            m_transformers.emplace_back(
                std::make_unique<SpectrumCircleTransformer>(m_settings,
                                                            visualizer));
        }
        else if (visualizer == VisConstants::k_ellipse_visualizer_name)
        {
            m_transformers.emplace_back(
                std::make_unique<EllipseTransformer>(m_settings, visualizer));
        }
        else if (visualizer == VisConstants::k_lorenz_visualizer_name)
        {
            m_transformers.emplace_back(
                std::make_unique<LorenzTransformer>(m_settings, visualizer));
        }
    }

    if (m_transformers.empty())
    {
        throw vis::VisException{"No visualizers defined"};
    }
}

vis::Visualizer::~Visualizer()
{
    if (m_pcm_buffer != nullptr)
    {
        free(m_pcm_buffer); // NOLINT
    }
}
