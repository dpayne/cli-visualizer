/*
 * ShmemAudioSource.cpp
 *
 *     Author: dnalor
 */

#include "Source/ShmemAudioSource.h"
#include "Utils/Logger.h"
#include <cerrno>
#include <cstring>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <unistd.h>

namespace
{
const int64_t k_read_attempt_sleep_interval_nanosecs = 2L * 1000000L; // milliseconds
struct timespec k_read_attempt_sleep_timespec = {
    0, k_read_attempt_sleep_interval_nanosecs};

#define SAMPLE_SIZE_OUT sizeof(pcm_stereo_sample)
#define VIS_BUF_SIZE 16384

typedef struct {
    pthread_rwlock_t rwlock;
    uint32_t buf_size;
    uint32_t buf_index;
    bool running;
    uint32_t rate;
    time_t updated;
    int16_t buffer[VIS_BUF_SIZE];
}  vis_t;

vis_t *mmap_area = NULL;

} // namespace

vis::ShmemAudioSource::ShmemAudioSource(
    const std::shared_ptr<const vis::Settings> settings)
    : m_settings{settings}
{
    open_shmem();
}

bool vis::ShmemAudioSource::open_shmem()
{
    m_shmem_fd = ::shm_open(m_settings->get_shmem_name().c_str(), O_RDONLY, 0666);

    if (m_shmem_fd < 0) {
        VIS_LOG(vis::LogLevel::ERROR, "ShmemAudioSource: Error reading shmem (name: %s): %s", m_settings->get_shmem_name().c_str(), strerror(errno));
        m_shmem_fd = -1;
        return false;
    }
    mmap_area = (vis_t *) mmap(NULL, sizeof( vis_t), PROT_READ, MAP_SHARED, m_shmem_fd, 0);
    if ((intptr_t)mmap_area == -1) {
        VIS_LOG(vis::LogLevel::ERROR, "ShmemAudioSource: mmap failed");
        return false;
    }
    return true;
}

bool vis::ShmemAudioSource::read(pcm_stereo_sample *buffer, const uint32_t buffer_size)
{
    if (m_shmem_fd < 0) {
        open_shmem();
    }

    auto buffer_size_bytes = static_cast<size_t>(SAMPLE_SIZE_OUT * buffer_size);
    size_t bytes_left = buffer_size_bytes;

    if (m_shmem_fd >= 0 && mmap_area != NULL)   {
        VIS_LOG(vis::LogLevel::DEBUG, "ShmemAudioSource: bufsz(mmap): %u / buf_index: %u / run: %u / rate: %u / bufsz(out): %u / bufsz(out_b): %zu",mmap_area->buf_size, mmap_area->buf_index, mmap_area->running, mmap_area->rate, buffer_size, buffer_size_bytes);

        memset(buffer, 0, buffer_size_bytes);
        if (mmap_area->running == true) {
            int n = 0;
            uint32_t old_buf_index;
            while (bytes_left > 0) {
                old_buf_index = mmap_area->buf_index;
                for ( unsigned int i = mmap_area->buf_index; i < mmap_area->buf_size && bytes_left != 0; i += 2) {
                    buffer[n].l = mmap_area->buffer[i];
                    buffer[n].r = mmap_area->buffer[i + 1];
                    n++;
                    bytes_left -= SAMPLE_SIZE_OUT; 
                }

                if( bytes_left > 0) {
                    VIS_LOG(vis::LogLevel::DEBUG, "ShmemAudioSource: sleep: bytes_left: %zu (index:%u)", bytes_left,  mmap_area->buf_index);
                    while( mmap_area->buf_index == old_buf_index)
                        nanosleep(&k_read_attempt_sleep_timespec, nullptr);
                }

            }
        }
        return true;
    }
    return false;
}

vis::ShmemAudioSource::~ShmemAudioSource()
{
    if (m_shmem_fd >= 0)    {
        ::close(m_shmem_fd);
    }
    if( mmap_area != NULL) {
        if ( munmap( mmap_area, sizeof( vis_t) ) != 0 ) {
            VIS_LOG(vis::LogLevel::WARN, "Could not munmap() area %p+%d: %s", mmap_area, sizeof( vis_t), strerror( errno ) );
        }
    }
}
