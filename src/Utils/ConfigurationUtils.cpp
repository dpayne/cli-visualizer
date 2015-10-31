/*
 * ConfigurationUtils.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <fstream>
#include <unordered_map>

#include "Utils/ConfigurationUtils.h"
#include "Utils/Logger.h"
#include "Utils/Utils.h"

namespace
{
const static std::string k_mpd_fifo_path_setting{"mpd.fifo.path"};
const static std::string k_mpd_fifo_path_default{"/tmp/mpd.fifo"};
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

    std::pair<std::string, std::string> split_line{"",""};

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

void vis::ConfigurationUtils::load_settings(Settings &settings)
{
    auto config_path = Utils::get_home_directory();
    config_path.append( VisConstants::k_default_config_path );
    load_settings(settings, config_path);
}

void vis::ConfigurationUtils::load_settings(Settings &settings,
                                            const std::string &config_path)
{
    const auto properties =
        vis::ConfigurationUtils::read_config(config_path);

    // setup mpd
    settings.set_mpd_fifo_path(Utils::get(properties, k_mpd_fifo_path_setting,
                                          k_mpd_fifo_path_default));

    // setup audio sources
    settings.set_audio_sources({VisConstants::k_mpd_audio_source_name});
}
