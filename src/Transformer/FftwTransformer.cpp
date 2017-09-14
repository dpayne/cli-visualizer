/*
 * FftwTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 *
 *     Much of this code was taken or inspired by cava
 * git@github.com:karlstav/cava.git
 *
 */

#include "Transformer/FftwTransformer.h"
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
const uint64_t k_max_silent_runs_before_sleep =
    3000ul / VisConstants::k_silent_sleep_milliseconds; // silent for 3 seconds
}

vis::FftwTransformer::FftwTransformer(
    const std::shared_ptr<const vis::Settings> settings,
    const std::string &name)
    : GenericTransformer(name), m_fftw_results{0}, m_fftw_input_left{nullptr},
      m_fftw_input_right{nullptr}, m_fftw_output_left{nullptr},
      m_fftw_output_right{nullptr}, m_fftw_plan_left{nullptr},
      m_fftw_plan_right{nullptr}, m_settings{settings}, m_silent_runs{0u}
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

bool vis::FftwTransformer::prepare_fft_input(pcm_stereo_sample *buffer,
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

bool vis::FftwTransformer::is_input_available(bool is_stereo,
                                              pcm_stereo_sample *buffer)
{
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

    if (m_silent_runs >= k_max_silent_runs_before_sleep)
    {
        VIS_LOG(vis::LogLevel::DEBUG, "No input, Sleeping for %d milliseconds",
                VisConstants::k_silent_sleep_milliseconds);
        std::this_thread::sleep_for(std::chrono::milliseconds(
            VisConstants::k_silent_sleep_milliseconds));
        return false;
    }

    return true;
}

void vis::FftwTransformer::execute_fftw(bool is_stereo)
{
    m_fftw_plan_left = fftw_plan_dft_r2c_1d(
        static_cast<int>(m_settings->get_sample_size()), m_fftw_input_left,
        m_fftw_output_left, FFTW_ESTIMATE);

    if (is_stereo)
    {
        m_fftw_plan_right = fftw_plan_dft_r2c_1d(
            static_cast<int>(m_settings->get_sample_size()), m_fftw_input_right,
            m_fftw_output_right, FFTW_ESTIMATE);
    }

    fftw_execute(m_fftw_plan_left);

    if (is_stereo)
    {
        fftw_execute(m_fftw_plan_right);
    }
}

void vis::FftwTransformer::destroy_fftw(bool is_stereo)
{

    fftw_destroy_plan(m_fftw_plan_left);

    if (is_stereo)
    {
        fftw_destroy_plan(m_fftw_plan_right);
    }
}

void vis::FftwTransformer::recalculate_cutoff_frequencies(
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

void vis::FftwTransformer::apply_blackman_windowing(uint32_t sample_size,
                                                    double *input)
{
    const auto a0 = (1.0 - 0.16) / 2.0;
    const auto a1 = 0.5;
    const auto a2 = 0.16 / 2.0;
    for (auto i = 0u; i < sample_size; ++i)
    {
        input[i] = input[i] * a0 -
                   a1 * std::cos((2 * M_PI * i) / (sample_size - 1)) +
                   a2 * std::cos((4 * M_PI * i) / (sample_size - 1));
    }
}

void vis::FftwTransformer::apply_windowing(bool is_stereo, uint32_t sample_size,
                                           vis::Windowing windowing_func,
                                           double *input_left,
                                           double *input_right)
{
    // TODO:(dpayne) try other windowing functions like HANNING
    switch (windowing_func)
    {
    case vis::Windowing::Blackman:
        apply_blackman_windowing(sample_size, input_left);

        if (is_stereo)
        {
            apply_blackman_windowing(sample_size, input_right);
        }
        break;
    case vis::Windowing::Hanning:
        break;
    case vis::Windowing::None:
        break;
    }
}

void vis::FftwTransformer::generate_bands(
    const uint32_t number_of_bars, const size_t fftw_results,
    const std::vector<uint32_t> &low_cutoff_frequencies,
    const std::vector<uint32_t> &high_cutoff_frequencies,
    bool boost_high_frequencies, const fftw_complex *fftw_output,
    std::vector<double> *bars) const
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
        if (boost_high_frequencies)
        {
            (*bars)[i] *= (std::log2(2 + i) * (100.0 / number_of_bars));
            (*bars)[i] = std::pow((*bars)[i], 0.5);
        }
    }
}

vis::FftwTransformer::~FftwTransformer()
{
    fftw_free(m_fftw_input_left);
    fftw_free(m_fftw_input_right);

    fftw_free(m_fftw_output_left);
    fftw_free(m_fftw_output_right);
}
