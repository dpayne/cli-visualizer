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

    virtual ~MpdAudioSource();

    virtual bool read(pcm_stereo_sample *buffer, uint32_t buffer_size);

  private:
    const vis::Settings *const m_settings;
    std::ifstream m_fifo_stream;
};
}

#endif
