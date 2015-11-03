/*
 * VisConstants.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_CONSTANTS_H
#define _VIS_CONSTANTS_H

#include <string>

namespace VisConstants
{
const std::string k_true{"true"};

const std::string k_mpd_audio_source_name{"mpd"};
const uint32_t k_default_fps = 25;
const uint32_t k_default_sampling_frequency = 44100;
const std::string k_default_config_path{"vis/config"};
const std::string k_default_colors_path{"vis/colors"};
const std::string k_default_log_path{"vis/vis.log"};
}

#endif
