/*
 * Utils.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_UTILS_H
#define _VIS_UTILS_H

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <ncurses.h>

#include "Domain/VisConstants.h"
#include "Domain/VisTypes.h"

namespace
{

const static std::unordered_map<std::string, short> color_map{
    {"black", COLOR_BLACK},     {"blue", COLOR_BLUE},     {"cyan", COLOR_CYAN},
    {"green", COLOR_GREEN},     {"yellow", COLOR_YELLOW}, {"red", COLOR_RED},
    {"magenta", COLOR_MAGENTA}, {"white", COLOR_WHITE},
};
}

namespace vis
{

class Utils
{

  public:
    static inline std::string get_home_directory()
    {
        const char *homedir;

        if ((homedir = getenv("HOME")) == nullptr)
        {
            homedir = getpwuid(getuid())->pw_dir;
        }

        std::string homedir_str{homedir};
        homedir_str.push_back('/');

        return homedir_str;
    }

    static inline std::string lowercase(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    static inline bool to_bool(const std::string &s)
    {
        return (s == "1" || lowercase(s) == VisConstants::k_true);
    }

    template <class E, class V>
    static inline const V &get(const std::unordered_map<E, V> &map,
                               const E &key, const V &default_value)
    {
        auto iter = map.find(key);
        if (iter != map.end())
        {
            return iter->second;
        }
        else
        {
            return default_value;
        }
    }

    static inline uint32_t
    get(const std::unordered_map<std::string, std::string> &map,
        const std::string &key, const uint32_t default_value)
    {
        auto iter = map.find(key);
        if (iter != map.end())
        {
            return to_uint(iter->second);
        }
        else
        {
            return default_value;
        }
    }

    static inline bool
    get(const std::unordered_map<std::string, std::string> &map,
        const std::string &key, const bool default_value)
    {
        auto iter = map.find(key);
        if (iter != map.end())
        {
            return to_bool(iter->second);
        }
        else
        {
            return default_value;
        }
    }

    static inline std::pair<std::string, std::string> &
    split_first(const std::string &s, char delim,
                std::pair<std::string, std::string> &elems)
    {
        auto index_of_first_elem = s.find_first_of(delim);

        // delimiter not found
        if (index_of_first_elem == std::string::npos)
        {
            elems.first = s;
            elems.second.clear();
            return elems;
        }

        elems.first = s.substr(0, index_of_first_elem);

        // nothing after the delimiter
        if (index_of_first_elem == (s.size() - 1))
        {
            elems.second.clear();
        }
        else
        {
            elems.second = s.substr(index_of_first_elem, std::string::npos);
        }

        return elems;
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
            return iter->second;
        }

        return static_cast<vis::ColorIndex>(std::atoi(str.c_str()));
    }

    static inline int32_t to_int(const std::string &str)
    {
        if (str.empty())
            return 0;

        return std::atoi(str.c_str());
    }

    static inline uint32_t to_uint(const std::string &str)
    {
        if (str.empty())
            return 0;

        return static_cast<uint32_t>(std::strtoul(str.c_str(), nullptr, 0));
    }

    static inline std::vector<std::string> split(const std::string &s,
                                                 char delim)
    {
        std::vector<std::string> result;
        return split(s, delim, result);
    }

    static inline std::vector<std::string> &
    split(const std::string &s, char delim, std::vector<std::string> &elems)
    {
        std::stringstream ss(s);
        std::string item;

        elems.clear();

        while (std::getline(ss, item, delim))
        {
            elems.push_back(item);
        }

        return elems;
    }

  private:
    explicit Utils();

    virtual ~Utils();
};
}

#endif
