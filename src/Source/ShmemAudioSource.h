/*
 * ShmemAudioSource.h
 *
 */

#ifndef _VIS_SHMEM_AUDIO_SOURCE_H
#define _VIS_SHMEM_AUDIO_SOURCE_H

#include <fstream>
#include <memory>

#include "Domain/Settings.h"
#include "Source/AudioSource.h"

namespace vis
{

class ShmemAudioSource : public vis::AudioSource
{
    public:
        explicit ShmemAudioSource(
                const std::shared_ptr<const vis::Settings> settings);

        ShmemAudioSource(const ShmemAudioSource &other) = delete;

        ShmemAudioSource(const ShmemAudioSource &&other) = delete;

        ShmemAudioSource &operator=(const ShmemAudioSource &v) = delete;

        ShmemAudioSource &operator=(ShmemAudioSource &&v) = delete;

        ~ShmemAudioSource() override;

        bool read(pcm_stereo_sample *buffer, uint32_t buffer_size) override;

    private:
        const std::shared_ptr<const Settings> m_settings;

        int32_t m_shmem_fd{};

        bool open_shmem();
};
} // namespace vis

#endif
