/*
 * VisConstants.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_CONSTANTS_H
#define _VIS_CONSTANTS_H

#include <cstdlib>
#include <string>
#include <vector>

#include "Domain/VisTypes.h"
#include "Writer/NcursesWriter.h"

namespace VisConstants
{

// max extended color values, assumes extended colors fit in signed shorts
static const int16_t k_max_extended_color = 2 << 13; // 16384
static const int16_t k_default_max_color = 256;
static const std::string k_disabled_gradient_color_config{"gradient=false"};
static const std::string k_enabled_gradient_color_config{"gradient=true"};

// Various constants
static const std::string k_default_locale{"en_US.UTF-8"};
static const wchar_t k_space_wchar{L' '};
static const wchar_t k_full_block_wchar{L'\u2588'};
static const wchar_t k_hash_wchar{L'#'};

static const double k_pi = 3.14159265358979323846;

static const std::string k_true{"true"};
static const std::string k_false{"false"};
static const std::string k_mpd_audio_source_name{"mpd"};
static const std::string k_osx_audio_source_name{"osx"};
static const std::string k_port_audio_source_name{"port"};
static const std::string k_default_visualizer_port_audio_source;
static const std::string k_pulse_audio_source_name{"pulse"};
static const std::string k_shmem_audio_source_name{"shmem"};
static const std::string k_default_visualizer_pulse_audio_source;
static const std::string k_spectrum_circle_visualizer_name{"spectrum_circle"};
static const std::string k_ellipse_visualizer_name{"ellipse"};
static const std::string k_lorenz_visualizer_name{"lorenz"};
static const int64_t k_default_visualizer_rotation_interval =
    0L; // 0 means disabled
// Defaults
static const double k_default_scaling_multiplier = 1.0;

static const uint32_t k_default_fps = 20;

const static std::string k_default_audio_source{k_pulse_audio_source_name};
const static std::string k_default_mpd_fifo_path{"/tmp/mpd.fifo"};
const static std::string k_default_shmem_name{"/squeezelite-00:00:00:00:00:00"};
const static std::string k_default_visualizers{"spectrum,ellipse,lorenz"};

// visualizer names
static const std::string k_spectrum_visualizer_name{"spectrum"};

// frequency defaults
static const uint32_t k_default_sampling_frequency = 44100;
static const uint32_t k_default_low_cutoff_frequency = 30;
static const uint32_t k_default_high_cutoff_frequency = 22050;

// config path defaults
// XDG_CONFIG_HOME is usually "~/.config"
static const char *k_xdg_config_home{std::getenv("XDG_CONFIG_HOME")};

// http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
// use ${XDG_CONFIG_HOME}/vis/ as the default config directory
static const std::string k_default_config_path_root{
    (k_xdg_config_home != nullptr
         ? std::string{k_xdg_config_home}
         : std::string{std::getenv("HOME")} + "/.config") +
    "/vis/"};
static const std::string k_default_config_path =
    k_default_config_path_root + "config";
static const std::string k_colors_directory{k_default_config_path_root +
                                            "colors/"};
static const std::string k_default_log_path{k_default_config_path_root +
                                            "vis.log"};
static const std::string k_default_colors_path{"colors"};

// Default characters for visualizers
static const wchar_t k_default_spectrum_character{k_space_wchar};
static const wchar_t k_default_lorenz_character{k_full_block_wchar};
static const wchar_t k_default_ellipse_character{k_full_block_wchar};
static const wchar_t k_default_lorenz_character_osx{k_hash_wchar};
static const wchar_t k_default_ellipse_character_osx{k_hash_wchar};

// ellipse defaults
static const uint32_t k_default_ellipse_radius{2};

// monstercat smoothing defaults
static const double k_default_monstercat_smoothing_factor{1.5};
static const uint32_t k_default_monstercat_bar_width = 1;
static const uint32_t k_default_monstercat_bar_spacing = 0;

// sgs smoothing defaults
static const uint32_t k_default_sgs_smoothing_points{
    3}; // this should be an odd number
static const uint32_t k_default_sgs_smoothing_passes{2};

// spectrum defaults
static const uint32_t k_default_spectrum_bar_width = 2;
static const uint32_t k_default_spectrum_bar_spacing = 1;
static const vis::SmoothingMode k_default_spectrum_smoothing_mode =
    vis::SmoothingMode::Sgs;
static const vis::FalloffMode k_default_spectrum_falloff_mode =
    vis::FalloffMode::Fill;
static const double k_default_spectrum_falloff_weight = 0.95;
static const double k_default_spectrum_top_margin = 0.0;
static const double k_default_spectrum_bottom_margin = 0.0;
static const double k_default_spectrum_right_margin = 0.0;
static const double k_default_spectrum_left_margin = 0.0;

static const bool k_default_spectrum_reversed = false;

// input timeouts
static const int64_t k_default_mpd_fifo_timeout = 1l;
static const int32_t k_default_user_input_timeout_milliseconds = 500;
static const uint64_t k_silent_sleep_milliseconds = 100ul;

// 16 bit colorscheme
static const std::vector<vis::ColorDefinition> k_default_16_colors = {
    vis::ColorDefinition{4, -1, -1, -1},  vis::ColorDefinition{12, -1, -1, -1},
    vis::ColorDefinition{6, -1, -1, -1},  vis::ColorDefinition{14, -1, -1, -1},
    vis::ColorDefinition{2, -1, -1, -1},  vis::ColorDefinition{10, -1, -1, -1},
    vis::ColorDefinition{11, -1, -1, -1}, vis::ColorDefinition{3, -1, -1, -1},
    vis::ColorDefinition{5, -1, -1, -1},  vis::ColorDefinition{1, -1, -1, -1},
    vis::ColorDefinition{13, -1, -1, -1}, vis::ColorDefinition{9, -1, -1, -1},
    vis::ColorDefinition{7, -1, -1, -1},  vis::ColorDefinition{15, -1, -1, -1},
    vis::ColorDefinition{0, -1, -1, -1},
};

// 8 bit colorscheme
static const std::vector<vis::ColorDefinition> k_default_8_colors = {
    vis::ColorDefinition{4, -1, -1, -1}, vis::ColorDefinition{6, -1, -1, -1},
    vis::ColorDefinition{2, -1, -1, -1}, vis::ColorDefinition{3, -1, -1, -1},
    vis::ColorDefinition{5, -1, -1, -1}, vis::ColorDefinition{1, -1, -1, -1},
    vis::ColorDefinition{7, -1, -1, -1}, vis::ColorDefinition{0, -1, -1, -1}};
} // namespace VisConstants

#endif
