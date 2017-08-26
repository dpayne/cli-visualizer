/*
 * MpdAudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_MPD_AUDIO_SOURCE_H
#define _VIS_MPD_AUDIO_SOURCE_H

#include <fstream>
#include <memory>

#include "Domain/Settings.h"
#include "Source/AudioSource.h"

namespace vis
{

class MpdAudioSource : public vis::AudioSource
{
  public:
    explicit MpdAudioSource(
        const std::shared_ptr<const vis::Settings> settings);

    MpdAudioSource(const MpdAudioSource &other) = delete;

    MpdAudioSource(const MpdAudioSource &&other) = delete;

    MpdAudioSource &operator=(const MpdAudioSource &v) = delete;

    MpdAudioSource &operator=(MpdAudioSource &&v) = delete;

    ~MpdAudioSource() override;

    bool read(pcm_stereo_sample *buffer, uint32_t buffer_size) override;

  private:
    const std::shared_ptr<const Settings> m_settings;

    int32_t m_mpd_fifo_fd{};

    bool open_mpd_fifo();
};
} // namespace vis

#endif
