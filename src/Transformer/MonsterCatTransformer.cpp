/*
 * MonsterCatTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 *
 */

#include "Transformer/MonsterCatTransformer.h"
#include <iostream>
#include <cmath>
#include <iostream>
#include <stdio.h>

vis::MonsterCatTransformer::MonsterCatTransformer(
    const Settings *const settings)
    : SpectrumTransformer(settings)
{
}

vis::MonsterCatTransformer::~MonsterCatTransformer()
{
}

void vis::MonsterCatTransformer::draw(int32_t win_height, int32_t win_width,
                                      bool flipped, vis::NcursesWriter *writer)
{
    // cut bandwidth a little to achieve better look
    const auto bins_per_bar =
        ((m_fftw_results) / static_cast<size_t>(win_width)) * (7.0 / 10);

    double bar_height, top_row_bar_height;

    m_freq_magnitudes_top_row.reserve(m_freq_magnitudes.size());
    std::vector<double> bar_heights(static_cast<size_t>(win_width), 0.0);

    for (auto column_index = 0; column_index < win_width; ++column_index)
    {
        bar_height = 0;
        top_row_bar_height = 0;
        for (auto j = 0u; j < bins_per_bar; ++j)
        {
            auto freq_magnitude_index =
                static_cast<size_t>((column_index * bins_per_bar) + j) +
                20; // TODO: make min freq configurable

            if (j >= m_freq_magnitudes_top_row.size())
            {
                top_row_bar_height += m_freq_magnitudes[freq_magnitude_index];
                m_freq_magnitudes_top_row.push_back(
                    m_freq_magnitudes[freq_magnitude_index]);
            }
            else
            {
                double difference = m_freq_magnitudes_top_row[freq_magnitude_index] - m_freq_magnitudes[freq_magnitude_index];
                m_freq_magnitudes_top_row[freq_magnitude_index] =
                    std::max(m_freq_magnitudes_top_row[freq_magnitude_index] *
                                 std::pow(0.9995, difference),
                             m_freq_magnitudes[freq_magnitude_index]);

                top_row_bar_height +=
                    m_freq_magnitudes_top_row[freq_magnitude_index];
            }

            bar_height += m_freq_magnitudes[freq_magnitude_index];
        }

        bar_heights[static_cast<size_t>(column_index)] =
            std::max(bar_height, top_row_bar_height);
    }

    auto smoothed_bar_heights = smooth(bar_heights);

    for (auto column_index = 0; column_index < win_width; ++column_index)
    {
        int32_t bar_bound_height = normalize_height(
            win_height, bins_per_bar,
            smoothed_bar_heights[static_cast<size_t>(column_index)]);

        auto row_index_sign = flipped ? -1 : 1;
        std::wstring msg{m_settings->get_spectrum_character()};
        for (auto row_index = 0; row_index <= bar_bound_height; ++row_index)
        {
            if ( row_index != 0 )
            {
                writer->write(win_height + (row_index_sign * row_index),
                              column_index, writer->to_color(row_index, win_height),
                              msg);
            }
        }
    }
}

int32_t vis::MonsterCatTransformer::normalize_height(const int32_t win_height,
                                                     const double bins_per_bar,
                                                     const double bar_height)
{
    return std::min(
        static_cast<int32_t>((std::pow(bar_height, 0.5) / bins_per_bar) / 1.75),
        win_height);
}

std::vector<double>
vis::MonsterCatTransformer::smooth(const std::vector<double> &frequencies)
{
    std::vector<double> smoothed_frequencies = frequencies;
    std::vector<double> original_frequencies = frequencies;

    // TODO: make this configurable
    uint32_t smoothing_passes = 1;
    uint32_t smoothing_points = 3;

    for (auto pass = 0u; pass < smoothing_passes; ++pass)
    {
        auto pivot = static_cast<uint32_t>(std::floor(smoothing_points / 2.0));

        for (auto i = 0u; i < pivot; ++i)
        {
            smoothed_frequencies[i] = original_frequencies[i];
            smoothed_frequencies[original_frequencies.size() - i - 1] =
                original_frequencies[original_frequencies.size() - i - 1];
        }

        double smoothing_constant = 1.0 / (2.0 * pivot + 1.0);
        for (auto i = pivot; i < (original_frequencies.size() - pivot); ++i)
        {
            auto sum = 0.0;
            for (auto j = 0u; j <= (2 * pivot); ++j)
            {
                sum +=
                    (smoothing_constant * original_frequencies[i + j - pivot]) +
                    j - pivot;
            }
            smoothed_frequencies[i] = sum;
        }

        // prepare for next pass
        original_frequencies = smoothed_frequencies;
    }

    return smoothed_frequencies;
}
