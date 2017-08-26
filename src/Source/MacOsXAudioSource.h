/*
 * MacOsXAudioSource.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_MAC_OS_X_AUDIO_SOURCE_H
#define _VIS_MAC_OS_X_AUDIO_SOURCE_H

#include "Domain/Settings.h"
#include "Domain/VisTypes.h"
#include "Source/AudioSource.h"
#include <cstdint>

#ifdef _OS_OSX
// TODO: put mac os x specific headers here
#endif

namespace vis
{

class MacOsXAudioSource : public vis::AudioSource
{
  public:
    explicit MacOsXAudioSource(const vis::Settings *settings);

    ~MacOsXAudioSource() override;

    /**
     * Reads "buffer_size" frames of the audio stream into "buffer"
     */
    bool read(pcm_stereo_sample *buffer, uint32_t buffer_size) override;

  private:
#ifdef _OS_OSX
// TODO: put mac os x specific features here
#endif
};
} // namespace vis

#endif
