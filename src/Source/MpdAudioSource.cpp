/*
 * MpdAudioSource.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Source/MpdAudioSource.h"
#include "Utils/Logger.h"

vis::MpdAudioSource::MpdAudioSource(const Settings *const settings) : m_settings(settings)
{
    // open mpd file
    try
    {
        m_fifo_stream.open(m_settings->get_mpd_fifo_path(), std::ifstream::in);
    }
    catch (std::exception &e)
    {
        VIS_LOG(vis::LogLevel::ERROR, "Could not open mpd fifo file: \"%s\"", m_settings->get_mpd_fifo_path().c_str());
    }
}

bool vis::MpdAudioSource::read(char *buffer, uint32_t buffer_size)
{
    if (m_fifo_stream.is_open() && m_fifo_stream.good() && !m_fifo_stream.eof())
    {
        // read buffer
        m_fifo_stream.read(buffer, buffer_size);

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
