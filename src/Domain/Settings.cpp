/*
 * Settings.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"

vis::Settings::Settings()
    : m_fps{VisConstants::kDefaultFps},
      m_sampling_frequency{VisConstants::kDefaultSamplingFrequency},
      m_is_stereo_enabled{true}
{
}

vis::Settings::~Settings()
{
}
