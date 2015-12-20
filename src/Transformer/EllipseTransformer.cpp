/*
 * EllipseTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 *
 *     Much of this code was taken or inspired by cava
 * git@github.com:karlstav/cava.git
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

vis::EllipseTransformer::EllipseTransformer(
    const Settings *const settings)
    : GenericTransformer(settings), m_settings{settings}
{
}

vis::EllipseTransformer::~EllipseTransformer()
{
}

void vis::EllipseTransformer::execute_mono(pcm_stereo_sample *buffer,
                  vis::NcursesWriter *writer)
{
    //TODO: do something different for mono
    execute_stereo(buffer, writer);
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

    // Makes the radius of each ring be approximately 2 cells wide.
    const auto radius = static_cast<int32_t>(2 * m_settings->get_colors().size());

    const auto max_color_index =
        static_cast<int32_t>(std::floor(std::sqrt(win_width * win_width + 4 * win_height * win_height)));
    m_precomputed_colors.reserve(static_cast<size_t>(win_height));
    for (auto i = 0; i < max_color_index; ++i)
    {
        m_precomputed_colors[static_cast<size_t>(i)] =
            writer->to_color(i, radius, true);
    }

    writer->clear();

    int32_t x;
    int32_t y;
    //TODO: change this to ellipse specific char
    std::wstring msg{m_settings->get_ellipse_character()};
    for (auto i = 0ul; i < m_settings->get_sample_size(); ++i)
    {
        x = buffer[i].l / 32768 *
            (buffer[i].l < 0 ? left_half_width : right_half_width);

        y = buffer[i].r / 32768 *
            (buffer[i].r < 0 ? top_half_height : bottom_half_height);

        // The arguments to the to_color function roughly follow a circle
        // equation where
        // the center is not centered around (0,0). For example (x - w)^2 +
        // (y-h)+2 = r^2
        // centers the circle around the point (w,h). Because fonts are not all
        // the same
        // size, this will not always generate a perfect circle.
        const auto color_index = static_cast<uint64_t>(std::floor(std::sqrt(x * x + 4 * y * y )));
        writer->write(
            left_half_width + x, top_half_height + y,
            m_precomputed_colors[color_index],
            msg);
    }

    writer->flush();
}
