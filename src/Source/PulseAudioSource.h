/*
 * PulseAudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_PULSE_AUDIO_SOURCE_H
#define _VIS_PULSE_AUDIO_SOURCE_H

#include <fstream>
#include <memory>

#ifdef _ENABLE_PULSE
#include <pulse/error.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#endif

#include "Domain/Settings.h"
#include "Source/AudioSource.h"

namespace vis
{

class PulseAudioSource : public vis::AudioSource
{
  public:
    explicit PulseAudioSource(
        const std::shared_ptr<const vis::Settings> settings);

    PulseAudioSource(const PulseAudioSource &other) = delete;

    PulseAudioSource(const PulseAudioSource &&other) = delete;

    PulseAudioSource &operator=(const PulseAudioSource &v) = delete;

    PulseAudioSource &operator=(PulseAudioSource &&v) = delete;

#ifdef _ENABLE_PULSE
    ~PulseAudioSource() override;
#else
    ~PulseAudioSource() override = default;
#endif

    bool read(pcm_stereo_sample *buffer, uint32_t buffer_size) override;

  private:
#ifdef _ENABLE_PULSE
    const std::shared_ptr<const Settings> m_settings;

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

    bool open_pulseaudio_source(uint32_t max_buffer_size);
};
} // namespace vis

#endif
