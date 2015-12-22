/*
 * SpectrumCircleTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Domain/VisConstants.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Transformer/SpectrumCircleTransformer.h"

namespace
{
}

vis::SpectrumCircleTransformer::SpectrumCircleTransformer(const Settings *const settings)
    : SpectrumTransformer(settings), m_settings{settings}
{
}

vis::SpectrumCircleTransformer::~SpectrumCircleTransformer()
{
}


void vis::SpectrumCircleTransformer::draw_bars(const std::vector<double> &bars,
               const std::vector<double> &bars_falloff, int32_t win_height,
               const bool flipped, const std::wstring &bar_row_msg,
               vis::NcursesWriter *writer)
{
    auto x = 0.0;
    auto y = 0.0;

    const auto half_height = win_height / 2;
    const auto win_width = NcursesUtils::get_window_width();

    m_settings->get_fps();

    for (auto i = 0u; i < std::min(bars.size(), bars_falloff.size()); ++i)
    {
        auto distance =
            std::sqrt(std::pow(bars[i], 2) +
                      std::pow(0.0, 2));
        ColorIndex color_distance = writer->to_color(
            static_cast<int32_t>(distance), 16);

        const auto scaling_multiplier =
            1.25 * (half_height);

        // Rotate around X and Y axis.
        x = x * scaling_multiplier;
        y = y * scaling_multiplier;

        for ( auto row_index = 0u; row_index < bars[i]; ++row_index)
        {
            if ( flipped )
            {
                write(static_cast<int32_t>(half_height + y),
                              static_cast<int32_t>(x + win_width / 2.0),
                              color_distance, bar_row_msg, writer);
            }
            else
            {
                write(static_cast<int32_t>(half_height + y),
                              static_cast<int32_t>(x + win_width / 2.0),
                              color_distance, bar_row_msg, writer);
            }
        }
    }
}
