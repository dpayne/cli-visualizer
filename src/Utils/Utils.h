/*
 * Utils.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_UTILS_H
#define _VIS_UTILS_H

#include "Domain/VisConstants.h"
#include "Domain/VisTypes.h"

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <algorithm>
#include <sstream>
#include <unordered_map>

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

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

        return default_value;
    }

    /**
     * Helper method for getting a wchar_t value from a unordered map with a
     * default.
     */
    template <class E>
    static inline wchar_t get(const std::unordered_map<E, std::string> &map,
                              const E &key, const wchar_t default_value)
    {
        auto iter = map.find(key);
        if (iter != map.end())
        {
            std::wstring wstr{iter->second.begin(), iter->second.end()};
            if (wstr.empty())
            {
                return '\0';
            }

            return wstr.c_str()[0];
        }

        return default_value;
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

        return default_value;
    }

    /**
     * Helper method for getting a double value from a unordered map with a
     * default.
     */
    template <class E, class V>
    static inline double get(const std::unordered_map<E, V> &map, const E &key,
                             const double default_value)
    {
        auto iter = map.find(key);
        if (iter != map.end())
        {
            return std::stod(iter->second);
        }

        return default_value;
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

        return default_value;
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

        return default_value;
    }

    /**
     * Helper method for getting a int64_t from a unordered map with a default.
     */
    template <class E, class V>
    static inline int64_t get(const std::unordered_map<E, V> &map, const E &key,
                              const int64_t default_value)
    {
        auto iter = map.find(key);
        if (iter != map.end())
        {
            return to_long(iter->second);
        }

        return default_value;
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

    /**
     * Helper method to convert string to long. If string is empty, 0 is
     * returned.
     */
    static inline int64_t to_long(const std::string &str)
    {
        if (str.empty())
        {
            return 0L;
        }

        return std::atoi(str.c_str());
    }

    /**
     * Helper method to convert string to int. If string is empty, 0 is
     * returned.
     */
    static inline int32_t to_int(const std::string &str)
    {
        if (str.empty())
        {
            return 0;
        }

        return std::atoi(str.c_str());
    }

    /**
     * Helper method to convert string to unsigned int. If string is empty, 0 is
     * returned.
     */
    static inline uint32_t to_uint(const std::string &str)
    {
        if (str.empty())
        {
            return 0;
        }

        return static_cast<uint32_t>(std::strtoul(str.c_str(), nullptr, 0));
    }

    /**
     * Helper method to split a string by the delimiter "delim"
     */
    static inline std::vector<std::string> split(const std::string &s,
                                                 char delim)
    {
        std::vector<std::string> result;
        return split(s, delim, result);
    }

    /**
     * Helper method to split a string by the delimiter "delim", contents are
     * put input the vector "elems"
     */
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

    /**
     * Helper method to convert a single hex character to the equivalent integer
     * representation.
     */
    static inline int64_t hex_to_int(char c)
    {
        int64_t hex_number;
        if (c >= '0' && c <= '9')
        {
            hex_number = static_cast<int64_t>(c - '0');
        }
        else
        {
            if (c >= 'a' && c <= 'z')
            {
                c -= 32;
            }

            if (c >= 'A' && c <= 'F')
            {
                hex_number = static_cast<int64_t>(10 + (c - 'A'));
            }
            else
            {
                hex_number = 0;
            }
        }

        return hex_number;
    }

    /**
     * Helper method to convert a hex string to the equivalent integer
     * representation.
     */
    static inline int64_t hex_to_int(const std::string &hex)
    {
        auto decimalValue = 0l;
        for (const auto &elem : hex)
        {
            decimalValue = decimalValue * 16 + hex_to_int(elem);
        }

        return decimalValue;
    }

  private:
    explicit Utils();

    virtual ~Utils();
};
}

#endif
