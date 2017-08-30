/*
 * ConfigurationUtils.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <cmath>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map>

#include "Domain/VisException.h"
#include "Utils/ConfigurationUtils.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Utils/Utils.h"

namespace
{
const std::string k_audio_sources_setting{"audio.sources"}; // mpd,alsa

const std::string k_mpd_fifo_path_setting{"mpd.fifo.path"};

const std::string k_stereo_enabled_setting{"audio.stereo.enabled"};

const std::string k_rotation_interval_setting{"visualizer.rotation.secs"};

const std::string k_vis_pulse_audio_source_setting{"audio.pulse.source"};

const std::string k_visualizers_setting{"visualizers"};
const std::string k_fps_setting{"visualizer.fps"};

const std::string k_color_scheme_path_setting{"colors.scheme"};
const std::string k_sampling_frequency_setting{"audio.sampling.frequency"};
const std::string k_low_cutoff_frequency_setting{"audio.low.cutoff.frequency"};
const std::string k_high_cutoff_frequency_setting{
    "audio.high.cutoff.frequency"};

const std::string k_scaling_multiplier_setting{"visualizer.scaling.multiplier"};

const std::string k_lorenz_character_setting{"visualizer.lorenz.character"};

const std::string k_ellipse_character_setting{"visualizer.ellipse.character"};
const std::string k_ellipse_radius_setting{"visualizer.ellipse.radius"};

// Spectrum settings
const std::string k_spectrum_character_setting{"visualizer.spectrum.character"};
const std::string k_spectrum_bar_width_setting{"visualizer.spectrum.bar.width"};
const std::string k_spectrum_bar_spacing_setting{
    "visualizer.spectrum.bar.spacing"};
const std::string k_spectrum_smoothing_mode_setting{
    "visualizer.spectrum.smoothing.mode"};
const std::string k_spectrum_falloff_mode_setting{
    "visualizer.spectrum.falloff.mode"};
const std::string k_spectrum_falloff_weight_setting{
    "visualizer.spectrum.falloff.weight"};

const std::string k_spectrum_top_margin_setting{
    "visualizer.spectrum.top.margin"};
const std::string k_spectrum_bottom_margin_setting{
    "visualizer.spectrum.bottom.margin"};
const std::string k_spectrum_right_margin_setting{
    "visualizer.spectrum.right.margin"};
const std::string k_spectrum_left_margin_setting{
    "visualizer.spectrum.left.margin"};

const std::string k_spectrum_reversed_setting{"visualizer.spectrum.reversed"};

const std::string k_monstercat_smoothing_factor_setting{
    "visualizer.monstercat.smoothing.factor"};

const std::string k_sgs_smoothing_points_setting{
    "visualizer.sgs.smoothing.points"};
const std::string k_sgs_smoothing_passes_setting{
    "visualizer.sgs.smoothing.passes"};
} // namespace

std::unordered_map<std::string, std::wstring>
vis::ConfigurationUtils::read_config(const std::string &config_path,
                                     const std::locale &loc)
{
    std::unordered_map<std::string, std::wstring> properties_map;

    std::wifstream file(config_path.c_str(), std::wifstream::in);
    std::wstring line;
    file.imbue(loc);

    if (!file.good())
    {
        VIS_LOG(vis::LogLevel::WARN, "Configuration not found at %s",
                config_path.c_str());
        return properties_map;
    }

    std::pair<std::wstring, std::wstring> split_line{L"", L""};

    while (file.good() && std::getline(file, line))
    {
        if (!line.empty() && line[0] != L'#')
        {
            vis::Utils::split_first(line, L'=', &split_line);

            if (!split_line.first.empty())
            {
                properties_map[Utils::wstring_to_string(split_line.first)] =
                    split_line.second;
            }
            else
            {
                VIS_LOG(vis::LogLevel::WARN,
                        "Configuration line was not valid at %s", line.c_str());
            }
        }
    }

    return properties_map;
}

void vis::ConfigurationUtils::add_color_gradients(
    const vis::ColorDefinition color, const double gradient_interval,
    std::vector<vis::ColorDefinition> *colors)
{
    if (colors->empty())
    {
        colors->push_back(color);
        return;
    }

    auto previous_color = (*colors)[colors->size() - 1];
    auto start_color = previous_color;

    const double red_diff =
        (color.get_red() - previous_color.get_red()) / gradient_interval;
    const double green_diff =
        (color.get_green() - previous_color.get_green()) / gradient_interval;
    const double blue_diff =
        (color.get_blue() - previous_color.get_blue()) / gradient_interval;

    for (auto i = 0u; i < std::round(gradient_interval); ++i)
    {
        const auto red = static_cast<int16_t>(
            std::round(start_color.get_red() + (red_diff * i)));

        const auto green = static_cast<int16_t>(
            std::round(start_color.get_green() + (green_diff * i)));

        const auto blue = static_cast<int16_t>(
            std::round(start_color.get_blue() + (blue_diff * i)));

        const auto color_index = NcursesUtils::to_ansi_color(red, green, blue);

        // gradients will generally result in a lot of duplicates
        if (previous_color.get_color_index() != color_index)
        {
            previous_color =
                vis::ColorDefinition{color_index, red, green, blue};
            colors->push_back(previous_color);
        }
    }
    colors->push_back(color);
}

std::vector<vis::ColorDefinition>
vis::ConfigurationUtils::read_colors(const std::string &colors_path)
{
    std::vector<vis::ColorDefinition> colors;

    std::ifstream file(colors_path.c_str(), std::ifstream::in);
    std::string line;
    if (!file.good())
    {
        VIS_LOG(vis::LogLevel::WARN, "Colors configuration not found at %s",
                colors_path.c_str());
        throw vis::VisException("Colors configuration not found at %s",
                                colors_path.c_str());
    }
    std::vector<std::string> lines;
    bool is_gradient_enabled = true;

    while (file.good() && std::getline(file, line))
    {
        if (!line.empty())
        {
            // first line, check for disabling gradient
            if (lines.empty() &&
                line == VisConstants::k_disabled_gradient_color_config)
            {
                is_gradient_enabled = false;
            }
            else
            {
                lines.push_back(line);
            }
        }
    }

    double gradient_interval =
        static_cast<double>(NcursesUtils::get_max_color()) /
        static_cast<double>(lines.size());

    std::vector<std::string> split_line;

    for (const auto &color_line : lines)
    {
        if (color_line.size() >= 7)
        {
            const auto hex_color =
                vis::Utils::hex_to_int(color_line.substr(1, 6));

            const int16_t red = (hex_color >> 16) % 256;
            const int16_t green = (hex_color >> 8) % 256;
            const int16_t blue = hex_color % 256;

            const auto color = vis::ColorDefinition{
                NcursesUtils::to_ansi_color(red, green, blue), red, green,
                blue};
            if (is_gradient_enabled)
            {
                add_color_gradients(color, gradient_interval, &colors);
            }
            else
            {
                colors.push_back(color);
            }
        }
        else
        {
            const auto basic_color = NcursesUtils::to_basic_color(color_line);
            if (basic_color.get_color_index() >= 0)
            {
                if (is_gradient_enabled)
                {
                    add_color_gradients(basic_color, gradient_interval, &colors);
                }
                else
                {
                    colors.push_back(basic_color);
                }
            }
            else
            {
                VIS_LOG(vis::LogLevel::WARN, "Configuration color "
                                             "definition line was not "
                                             "valid at %s",
                        color_line.c_str());
            }
        }
    }

    return colors;
}

vis::FalloffMode vis::ConfigurationUtils::read_falloff_mode(
    const std::unordered_map<std::string, std::wstring> &properties,
    const std::string &config_param,
    const vis::FalloffMode default_falloff_mode)
{
    auto falloff_mode_str = vis::Utils::get(
        properties, k_spectrum_falloff_mode_setting, std::string{""});

    vis::FalloffMode falloff_mode = default_falloff_mode;

    if (!falloff_mode_str.empty())
    {
        std::transform(falloff_mode_str.begin(), falloff_mode_str.end(),
                       falloff_mode_str.begin(), ::tolower);

        if (falloff_mode_str == "none")
        {
            falloff_mode = vis::FalloffMode::None;
        }
        else if (falloff_mode_str == "fill")
        {
            falloff_mode = vis::FalloffMode::Fill;
        }
        else if (falloff_mode_str == "top")
        {
            falloff_mode = vis::FalloffMode::Top;
        }
        else
        {
            VIS_LOG(vis::LogLevel::ERROR, "Invalid falloff mode %s for %s",
                    falloff_mode_str.c_str(), config_param.c_str());
        }
    }

    return falloff_mode;
}

vis::SmoothingMode vis::ConfigurationUtils::read_smoothing_mode(
    const std::unordered_map<std::string, std::wstring> &properties,
    const std::string &config_param,
    const vis::SmoothingMode default_smoothing_mode)
{
    auto smoothing_mode_str = vis::Utils::get(
        properties, k_spectrum_smoothing_mode_setting, std::string{""});

    vis::SmoothingMode smoothing_mode = default_smoothing_mode;

    if (!smoothing_mode_str.empty())
    {
        std::transform(smoothing_mode_str.begin(), smoothing_mode_str.end(),
                       smoothing_mode_str.begin(), ::tolower);

        if (smoothing_mode_str == "none")
        {
            smoothing_mode = vis::SmoothingMode::None;
        }
        else if (smoothing_mode_str == "monstercat")
        {
            smoothing_mode = vis::SmoothingMode::MonsterCat;
        }
        else if (smoothing_mode_str == "sgs")
        {
            smoothing_mode = vis::SmoothingMode::Sgs;
        }
        else
        {
            VIS_LOG(vis::LogLevel::ERROR, "Invalid spectrum mode %s for %s",
                    smoothing_mode_str.c_str(), config_param.c_str());
        }
    }

    return smoothing_mode;
}

void vis::ConfigurationUtils::load_settings(
    const std::shared_ptr<Settings> settings, const std::locale &loc)
{
    const auto config_path = VisConstants::k_default_config_path;
    load_settings(settings, config_path, loc);
}

void vis::ConfigurationUtils::setup_default_colors(
    const std::shared_ptr<Settings> settings)
{
    const auto max_color = static_cast<double>(NcursesUtils::get_max_color());
    const double frequency = (M_PI * 2.0) / max_color;

    // used to remove duplicates
    std::set<int16_t> colors_uniq;
    std::vector<vis::ColorDefinition> colors;

    const auto width = 127.0;
    const auto center = 128.0;

    for (int16_t i = 0; i < static_cast<int16_t>(max_color); ++i)
    {
        const auto red = static_cast<int16_t>(
            std::sin(frequency * i + 4.0) * width + center);
        const auto green = static_cast<int16_t>(
            std::sin(frequency * i + 0.0) * width + center);
        const auto blue = static_cast<int16_t>(
            std::sin(frequency * i + 2.0) * width + center);

        const auto color_index = NcursesUtils::to_ansi_color(red, green, blue);

        if (colors_uniq.find(color_index) == colors_uniq.end())
        {
            colors.emplace_back(color_index, red, green, blue);
            colors_uniq.insert(color_index);
        }
    }

    settings->set_colors(colors);
}

void vis::ConfigurationUtils::load_color_settings_from_color_scheme(
    const std::string &color_scheme, const std::shared_ptr<Settings> settings)
{
    if (!color_scheme.empty())
    {
        const auto colors_config_path =
            VisConstants::k_colors_directory + color_scheme;
        if (!colors_config_path.empty())
        {
            settings->set_colors(
                vis::ConfigurationUtils::read_colors(colors_config_path));
        }
    }
}

void vis::ConfigurationUtils::load_color_settings(
    const std::shared_ptr<Settings> settings)
{
    if (!settings->get_color_schemes().empty())
    {
        load_color_settings_from_color_scheme(settings->get_color_schemes()[0],
                                              settings);
    }

    if (settings->get_colors().empty())
    {
        int32_t number_of_colors_supported =
            NcursesUtils::number_of_colors_supported();

        if (number_of_colors_supported <= 0)
        {
            setup_default_colors(settings);
        }
        else if (number_of_colors_supported <= 8)
        {
            settings->set_colors(VisConstants::k_default_8_colors);
        }
        else if (number_of_colors_supported <= 16)
        {
            settings->set_colors(VisConstants::k_default_16_colors);
        }
        else
        {
            setup_default_colors(settings);
        }
    }
}

void vis::ConfigurationUtils::validate_setting_is_not_negative(
    const double t, const std::string &setting)
{
    if (t < 0.0)
    {
        throw vis::VisException("Invalid settings %s=%d", setting.c_str(), t);
    }
}

void vis::ConfigurationUtils::validate_setting_is_greater_than_zero(
    const double t, const std::string &setting)
{
    if (t <= 0.0)
    {
        throw vis::VisException("Invalid settings %s=%d", setting.c_str(), t);
    }
}

void vis::ConfigurationUtils::load_settings(
    const std::shared_ptr<Settings> settings, const std::string &config_path,
    const std::locale &loc)
{
    const auto properties =
        vis::ConfigurationUtils::read_config(config_path, loc);

    // setup mpd
    settings->set_mpd_fifo_path(
        Utils::get(properties, k_mpd_fifo_path_setting,
                   VisConstants::k_default_mpd_fifo_path));

// enable pulse audio by default if available
#ifdef _ENABLE_PULSE
    const std::string default_audio_source =
        VisConstants::k_pulse_audio_source_name;
#else
    const std::string default_audio_source =
        VisConstants::k_mpd_audio_source_name;
#endif

    // setup audio sources
    settings->set_audio_source(
        Utils::get(properties, k_audio_sources_setting, default_audio_source));

    settings->set_scaling_multiplier(
        Utils::get(properties, k_scaling_multiplier_setting,
                   VisConstants::k_default_scaling_multiplier));

    settings->set_fps(
        Utils::get(properties, k_fps_setting, VisConstants::k_default_fps));
    validate_setting_is_greater_than_zero(settings->get_fps(), k_fps_setting);

    settings->set_sampling_frequency(
        Utils::get(properties, k_sampling_frequency_setting,
                   VisConstants::k_default_sampling_frequency));
    validate_setting_is_greater_than_zero(settings->get_sampling_frequency(),
                                          k_sampling_frequency_setting);

    settings->set_low_cutoff_frequency(
        Utils::get(properties, k_low_cutoff_frequency_setting,
                   VisConstants::k_default_low_cutoff_frequency));
    validate_setting_is_greater_than_zero(settings->get_low_cutoff_frequency(),
                                          k_low_cutoff_frequency_setting);

    settings->set_high_cutoff_frequency(
        Utils::get(properties, k_high_cutoff_frequency_setting,
                   VisConstants::k_default_high_cutoff_frequency));
    validate_setting_is_greater_than_zero(settings->get_high_cutoff_frequency(),
                                          k_high_cutoff_frequency_setting);

    settings->set_spectrum_character(
        Utils::get(properties, k_spectrum_character_setting,
                   VisConstants::k_default_spectrum_character));

    settings->set_spectrum_bar_width(
        Utils::get(properties, k_spectrum_bar_width_setting,
                   VisConstants::k_default_spectrum_bar_width));
    validate_setting_is_greater_than_zero(settings->get_spectrum_bar_width(),
                                          k_spectrum_bar_width_setting);

    settings->set_spectrum_bar_spacing(
        Utils::get(properties, k_spectrum_bar_spacing_setting,
                   VisConstants::k_default_spectrum_bar_spacing));
    validate_setting_is_not_negative(settings->get_spectrum_bar_spacing(),
                                          k_spectrum_bar_spacing_setting);

    settings->set_spectrum_smoothing_mode(
        read_smoothing_mode(properties, k_spectrum_smoothing_mode_setting,
                            VisConstants::k_default_spectrum_smoothing_mode));

    settings->set_spectrum_falloff_mode(
        read_falloff_mode(properties, k_spectrum_falloff_mode_setting,
                          VisConstants::k_default_spectrum_falloff_mode));

    settings->set_spectrum_falloff_weight(
        Utils::get(properties, k_spectrum_falloff_weight_setting,
                   VisConstants::k_default_spectrum_falloff_weight));
    validate_setting_is_greater_than_zero(
        settings->get_spectrum_falloff_weight(),
        k_spectrum_falloff_weight_setting);

    settings->set_spectrum_top_margin(
        Utils::get(properties, k_spectrum_top_margin_setting,
                   VisConstants::k_default_spectrum_top_margin));
    validate_setting_is_not_negative(settings->get_spectrum_top_margin(),
                                     k_spectrum_top_margin_setting);

    settings->set_spectrum_bottom_margin(
        Utils::get(properties, k_spectrum_bottom_margin_setting,
                   VisConstants::k_default_spectrum_bottom_margin));
    validate_setting_is_not_negative(settings->get_spectrum_bottom_margin(),
                                     k_spectrum_bottom_margin_setting);

    settings->set_spectrum_right_margin(
        Utils::get(properties, k_spectrum_right_margin_setting,
                   VisConstants::k_default_spectrum_right_margin));
    validate_setting_is_not_negative(settings->get_spectrum_right_margin(),
                                     k_spectrum_right_margin_setting);

    settings->set_spectrum_left_margin(
        Utils::get(properties, k_spectrum_left_margin_setting,
                   VisConstants::k_default_spectrum_left_margin));
    validate_setting_is_not_negative(settings->get_spectrum_left_margin(),
                                     k_spectrum_left_margin_setting);

    settings->set_is_spectrum_reversed(
        Utils::get(properties, k_spectrum_reversed_setting,
                   VisConstants::k_default_spectrum_reversed));

    settings->set_monstercat_smoothing_factor(
        Utils::get(properties, k_monstercat_smoothing_factor_setting,
                   VisConstants::k_default_monstercat_smoothing_factor));
    validate_setting_is_greater_than_zero(
        settings->get_monstercat_smoothing_factor(),
        k_monstercat_smoothing_factor_setting);

    settings->set_sgs_smoothing_points(
        Utils::get(properties, k_sgs_smoothing_points_setting,
                   VisConstants::k_default_sgs_smoothing_points));
    validate_setting_is_greater_than_zero(settings->get_sgs_smoothing_points(),
                                          k_sgs_smoothing_points_setting);

    settings->set_sgs_smoothing_passes(
        Utils::get(properties, k_sgs_smoothing_passes_setting,
                   VisConstants::k_default_sgs_smoothing_passes));
    validate_setting_is_greater_than_zero(settings->get_sgs_smoothing_passes(),
                                          k_sgs_smoothing_passes_setting);

#ifdef _OS_OSX
    // ncurses on Mac OS X doesn't support wide chars by default, so use a
    // non-wide character for the default
    wchar_t default_lorenz_char = VisConstants::k_default_lorenz_character_osx;
    wchar_t default_ellipse_char =
        VisConstants::k_default_ellipse_character_osx;
#else
    wchar_t default_lorenz_char = VisConstants::k_default_lorenz_character;
    wchar_t default_ellipse_char = VisConstants::k_default_ellipse_character;
#endif

    settings->set_lorenz_character(Utils::get(
        properties, k_lorenz_character_setting, default_lorenz_char));

    settings->set_ellipse_character(Utils::get(
        properties, k_ellipse_character_setting, default_ellipse_char));

    settings->set_ellipse_radius(
        Utils::get(properties, k_ellipse_radius_setting,
                   VisConstants::k_default_ellipse_radius));
    validate_setting_is_greater_than_zero(settings->get_ellipse_radius(),
                                          k_ellipse_radius_setting);

    settings->set_rotation_interval(
        Utils::get(properties, k_rotation_interval_setting,
                   VisConstants::k_default_visualizer_rotation_interval));
    validate_setting_is_not_negative(settings->get_rotation_interval(),
                                     k_rotation_interval_setting);

    settings->set_pulse_audio_source(
        Utils::get(properties, k_vis_pulse_audio_source_setting,
                   VisConstants::k_default_visualizer_pulse_audio_source));

    settings->set_is_stereo_enabled(
        Utils::get(properties, k_stereo_enabled_setting, true));

    settings->set_color_schemes(Utils::split(
        Utils::get(properties, k_color_scheme_path_setting, std::string{""}),
        ','));

    const auto visualizers =
        Utils::split(Utils::get(properties, k_visualizers_setting,
                                VisConstants::k_default_visualizers),
                     ',');

    settings->set_visualizers(visualizers);
}
