/*
 * Settings.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_SETTINGS_H
#define _VIS_SETTINGS_H

#include "Domain/ColorDefinition.h"
#include <string>
#include <vector>

namespace vis
{

class Settings
{
  public:
    explicit Settings();

    ~Settings();

    uint32_t get_sampling_frequency() const noexcept
    {
        return m_sampling_frequency;
    }

    void set_sampling_frequency(const uint32_t sampling_frequency)
    {
        m_sampling_frequency = sampling_frequency;
    }

    uint32_t get_fps() const noexcept
    {
        return m_fps;
    }

    void set_fps(const uint32_t fps)
    {
        m_fps = fps;
    }

    uint32_t get_sample_size() const noexcept
    {
        return m_sampling_frequency / m_fps;
    }

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

    void set_is_stereo_enabled(bool is_stereo_enabled)
    {
        m_is_stereo_enabled = is_stereo_enabled;
    }

    bool is_stereo_enabled() const noexcept
    {
        return m_is_stereo_enabled;
    }

    void set_is_color_enabled(bool is_color_enabled)
    {
        m_is_color_enabled = is_color_enabled;
    }

    bool is_color_enabled() const noexcept
    {
        return m_is_color_enabled;
    }

    std::vector<vis::ColorIndex> get_colors() const noexcept
    {
        return m_colors;
    }

    void set_colors(const std::vector<vis::ColorIndex> &colors)
    {
        m_colors = colors;
    }

    std::vector<vis::ColorDefinition> get_color_definitions() const noexcept
    {
        return m_color_definitions;
    }

    void set_color_definitions(
        const std::vector<vis::ColorDefinition> &color_definitions)
    {
        m_color_definitions = color_definitions;
    }

  private:
    std::string m_mpd_fifo_path;
    uint32_t m_fps;
    uint32_t m_sampling_frequency;
    bool m_is_stereo_enabled;
    bool m_is_color_enabled;
    std::vector<std::string> m_audio_sources;
    std::vector<vis::ColorDefinition> m_color_definitions;
    std::vector<vis::ColorIndex> m_colors;
};
}

#endif
