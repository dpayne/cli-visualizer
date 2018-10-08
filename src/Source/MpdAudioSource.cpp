/*
 * MpdAudioSource.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Source/MpdAudioSource.h"
#include "Utils/Logger.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>

namespace
{
const int32_t k_read_attempts = 100;
const int64_t k_read_attempt_sleep_interval_nanosecs =
    1L * 1000000L; // 1 millisecond
struct timespec k_read_attempt_sleep_timespec = {
    0, k_read_attempt_sleep_interval_nanosecs};
} // namespace

vis::MpdAudioSource::MpdAudioSource(
    const std::shared_ptr<const vis::Settings> settings)
    : m_settings{settings}
{
    open_mpd_fifo();
}

bool vis::MpdAudioSource::open_mpd_fifo()
{
    m_mpd_fifo_fd = ::open(m_settings->get_mpd_fifo_path().c_str(), O_RDONLY);

    if (m_mpd_fifo_fd < 0)
    {
        VIS_LOG(vis::LogLevel::WARN, "Error reading file: %s", strerror(errno));
        m_mpd_fifo_fd = -1;
        return false;
    }

    auto flags = fcntl(m_mpd_fifo_fd, F_GETFL, 0);
    auto retval = fcntl(m_mpd_fifo_fd, F_SETFL, flags | O_NONBLOCK);

    if (retval < 0)
    {
        VIS_LOG(vis::LogLevel::ERROR,
                "Could not set correct file controls on mpd fifo file: %s",
                strerror(errno));
    }

    return true;
}

bool vis::MpdAudioSource::read(pcm_stereo_sample *buffer,
                               const uint32_t buffer_size)
{
    // try to re-open the stream if it has been closed
    if (m_mpd_fifo_fd < 0)
    {
        open_mpd_fifo();
    }

    auto buffer_size_bytes =
        static_cast<size_t>(sizeof(pcm_stereo_sample) * buffer_size);
    size_t bytes_left = buffer_size_bytes;

    if (m_mpd_fifo_fd >= 0)
    {
        auto attempts = 0;
        memset(buffer, 0, buffer_size_bytes);
        while (bytes_left > 0)
        {
            // Read buffer
            int64_t bytes_read = ::read(m_mpd_fifo_fd, buffer, bytes_left);

            // No bytes left
            if (bytes_read == 0)
            {
                VIS_LOG(vis::LogLevel::WARN, "Could not read any bytes");
                return false;
            }
            // Error reading file. Since non-blocking is set, it's possible
            // there's not enough data yet
            if (bytes_read == -1)
            {
                auto error_code = errno;

                // EAGAIN means data is not ready yet
                if (error_code == EAGAIN)
                {

                    // Try up to k_read_attempts before quiting
                    if (attempts > k_read_attempts)
                    {
                        VIS_LOG(vis::LogLevel::WARN,
                                "Could not finish reading "
                                "buffer, bytes read: %d    "
                                "buffer size: ",
                                bytes_read, buffer_size_bytes);

                        // zero out buffer
                        memset(buffer, 0, buffer_size_bytes);
                        ::close(m_mpd_fifo_fd);
                        m_mpd_fifo_fd = -1;
                        return false;
                    }

                    nanosleep(&k_read_attempt_sleep_timespec, nullptr);
                    ++attempts;
                }
                else
                {
                    VIS_LOG(vis::LogLevel::WARN, "Error reading file: %d %s",
                            error_code, strerror(error_code));
                }
            }
            // Bytes were read fine, continue until buffer is full
            else
            {
                bytes_left -= static_cast<size_t>(bytes_read);
            }
        }

        // Success fully read entire buffer
        return true;
    }

    return false;
}

vis::MpdAudioSource::~MpdAudioSource()
{
    if (m_mpd_fifo_fd >= 0)
    {
        ::close(m_mpd_fifo_fd);
    }
}
