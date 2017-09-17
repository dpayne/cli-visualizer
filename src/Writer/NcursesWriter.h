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

namespace vis
{

class NcursesWriter
{
  public:
    explicit NcursesWriter();

    NcursesWriter(const NcursesWriter &) = delete;

    NcursesWriter(const NcursesWriter &&) = delete;

    NcursesWriter &operator=(const NcursesWriter &) = delete;

    NcursesWriter &operator=(const NcursesWriter &&) = delete;

    virtual ~NcursesWriter();

    virtual void write(int32_t row, int32_t column, ColorDefinition color,
                       const std::wstring &msg, wchar_t character);

    virtual void clear();

    /**
     * A scaling function for coloring. For numbers 0 to max this function
     * returns a coloring from the lowest color to the highest. If "wrap" is set
     * to true, colors will not loop from 0 to max. If "wrap" is set to true
     * then colors will wrap.
     */
    virtual ColorDefinition to_color_pair(int32_t number, int32_t max,
                                          std::vector<ColorDefinition> colors,
                                          bool wrap) const;
    /**
     * Initialize color pairs for ncurses
     */
    void setup_colors(bool is_override_terminal_colors,
                      const std::vector<ColorDefinition> &colors);

    virtual void flush();

  private:
    /**
     * Initialize color pairs for ncurses using a given set of colors
     */
    void setup_color_pairs(bool is_override_terminal_colors,
                           const std::vector<ColorDefinition> &colors);

    /**
     * Write msg to ncurses window using background colors
     */
    void write_background(int32_t height, int32_t width,
                          vis::ColorDefinition color, const std::wstring &msg);

    /**
     * Write msg to ncurses window using foreground colors
     */
    void write_foreground(int32_t height, int32_t width,
                          vis::ColorDefinition color, const std::wstring &msg);
};
} // namespace vis

#endif
