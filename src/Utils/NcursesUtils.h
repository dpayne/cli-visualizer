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

#ifdef NCURSESW
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

namespace vis
{

class NcursesUtils
{

  public:
    static inline int32_t get_window_height()
    {
        int32_t width, height;

        // getmaxyx is actually a macro, not a function, so pass it two integers
        getmaxyx(stdscr, height, width);

        return height;
    }

    static inline int32_t get_window_width()
    {
        int32_t width, height;

        // getmaxyx is actually a macro, not a function, so pass it two integers
        getmaxyx(stdscr, height, width);

        return width;
    }

    static inline int32_t get_user_input()
    {
        timeout(0);
        return getch();
    }

  private:
    explicit NcursesUtils();

    virtual ~NcursesUtils();
};
}

#endif
