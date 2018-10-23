/*
 * SpectrumTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 *
 *     Much of this code was taken or inspired by cava
 * git@github.com:karlstav/cava.git
 *
 */

#include "Transformer/SpectrumTransformer.h"
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

namespace
{
const size_t k_secs_for_autoscaling = 30;
const double k_autoscaling_reset_window_percent = 0.10;
const double k_autoscaling_erase_percent_on_reset = 0.75;
const double k_deviation_amount_to_reset =
    1.0; // amount of deviation needed between short term and long term moving
         // max height averages to trigger an auto scaling reset

const double k_minimum_bar_height = 0.125;
const uint64_t k_max_silent_runs_before_sleep =
    3000ul / VisConstants::k_silent_sleep_milliseconds; // silent for 3 seconds
} // namespace

vis::SpectrumTransformer::SpectrumTransformer(
    const std::shared_ptr<const vis::Settings> settings,
    const std::string &name)
    : GenericTransformer(name), m_settings{settings}, m_fftw_results{0},
      m_fftw_input_left{nullptr}, m_fftw_input_right{nullptr},
      m_fftw_output_left{nullptr}, m_fftw_output_right{nullptr},
      m_fftw_plan_left{nullptr}, m_fftw_plan_right{nullptr},
      m_previous_win_width{0}, m_silent_runs{0u}
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
}

bool vis::SpectrumTransformer::prepare_fft_input(pcm_stereo_sample *buffer,
                                                 uint32_t sample_size,
                                                 double *fftw_input,
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

void vis::SpectrumTransformer::execute(pcm_stereo_sample *buffer,
                                       vis::NcursesWriter *writer,
                                       const bool is_stereo)
{
    const auto win_height = NcursesUtils::get_window_height();
    const auto win_width = NcursesUtils::get_window_width();

    auto right_margin = static_cast<int32_t>(
        m_settings->get_spectrum_right_margin() * win_width);
    auto left_margin = static_cast<int32_t>(
        m_settings->get_spectrum_left_margin() * win_width);

    auto width = win_width - right_margin - left_margin;

    bool is_silent_left = true;
    bool is_silent_right = true;

    if (is_stereo)
    {
        is_silent_left =
            prepare_fft_input(buffer, m_settings->get_sample_size(),
                              m_fftw_input_left, vis::ChannelMode::Left);
        is_silent_right =
            prepare_fft_input(buffer, m_settings->get_sample_size(),
                              m_fftw_input_right, vis::ChannelMode::Right);
    }
    else
    {
        is_silent_left =
            prepare_fft_input(buffer, m_settings->get_sample_size(),
                              m_fftw_input_left, vis::ChannelMode::Both);
    }

    if (!(is_silent_left && is_silent_right))
    {
        m_silent_runs = 0;
    }
    // if there is no sound, do not do any processing and sleep
    else
    {
        ++m_silent_runs;
    }

    if (m_silent_runs < k_max_silent_runs_before_sleep)
    {
        m_fftw_plan_left = fftw_plan_dft_r2c_1d(
            static_cast<int>(m_settings->get_sample_size()), m_fftw_input_left,
            m_fftw_output_left, FFTW_ESTIMATE);

        if (is_stereo)
        {
            m_fftw_plan_right = fftw_plan_dft_r2c_1d(
                static_cast<int>(m_settings->get_sample_size()),
                m_fftw_input_right, m_fftw_output_right, FFTW_ESTIMATE);
        }

        std::wstring bar_row_msg =
            create_bar_row_msg(m_settings->get_spectrum_character(),
                               m_settings->get_spectrum_bar_width());

        auto number_of_bars = std::max(
            static_cast<uint32_t>(std::floor(
                static_cast<uint32_t>(width) /
                (bar_row_msg.size() + m_settings->get_spectrum_bar_spacing()))),
            1u);

        fftw_execute(m_fftw_plan_left);

        if (is_stereo)
        {
            fftw_execute(m_fftw_plan_right);
        }

        auto top_margin = static_cast<int32_t>(
            m_settings->get_spectrum_top_margin() * win_height);

        auto height = win_height;
        height -= top_margin;
        if (is_stereo)
        {
            height = height / 2;
        }

        create_spectrum_bars(m_fftw_output_left, m_fftw_results, height, width,
                             number_of_bars, &m_bars_left,
                             &m_bars_falloff_left);
        create_spectrum_bars(m_fftw_output_right, m_fftw_results, height, width,
                             number_of_bars, &m_bars_right,
                             &m_bars_falloff_right);

        // clear screen before writing
        writer->clear();

        auto max_bar_height = height;
        if (is_stereo)
        {
            ++max_bar_height; // add one so that the spectrums overlap in the
                              // middle
        }

        draw_bars(m_bars_left, m_bars_falloff_left, max_bar_height, true,
                  bar_row_msg, writer);
        draw_bars(m_bars_right, m_bars_falloff_right, max_bar_height, false,
                  bar_row_msg, writer);

        writer->flush();

        fftw_destroy_plan(m_fftw_plan_left);

        if (is_stereo)
        {
            fftw_destroy_plan(m_fftw_plan_right);
        }
    }
    else
    {
        VIS_LOG(vis::LogLevel::DEBUG, "No input, Sleeping for %d milliseconds",
                VisConstants::k_silent_sleep_milliseconds);
        std::this_thread::sleep_for(std::chrono::milliseconds(
            VisConstants::k_silent_sleep_milliseconds));
    }
}

void vis::SpectrumTransformer::execute_stereo(pcm_stereo_sample *buffer,
                                              vis::NcursesWriter *writer)
{
    execute(buffer, writer, true);
}

void vis::SpectrumTransformer::execute_mono(pcm_stereo_sample *buffer,
                                            vis::NcursesWriter *writer)
{
    execute(buffer, writer, false);
}

void vis::SpectrumTransformer::smooth_bars(std::vector<double> *bars)
{
    switch (m_settings->get_spectrum_smoothing_mode())
    {
    case vis::SmoothingMode::None:
        // Do nothing
        break;
    case vis::SmoothingMode::MonsterCat:
        monstercat_smoothing(bars);
        break;
    case vis::SmoothingMode::Sgs:
        sgs_smoothing(bars);
        break;
    }
}

void vis::SpectrumTransformer::sgs_smoothing(std::vector<double> *bars)
{
    auto original_bars = *bars;

    auto smoothing_passes = m_settings->get_sgs_smoothing_passes();
    auto smoothing_points = m_settings->get_sgs_smoothing_points();

    for (auto pass = 0u; pass < smoothing_passes; ++pass)
    {
        auto pivot = static_cast<uint32_t>(std::floor(smoothing_points / 2.0));

        for (auto i = 0u; i < pivot; ++i)
        {
            (*bars)[i] = original_bars[i];
            (*bars)[original_bars.size() - i - 1] =
                original_bars[original_bars.size() - i - 1];
        }

        auto smoothing_constant = 1.0 / (2.0 * pivot + 1.0);
        for (auto i = pivot; i < (original_bars.size() - pivot); ++i)
        {
            auto sum = 0.0;
            for (auto j = 0u; j <= (2 * pivot); ++j)
            {
                sum += (smoothing_constant * original_bars[i + j - pivot]) + j -
                       pivot;
            }
            (*bars)[i] = sum;
        }

        // prepare for next pass
        if (pass < (smoothing_passes - 1))
        {
            original_bars = *bars;
        }
    }
}

void vis::SpectrumTransformer::monstercat_smoothing(std::vector<double> *bars)
{
    auto bars_length = static_cast<int64_t>(bars->size());

    // re-compute weights if needed, this is a performance tweak to computer the
    // smoothing considerably faster
    if (m_monstercat_smoothing_weights.size() != bars->size())
    {
        m_monstercat_smoothing_weights.reserve(bars->size());
        for (auto i = 0u; i < bars->size(); ++i)
        {
            m_monstercat_smoothing_weights[i] =
                std::pow(m_settings->get_monstercat_smoothing_factor(), i);
        }
    }

    // apply monstercat sytle smoothing
    // Since this type of smoothing smoothes the bars around it, doesn't make
    // sense to smooth the first value so skip it.
    for (auto i = 1l; i < bars_length; ++i)
    {
        auto outer_index = static_cast<size_t>(i);

        if ((*bars)[outer_index] < k_minimum_bar_height)
        {
            (*bars)[outer_index] = k_minimum_bar_height;
        }
        else
        {
            for (int64_t j = 0; j < bars_length; ++j)
            {
                if (i != j)
                {
                    const auto index = static_cast<size_t>(j);
                    const auto weighted_value =
                        (*bars)[outer_index] /
                        m_monstercat_smoothing_weights[static_cast<size_t>(
                            std::abs(i - j))];

                    // Note: do not use max here, since it's actually slower.
                    // Separating the assignment from the comparison avoids an
                    // unneeded assignment when (*bars)[index] is the largest
                    // which
                    // is often
                    if ((*bars)[index] < weighted_value)
                    {
                        (*bars)[index] = weighted_value;
                    }
                }
            }
        }
    }
}

void vis::SpectrumTransformer::apply_falloff(
    const std::vector<double> &bars, std::vector<double> *falloff_bars) const
{
    // Screen size has change which means previous falloff values are not valid
    if (falloff_bars->size() != bars.size())
    {
        *falloff_bars = bars;
        return;
    }

    for (auto i = 0u; i < bars.size(); ++i)
    {
        // falloff should always by at least one
        auto falloff_value = std::min(
            (*falloff_bars)[i] * m_settings->get_spectrum_falloff_weight(),
            (*falloff_bars)[i] - 1);

        (*falloff_bars)[i] = std::max(falloff_value, bars[i]);
    }
}

void vis::SpectrumTransformer::calculate_moving_average_and_std_dev(
    const double new_value, const size_t max_number_of_elements,
    std::vector<double> *old_values, double *moving_average,
    double *std_dev) const
{
    if (old_values->size() > max_number_of_elements)
    {
        old_values->erase(old_values->begin());
    }

    old_values->push_back(new_value);

    auto sum = std::accumulate(old_values->begin(), old_values->end(), 0.0);
    *moving_average = sum / old_values->size();

    auto squared_summation = std::inner_product(
        old_values->begin(), old_values->end(), old_values->begin(), 0.0);
    *std_dev = std::sqrt((squared_summation / old_values->size()) -
                         std::pow(*moving_average, 2));
}

void vis::SpectrumTransformer::scale_bars(const int32_t height,
                                          std::vector<double> *bars)
{
    if (bars->empty())
    {
        return;
    }

    const auto max_height_iter = std::max_element(bars->begin(), bars->end());

    // max number of elements to calculate for moving average
    const auto max_number_of_elements = static_cast<size_t>(
        ((k_secs_for_autoscaling * m_settings->get_sampling_frequency()) /
         (static_cast<double>(m_settings->get_sample_size()))) *
        2.0);

    double std_dev = 0.0;
    double moving_average = 0.0;
    calculate_moving_average_and_std_dev(
        *max_height_iter, max_number_of_elements, &m_previous_max_heights,
        &moving_average, &std_dev);

    maybe_reset_scaling_window(*max_height_iter, max_number_of_elements,
                               &m_previous_max_heights, &moving_average,
                               &std_dev);

    auto max_height = moving_average + (2 * std_dev);
    max_height = std::max(max_height, 1.0); // avoid division by zero when
                                            // height is zero, this happens when
                                            // the sound is muted

    for (double &bar : *bars)
    {
        bar = std::min(static_cast<double>(height - 1),
                       ((bar / max_height) * height) - 1);
    }
}

void vis::SpectrumTransformer::maybe_reset_scaling_window(
    const double current_max_height, const size_t max_number_of_elements,
    std::vector<double> *values, double *moving_average, double *std_dev)
{
    const auto reset_window_size =
        (k_autoscaling_reset_window_percent * max_number_of_elements);
    // Current max height is much larger than moving average, so throw away most
    // values re-calculate
    if (static_cast<double>(values->size()) > reset_window_size)
    {
        // get average over scaling window
        auto average_over_reset_window =
            std::accumulate(values->begin(),
                            values->begin() +
                                static_cast<int64_t>(reset_window_size),
                            0.0) /
            reset_window_size;

        // if short term average very different from long term moving average,
        // reset window and re-calculate
        if (std::abs(average_over_reset_window - *moving_average) >
            (k_deviation_amount_to_reset * (*std_dev)))
        {
            values->erase(values->begin(),
                          values->begin() +
                              static_cast<int64_t>(
                                  (static_cast<double>(values->size()) *
                                   k_autoscaling_erase_percent_on_reset)));

            calculate_moving_average_and_std_dev(current_max_height,
                                                 max_number_of_elements, values,
                                                 moving_average, std_dev);
        }
    }
}

std::wstring
vis::SpectrumTransformer::create_bar_row_msg(const wchar_t character,
                                             uint32_t bar_width)
{
    std::wstring bar_row_msg;

    for (auto i = 0u; i < bar_width; ++i)
    {
        bar_row_msg.push_back(character);
    }

    return bar_row_msg;
}

void vis::SpectrumTransformer::create_spectrum_bars(
    fftw_complex *fftw_output, const size_t fftw_results,
    const int32_t win_height, const int32_t win_width,
    const uint32_t number_of_bars, std::vector<double> *bars,
    std::vector<double> *bars_falloff)
{
    // cut off frequencies only have to be re-calculated if number of bars
    // change
    if (m_previous_win_width != win_width)
    {
        recalculate_cutoff_frequencies(
            number_of_bars, &m_low_cutoff_frequencies,
            &m_high_cutoff_frequencies, &m_frequency_constants_per_bin);
        m_previous_win_width = win_width;
    }

    // Separate the frequency spectrum into bars, the number of bars is based on
    // screen width
    generate_bars(number_of_bars, fftw_results, m_low_cutoff_frequencies,
                  m_high_cutoff_frequencies, fftw_output, bars);

    // smoothing
    smooth_bars(bars);

    // scale bars
    scale_bars(win_height, bars);

    // falloff, save values for next falloff run
    apply_falloff(*bars, bars_falloff);
}

void vis::SpectrumTransformer::draw_bars(
    const std::vector<double> &bars, const std::vector<double> &bars_falloff,
    int32_t win_height, const bool flipped, const std::wstring &bar_row_msg,
    vis::NcursesWriter *writer)
{
    recalculate_colors(static_cast<size_t>(win_height),
                       m_settings->get_colors(), &m_precomputed_colors, writer);

    const auto full_win_width = NcursesUtils::get_window_width();
    const auto full_win_height = NcursesUtils::get_window_height();
    auto top_margin = static_cast<int32_t>(
        m_settings->get_spectrum_top_margin() * full_win_height);
    auto bottom_margin = static_cast<int32_t>(
        m_settings->get_spectrum_bottom_margin() * full_win_height);
    auto left_margin = static_cast<int32_t>(
        m_settings->get_spectrum_left_margin() * full_win_width);

    for (auto original_column_index = 0u; original_column_index < bars.size();
         ++original_column_index)
    {
        auto column_index = original_column_index;
        if (m_settings->is_spectrum_reversed())
        {
            column_index =
                static_cast<uint32_t>(bars.size()) - original_column_index - 1;
        }

        auto bar_height = 0.0;

        switch (m_settings->get_spectrum_falloff_mode())
        {
        case vis::FalloffMode::None:
            bar_height = bars[column_index];
            break;
        case vis::FalloffMode::Fill:
            bar_height = bars_falloff[column_index];
            break;
        case vis::FalloffMode::Top:
            bar_height = bars[column_index];
            break;
        }

        bar_height = std::max(0.0, bar_height);

        for (auto row_index = 0; row_index <= static_cast<int32_t>(bar_height);
             ++row_index)
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

            auto column =
                static_cast<int32_t>(original_column_index) *
                static_cast<int32_t>((bar_row_msg.size() +
                                      m_settings->get_spectrum_bar_spacing()));

            writer->write(row_height + top_margin - bottom_margin,
                          column + left_margin,
                          m_precomputed_colors[static_cast<size_t>(row_index)],
                          bar_row_msg, m_settings->get_spectrum_character());
        }

        if (m_settings->get_spectrum_falloff_mode() == vis::FalloffMode::Top)
        {
            auto row_index = static_cast<int32_t>(bars_falloff[column_index]);
            int32_t top_row_height;

            // left channel grows up, right channel grows down
            if (flipped)
            {
                top_row_height = win_height - row_index - 1;
            }
            else
            {
                top_row_height = win_height + row_index - 1;
            }

            if (row_index > 0)
            {
                auto column = static_cast<int32_t>(original_column_index) *
                              static_cast<int32_t>(
                                  (bar_row_msg.size() +
                                   m_settings->get_spectrum_bar_spacing()));

                writer->write(
                    top_row_height + top_margin - bottom_margin,
                    column + left_margin,
                    m_precomputed_colors[static_cast<size_t>(row_index)],
                    bar_row_msg, m_settings->get_spectrum_character());
            }
        }
    }
}

void vis::SpectrumTransformer::recalculate_cutoff_frequencies(
    uint32_t number_of_bars, std::vector<uint32_t> *low_cutoff_frequencies,
    std::vector<uint32_t> *high_cutoff_frequencies,
    std::vector<double> *freqconst_per_bin)
{
    auto freqconst =
        std::log10(
            static_cast<double>(m_settings->get_low_cutoff_frequency()) /
            static_cast<double>(m_settings->get_high_cutoff_frequency())) /
        ((1.0 / (static_cast<double>(number_of_bars) + 1.0)) - 1.0);

    (*low_cutoff_frequencies) = std::vector<uint32_t>(number_of_bars + 1);
    (*high_cutoff_frequencies) = std::vector<uint32_t>(number_of_bars + 1);
    (*freqconst_per_bin) = std::vector<double>(number_of_bars + 1);

    for (auto i = 0u; i <= number_of_bars; ++i)
    {
        (*freqconst_per_bin)[i] =
            static_cast<double>(m_settings->get_high_cutoff_frequency()) *
            std::pow(10.0,
                     (freqconst * -1) +
                         (((i + 1.0) / (number_of_bars + 1.0)) * freqconst));

        auto frequency = (*freqconst_per_bin)[i] /
                         (m_settings->get_sampling_frequency() / 2.0);

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

void vis::SpectrumTransformer::generate_bars(
    const uint32_t number_of_bars, const size_t fftw_results,
    const std::vector<uint32_t> &low_cutoff_frequencies,
    const std::vector<uint32_t> &high_cutoff_frequencies,
    const fftw_complex *fftw_output, std::vector<double> *bars) const
{
    if (bars->size() != number_of_bars)
    {
        bars->resize(number_of_bars, 0.0);
    }

    for (auto i = 0u; i < number_of_bars; ++i)
    {
        double freq_magnitude = 0.0;
        for (auto cutoff_freq = low_cutoff_frequencies[i];
             cutoff_freq <= high_cutoff_frequencies[i] &&
             cutoff_freq < fftw_results;
             ++cutoff_freq)
        {
            freq_magnitude += std::sqrt(
                (fftw_output[cutoff_freq][0] * fftw_output[cutoff_freq][0]) +
                (fftw_output[cutoff_freq][1] * fftw_output[cutoff_freq][1]));
        }

        (*bars)[i] = freq_magnitude / (high_cutoff_frequencies[i] -
                                       low_cutoff_frequencies[i] + 1);

        // boost high frequencies
        (*bars)[i] *= (std::log2(2 + i) * (100.0 / number_of_bars));
        (*bars)[i] = std::pow((*bars)[i], 0.5);
    }
}

vis::SpectrumTransformer::~SpectrumTransformer()
{
    fftw_free(m_fftw_input_left);
    fftw_free(m_fftw_input_right);

    fftw_free(m_fftw_output_left);
    fftw_free(m_fftw_output_right);
}
