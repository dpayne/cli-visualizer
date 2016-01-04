/*
 * Visualizer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Visualizer.h"
#include "Source/MpdAudioSource.h"
#include "Domain/VisConstants.h"
#include "Domain/VisException.h"
#include "Transformer/SpectrumTransformer.h"
#include "Transformer/SpectrumCircleTransformer.h"
#include "Transformer/EllipseTransformer.h"
#include "Transformer/LorenzTransformer.h"
#include "Utils/NcursesUtils.h"

#include <thread>
#include <iostream>

vis::Visualizer::Visualizer(const vis::Settings *const settings)
    : m_current_audio_source_index{0}, m_current_transformer_index{0},
      m_shutdown{false}, m_settings{settings}, m_pcm_buffer{nullptr}
{
    m_pcm_buffer = static_cast<pcm_stereo_sample *>(
        calloc(m_settings->get_sample_size(), sizeof(pcm_stereo_sample)));
}

void vis::Visualizer::add_audio_source(const std::string &audio_source)
{
    if (audio_source == VisConstants::k_mpd_audio_source_name)
    {
        m_audio_sources.emplace_back(
            std::make_unique<vis::MpdAudioSource>(m_settings));
    }
}

void vis::Visualizer::run()
{
    setup_audio_sources();

    setup_transformers();

    auto audioSource = get_current_audio_source();
    auto transformer = get_current_transformer();

    m_writer = std::make_unique<NcursesWriter>(m_settings);

    auto last_rotation_timestamp =
        std::chrono::system_clock::now().time_since_epoch() /
        std::chrono::seconds(1);

    while (!should_shutdown())
    {
        // Process user controls
        process_user_input();

        if (audioSource->read(m_pcm_buffer, m_settings->get_sample_size()))
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

        // update sources and transformers
        audioSource = get_current_audio_source();
        transformer = get_current_transformer();
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
    case 'q':
        shutdown();
        break;
    default:
        // do nothing
        break;
    }
}

void vis::Visualizer::setup_audio_sources()
{
    for (const auto &audioSource : m_settings->get_audio_sources())
    {
        add_audio_source(audioSource);
    }

    // Throw an error if there are no audio sources
    if (m_audio_sources.size() < 1)
    {
        throw vis::VisException{"No audio sources defined"};
    }
}

void vis::Visualizer::setup_transformers()
{
    for (const auto &visualizer : m_settings->get_visualizers())
    {
        if (visualizer == VisConstants::k_spectrum_visualizer_name)
        {
            m_transformers.emplace_back(
                std::make_unique<SpectrumTransformer>(m_settings));
        }
        else if (visualizer == VisConstants::k_spectrum_circle_visualizer_name)
        {
            m_transformers.emplace_back(
                std::make_unique<SpectrumCircleTransformer>(m_settings));
        }
        else if (visualizer == VisConstants::k_ellipse_visualizer_name)
        {
            m_transformers.emplace_back(
                std::make_unique<EllipseTransformer>(m_settings));
        }
        else if (visualizer == VisConstants::k_lorenz_visualizer_name)
        {
            m_transformers.emplace_back(
                std::make_unique<LorenzTransformer>(m_settings));
        }
    }
}

vis::Visualizer::~Visualizer()
{
    if (m_pcm_buffer != nullptr)
    {
        free(m_pcm_buffer);
    }
}
