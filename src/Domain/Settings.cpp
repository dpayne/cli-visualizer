/*
 * Settings.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"

vis::Settings::Settings()
    : m_fps{VisConstants::k_default_fps},
      m_sampling_frequency{VisConstants::k_default_sampling_frequency},
      m_low_cutoff_frequency{VisConstants::k_default_low_cutoff_frequency},
      m_high_cutoff_frequency{VisConstants::k_default_high_cutoff_frequency},
      m_is_stereo_enabled{true}, m_is_color_enabled{true},
      m_spectrum_character{VisConstants::k_default_spectrum_character},
      m_monstercat_character{VisConstants::k_default_monstercat_character},
      m_monstercat_bar_width{VisConstants::k_default_monstercat_bar_width},
      m_monstercat_bar_spacing{VisConstants::k_default_monstercat_bar_spacing},
      m_monstercat_smoothing_factor{VisConstants::k_default_monstercat_smoothing_factor}
{
}

vis::Settings::~Settings()
{
}
