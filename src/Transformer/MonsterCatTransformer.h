/*
 * MonsterCatTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _MONSTER_CAT_TRANSFORMER_H
#define _MONSTER_CAT_TRANSFORMER_H

#include <fftw3.h>
#include <functional>
#include "Transformer/GenericTransformer.h"
#include "Writer/NcursesWriter.h"
#include "Domain/Settings.h"

namespace vis
{

enum class ChannelMode
{
    Left = 0,
    Right,
    Both
};

class MonsterCatTransformer : public GenericTransformer
{
  public:
    explicit MonsterCatTransformer(const Settings *const settings);

    ~MonsterCatTransformer() override;

    void execute_mono(pcm_stereo_sample *buffer,
                      vis::NcursesWriter *writer) override;
    void execute_stereo(pcm_stereo_sample *buffer,
                        vis::NcursesWriter *writer) override;
  private:
    const Settings *const m_settings;

    /** --- BEGIN MEMBER VARIABLES --- */

    /** --- BEGIN --- fft calculations vars **/
    size_t m_fftw_results;

    double *m_fftw_input_left;
    double *m_fftw_input_right;

    fftw_complex *m_fftw_output_left;
    fftw_complex *m_fftw_output_right;

    fftw_plan m_fftw_plan_left;
    fftw_plan m_fftw_plan_right;
    /** --- END --- fft calculations vars **/

    /** --- BEGIN --- frequency cutoff calculations vars **/
    std::vector<uint32_t> m_low_cutoff_frequencies;

    std::vector<uint32_t> m_high_cutoff_frequencies;

    std::vector<double> m_frequency_constants_per_bin;

    int32_t m_previous_win_width; // Used to determine if freq cutoffs need to
                                  // be re-calculated
    /** --- END --- frequency cutoff calculations vars **/

    uint64_t m_silent_runs; // Used to determine if the transformer should sleep
                            // and wait for input

    std::vector<double> m_previous_falloff_values; // Holds the previous runs
                                                   // values, this is used to to
                                                   // apply falloff effect for
                                                   // the current run

    std::vector<double> m_previous_max_heights; // Holds the previous runs max
                                                // heights, this is used for
                                                // auto-scaling

    std::vector<double> m_monstercat_smoothing_weights;
    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */
    bool prepare_fft_input(pcm_stereo_sample *buffer, uint32_t sample_size,
                           double *fftw_input, ChannelMode channel_mode);

    virtual std::vector<double>
    create_spectrum_bars(fftw_complex *fftw_output, const size_t fftw_results,
                         const int32_t win_height, const int32_t win_width,
                         const uint32_t number_of_bars);

    std::vector<double>
    generate_bars(const uint32_t number_of_bars,
                  const fftw_complex *fftw_output, const size_t fftw_results,
                  const std::vector<uint32_t> &low_cutoff_frequencies,
                  const std::vector<uint32_t> &high_cutoff_frequencies) const;

    void recalculate_cutoff_frequencies(
        uint32_t number_of_bars, std::vector<uint32_t> *low_cutoff_frequencies,
        std::vector<uint32_t> *high_cutoff_frequencies,
        std::vector<double> *freqconst_per_bin);

    void draw_bars(const std::vector<double> &bars, int32_t win_height,
                   const bool flipped, const std::wstring &bar_row_msg,
                   vis::NcursesWriter *writer);

    void smooth_bars(std::vector<double> &bars);

    std::vector<double> apply_falloff(const std::vector<double> &bars,
                                      std::vector<double> &previous_bars) const;

    void calculate_moving_average_and_std_dev(
        const double new_value, const size_t max_number_of_elements,
        std::vector<double> &old_values, double *moving_average,
        double *std_dev) const;

    void maybe_reset_scaling_window(const double current_max_height,
                                    const size_t max_number_of_elements,
                                    std::vector<double> *values,
                                    double *moving_average, double *std_dev);

    void scale_bars(std::vector<double> &bars,
                                   const int32_t height);

    std::wstring create_bar_row_msg(const wchar_t character, uint32_t bar_width,
                                    uint32_t bar_spacing);
    /** --- END MEMBER FUNCTIONS --- */
};
}

#endif
