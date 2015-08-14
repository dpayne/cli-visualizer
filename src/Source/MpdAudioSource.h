/*
 * MpdAudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _MPD_AUDIO_SOURCE_H
#define _MPD_AUDIO_SOURCE_H

#include "Source/AudioSource.h"

class MpdAudioSource : public AudioSource
{
    public:
        explicit MpdAudioSource();

        ~MpdAudioSource();
};

#endif
