/*
 * ConfigurationUtils.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_CONFIGURATION_UTILS_H
#define _VIS_CONFIGURATION_UTILS_H

#include <locale>
#include <unordered_map>
#include <memory>

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"

namespace vis
{

class ConfigurationUtils
{
  public:
    static void load_settings(const std::shared_ptr<Settings> settings,
                              const std::string &config_path,
                              const std::locale &loc);

    static void load_settings(const std::shared_ptr<Settings> settings,
                              const std::locale &loc);

    static void load_color_settings(const std::shared_ptr<Settings> setttings);

    static void load_color_settings_from_color_scheme(
        const std::string &color_scheme,
        const std::shared_ptr<Settings> settings);

  private:
    static std::unordered_map<std::string, std::wstring>
    read_config(const std::string &config_path, const std::locale &loc);

    static void setup_default_colors(const std::shared_ptr<Settings> settings);

    static vis::SmoothingMode read_smoothing_mode(
        const std::unordered_map<std::string, std::wstring> &properties,
        const std::string &config_param,
        const vis::SmoothingMode default_smoothing_mode);

    static vis::FalloffMode read_falloff_mode(
        const std::unordered_map<std::string, std::wstring> &properties,
        const std::string &config_param,
        const vis::FalloffMode default_falloff_mode);

    static void add_color_gradients(const vis::ColorDefinition color,
                                    const double gradient_interval,
                                    std::vector<vis::ColorDefinition> &colors);

    static std::vector<vis::ColorDefinition>
    read_colors(const std::string &colors_path);

    explicit ConfigurationUtils();

    virtual ~ConfigurationUtils();
};
}

#endif
