/*
 * MonsterCatTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 *
 *     This visualizer is an attempt to duplicate monstercat visualizer
 *
 *     Much of this code was taken or inspired by cava
 * git@github.com:karlstav/cava.git
 */

#include "Utils/Logger.h"
#include "Transformer/MonsterCatTransformer.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <numeric>

namespace
{
static const size_t kSecsForAutoscaling = 30;
static const double kAutoScalingResetWindowPercent = 0.10;
static const double kAutoScalingErasePercentOnReset = 0.75;
static const double kDeviationAmountToReset =
    1.0; // amount of deviation needed between short term and long term moving
         // max height averages to trigger an auto scaling reset

static const double kMinimumBarHeight = 0.125;
static const uint64_t kSilentSleepMilliSeconds = 100;
static const uint64_t kMaxSilentRunsBeforeSleep =
    3000ul / kSilentSleepMilliSeconds; // silent for 3 seconds
}

vis::MonsterCatTransformer::MonsterCatTransformer(
    const Settings *const settings)
    : m_settings{settings}, m_previous_win_width{0}, m_silent_runs{0u}
{
    m_fftw_results =
        (static_cast<size_t>(m_settings->get_sample_size()) / 2) + 1;

    m_fftw_input_left = static_cast<double *>(
        fftw_malloc(sizeof(double) * m_settings->get_sample_size()));
    m_fftw_input_right = static_cast<double *>(
        fftw_malloc(sizeof(double) * m_settings->get_sample_size()));

    m_fftw_output_left = static_cast<fftw_complex *>(
        fftw_malloc(sizeof(fftw_complex) * m_fftw_results));
    m_fftw_output_right = static_cast<fftw_complex *>(
        fftw_malloc(sizeof(fftw_complex) * m_fftw_results));

    m_fftw_plan_left = fftw_plan_dft_r2c_1d(
        static_cast<int>(m_settings->get_sample_size()), m_fftw_input_left,
        m_fftw_output_left, FFTW_ESTIMATE);
    m_fftw_plan_right = fftw_plan_dft_r2c_1d(
        static_cast<int>(m_settings->get_sample_size()), m_fftw_input_right,
        m_fftw_output_right, FFTW_ESTIMATE);
}

bool vis::MonsterCatTransformer::prepare_fft_input(
    pcm_stereo_sample *buffer, uint32_t sample_size, double *fftw_input,
    ChannelMode channel_mode)
{
    bool is_silent = true;

    for (auto i = 0u; i < sample_size; ++i)
    {
        switch (channel_mode)
        {
        case ChannelMode::Left:
            fftw_input[i] = buffer[i].l;
            break;
        case ChannelMode::Right:
            fftw_input[i] = buffer[i].r;
            break;
        case ChannelMode::Both:
            fftw_input[i] = buffer[i].l + buffer[i].r;
            break;
        }

        if (is_silent && fftw_input[i] > 0)
        {
            is_silent = false;
        }
    }

    return is_silent;
}

void vis::MonsterCatTransformer::execute_stereo(pcm_stereo_sample *buffer,
                                                vis::NcursesWriter *writer)
{
    const auto win_height = get_window_height();
    const auto win_width = get_window_width();

    bool is_silent_left =
        prepare_fft_input(buffer, m_settings->get_sample_size(),
                          m_fftw_input_left, vis::ChannelMode::Left);
    bool is_silent_right =
        prepare_fft_input(buffer, m_settings->get_sample_size(),
                          m_fftw_input_right, vis::ChannelMode::Right);

    if (!(is_silent_left && is_silent_right))
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
        std::wstring bar_row_msg =
            create_bar_row_msg(m_settings->get_monstercat_character(),
                               m_settings->get_monstercat_bar_width(),
                               m_settings->get_monstercat_bar_spacing());
        uint32_t number_of_bars = static_cast<uint32_t>(
            std::floor(static_cast<uint32_t>(win_width) / bar_row_msg.size()));

        fftw_execute(m_fftw_plan_left);
        fftw_execute(m_fftw_plan_right);

        const auto half_height = win_height / 2;
        const auto bars_left =
            create_spectrum_bars(m_fftw_output_left, m_fftw_results,
                                 half_height, win_width, number_of_bars);
        const auto bars_right =
            create_spectrum_bars(m_fftw_output_right, m_fftw_results,
                                 half_height, win_width, number_of_bars);

        // clear screen before writing
        writer->clear();

        draw_bars(bars_left, half_height + 1, true, bar_row_msg, writer);
        draw_bars(bars_right, half_height + 1, false, bar_row_msg, writer);

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

void vis::MonsterCatTransformer::execute_mono(pcm_stereo_sample *buffer,
                                              vis::NcursesWriter *writer)
{
    const auto win_height = get_window_height();
    const auto win_width = get_window_width();

    bool is_silent =
        prepare_fft_input(buffer, m_settings->get_sample_size(),
                          m_fftw_input_left, vis::ChannelMode::Both);

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
        std::wstring bar_row_msg =
            create_bar_row_msg(m_settings->get_monstercat_character(),
                               m_settings->get_monstercat_bar_width(),
                               m_settings->get_monstercat_bar_spacing());
        uint32_t number_of_bars = static_cast<uint32_t>(
            std::floor(static_cast<uint32_t>(win_width) / bar_row_msg.size()));
        fftw_execute(m_fftw_plan_left);
        const auto bars =
            create_spectrum_bars(m_fftw_output_left, m_fftw_results, win_height,
                                 win_width, number_of_bars);

        // clear screen before writing
        writer->clear();
        draw_bars(bars, win_height, true, bar_row_msg, writer);
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

void
vis::MonsterCatTransformer::smooth_bars(std::vector<double> &bars) const
{
    // apply monstercat sytle smoothing
    int64_t bars_length = static_cast<int64_t>(bars.size());
    for (int64_t i = 1; i < bars_length; ++i)
    {
        if (bars[static_cast<size_t>(i)] < kMinimumBarHeight)
        {
            bars[static_cast<size_t>(i)] = kMinimumBarHeight;
        }

        for (int64_t j = std::max((i - static_cast<int64_t>(100)), static_cast<int64_t>(0));
             j < std::min(bars_length, i + 100); ++j)
        {
            if (i != j)
            {
                const double weight = std::pow(1.5, std::abs(i-j));
                const size_t index = static_cast<size_t>(j);
                bars[index] = std::max(bars[index] / weight, bars[index]);
            }
        }
    }
}

std::vector<double> vis::MonsterCatTransformer::apply_falloff(
    const std::vector<double> &bars, std::vector<double> &previous_bars) const
{
    // Screen size has change which means previous falloff values are not valid
    if (previous_bars.size() != bars.size())
    {
        return bars;
    }
    else
    {
        for (auto i = 0u; i < bars.size(); ++i)
        {
            auto difference = std::abs(previous_bars[i] - bars[i]);
            previous_bars[i] = std::max(
                previous_bars[i] * std::pow(0.99, difference), bars[i]);
        }

        return previous_bars;
    }
}

void vis::MonsterCatTransformer::calculate_moving_average_and_std_dev(
    const double new_value, const size_t max_number_of_elements,
    std::vector<double> &old_values, double *moving_average,
    double *std_dev) const
{
    if (old_values.size() > max_number_of_elements)
    {
        old_values.erase(old_values.begin());
    }

    old_values.push_back(new_value);

    auto sum = std::accumulate(old_values.begin(), old_values.end(), 0.0);
    *moving_average = sum / old_values.size();

    auto squared_summation = std::inner_product(
        old_values.begin(), old_values.end(), old_values.begin(), 0.0);
    *std_dev = std::sqrt((squared_summation / old_values.size()) -
                         std::pow(*moving_average, 2));
}

void vis::MonsterCatTransformer::scale_bars(std::vector<double> &bars,
                                       const int32_t height)
{
    const auto max_height_iter = std::max_element(bars.begin(), bars.end());

    // max number of elements to calculate for moving average
    const auto max_number_of_elements = static_cast<size_t>(
        ((kSecsForAutoscaling * m_settings->get_sampling_frequency()) /
         (static_cast<double>(m_settings->get_sample_size()))) *
        2.0);

    double std_dev = 0.0;
    double moving_average = 0.0;
    calculate_moving_average_and_std_dev(
        *max_height_iter, max_number_of_elements, m_previous_max_heights,
        &moving_average, &std_dev);

    maybe_reset_scaling_window(
        *max_height_iter, max_number_of_elements, &m_previous_max_heights,
        &moving_average, &std_dev);

    auto max_height = moving_average + (2 * std_dev);

    for (auto i = 0u; i < bars.size(); ++i)
    {
        bars[i] = std::min(static_cast<double>(height - 1),
                                  ((bars[i] / max_height) * height) - 1);
    }
}

void vis::MonsterCatTransformer::maybe_reset_scaling_window(
    const double current_max_height, const size_t max_number_of_elements,
    std::vector<double> *values, double *moving_average, double *std_dev)
{
    const auto reset_window_size =
        (kAutoScalingResetWindowPercent * max_number_of_elements);
    // Current max height is much larger than moving average, so throw away most
    // values re-calculate
    if (static_cast<double>(values->size()) > reset_window_size)
    {
        // get average over scaling window
        auto average_over_reset_window =
            std::accumulate(values->begin(),
                            values->begin() + static_cast<int64_t>(reset_window_size), 0.0) /
            reset_window_size;

        // if short term average very different from long term moving average,
        // reset window and re-calculate
        if (std::abs(average_over_reset_window - *moving_average) >
            (kDeviationAmountToReset * (*std_dev)))
        {
            values->erase(
                values->begin(),
                values->begin() +
                    static_cast<long>((static_cast<double>(values->size()) *
                                       kAutoScalingErasePercentOnReset)));

            calculate_moving_average_and_std_dev(
                current_max_height, max_number_of_elements, *values,
                moving_average, std_dev);
        }
    }
}

std::wstring vis::MonsterCatTransformer::create_bar_row_msg(
    const wchar_t character, uint32_t bar_width, uint32_t bar_spacing)
{
    std::wstring bar_row_msg;

    for (auto i = 0u; i < bar_width; ++i)
    {
        bar_row_msg.push_back(character);
    }

    for (auto i = 0u; i < bar_spacing; ++i)
    {
        bar_row_msg.push_back(' ');
    }

    return bar_row_msg;
}

std::vector<double> vis::MonsterCatTransformer::create_spectrum_bars(
    fftw_complex *fftw_output, const size_t fftw_results,
    const int32_t win_height, const int32_t win_width,
    const uint32_t number_of_bars)
{
    // cut off frequencies only have to be re-calculated if number of bars
    // change
    if (m_previous_win_width != win_width)
    {
        recalculate_cutoff_frequencies(number_of_bars,
                                       &m_low_cutoff_frequencies,
                                       &m_high_cutoff_frequencies);
        m_previous_win_width = win_width;
    }

    // Separate the frequency spectrum into bars, the number of bars is based on
    // screen width
    auto bars =
        generate_bars(number_of_bars, fftw_output, fftw_results,
                      m_low_cutoff_frequencies, m_high_cutoff_frequencies);

    // smoothing
    smooth_bars(bars);

    // scale bars
    scale_bars(bars, win_height);

    // falloff, save values for next falloff run
    m_previous_falloff_values =
        apply_falloff(bars, m_previous_falloff_values);

    return m_previous_falloff_values;
}

void vis::MonsterCatTransformer::draw_bars(const std::vector<double> &bars,
                                           int32_t win_height,
                                           const bool flipped,
                                           const std::wstring &bar_row_msg,
                                           vis::NcursesWriter *writer)
{
    for (auto column_index = 0u; column_index < bars.size(); ++column_index)
    {
        for (auto row_index = 0;
             row_index <= static_cast<int32_t>(bars[column_index]); ++row_index)
        {
            if (bars[column_index] > 0)
            {
                int32_t row_height;

                // left channel grows up, right channel grows down
                if (flipped)
                {
                    row_height = win_height - row_index - 1;
                }
                else
                {
                    row_height = win_height + row_index - 1;
                }

                writer->write(
                    row_height, static_cast<int32_t>(column_index) *
                                    static_cast<int32_t>(bar_row_msg.size()),
                    writer->to_color(row_index, win_height), bar_row_msg);
            }
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

        (*low_cutoff_frequencies)[i] = static_cast<uint32_t>(std::floor(
            frequency *
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

        result_freq_magnitudes[i] =
            freq_magnitude /
            (high_cutoff_frequencies[i] - low_cutoff_frequencies[i] + 1);
    }

    return result_freq_magnitudes;
}

vis::MonsterCatTransformer::~MonsterCatTransformer()
{
    fftw_destroy_plan(m_fftw_plan_left);
    fftw_destroy_plan(m_fftw_plan_right);
    fftw_free(m_fftw_input_left);
    fftw_free(m_fftw_input_right);
    fftw_free(m_fftw_output_left);
    fftw_free(m_fftw_output_right);
}
