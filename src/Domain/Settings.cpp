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
      m_is_stereo_enabled{true},
      m_spectrum_character{VisConstants::k_default_spectrum_character},
      m_lorenz_character{VisConstants::k_default_lorenz_character},
      m_ellipse_character{VisConstants::k_default_ellipse_character},
      m_ellipse_radius{VisConstants::k_default_ellipse_radius},
      m_spectrum_bar_width{VisConstants::k_default_spectrum_bar_width},
      m_spectrum_bar_spacing{VisConstants::k_default_spectrum_bar_spacing},
      m_monstercat_smoothing_factor{
          VisConstants::k_default_monstercat_smoothing_factor},
      m_sgs_smoothing_points{VisConstants::k_default_sgs_smoothing_points},
      m_sgs_smoothing_passes{VisConstants::k_default_sgs_smoothing_passes},
      m_spectrum_smoothing_mode{
          VisConstants::k_default_spectrum_smoothing_mode},
      m_spectrum_falloff_mode{VisConstants::k_default_spectrum_falloff_mode},
      m_spectrum_falloff_weight{VisConstants::k_default_spectrum_falloff_weight}
{
}

vis::Settings::~Settings()
{
}
