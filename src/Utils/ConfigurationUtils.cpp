/*
 * ConfigurationUtils.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <fstream>
#include <string>
#include <unordered_map>

#include "Utils/ConfigurationUtils.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Utils/Utils.h"

namespace
{
const static std::string k_audio_sources_setting{"audio.sources"}; // mpd,alsa

const static std::string k_mpd_fifo_path_setting{"mpd.fifo.path"};

const static std::string k_stereo_enabled_setting{"audio.stereo.enabled"};

const static std::string k_vis_rotation_interval{"visualizer.rotation.secs"};

const static std::string k_vis_pulse_audio_source{"audio.pulse.source"};

const static std::string k_visualizers_setting{"visualizers"};
const static std::string k_fps_setting{"visualizer.fps"};

const static std::string k_color_scheme_path_setting{"colors.scheme"};
const static std::string k_sampling_frequency_setting{
    "audio.sampling.frequency"};
const static std::string k_low_cutoff_frequency_setting{
    "audio.low.cutoff.frequency"};
const static std::string k_high_cutoff_frequency_setting{
    "audio.high.cutoff.frequency"};

const static std::string k_lorenz_character{"visualizer.lorenz.character"};

const static std::string k_ellipse_character{"visualizer.ellipse.character"};
const static std::string k_ellipse_radius{"visualizer.ellipse.radius"};

// Spectrum settings
const static std::string k_spectrum_character{"visualizer.spectrum.character"};
const static std::string k_spectrum_bar_width{"visualizer.spectrum.bar.width"};
const static std::string k_spectrum_bar_spacing{
    "visualizer.spectrum.bar.spacing"};
const static std::string k_spectrum_smoothing_mode{
    "visualizer.spectrum.smoothing.mode"};
const static std::string k_spectrum_falloff_mode{
    "visualizer.spectrum.falloff.mode"};
const static std::string k_spectrum_falloff_weight{
    "visualizer.spectrum.falloff.weight"};

const static std::string k_spectrum_top_margin{
    "visualizer.spectrum.top.margin"};
const static std::string k_spectrum_bottom_margin{
    "visualizer.spectrum.bottom.margin"};
const static std::string k_spectrum_right_margin{
    "visualizer.spectrum.right.margin"};
const static std::string k_spectrum_left_margin{
    "visualizer.spectrum.left.margin"};

const static std::string k_spectrum_reversed{"visualizer.spectrum.reversed"};

const static std::string k_monstercat_smoothing_factor{
    "visualizer.monstercat.smoothing.factor"};

const static std::string k_sgs_smoothing_points{
    "visualizer.sgs.smoothing.points"};
const static std::string k_sgs_smoothing_passes{
    "visualizer.sgs.smoothing.passes"};
}

vis::ConfigurationUtils::ConfigurationUtils()
{
}

vis::ConfigurationUtils::~ConfigurationUtils()
{
}

std::unordered_map<std::string, std::string>
vis::ConfigurationUtils::read_config(const std::string &config_path)
{
    std::unordered_map<std::string, std::string> properties_map;

    std::ifstream file(config_path.c_str(), std::ifstream::in);
    std::string line;
    if (!file.good())
    {
        VIS_LOG(vis::LogLevel::WARN, "Configuration not found at %s",
                config_path.c_str());
        return properties_map;
    }

    std::pair<std::string, std::string> split_line{"", ""};

    while (file.good() && std::getline(file, line))
    {
        if (!line.empty() && line[0] != '#')
        {
            vis::Utils::split_first(line, '=', split_line);

            if (!split_line.first.empty())
            {
                properties_map[split_line.first] = split_line.second;
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

std::vector<vis::ColorIndex>
vis::ConfigurationUtils::read_colors(const std::string &colors_path)
{
    std::vector<vis::ColorIndex> colors;

    std::ifstream file(colors_path.c_str(), std::ifstream::in);
    std::string line;
    if (!file.good())
    {
        VIS_LOG(vis::LogLevel::WARN, "Colors configuration not found at %s",
                colors_path.c_str());
        return colors;
    }

    std::vector<std::string> split_line;

    while (file.good() && std::getline(file, line))
    {
        if (!line.empty())
        {
            if (line.size() >= 7)
            {
                auto hex_color = vis::Utils::hex_to_int(line.substr(1, 6));

                int16_t red = (hex_color >> 16) % 256;
                int16_t green = (hex_color >> 8) % 256;
                int16_t blue = hex_color % 256;

                colors.push_back(NcursesUtils::to_ansi_color(red, green, blue));
            }
            else
            {
                auto basic_color = NcursesUtils::to_basic_color(line);
                if (basic_color >= 0)
                {
                    colors.push_back(basic_color);
                }
                else
                {
                    VIS_LOG(vis::LogLevel::WARN, "Configuration color "
                                                 "definition line was not "
                                                 "valid at %s",
                            line.c_str());
                }
            }
        }
    }

    return colors;
}

vis::FalloffMode vis::ConfigurationUtils::read_falloff_mode(
    const std::unordered_map<std::string, std::string> properties,
    const std::string &config_param,
    const vis::FalloffMode default_falloff_mode)
{
    auto falloff_mode_str =
        vis::Utils::get(properties, k_spectrum_falloff_mode, std::string{""});

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
    const std::unordered_map<std::string, std::string> properties,
    const std::string &config_param,
    const vis::SmoothingMode default_smoothing_mode)
{
    auto smoothing_mode_str =
        vis::Utils::get(properties, k_spectrum_smoothing_mode, std::string{""});

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

void vis::ConfigurationUtils::load_settings(Settings &settings)
{
    auto config_path = VisConstants::k_default_config_path;
    load_settings(settings, config_path);
}

void vis::ConfigurationUtils::load_settings(Settings &settings,
                                            const std::string &config_path)
{
    const auto properties = vis::ConfigurationUtils::read_config(config_path);

    // setup mpd
    settings.set_mpd_fifo_path(
        Utils::get(properties, k_mpd_fifo_path_setting,
                   VisConstants::k_default_mpd_fifo_path));

    // setup audio sources
    settings.set_audio_sources(
        Utils::split(Utils::get(properties, k_audio_sources_setting,
                                VisConstants::k_default_audio_sources),
                     ','));

    settings.set_fps(
        Utils::get(properties, k_fps_setting, VisConstants::k_default_fps));

    settings.set_sampling_frequency(
        Utils::get(properties, k_sampling_frequency_setting,
                   VisConstants::k_default_sampling_frequency));

    settings.set_low_cutoff_frequency(
        Utils::get(properties, k_low_cutoff_frequency_setting,
                   VisConstants::k_default_low_cutoff_frequency));

    settings.set_high_cutoff_frequency(
        Utils::get(properties, k_high_cutoff_frequency_setting,
                   VisConstants::k_default_high_cutoff_frequency));

    settings.set_spectrum_character(
        Utils::get(properties, k_spectrum_character,
                   VisConstants::k_default_spectrum_character));

    settings.set_spectrum_bar_width(
        Utils::get(properties, k_spectrum_bar_width,
                   VisConstants::k_default_spectrum_bar_width));

    settings.set_spectrum_bar_spacing(
        Utils::get(properties, k_spectrum_bar_spacing,
                   VisConstants::k_default_spectrum_bar_spacing));

    settings.set_spectrum_smoothing_mode(
        read_smoothing_mode(properties, k_spectrum_smoothing_mode,
                            VisConstants::k_default_spectrum_smoothing_mode));

    settings.set_spectrum_falloff_mode(
        read_falloff_mode(properties, k_spectrum_falloff_mode,
                          VisConstants::k_default_spectrum_falloff_mode));

    settings.set_spectrum_falloff_weight(
        Utils::get(properties, k_spectrum_falloff_weight,
                   VisConstants::k_default_spectrum_falloff_weight));

    settings.set_spectrum_top_margin(
        Utils::get(properties, k_spectrum_top_margin,
                   VisConstants::k_default_spectrum_top_margin));

    settings.set_spectrum_bottom_margin(
        Utils::get(properties, k_spectrum_bottom_margin,
                   VisConstants::k_default_spectrum_bottom_margin));

    settings.set_spectrum_right_margin(
        Utils::get(properties, k_spectrum_right_margin,
                   VisConstants::k_default_spectrum_right_margin));

    settings.set_spectrum_left_margin(
        Utils::get(properties, k_spectrum_left_margin,
                   VisConstants::k_default_spectrum_left_margin));

    settings.set_is_spectrum_reversed(
        Utils::get(properties, k_spectrum_reversed,
                   VisConstants::k_default_spectrum_reversed));

    settings.set_monstercat_smoothing_factor(
        Utils::get(properties, k_monstercat_smoothing_factor,
                   VisConstants::k_default_monstercat_smoothing_factor));

    settings.set_sgs_smoothing_points(
        Utils::get(properties, k_sgs_smoothing_points,
                   VisConstants::k_default_sgs_smoothing_points));

    settings.set_sgs_smoothing_passes(
        Utils::get(properties, k_sgs_smoothing_passes,
                   VisConstants::k_default_sgs_smoothing_passes));

    settings.set_lorenz_character(
        Utils::get(properties, k_lorenz_character,
                   VisConstants::k_default_lorenz_character));

    settings.set_ellipse_character(
        Utils::get(properties, k_ellipse_character,
                   VisConstants::k_default_ellipse_character));

    settings.set_ellipse_radius(Utils::get(
        properties, k_ellipse_radius, VisConstants::k_default_ellipse_radius));

    settings.set_rotation_interval(
        Utils::get(properties, k_vis_rotation_interval,
                   VisConstants::k_default_visualizer_rotation_interval));

    settings.set_pulse_audio_source(
        Utils::get(properties, k_vis_pulse_audio_source,
                   VisConstants::k_default_visualizer_pulse_audio_source));

    settings.set_is_stereo_enabled(
        Utils::get(properties, k_stereo_enabled_setting, true));

    // set color definitions
    auto colors_path = VisConstants::k_colors_directory;
    colors_path.append(Utils::get(properties, k_color_scheme_path_setting,
                                  VisConstants::k_default_colors_path));
    settings.set_colors(vis::ConfigurationUtils::read_colors(colors_path));

    if (settings.get_colors().empty())
    {
        settings.set_colors(VisConstants::k_default_colors);
    }

    const auto visualizers =
        Utils::split(Utils::get(properties, k_visualizers_setting,
                                VisConstants::k_default_visualizers),
                     ',');

    settings.set_visualizers(visualizers);
}
