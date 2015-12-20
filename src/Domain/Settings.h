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

    uint32_t get_low_cutoff_frequency() const noexcept
    {
        return m_low_cutoff_frequency;
    }

    void set_low_cutoff_frequency(const uint32_t low_cutoff_frequency)
    {
        m_low_cutoff_frequency = low_cutoff_frequency;
    }

    uint32_t get_high_cutoff_frequency() const noexcept
    {
        return m_high_cutoff_frequency;
    }

    void set_high_cutoff_frequency(const uint32_t high_cutoff_frequency)
    {
        m_high_cutoff_frequency = high_cutoff_frequency;
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

    const std::vector<vis::ColorIndex> &get_colors() const noexcept
    {
        return m_colors;
    }

    void set_colors(const std::vector<vis::ColorIndex> &colors)
    {
        m_colors = colors;
    }

    const std::vector<vis::ColorDefinition> &get_color_definitions() const
        noexcept
    {
        return m_color_definitions;
    }

    void set_color_definitions(
        const std::vector<vis::ColorDefinition> &color_definitions)
    {
        m_color_definitions = color_definitions;
    }

    wchar_t get_spectrum_character() const noexcept
    {
        return m_spectrum_character;
    }

    void set_spectrum_character(wchar_t spectrum_character)
    {
        m_spectrum_character = spectrum_character;
    }

    wchar_t get_ellipse_character() const noexcept
    {
        return m_ellipse_character;
    }

    void set_ellipse_character(wchar_t ellipse_character)
    {
        m_ellipse_character = ellipse_character;
    }

    uint32_t get_ellipse_radius() const noexcept
    {
        return m_ellipse_radius;
    }

    void set_ellipse_radius(uint32_t ellipse_radius)
    {
        m_ellipse_radius = ellipse_radius;
    }

    void set_monstercat_smoothing_factor(double monstercat_smoothing_factor)
    {
        m_monstercat_smoothing_factor = monstercat_smoothing_factor;
    }

    double get_monstercat_smoothing_factor() const noexcept
    {
        return m_monstercat_smoothing_factor;
    }

    void set_sgs_smoothing_points(uint32_t sgs_smoothing_points)
    {
        m_sgs_smoothing_points = sgs_smoothing_points;
    }

    uint32_t get_sgs_smoothing_points() const noexcept
    {
        return m_sgs_smoothing_points;
    }

    void set_sgs_smoothing_passes(uint32_t sgs_smoothing_passes)
    {
        m_sgs_smoothing_passes = sgs_smoothing_passes;
    }

    uint32_t get_sgs_smoothing_passes() const noexcept
    {
        return m_sgs_smoothing_passes;
    }

    uint32_t get_spectrum_bar_width() const noexcept
    {
        return m_spectrum_bar_width;
    }

    void set_spectrum_bar_width(const uint32_t spectrum_bar_width)
    {
        m_spectrum_bar_width = spectrum_bar_width;
    }

    uint32_t get_spectrum_bar_spacing() const noexcept
    {
        return m_spectrum_bar_spacing;
    }

    void set_spectrum_bar_spacing(const uint32_t spectrum_bar_spacing)
    {
        m_spectrum_bar_spacing = spectrum_bar_spacing;
    }

    SmoothingMode get_spectrum_smoothing_mode() const noexcept
    {
        return m_spectrum_smoothing_mode;
    }

    void set_spectrum_smoothing_mode(SmoothingMode spectrum_smoothing_mode)
    {
        m_spectrum_smoothing_mode = spectrum_smoothing_mode;
    }

    void set_spectrum_falloff_mode(const vis::FalloffMode falloff_mode)
    {
        m_spectrum_falloff_mode = falloff_mode;
    }

    vis::FalloffMode get_spectrum_falloff_mode() const noexcept
    {
        return m_spectrum_falloff_mode;
    }

    void set_spectrum_falloff_weight(const double spectrum_falloff_weight)
    {
        m_spectrum_falloff_weight = spectrum_falloff_weight;
    }

    double get_spectrum_falloff_weight() const noexcept
    {
        return m_spectrum_falloff_weight;
    }

  private:
    std::string m_mpd_fifo_path;
    uint32_t m_fps;
    uint32_t m_sampling_frequency;
    uint32_t m_low_cutoff_frequency;
    uint32_t m_high_cutoff_frequency;
    bool m_is_stereo_enabled;
    bool m_is_color_enabled;
    std::vector<std::string> m_audio_sources;
    std::vector<vis::ColorDefinition> m_color_definitions;
    std::vector<vis::ColorIndex> m_colors;
    wchar_t m_spectrum_character;
    wchar_t m_ellipse_character;
    uint32_t m_ellipse_radius;
    uint32_t m_spectrum_bar_width;
    uint32_t m_spectrum_bar_spacing;
    double m_monstercat_smoothing_factor;
    uint32_t m_sgs_smoothing_points;
    uint32_t m_sgs_smoothing_passes;
    SmoothingMode m_spectrum_smoothing_mode;
    FalloffMode m_spectrum_falloff_mode;
    double m_spectrum_falloff_weight;
};
}

#endif
