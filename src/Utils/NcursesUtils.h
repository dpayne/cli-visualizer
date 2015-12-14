/*
 * NcursesUtils.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_NCURSES_UTILS_H
#define _VIS_NCURSES_UTILS_H

#include "Domain/VisConstants.h"
#include "Domain/VisTypes.h"
#include "Utils/Logger.h"
#include "Utils/Utils.h"

#include <map>

#include <ncurses.h>

namespace vis
{

const static std::map<std::string, int16_t> g_vis_color_map{
    {"black", COLOR_BLACK},     {"blue", COLOR_BLUE},     {"cyan", COLOR_CYAN},
    {"green", COLOR_GREEN},     {"yellow", COLOR_YELLOW}, {"red", COLOR_RED},
    {"magenta", COLOR_MAGENTA}, {"white", COLOR_WHITE},
};


class NcursesUtils
{

  public:
    /**
     * Return the basic color map for ncurses. This includes only 8 colors.
     * black   0
     * red     1
     * green   2
     * yellow  3
     * blue    4
     * magenta 5
     * cyan    6
     * white   7
     */
    static inline const std::map<std::string, int16_t> &get_default_color_map()
    {
        return g_vis_color_map;
    }

    /**
     * Convert a string to a color index. If the string is numeric the string is
     * convert to a number and returned as a ColorIndex. A color can be called
     * by name if it is one of the 8 basic colors:
     * black,red,green,yellow,blue,magenta,cyan,white.
     *
     * If the string is empty or if it is a name of a color that is not one of
     * the 8 basic colors, then black is returned.
     */
    static inline vis::ColorIndex to_color_index(const std::string &str)
    {
        if (str.empty())
        {
            return COLOR_BLACK;
        }

        auto iter = g_vis_color_map.find(vis::Utils::lowercase(str));
        if (iter != g_vis_color_map.end())
        {
            return iter->second;
        }

        if (!vis::Utils::is_numeric(str))
        {
            return COLOR_BLACK;
        }

        return static_cast<vis::ColorIndex>(std::atoi(str.c_str()));
    }

    static inline int32_t get_window_height()
    {
        int32_t width, height;

        //getmaxyx is actually a macro, not a function, so pass it two integers
        getmaxyx( stdscr, height, width );

        return height;
    }

    static inline int32_t get_window_width()
    {
        int32_t width, height;

        //getmaxyx is actually a macro, not a function, so pass it two integers
        getmaxyx( stdscr, height, width );

        return width;
    }

  private:
    explicit NcursesUtils();

    virtual ~NcursesUtils();

};
}

#endif
