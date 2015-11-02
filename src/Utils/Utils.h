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

#include "Domain/VisConstants.h"
#include "Domain/VisTypes.h"

namespace vis
{

class Utils
{

  public:
    /**
     * Returns true if the given string is an integer. Supports signed and
     * unsigned numbers between MIN_LONG and MAX_LONG
     */
    static inline bool is_numeric(const std::string &s)
    {
        char *p;
        std::strtol(s.c_str(), &p, 10);
        return *p == '\0';
    }

    /**
     * Return the current user's home directory. The directory will be return
     * with a '/' at the end.
     *
     * If the user's home directory cannot be found, empty string is returned.
     *
     *  Note: this will only work on *nix environments.
     */
    static inline std::string get_home_directory()
    {
        const char *homedir;

        // If $HOME environment variable is not set, fallback to getting it from
        // getpwuid
        if ((homedir = getenv("HOME")) == nullptr)
        {
            homedir = getpwuid(getuid())->pw_dir;
        }

        std::string homedir_str{homedir};

        // Add the '/' if the home directory was found
        if (!homedir_str.empty())
        {
            homedir_str.push_back('/');
            return homedir_str;
        }
        else
        {
            return std::string{};
        }
    }

    /**
     * lowercase and ascii string.
     *  Note: this will not work on multi-byte unicode strings
     */
    static inline std::string lowercase(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    /**
     *
     * Converts strings to bool's. "1" and "true" are true where "true" can be
     * mixed case, otherwise false is returned.
     *
     */
    static inline bool to_bool(const std::string &s)
    {
        return (s == "1" || lowercase(s) == VisConstants::k_true);
    }

    /**
     * Helper method for getting a value from a unordered map with a default.
     */
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

    /**
     * Helper method for getting a bool value from a unordered map with a
     * default.
     */
    template <class E, class V>
    static inline bool get(const std::unordered_map<E, V> &map, const E &key,
                           const bool default_value)
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

    /**
     * Helper method for getting a uint32_t from a unordered map with a default.
     */
    template <class E, class V>
    static inline uint32_t get(const std::unordered_map<E, V> &map,
                               const E &key, const uint32_t default_value)
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

    /**
     * Helper method for getting a int32_t from a unordered map with a default.
     */
    template <class E, class V>
    static inline int32_t get(const std::unordered_map<E, V> &map, const E &key,
                              const int32_t default_value)
    {
        auto iter = map.find(key);
        if (iter != map.end())
        {
            return to_int(iter->second);
        }
        else
        {
            return default_value;
        }
    }

    /**
     * Split a string by the first delimiter. Note, the delimiter will not be
     * included in either the first or second string in the pair.
     */
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
            elems.second = s.substr(index_of_first_elem + 1, std::string::npos);
        }

        return elems;
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
