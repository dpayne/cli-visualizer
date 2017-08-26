/*
 * AudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_AUDIO_SOURCE_H
#define _VIS_AUDIO_SOURCE_H

#include "Domain/VisTypes.h"
#include <cstdint>

namespace vis
{

class AudioSource
{
  public:
    explicit AudioSource();

    AudioSource(const AudioSource &other) = delete;

    AudioSource(const AudioSource &&other) = delete;

    AudioSource &operator=(const AudioSource &v) = delete;

    AudioSource &operator=(AudioSource &&v) = delete;

    virtual ~AudioSource();

    /**
     * Reads "buffer_size" frames of the audio stream into "buffer"
     */
    virtual bool read(pcm_stereo_sample *buffer, uint32_t buffer_size) = 0;
};
} // namespace vis

#endif
