/*
 * MacOsXAudioSource.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Source/MacOsXAudioSource.h"

vis::MacOsXAudioSource::MacOsXAudioSource(const vis::Settings *const settings)
{
    settings->get_visualizers();
}

vis::MacOsXAudioSource::~MacOsXAudioSource()
{
}

bool vis::MacOsXAudioSource::read(pcm_stereo_sample *buffer,
                                  const uint32_t buffer_size)
{
#ifdef _OS_OSX
// TODO: put mac os x specific code here
#endif
    if (buffer_size > 0 || buffer != nullptr)
    {
        return false;
    }

    return false;
}
