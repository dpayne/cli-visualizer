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

vis::SpectrumCircleTransformer::SpectrumCircleTransformer(
    const Settings *const settings)
    : SpectrumTransformer(settings), m_settings{settings}
{
}

vis::SpectrumCircleTransformer::~SpectrumCircleTransformer()
{
}

void vis::SpectrumCircleTransformer::draw_bars(
    const std::vector<double> &bars, const std::vector<double> &bars_falloff,
    int32_t win_height, const bool flipped, const std::wstring &bar_row_msg,
    vis::NcursesWriter *writer)
{
    const auto half_height = win_height / 2.0;
    const auto half_width = NcursesUtils::get_window_width() / 2.0;

    m_settings->get_fps();

    // TODO(dpayne): add const everywhere and use min of half height or half
    // width
    const auto max_distance =
        std::sqrt(std::pow(half_height, 2) + std::pow(half_height, 2));

    for (auto i = 0u; i < bars.size(); ++i)
    {
        auto rad =
            (static_cast<double>(i) / static_cast<double>(win_height)) * 360.0;

        auto bar_height = std::max(bars_falloff[i], (half_height / 2.0));
        for (auto row_index = 0u; row_index < bar_height; ++row_index)
        {
            auto x = static_cast<int32_t>(std::cos(rad) * row_index);

            auto y = static_cast<int32_t>(std::sin(rad) * row_index);

            const auto distance = std::sqrt(std::pow(x, 2) + std::pow(y, 2));

            // TODO(dpayne): precompute colors
            const ColorIndex color_distance =
                writer->to_color_pair(static_cast<int32_t>(distance),
                                      static_cast<int32_t>(max_distance));

            x += half_height;
            y += half_width;

            // TODO(dpayne): if stereo just use 180 degrees
            if (flipped)
            {
                writer->write(x, y, color_distance, bar_row_msg,
                              m_settings->get_spectrum_character());
            }
            else
            {
                writer->write(x, y, color_distance, bar_row_msg,
                              m_settings->get_spectrum_character());
            }
        }
    }
}
