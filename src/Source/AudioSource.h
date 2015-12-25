/*
 * AudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_AUDIO_SOURCE_H
#define _VIS_AUDIO_SOURCE_H

#include <cstdint>
#include "Domain/VisTypes.h"

namespace vis
{

class AudioSource
{
  public:
    explicit AudioSource();

    virtual ~AudioSource();

    /**
     * Reads "buffer_size" frames of the audio stream into "buffer"
     */
    virtual bool read(pcm_stereo_sample *buffer, uint32_t buffer_size) = 0;
};
}

#endif
