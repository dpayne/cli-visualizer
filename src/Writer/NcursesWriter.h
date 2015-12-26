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
                       const std::wstring &msg, const wchar_t character);

    virtual void clear();

    /**
     * A scaling function for coloring. For numbers 0 to max this function
     * returns a coloring from the lowest color to the highest. If "wrap" is set
     * to true, colors will not loop from 0 to max. If "wrap" is set to true
     * then colors will wrap.
     */
    virtual ColorIndex to_color_pair(int32_t number, int32_t max,
                                     bool wrap = true) const;

    virtual void flush();

  private:
    const Settings *const m_settings;

    /**
     * Approximate a RGB color to a ncurses color pair. This idea was originally
     * taken from ruby's rainbow gem https://github.com/sickill/rainbow
     *
     * This is not a true RGB color, just an approximation. This is also
     * dependent on terminal settings. For example, if the terminal colors have
     * all been set to red, then they will still all be red.
     */
    static int16_t to_ansi_color(const int16_t red, const int16_t green,
                                 const int16_t blue);

    /**
     * Initialize color pairs for ncurses
     */
    void setup_colors();

    static int16_t to_ansi_color_domain(const int16_t color);

    /**
     * Write msg to ncurses window using background colors
     */
    void write_background(int32_t height, int32_t width, vis::ColorIndex color,
                          const std::wstring &msg);

    /**
     * Write msg to ncurses window using foreground colors
     */
    void write_foreground(int32_t height, int32_t width, vis::ColorIndex color,
                          const std::wstring &msg);
};
}

#endif
