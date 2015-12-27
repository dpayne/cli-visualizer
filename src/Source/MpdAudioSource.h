/*
 * MpdAudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_MPD_AUDIO_SOURCE_H
#define _VIS_MPD_AUDIO_SOURCE_H

#include <fstream>

#include "Source/AudioSource.h"
#include "Domain/Settings.h"

namespace vis
{

class MpdAudioSource : public vis::AudioSource
{
  public:
    explicit MpdAudioSource(const vis::Settings *const settings);

    ~MpdAudioSource() override;

    bool read(pcm_stereo_sample *buffer, const uint32_t buffer_size) override;

  private:
    const vis::Settings *const m_settings;

    int32_t m_mpd_fifo_fd;

    bool open_mpd_fifo();
};
}

#endif
