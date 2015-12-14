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
static const std::string k_true{"true"};
static const std::string k_false{"false"};

static const std::string k_mpd_audio_source_name{"mpd"};
static const uint32_t k_default_fps = 25;
static const uint32_t k_default_sampling_frequency = 22050;
static const std::string k_default_config_path{"vis/config"};
static const std::string k_default_colors_path{"vis/colors"};
static const std::string k_default_log_path{"vis/vis.log"};
static const wchar_t k_default_spectrum_character{L'\u2588'};
}

#endif
