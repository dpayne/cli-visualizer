/*
 * BeatBoxesTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/BeatBoxesTransformer.h"
#include "Domain/VisConstants.h"
#include "Domain/VisException.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>

namespace
{
// TODO calculate these based on fps, number of samples should go back at least
// a second
const int32_t kNumberOfSubBands = 32;
const int32_t kNumOfOldSamples = 100;
}

vis::BeatBoxesTransformer::BeatBoxesTransformer(
    const std::shared_ptr<const vis::Settings> settings,
    const std::string &name)
    : FftwTransformer(settings, name), m_settings{settings}
{
}

vis::BeatBoxesTransformer::~BeatBoxesTransformer() = default;

void vis::BeatBoxesTransformer::execute_stereo(pcm_stereo_sample *buffer,
                                               vis::NcursesWriter *writer)
{
    execute(buffer, writer, true);
}

void vis::BeatBoxesTransformer::execute_mono(pcm_stereo_sample *buffer,
                                             vis::NcursesWriter *writer)
{
    execute(buffer, writer, false);
}

void vis::BeatBoxesTransformer::create_sub_bands(
    fftw_complex *fftw_output, const size_t fftw_results,
    const uint32_t number_of_sub_bands, std::vector<double> *sub_bands)
{
    // cut off frequencies only have to be re-calculated if number of sub_bands
    // change
    if (m_previous_number_of_sub_bands != number_of_sub_bands)
    {
        // sub_bands are no longer accurate
        m_sub_bands.clear();

        m_previous_number_of_sub_bands = number_of_sub_bands;

        recalculate_cutoff_frequencies(
            number_of_sub_bands, &m_low_cutoff_frequencies,
            &m_high_cutoff_frequencies, &m_frequency_constants_per_bin);
    }

    // Separate the frequency spectrum into sub_bands
    generate_bands(number_of_sub_bands, fftw_results, m_low_cutoff_frequencies,
                   m_high_cutoff_frequencies, false, fftw_output, sub_bands);
}

void vis::BeatBoxesTransformer::draw_boxes(
    int32_t win_height, int32_t win_width, int32_t right_margin,
    int32_t left_margin, int32_t top_margin, int32_t bottom_margin,
    const std::vector<double> &current_sub_bands,
    const std::deque<std::vector<double>> &previous_sub_bands,
    vis::NcursesWriter *writer)
{
    if (previous_sub_bands.size() < kNumOfOldSamples)
    {
        return;
    }

    // TODO rewrite this so it's in-place
    // would require re-writing generate_bars so that you're adding to each
    // sub_band array instead of a new  vector
    std::vector<double> sub_bands_avg(current_sub_bands.size(), 0.0);
    std::vector<double> sub_bands_stddev(current_sub_bands.size(), 0.0);

    uint32_t sub_band_with_highest_avg_ix = 0;
    double highest_avg = 0;

    for (auto i = 0u; i < current_sub_bands.size(); ++i)
    {
        for (auto j = 0u; j < previous_sub_bands.size(); ++j)
        {
            sub_bands_avg[i] += previous_sub_bands[j][i];
        }

        sub_bands_avg[i] = sub_bands_avg[i] / previous_sub_bands.size();
        if (sub_bands_avg[i] > highest_avg)
        {
            sub_band_with_highest_avg_ix = i;
            highest_avg = sub_bands_avg[i];
        }
    }

    // TODO(dpayne) use variance to throw out noisy values, if variance is high
    // ignore
    for (auto i = 0u; i < current_sub_bands.size(); ++i)
    {
        for (auto j = 0u; j < previous_sub_bands.size(); ++j)
        {
            const auto val = previous_sub_bands[j][i] - sub_bands_avg[i];
            sub_bands_stddev[i] += val * val;
        }

        sub_bands_stddev[i] =
            std::sqrt(sub_bands_stddev[i] / previous_sub_bands.size());
    }

    int32_t num_of_sub_bands_with_beat = 0;
    for (auto i = 0u; i < current_sub_bands.size(); ++i)
    {
        // half rectify (i.e. only take bands which have increased in volume
        // since the previous run)
        if (current_sub_bands[i] >
            previous_sub_bands[previous_sub_bands.size() - 1][i])
        {
            const auto diff = current_sub_bands[i] - sub_bands_avg[i];
            if (diff > (3.0 * sub_bands_stddev[i]))
            {
                ++num_of_sub_bands_with_beat;
                std::cerr << "Beat detected " << diff << " "
                          << sub_bands_stddev[i] << "\t" << sub_bands_avg[i]
                          << "\t" << num_of_sub_bands_with_beat << std::endl;
            }
        }
    }

    if (num_of_sub_bands_with_beat > 0)
    {
        // draw beat
        for (auto i = 0; i < 10; ++i)
        {
            for (auto j = 0; j < 10; ++j)
            {
                writer->write(10 + i, 10 + j,
                              VisConstants::k_default_16_colors[0], L" ", L' ');
            }
        }
    }
}

void vis::BeatBoxesTransformer::execute(pcm_stereo_sample *buffer,
                                        vis::NcursesWriter *writer,
                                        bool is_stereo)
{
    // skip run if there is no sound
    if (!is_input_available(is_stereo, buffer))
    {
        return;
    }

    const auto win_height = NcursesUtils::get_window_height();
    const auto win_width = NcursesUtils::get_window_width();

    // TODO: add settings specific to beat boxes
    const auto right_margin = static_cast<int32_t>(
        m_settings->get_spectrum_right_margin() * win_width);
    const auto left_margin = static_cast<int32_t>(
        m_settings->get_spectrum_left_margin() * win_width);
    const auto top_margin = static_cast<int32_t>(
        m_settings->get_spectrum_top_margin() * win_height);
    const auto bottom_margin = static_cast<int32_t>(
        m_settings->get_spectrum_top_margin() * win_height);

    const auto height = win_height - top_margin - bottom_margin;
    const auto width = win_width - left_margin - right_margin;

    if (width < 0 || height < 0)
    {
        throw vis::VisException("Invalid margin settings for %s",
                                get_name().c_str());
    }

    apply_windowing(is_stereo, m_settings->get_sample_size(),
                    vis::Windowing::Blackman, m_fftw_input_left,
                    m_fftw_input_right);

    execute_fftw(is_stereo);

    uint32_t number_of_sub_bands = kNumberOfSubBands;

    // TODO: (dpayne) recalculate this in place as a single sub_bands vector
    // then possibly use AVX to to sums across historical bands
    std::vector<double> current_sub_bands_left(number_of_sub_bands, 0.0);
    std::vector<double> current_sub_bands_right(number_of_sub_bands, 0.0);

    create_sub_bands(m_fftw_output_left, m_fftw_results, number_of_sub_bands,
                     &current_sub_bands_left);

    create_sub_bands(m_fftw_output_right, m_fftw_results, number_of_sub_bands,
                     &current_sub_bands_right);

    // combine left and right bands into one
    for (auto ix = 0u; ix < current_sub_bands_left.size(); ++ix)
    {
        current_sub_bands_left[ix] += current_sub_bands_right[ix];
    }

    // clear screen before writing
    writer->clear();

    // TODO: calculate BPM
    draw_boxes(win_height, win_width, right_margin, left_margin, top_margin,
               bottom_margin, current_sub_bands_left, m_sub_bands, writer);

    // save current sub_band
    if (m_sub_bands.size() > kNumOfOldSamples)
    {
        m_sub_bands.pop_front();
    }

    m_sub_bands.push_back(current_sub_bands_left);

    // TODO: draw random boxes

    writer->flush();

    destroy_fftw(is_stereo);
}
