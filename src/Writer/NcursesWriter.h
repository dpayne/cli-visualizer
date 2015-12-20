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

    virtual void write(int32_t height, int32_t width, ColorIndex color,
                       const std::wstring &msg);

    virtual void clear();

    virtual ColorIndex to_color(int32_t number, int32_t max,
                                bool wrap = true) const;

    virtual void flush();

    inline int32_t get_window_width()
    {
        return m_window_width;
    }

    inline int32_t get_window_height()
    {
        return m_window_height;
    }

  private:
    const Settings *const m_settings;

    int32_t m_window_width;
    int32_t m_window_height;

    WINDOW *m_window;

    void setup_colors();

    void setup_color(const vis::ColorDefinition &color);
};
}

#endif
