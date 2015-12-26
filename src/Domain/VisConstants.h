/*
 * VisConstants.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_CONSTANTS_H
#define _VIS_CONSTANTS_H

#include <string>
#include "Domain/VisTypes.h"

namespace VisConstants
{

//Various constants
static const wchar_t k_space_wchar{L' '};
static const wchar_t k_full_block_wchar{L'\u2588'};

static const double k_pi = 3.14159265358979323846;

static const std::string k_true{"true"};
static const std::string k_false{"false"};
static const std::string k_mpd_audio_source_name{"mpd"};
static const std::string k_spectrum_circle_visualizer_name{"spectrum_circle"};
static const std::string k_ellipse_visualizer_name{"ellipse"};
static const std::string k_lorenz_visualizer_name{"lorenz"};

//Defaults
static const uint32_t k_default_fps = 20;

const static std::string k_default_audio_sources{"mpd"};
const static std::string k_default_mpd_fifo_path{"/tmp/mpd.fifo"};
const static std::string k_default_visualizers{"spectrum,ellipse,lorenz"};

// visualizer names
static const std::string k_spectrum_visualizer_name{"spectrum"};

// frequency defaults
static const uint32_t k_default_sampling_frequency = 44100;
static const uint32_t k_default_low_cutoff_frequency = 30;
static const uint32_t k_default_high_cutoff_frequency = 22050;

// config path defaults
static const std::string k_default_config_path{".vis/config"};
static const std::string k_colors_directory{".vis/colors/"};
static const std::string k_default_colors_path{"colors"};
static const std::string k_default_log_path{".vis/vis.log"};

// Full block character is L
static const wchar_t k_default_spectrum_character{k_space_wchar};
static const wchar_t k_default_lorenz_character{k_space_wchar};
static const wchar_t k_default_ellipse_character{k_space_wchar};

// ellipse defaults
static const uint32_t k_default_ellipse_radius{2};

// monstercat smoothing defaults
static const double k_default_monstercat_smoothing_factor{1.5};
static const uint32_t k_default_monstercat_bar_width = 1;
static const uint32_t k_default_monstercat_bar_spacing = 0;

// sgs smoothing defaults
static const uint32_t k_default_sgs_smoothing_points{
    3}; // this should be an odd number
static const uint32_t k_default_sgs_smoothing_passes{1};

// spectrum defaults
static const uint32_t k_default_spectrum_bar_width = 2;
static const uint32_t k_default_spectrum_bar_spacing = 1;
static const vis::SmoothingMode k_default_spectrum_smoothing_mode =
    vis::SmoothingMode::MonsterCat;
static const vis::FalloffMode k_default_spectrum_falloff_mode =
    vis::FalloffMode::Fill;
static const double k_default_spectrum_falloff_weight = 0.99;

// input timeouts
static const long k_default_mpd_fifo_timeout = 1l;
static const int32_t k_default_user_input_timeout_milliseconds = 500;
static const uint64_t k_silent_sleep_milliseconds = 100ul;
}

#endif
