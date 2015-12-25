/*
 * EllipseTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Domain/VisConstants.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Transformer/EllipseTransformer.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <numeric>

namespace
{
}

vis::EllipseTransformer::EllipseTransformer(const Settings *const settings)
    : m_settings{settings}
{
}

vis::EllipseTransformer::~EllipseTransformer()
{
}

void vis::EllipseTransformer::execute_mono(pcm_stereo_sample *buffer,
                                           vis::NcursesWriter *writer)
{
    // TODO(dpayne): do something different for mono
    execute_stereo(buffer, writer);
}

void vis::EllipseTransformer::recalculate_colors(const size_t max,
                                                  std::vector<ColorIndex> &precomputed_colors,
                                                  const NcursesWriter *writer)
{
    // Makes the radius of each ring be approximately 2 cells wide.
    const auto radius =
        static_cast<int32_t>(m_settings->get_ellipse_radius() *
                             m_settings->get_color_definitions().size());

    if (precomputed_colors.size() != max)
    {
        precomputed_colors.resize(max, 0);
        for (size_t i = 0u; i < max; ++i)
        {
            precomputed_colors[i] =
                writer->to_color_pair(static_cast<int32_t>(i), radius, true);
        }
    }
}

void vis::EllipseTransformer::execute_stereo(pcm_stereo_sample *buffer,
                                             vis::NcursesWriter *writer)
{
    const auto win_height = NcursesUtils::get_window_height();
    const auto win_width = NcursesUtils::get_window_width();

    const auto left_half_width = win_width / 2;
    const auto right_half_width = win_width - left_half_width;

    const auto top_half_height = win_height / 2;
    const auto bottom_half_height = win_height - top_half_height;

    const auto max_color_index = static_cast<size_t>(std::floor(
        std::sqrt(win_width * win_width + 4 * win_height * win_height)));

    recalculate_colors(max_color_index, m_precomputed_colors, writer);

    writer->clear();

    double x;
    double y;
    std::wstring msg{m_settings->get_ellipse_character()};
    for (auto i = 0ul; i < m_settings->get_sample_size(); ++i)
    {
        x = static_cast<double>(static_cast<double>(buffer[i].l) / 32768.0) *
            (buffer[i].l < 0 ? left_half_width : right_half_width);

        y = static_cast<double>(static_cast<double>(buffer[i].r) / 32768.0) *
            (buffer[i].r < 0 ? top_half_height : bottom_half_height);

        // The arguments to the to_color function roughly follow a circle
        // equation where the center is not centered around (0,0). For example
        // (x - w)^2 + (y-h)+2 = r^2 centers the circle around the point (w,h).
        // Because fonts are not all the same size, this will not always
        // generate a perfect circle, hence the name "ellipse".
        const auto color_index =
            static_cast<uint64_t>(std::floor(std::sqrt(x * x + 4 * y * y)));

        writer->write(top_half_height + static_cast<int32_t>(y),
                      left_half_width + static_cast<int32_t>(x),
                      m_precomputed_colors[color_index], msg, m_settings->get_ellipse_character());
    }

    writer->flush();
}
