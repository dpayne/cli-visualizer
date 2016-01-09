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
#include <pulse/pulseaudio.h>
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

    pa_mainloop *m_pulseaudio_mainloop;

    std::string m_pulseaudio_default_source_name;

    static void pulseaudio_context_state_callback(pa_context *c,
                                                  void *userdata);

    static void pulseaudio_server_info_callback(pa_context *context,
                                                const pa_server_info *i,
                                                void *userdata);
#endif

    void populate_default_source_name();

    bool open_pulseaudio_source(const uint32_t max_buffer_size);
};
}

#endif
