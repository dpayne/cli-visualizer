/*
 * CircleTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/CircleTransformer.h"
#include <cmath>
#include <iostream>
#include <stdio.h>

vis::CircleTransformer::CircleTransformer(const Settings *const settings)
    : SpectrumTransformer( settings )
{
}

vis::CircleTransformer::~CircleTransformer()
{
}

void vis::CircleTransformer::draw(int32_t win_height,
                                             int32_t win_width, bool flipped,
                                             vis::NcursesWriter *writer)
{
    // cut bandwidth a little to achieve better look
    const auto bins_per_bar =
        (m_fftw_results / static_cast<size_t>(win_width)) * (7.0 / 10);

    double bar_height, top_row_bar_height;
    int32_t bar_bound_height, top_row_bar_bound_height;

    m_freq_magnitudes_top_row.reserve(m_freq_magnitudes.size());

    for (auto column_index = 0; column_index < win_width; ++column_index)
    {
        bar_height = 0;
        top_row_bar_height = 0;
        for (auto j = 0u; j < bins_per_bar; ++j)
        {
            auto freq_magnitude_index =
                static_cast<size_t>((column_index * bins_per_bar) + j);

            bar_height += m_freq_magnitudes[freq_magnitude_index];
            if (j >= m_freq_magnitudes_top_row.size())
            {
                top_row_bar_height = m_freq_magnitudes[freq_magnitude_index];
                m_freq_magnitudes_top_row.push_back(top_row_bar_height);
            }
            else
            {
                m_freq_magnitudes_top_row[freq_magnitude_index] =
                    std::max(m_freq_magnitudes_top_row[freq_magnitude_index] *
                                 k_top_row_decay_rate,
                             m_freq_magnitudes[freq_magnitude_index]);

                top_row_bar_height +=
                    m_freq_magnitudes_top_row[freq_magnitude_index];
            }
        }

        top_row_bar_height = std::max(top_row_bar_height, bar_height);

        bar_bound_height = normalize_height(win_width, win_height, column_index,
                                           bins_per_bar, bar_height);

        top_row_bar_bound_height =
            normalize_height(win_width, win_height, column_index, bins_per_bar,
                            top_row_bar_height);

        auto row_index_sign = flipped ? -1 : 1;
        std::wstring msg{m_settings->get_spectrum_character()};
        for (auto row_index = 0; row_index <= bar_bound_height; ++row_index)
        {
            writer->write(win_height + (row_index_sign * row_index),
                          column_index, writer->to_color(row_index, win_height),
                          msg);
        }

        // write the top row element, it will always be colored the max color.
        writer->write(win_height + (row_index_sign * top_row_bar_bound_height),
                      column_index,
                      writer->to_color(top_row_bar_bound_height, win_height),
                      msg);
    }
}
