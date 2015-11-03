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
const static std::string k_mpd_fifo_path_setting{"mpd.fifo.path"};
const static std::string k_mpd_fifo_path_default{"/tmp/mpd.fifo"};

const static std::string k_audio_sources_setting{"audio.sources"}; // mpd,alsa
const static std::string k_audio_sources_default{"mpd"};

const static std::string k_stereo_enabled_setting{"audio.stereo.enabled"};
const static std::string k_sampling_frequency_setting{
    "audio.sampling.frequency"};
const static std::string k_fps_setting{"visualizer.fps"};

const static std::string k_colors_setting{"colors"};
const static std::string k_colors_default{
    "black,blue,cyan,green,yellow,red,magenta,white"};

const static std::string k_colors_enabled_setting{"colors.enabled"};
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

    return properties_map;
}


std::vector<vis::ColorDefinition>
vis::ConfigurationUtils::read_colors(const std::string &colors_path)
{
    std::vector<vis::ColorDefinition> color_definitions;

    std::ifstream file(colors_path.c_str(), std::ifstream::in);
    std::string line;
    if (!file.good())
    {
        VIS_LOG(vis::LogLevel::WARN, "Colors configuration not found at %s",
                colors_path.c_str());
        return color_definitions;
    }

    std::vector<std::string> split_line;

    while (file.good() && std::getline(file, line))
    {
        vis::Utils::split(line, ',', split_line);

        if (split_line.size() == 4)
        {
            color_definitions.push_back(vis::ColorDefinition(
                NcursesUtils::to_color_index(split_line[0]), NcursesUtils::to_color_index(split_line[1]), NcursesUtils::to_color_index(split_line[2]), NcursesUtils::to_color_index(split_line[3])));
        }
        else
        {
            VIS_LOG(vis::LogLevel::WARN,
                    "Configuration line was not valid at %s", line.c_str());
        }
    }

    return color_definitions;
}

void vis::ConfigurationUtils::load_settings(Settings &settings)
{
    auto config_path = Utils::get_home_directory();
    config_path.append(VisConstants::k_default_config_path);
    load_settings(settings, config_path);
}

void vis::ConfigurationUtils::load_settings(Settings &settings,
                                            const std::string &config_path)
{
    const auto properties = vis::ConfigurationUtils::read_config(config_path);

    // setup mpd
    settings.set_mpd_fifo_path(Utils::get(properties, k_mpd_fifo_path_setting,
                                          k_mpd_fifo_path_default));

    // setup audio sources
    settings.set_audio_sources(
        Utils::split(Utils::get(properties, k_audio_sources_setting,
                                k_audio_sources_default),
                     ','));

    settings.set_fps(
        Utils::get(properties, k_fps_setting, VisConstants::k_default_fps));

    settings.set_sampling_frequency(
        Utils::get(properties, k_sampling_frequency_setting,
                   VisConstants::k_default_sampling_frequency));

    settings.set_is_stereo_enabled(
        Utils::get(properties, k_stereo_enabled_setting, true));

    settings.set_is_color_enabled(
        Utils::get(properties, k_colors_enabled_setting, true));

    const auto colors_strs = Utils::split(
        Utils::get(properties, k_colors_setting, k_colors_default), ',');

    std::vector<vis::ColorIndex> colors;

    for (const auto &str : colors_strs)
    {
        colors.push_back(NcursesUtils::to_color_index(str));
    }

    settings.set_colors(colors);

    auto colors_path = Utils::get_home_directory();
    colors_path.append(VisConstants::k_default_colors_path);
    settings.set_color_definitions(
        vis::ConfigurationUtils::read_colors(colors_path));
}
