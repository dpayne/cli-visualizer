/*
 * NcursesWriter.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _NCURSES_WRITER_H
#define _NCURSES_WRITER_H

#ifdef NCURSESW
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

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

    void write_background(int32_t height, int32_t width, vis::ColorIndex color,
                          const std::wstring &msg);

    virtual void clear();

    virtual ColorIndex to_color_pair(int32_t number, int32_t max,
                                     bool wrap = true) const;

    virtual void flush();

    static int16_t to_ansi_color(const int16_t red, const int16_t green,
                                 const int16_t blue);

    int16_t to_color_rainbow(int32_t number, int32_t max) const;

  private:
    const Settings *const m_settings;

    void setup_colors();

    static int16_t to_ansi_color_domain(const int16_t color);
};
}

#endif
