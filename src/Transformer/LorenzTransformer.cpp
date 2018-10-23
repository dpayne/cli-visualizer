/*
 * LorenzTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/LorenzTransformer.h"
#include "Domain/VisConstants.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <numeric>
#include <thread>

/**
 * This visualizer is based on the lorenz equation. The lorenz rotates on the
 * screen based on the volume of the left and right channels. The colors are
 * calculated by going from the two centers points to the edges of the screen.
 *
 * The volume also controls how large the lorenz grows, higher volumes will
 * generate more points and cause the lorenz to rotate faster.
 *
 * This visualizer looks best at a fairly small font with a large screen.
 */

namespace
{

// The actual value of k_max_rotation_count doesn't matter that much. This is
// just used to reset the rotation count so that the value never overflows.
const double k_max_rotation_count = 1000.0;

const size_t k_max_color_index_for_lorenz = 16;

const size_t k_color_distance_limit = 16384;

// These values were taken through experimentation on what seemed to work best.
const double k_lorenz_h = 0.01;
const double k_lorenz_a = 10.0;
const double k_lorenz_b1 = 7.1429;
const double k_lorenz_b2 = 0.000908845;
const double k_lorenz_c = 8.0 / 3.0;
} // namespace

vis::LorenzTransformer::LorenzTransformer(
    const std::shared_ptr<const vis::Settings> settings,
    const std::string &name)
    : GenericTransformer(name), m_settings{settings},
      m_rotation_count_left{0.0}, m_rotation_count_right{0.0},
      m_max_color_index{k_max_color_index_for_lorenz}
{
}

vis::LorenzTransformer::~LorenzTransformer() = default;

void vis::LorenzTransformer::execute_mono(pcm_stereo_sample *buffer,
                                          vis::NcursesWriter *writer)
{
    // TODO(dpayne): do something different for mono
    execute_stereo(buffer, writer);
}

void vis::LorenzTransformer::execute_stereo(pcm_stereo_sample *buffer,
                                            vis::NcursesWriter *writer)
{
    const auto win_height = NcursesUtils::get_window_height();
    const auto half_height = win_height / 2;
    const auto win_width = NcursesUtils::get_window_width();

    const auto samples = m_settings->get_sample_size();

    m_rotation_count_left = m_rotation_count_left >= k_max_rotation_count
                                ? 0
                                : m_rotation_count_left;
    m_rotation_count_right = m_rotation_count_right >= k_max_rotation_count
                                 ? 0
                                 : m_rotation_count_right;

    auto average_left = 0.0;
    auto average_right = 0.0;
    for (auto i = 0u; i < samples; ++i)
    {
        average_left += std::abs(buffer[i].l);
        average_right += std::abs(buffer[i].r);
    }

    average_left = average_left / samples * 2.0;
    average_right = average_right / samples;

    const auto rotation_interval_left =
        (average_left * (m_settings->get_fps() / 65536.0));
    const auto rotation_interval_right =
        (average_right * (m_settings->get_fps() / 65536.0));

    const auto overridden_scaling_multiplier =
        m_settings->get_scaling_multiplier();
    const auto average =
        overridden_scaling_multiplier * (average_left + average_right) / 2.0;

    // lorenz_b will range from 11.7 to 64.4. Below 10 the lorenz is pretty much
    // just a disc and after 64.4 the size increases dramatically.
    // The equation was generated using linear curve fitting
    // http://www.wolframalpha.com/input/?i=quadratic+fit+%7B10%2C1%7D%2C%7B18000%2C32%7D%2C%7B45000%2C48%7D%2C%7B65536%2C64.4%7D
    const auto lorenz_b = k_lorenz_b1 + k_lorenz_b2 * average;

    // Calculate the center of the lorenz. Described here under the heading
    // Equilibria http://www.me.rochester.edu/courses/ME406/webexamp5/loreq.pdf
    const auto z_center = -1 + lorenz_b;
    const auto equilbria = std::sqrt(k_lorenz_c * lorenz_b - k_lorenz_c);

    // Calculate the scaling factor. The bounds for the complete lorenz are
    // given here http://www.me.rochester.edu/courses/ME406/webexamp5/loreq.pdf
    // Approximately the first 100 points of the lorenz are usually outliers
    // from the main body of the lorenz, so multiplying by 1.25
    // adjusts the bounds so that the main body takes up most of the height of
    // the screen.
    // Only consider max y coordinate since both max z and max x will be much
    // smaller than the max y.
    const auto scaling_multiplier =
        1.25 * (half_height) /
        std::sqrt((k_lorenz_c * lorenz_b * lorenz_b) -
                  (std::pow(z_center - lorenz_b, 2) / std::pow(lorenz_b, 2)));

    // Calculate the horizontal and vertical rotation angles. This is dependent
    // on the volume of the left and right channels.
    auto rotation_angle_x = (m_rotation_count_left * 2.0 * VisConstants::k_pi) /
                            k_max_rotation_count;
    auto rotation_angle_y =
        (m_rotation_count_right * 2.0 * VisConstants::k_pi) /
        k_max_rotation_count;

    auto deg_multiplier_cos_x = std::cos(rotation_angle_x);
    auto deg_multiplier_sin_x = std::sin(rotation_angle_x);

    auto deg_multiplier_cos_y = std::cos(rotation_angle_y);
    auto deg_multiplier_sin_y = std::sin(rotation_angle_y);

    auto x = 0.0;
    auto y = 0.0;
    auto z = 0.0;

    auto x0 = 0.1;
    auto y0 = 0.0;
    auto z0 = 0.0;

    writer->clear();

    // k_max_color_index_for_lorenz was chosen mostly through experimentation on
    // what seemed to work well.
    recalculate_colors(m_max_color_index, m_settings->get_colors(),
                       &m_precomputed_colors, writer);

    std::wstring msg{m_settings->get_lorenz_character()};
    for (auto i = 0u; i < samples; ++i)
    {
        auto x1 = x0 + k_lorenz_h * k_lorenz_a * (y0 - x0);
        auto y1 = y0 + k_lorenz_h * (x0 * (lorenz_b - z0) - y0);
        auto z1 = z0 + k_lorenz_h * (x0 * y0 - k_lorenz_c * z0);
        x0 = x1;
        y0 = y1;
        z0 = z1;

        // color points based on distance from equiliria. This must be done
        // before rotation
        auto distance_p1 =
            std::sqrt(std::pow(x0 - equilbria, 2) +
                      std::pow(y0 - equilbria, 2) + std::pow(z0 - z_center, 2));
        auto distance_p2 =
            std::sqrt(std::pow(x0 + equilbria, 2) +
                      std::pow(y0 + equilbria, 2) + std::pow(z0 - z_center, 2));
        auto color_distance =
            static_cast<size_t>(std::min(distance_p1, distance_p2));

        if (color_distance > m_max_color_index)
        {
            m_max_color_index =
                std::min(k_color_distance_limit, color_distance);
            recalculate_colors(m_max_color_index, m_settings->get_colors(),
                               &m_precomputed_colors, writer);
        }

        // We want to rotate around the center of the lorenz. so we offset zaxis
        // so that the center of the lorenz is at point (0,0,0)
        x = x0;
        y = y0;
        z = z0 - z_center;

        // Rotate around X and Y axis.
        auto xRxy = x * deg_multiplier_cos_y + z * deg_multiplier_sin_y;
        auto yRxy = x * deg_multiplier_sin_x * deg_multiplier_sin_y +
                    y * deg_multiplier_cos_x -
                    z * deg_multiplier_cos_y * deg_multiplier_sin_x;

        x = xRxy * scaling_multiplier;
        y = yRxy * scaling_multiplier;

        // Throw out any points outside the window
        if (y > (half_height * -1) && y < half_height &&
            x > ((static_cast<double>(win_width) / 2.0) * -1) &&
            x < (static_cast<double>(win_width) / 2.0))
        {
            // skip the first 100 since values under 100 stick out too much from
            // the reset of the points
            if (i > 100)
            {
                writer->write(static_cast<int32_t>(y + half_height),
                              static_cast<int32_t>(x + win_width / 2.0),
                              m_precomputed_colors[color_distance], msg,
                              m_settings->get_lorenz_character());
            }
        }
    }

    writer->flush();

    m_rotation_count_left += rotation_interval_left;
    m_rotation_count_right += rotation_interval_right;
}
