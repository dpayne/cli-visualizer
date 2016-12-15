/*
 * ConfigurationUtils.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_CONFIGURATION_UTILS_H
#define _VIS_CONFIGURATION_UTILS_H

#include <unordered_map>
#include <locale>

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"

namespace vis
{

class ConfigurationUtils
{
  public:
    static void load_settings(Settings &settings,
                              const std::string &config_path, const std::locale & loc);

    static void load_settings(Settings &settings, const std::locale & loc);

  private:
    static std::unordered_map<std::string, std::wstring>
    read_config(const std::string &config_path, const std::locale & loc);

    static vis::SmoothingMode read_smoothing_mode(
        const std::unordered_map<std::string, std::wstring> &properties,
        const std::string &config_param,
        const vis::SmoothingMode default_smoothing_mode);

    static vis::FalloffMode read_falloff_mode(
        const std::unordered_map<std::string, std::wstring> &properties,
        const std::string &config_param,
        const vis::FalloffMode default_falloff_mode);

    static std::vector<vis::ColorIndex>
    read_colors(const std::string &colors_path);

    explicit ConfigurationUtils();

    virtual ~ConfigurationUtils();
};
}

#endif
