/*
 * PortAudioSource.h
 *
 * Created on: May 1, 2020
 *     Author: noriah
 */

#ifndef _VIS_PORT_AUDIO_SOURCE_H
#define _VIS_PORT_AUDIO_SOURCE_H

#include <fstream>
#include <memory>

#ifdef _ENABLE_PORT
#include <portaudio.h>
#endif

#include "Domain/Settings.h"
#include "Source/AudioSource.h"

namespace vis
{

class PortAudioSource : public vis::AudioSource
{
  public:
    explicit PortAudioSource(
        const std::shared_ptr<const vis::Settings> settings);

    PortAudioSource(const PortAudioSource &other) = delete;

    PortAudioSource(const PortAudioSource &&other) = delete;

    PortAudioSource &operator=(const PortAudioSource &v) = delete;

    PortAudioSource &operator=(PortAudioSource &&v) = delete;

#ifdef _ENABLE_PORT
    ~PortAudioSource() override;
#else
    ~PortAudioSource() override = default;
#endif

    bool read(pcm_stereo_sample *buffer, uint32_t buffer_size) override;

  private:
#ifdef _ENABLE_PORT
    const std::shared_ptr<const Settings> m_settings;

    PaStream *m_portaudio_stream;
    PaStreamParameters m_portaudio_params;

#endif

    bool open_portaudio_source(uint32_t max_buffer_size);
};
} // namespace vis

#endif
