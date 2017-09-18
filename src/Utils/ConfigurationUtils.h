/*
 * ConfigurationUtils.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_CONFIGURATION_UTILS_H
#define _VIS_CONFIGURATION_UTILS_H

#include <locale>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"

namespace vis
{

class ConfigurationUtils
{
  public:
    static void load_settings(std::shared_ptr<Settings> settings,
                              const std::string &config_path,
                              const std::locale &loc);

    static void load_settings(std::shared_ptr<Settings> settings,
                              const std::locale &loc);

    static void load_color_settings(std::shared_ptr<Settings> settings);

    static void
    load_color_settings_from_color_scheme(const std::string &color_scheme,
                                          std::shared_ptr<Settings> settings);

  protected:
    static std::unordered_map<std::string, std::wstring>
    read_config(const std::string &config_path, const std::locale &loc);

    static void setup_default_colors(std::shared_ptr<Settings> settings);

    static vis::SmoothingMode read_smoothing_mode(
        const std::unordered_map<std::string, std::wstring> &properties,
        const std::string &config_param,
        vis::SmoothingMode default_smoothing_mode);

    static vis::FalloffMode read_falloff_mode(
        const std::unordered_map<std::string, std::wstring> &properties,
        const std::string &config_param, vis::FalloffMode default_falloff_mode);

    static void add_color_gradients(bool is_override_terminal_colors,
                                    const vis::ColorDefinition &color,
                                    double gradient_interval,
                                    std::vector<vis::ColorDefinition> *colors);

    static std::vector<vis::ColorDefinition>
    read_colors(bool is_override_terminal_colors,
                const std::string &colors_path);

    static void validate_setting_is_not_negative(const double t,
                                                 const std::string &setting);

    static void
    validate_setting_is_greater_than_zero(const double t,
                                          const std::string &setting);

    static double get_gradient_interval(int32_t number_of_colors,
                                        int32_t number_of_colors_supported);

    static std::vector<vis::ColorDefinition>
    read_color_lines(bool is_override_terminal_colors,
                     const std::vector<std::string> &lines);

    static std::vector<vis::ColorDefinition>
    colors_with_gradients(bool is_override_terminal_colors,
                          const std::vector<vis::ColorDefinition> &colors);
};
} // namespace vis

#endif
