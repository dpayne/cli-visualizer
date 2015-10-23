/*
 * AudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_AUDIO_SOURCE_H
#define _VIS_AUDIO_SOURCE_H

#include <cstdint>

namespace vis
{

class AudioSource
{
  public:
    explicit AudioSource();

    virtual ~AudioSource();

    virtual bool read(char *buffer, uint32_t buffer_size) = 0;
};
}

#endif
