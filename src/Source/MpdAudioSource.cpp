/*
 * MpdAudioSource.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Source/MpdAudioSource.h"
#include "Utils/Logger.h"
#include <string.h>

vis::MpdAudioSource::MpdAudioSource(const Settings *const settings)
    : m_settings{settings}
{
    // open mpd file
    m_fifo_stream.open(m_settings->get_mpd_fifo_path(), std::ifstream::in);
}

bool vis::MpdAudioSource::read(pcm_stereo_sample *buffer, uint32_t buffer_size)
{
    if (m_fifo_stream.is_open() && m_fifo_stream.good() && !m_fifo_stream.eof())
    {
        // read buffer
        m_fifo_stream.read(reinterpret_cast<char *>(buffer),
                           buffer_size * sizeof(pcm_stereo_sample));

        //Could not read the buffer size, so zero out memory
        if ( m_fifo_stream.fail() )
        {
            //zero out buffer
            memset(buffer, 0, sizeof(pcm_stereo_sample) * m_settings->get_sample_size());
        }

        if (m_fifo_stream.good() && !m_fifo_stream.eof())
        {
            return true;
        }
    }

    return false;
}

vis::MpdAudioSource::~MpdAudioSource()
{
    if (m_fifo_stream.is_open())
    {
        m_fifo_stream.close();
    }
}
