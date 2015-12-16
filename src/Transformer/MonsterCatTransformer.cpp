/*
 * MonsterCatTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 *
 *     A lot of this code was originally taken from ncmpcpp's spectrum
 * visualizer https://github.com/arybczak/ncmpcpp/blob/master/src/visualizer.cpp
 */

#include "Utils/Logger.h"
#include "Transformer/MonsterCatTransformer.h"
#include <climits>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <thread>

namespace
{
static const double kMinimumBarHeight = 0.125;
static const uint64_t kSilentSleepMilliSeconds = 100;
static const uint64_t kMaxSilentRunsBeforeSleep =
    3000ul / kSilentSleepMilliSeconds; // silent for 3 seconds
}

vis::MonsterCatTransformer::MonsterCatTransformer(
    const Settings *const settings)
    : m_settings{settings}, m_silent_runs{0u}, m_previous_win_width{0}
{
    m_fftw_results =
        (static_cast<size_t>(m_settings->get_sample_size()) / 2) + 1;
    m_fftw_input = static_cast<double *>(
        fftw_malloc(sizeof(double) * m_settings->get_sample_size()));
    m_fftw_output = static_cast<fftw_complex *>(
        fftw_malloc(sizeof(fftw_complex) * m_fftw_results));
    m_fftw_plan =
        fftw_plan_dft_r2c_1d(static_cast<int>(m_settings->get_sample_size()),
                             m_fftw_input, m_fftw_output, FFTW_ESTIMATE);
}

void vis::MonsterCatTransformer::execute_stereo(pcm_stereo_sample *buffer,
                                                vis::NcursesWriter *writer)
{
    const auto win_height = get_window_height();
    const auto win_width = get_window_width();

    const auto half_height = win_height / 2;

    // copy left channel samples to fftw input array
    for (auto i = 0u; i < m_settings->get_sample_size(); ++i)
    {
        m_fftw_input[i] = buffer[i].l;
    }

    execute_fftw_plan();

    // clear screen before writing
    writer->clear();
    draw(half_height, win_width, true, writer);

    // copy right channel samples to fftw input array
    for (auto i = 0u; i < m_settings->get_sample_size(); ++i)
    {
        m_fftw_input[i] = buffer[i].r;
    }

    execute_fftw_plan();

    draw(half_height, win_width, false, writer);

    writer->flush();
}

void vis::MonsterCatTransformer::execute_mono(pcm_stereo_sample *buffer,
                                              vis::NcursesWriter *writer)
{
    const auto win_height = get_window_height();
    const auto win_width = get_window_width();

    bool is_silent = true;

    // copy left channel samples to fftw input array
    for (auto i = 0u; i < m_settings->get_sample_size(); ++i)
    {
        m_fftw_input[i] = buffer[i].l + buffer[i].r;

        if (is_silent && m_fftw_input[i] > 0)
        {
            is_silent = false;
        }
    }

    if (!is_silent)
    {
        m_silent_runs = 0;
    }
    // if there is no sound, do not do any processing and sleep
    else
    {
        ++m_silent_runs;
    }

    if (m_silent_runs < kMaxSilentRunsBeforeSleep)
    {
        execute_fftw_plan();

        // clear screen before writing
        writer->clear();
        draw(win_height, win_width, true, writer);

        writer->flush();
    }
    else
    {
        VIS_LOG(vis::LogLevel::DEBUG, "No input, Sleeping for %d milliseconds",
                kSilentSleepMilliSeconds);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(kSilentSleepMilliSeconds));
    }
}

std::vector<double> vis::MonsterCatTransformer::smooth_bars_pre_falloff(
    const std::vector<double> &bars) const
{
    // apply monstercat sytle smoothing
    std::vector<double> smoothed_bars = bars;
    for (auto i = 0u; i < bars.size(); ++i)
    {
        if (smoothed_bars[i] < kMinimumBarHeight)
        {
            smoothed_bars[i] = kMinimumBarHeight;
        }

        for (auto j = (i - 1); i != 0u && j != 0u; --j)
        {
            smoothed_bars[j] = std::max(
                smoothed_bars[i] / std::pow(1.5, (i - j)), smoothed_bars[j]);
        }

        for ( auto j = (i + 1); j < bars.size(); ++j)
        {
            smoothed_bars[j] = std::max(
                smoothed_bars[i] / std::pow(1.5, (i - j)), smoothed_bars[j]);
        }
    }

    return smoothed_bars;
}

std::vector<double> vis::MonsterCatTransformer::smooth_bars_post_falloff(
    const std::vector<double> &bars) const
{
    return bars;
}

std::vector<double>
vis::MonsterCatTransformer::apply_falloff(const std::vector<double> &bars) const
{
    return bars;
}

std::vector<double>
vis::MonsterCatTransformer::scale_bars(const std::vector<double> &bars,
                                       const int32_t height) const
{
    const auto max_height_iter = std::max_element(bars.begin(), bars.end());
    static double max_height = 0.0;
    max_height = std::max(max_height, *max_height_iter);

    std::vector<double> scaled_bars(bars.size());

    for (auto i = 0u; i < bars.size(); ++i)
    {
        scaled_bars[i] = std::min(static_cast<double>(height),
                                  ((bars[i] / max_height) * height) - 1);
    }

    return scaled_bars;
}

std::wstring vis::MonsterCatTransformer::create_bar_row_msg(
    const wchar_t character, uint32_t bar_width, uint32_t bar_spacing)
{
    std::wstring bar_row_msg;

    for ( auto i = 0u; i < bar_width; ++i )
    {
       bar_row_msg.push_back(character);
    }

    for ( auto i = 0u; i < bar_spacing; ++i )
    {
       bar_row_msg.push_back(' ');
    }

    return bar_row_msg;
}

void vis::MonsterCatTransformer::draw(int32_t win_height, int32_t win_width,
                                      bool flipped, vis::NcursesWriter *writer)
{
    std::wstring bar_row_msg = create_bar_row_msg(m_settings->get_monstercat_character(), m_settings->get_monstercat_bar_width(), m_settings->get_monstercat_bar_spacing());
    uint32_t number_of_bars = static_cast<uint32_t>(std::floor(static_cast<uint32_t>(win_width) / bar_row_msg.size()));

    // cut off frequencies only have to be re-calculated if number of bars
    // change
    if (m_previous_win_width != win_width)
    {
        recalculate_cutoff_frequencies(number_of_bars, &m_low_cutoff_frequencies,
                                       &m_high_cutoff_frequencies);
        m_previous_win_width = win_width;
    }

    // Separate the frequency spectrum into bars, the number of bars is based on
    // screen width
    auto raw_bars =
        generate_bars(number_of_bars, m_fftw_output, m_fftw_results,
                      m_low_cutoff_frequencies, m_high_cutoff_frequencies);

    // pre-falloff smoothing
    auto pre_falloff_smoothed_bars = smooth_bars_pre_falloff(raw_bars);

    // falloff
    auto falloff_bars = apply_falloff(pre_falloff_smoothed_bars);

    // post-falloff smoothing
    auto post_falloff_smoothed_bars = smooth_bars_post_falloff(falloff_bars);

    // scale bars
    auto scaled_bars = scale_bars(post_falloff_smoothed_bars, win_height);

    draw_bars(scaled_bars, win_height, number_of_bars, flipped, bar_row_msg, writer);
}

void vis::MonsterCatTransformer::draw_bars(const std::vector<double> &bars,
                                           int32_t win_height,
                                           uint32_t number_of_bars,
                                           const bool flipped,
                                           const std::wstring &bar_row_msg,
                                           vis::NcursesWriter *writer)
{
    for (auto column_index = 0u; column_index < number_of_bars; ++column_index)
    {
        for (auto row_index = 0; row_index <= static_cast<int32_t>(bars[column_index]); ++row_index)
        {
            int32_t row_height;
            if (flipped)
            {
                row_height = win_height - row_index - 1;
            }
            else
            {
                row_height = win_height + row_index - 1;
            }
            writer->write(row_height, static_cast<int32_t>(column_index) * static_cast<int32_t>(bar_row_msg.size()),
                          writer->to_color(row_index, win_height), bar_row_msg);
        }
    }
}

void vis::MonsterCatTransformer::recalculate_cutoff_frequencies(
    uint32_t number_of_bars, std::vector<uint32_t> *low_cutoff_frequencies,
    std::vector<uint32_t> *high_cutoff_frequencies)
{
    auto freqconst =
        std::log10(
            static_cast<double>(m_settings->get_low_cutoff_frequency()) /
            static_cast<double>(m_settings->get_high_cutoff_frequency())) /
        ((1.0 / (static_cast<double>(number_of_bars) + 1.0)) - 1.0);

    std::vector<double> freqconst_per_bin(number_of_bars);

    (*low_cutoff_frequencies) = std::vector<uint32_t>(number_of_bars + 1);
    (*high_cutoff_frequencies) = std::vector<uint32_t>(number_of_bars + 1);

    for (auto i = 0u; i < number_of_bars; ++i)
    {
        freqconst_per_bin[i] =
            static_cast<double>(m_settings->get_high_cutoff_frequency()) *
            std::pow(10.0,
                     (freqconst * -1) +
                         (((i + 1.0) / (number_of_bars + 1.0)) * freqconst));

        auto frequency =
            freqconst_per_bin[i] / (m_settings->get_sampling_frequency() / 2.0);

        (*low_cutoff_frequencies)[i] =
            static_cast<uint32_t>(std::floor(frequency *
            (static_cast<double>(m_settings->get_sample_size()) / 4.0)));

        if (i > 0)
        {
            if ((*low_cutoff_frequencies)[i] <=
                (*low_cutoff_frequencies)[i - 1])
            {
                (*low_cutoff_frequencies)[i] =
                    (*low_cutoff_frequencies)[i - 1] + 1;
            }
            (*high_cutoff_frequencies)[i - 1] =
                (*low_cutoff_frequencies)[i - 1];
        }
    }
}

std::vector<double> vis::MonsterCatTransformer::generate_bars(
    const uint32_t number_of_bars, const fftw_complex *fftw_output,
    const size_t fftw_results,
    const std::vector<uint32_t> &low_cutoff_frequencies,
    const std::vector<uint32_t> &high_cutoff_frequencies) const
{
    std::vector<double> result_freq_magnitudes(fftw_results);

    for (auto i = 0u; i < number_of_bars; ++i)
    {
        double freq_magnitude = 0.0;
        for (auto cutoff_freq = low_cutoff_frequencies[i];
             cutoff_freq <= high_cutoff_frequencies[i]; ++cutoff_freq)
        {
            freq_magnitude +=
                std::sqrt((fftw_output[i][0] * fftw_output[i][0]) +
                          (fftw_output[i][1] * fftw_output[i][1]));
        }

        // TODO: multiply by sensitivity to get better scaling
        result_freq_magnitudes[i] =
            freq_magnitude /
            (high_cutoff_frequencies[i] - low_cutoff_frequencies[i] + 1);
    }

    return result_freq_magnitudes;
}

void vis::MonsterCatTransformer::execute_fftw_plan()
{
    fftw_execute(m_fftw_plan);
}

vis::MonsterCatTransformer::~MonsterCatTransformer()
{
    fftw_destroy_plan(m_fftw_plan);
    fftw_free(m_fftw_input);
    fftw_free(m_fftw_output);
}
