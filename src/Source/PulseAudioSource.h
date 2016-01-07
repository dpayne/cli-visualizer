/*
 * PulseAudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_PULSE_AUDIO_SOURCE_H
#define _VIS_PULSE_AUDIO_SOURCE_H

#include <fstream>

#ifdef _ENABLE_PULSE
#include <pulse/simple.h>
#include <pulse/error.h>
#endif

#include "Source/AudioSource.h"
#include "Domain/Settings.h"

namespace vis
{

class PulseAudioSource : public vis::AudioSource
{
  public:
    explicit PulseAudioSource(const vis::Settings *const settings);

    ~PulseAudioSource() override;

    bool read(pcm_stereo_sample *buffer, const uint32_t buffer_size) override;

  private:
#ifdef _ENABLE_PULSE
    const vis::Settings *const m_settings;

    pa_simple *m_pulseaudio_simple;
#endif

    bool open_pulseaudio_source(const uint32_t max_buffer_size);
};
}

#endif
