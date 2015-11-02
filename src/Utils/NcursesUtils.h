/*
 * NcursesUtils.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_NCURSES_UTILS_H
#define _VIS_NCURSES_UTILS_H

#include <map>
#include <ncurses.h>

#include "Domain/VisConstants.h"
#include "Domain/VisTypes.h"
#include "Utils/Logger.h"

namespace
{

const static std::map<std::string, short> color_map{
    {"black", COLOR_BLACK},     {"blue", COLOR_BLUE},     {"cyan", COLOR_CYAN},
    {"green", COLOR_GREEN},     {"yellow", COLOR_YELLOW}, {"red", COLOR_RED},
    {"magenta", COLOR_MAGENTA}, {"white", COLOR_WHITE},
};
}

namespace vis
{

class NcursesUtils
{

  public:
    static inline std::map<std::string, short> get_default_color_map()
    {
        return color_map;
    }

    static inline vis::ColorIndex to_color_index(const std::string &str)
    {
        if (str.empty())
        {
            return COLOR_BLACK;
        }

        auto iter = color_map.find(str);
        if (iter != color_map.end())
        {
            VIS_LOG(vis::LogLevel::ERROR, "%d", iter->second);

            return iter->second;
        }

        return static_cast<vis::ColorIndex>(std::atoi(str.c_str()));
    }

  private:
    explicit NcursesUtils();

    virtual ~NcursesUtils();
};
}

#endif
