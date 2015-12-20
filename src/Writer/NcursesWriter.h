/*
 * NcursesWriter.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _NCURSES_WRITER_H
#define _NCURSES_WRITER_H

#include <ncurses.h>
#include "Domain/ColorDefinition.h"
#include "Domain/Settings.h"

namespace vis
{

class NcursesWriter
{
  public:
    explicit NcursesWriter(const Settings *const settings);

    virtual ~NcursesWriter();

    virtual void write_info(int32_t height, int32_t width, ColorIndex color,
                       const std::wstring &msg);

    virtual void write_visualizer(int32_t height, int32_t width, ColorIndex color,
                       const std::wstring &msg);

    virtual void clear_info();

    virtual void clear_visualizer();

    virtual ColorIndex to_color(int32_t number, int32_t max,
                                bool wrap = true) const;

    virtual void flush_visualizer();

    virtual void flush_info();

    inline int32_t get_visualizer_window_width()
    {
        return m_visualizer_window_width;
    }

    inline int32_t get_visualizer_window_height()
    {
        return m_visualizer_window_height;
    }

    inline int32_t get_info_window_width()
    {
        return m_info_window_width;
    }

    inline int32_t get_info_window_height()
    {
        return m_info_window_height;
    }

  private:
    const Settings *const m_settings;

    int32_t m_visualizer_window_width;
    int32_t m_visualizer_window_height;

    int32_t m_info_window_width;
    int32_t m_info_window_height;

    WINDOW *m_visualizer_window;
    WINDOW *m_info_window;

    void setup_colors();

    void setup_color(const vis::ColorDefinition &color);

    void write(WINDOW * win, int32_t height, int32_t width, ColorIndex color,
               const std::wstring &msg);

    void clear(WINDOW * win);

    void flush(WINDOW * win);
};
}

#endif
