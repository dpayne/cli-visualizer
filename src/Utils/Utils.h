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
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace vis
{

class Utils
{

  public:

    static inline std::string get_home_directory()
    {
        const char *homedir;

        if ((homedir = getenv("HOME")) == NULL)
        {
            homedir = getpwuid(getuid())->pw_dir;
        }

        std::string homedir_str{homedir};
        homedir_str.push_back( '/' );

        return homedir_str;
    }

    template <class E, class V>
    static inline const E &get(const std::unordered_map<E, V> &map,
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

    static inline std::pair<std::string, std::string> &split_first(const std::string &s, char delim,
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

