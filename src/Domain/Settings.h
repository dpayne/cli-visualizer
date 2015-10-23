/*
 * Settings.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_SETTINGS_H
#define _VIS_SETTINGS_H

#include <string>
#include <vector>

namespace vis
{

class Settings
{
  public:
    explicit Settings();

    ~Settings();

    std::string get_mpd_fifo_path() const noexcept
    {
        return m_mpd_fifo_path;
    }

    void set_mpd_fifo_path(const std::string &path)
    {
        m_mpd_fifo_path = path;
    }

    std::vector<std::string> get_audio_sources() const noexcept
    {
        return m_audio_sources;
    }

    void set_audio_sources(const std::vector<std::string> &audio_sources)
    {
        m_audio_sources = audio_sources;
    }

  private:
    std::string m_mpd_fifo_path;
    std::vector<std::string> m_audio_sources;
};
}

#endif
