/*
 * SpectrumTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _SPECTRUM_TRANSFORMER_H
#define _SPECTRUM_TRANSFORMER_H

#include <memory>

#include "Domain/Settings.h"
#include "Transformer/GenericTransformer.h"
#include "Writer/NcursesWriter.h"
#include <fftw3.h>

namespace vis
{

class SpectrumTransformer : public GenericTransformer
{
  public:
    explicit SpectrumTransformer(
        const std::shared_ptr<const vis::Settings> settings,
        const std::string &name);

    SpectrumTransformer(const SpectrumTransformer &other) = delete;

    SpectrumTransformer(const SpectrumTransformer &&other) = delete;

    SpectrumTransformer &operator=(const SpectrumTransformer &v) = delete;

    SpectrumTransformer &operator=(SpectrumTransformer &&v) = delete;

    ~SpectrumTransformer() override;

    void execute_mono(pcm_stereo_sample *buffer,
                      vis::NcursesWriter *writer) override;
    void execute_stereo(pcm_stereo_sample *buffer,
                        vis::NcursesWriter *writer) override;

    void clear_colors() override
    {
        m_precomputed_colors.clear();
    }

  private:
    void execute(pcm_stereo_sample *buffer, vis::NcursesWriter *writer,
                 bool is_stereo);
    const std::shared_ptr<const Settings> m_settings;

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

    // Holds the current bar heights after processing, this is done as a member
    // function to avoid memory allocations on every run
    std::vector<double> m_bars_left;
    std::vector<double> m_bars_right;

    // falloff vectors hold the previous runs values, this is used to to apply
    // falloff effect for the current run
    std::vector<double> m_bars_falloff_left;
    std::vector<double> m_bars_falloff_right;

    std::vector<double> m_previous_max_heights;

    // Used by monstercat smoothing to apply weights to a bar's height as
    // determined by it's frequency range
    // Note: this is only re-computed when screen width changes
    std::vector<double> m_monstercat_smoothing_weights;

    // Pre-compute colors calculations to avoid duplicate work
    // Note: this is only re-computed when screen height changes
    std::vector<vis::ColorDefinition> m_precomputed_colors;

    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */

    /**
     * Copies the channel given by "channel_mode" to the fftw_input buffer
     */
    bool prepare_fft_input(pcm_stereo_sample *buffer, uint32_t sample_size,
                           double *fftw_input, ChannelMode channel_mode);

    /**
     * Populates "bars" and "bars_falloff" with the bar heights to be displayed
     */
    virtual void create_spectrum_bars(fftw_complex *fftw_output,
                                      size_t fftw_results, int32_t win_height,
                                      int32_t win_width,
                                      uint32_t number_of_bars,
                                      std::vector<double> *bars,
                                      std::vector<double> *bars_falloff);

    void generate_bars(uint32_t number_of_bars, size_t fftw_results,
                       const std::vector<uint32_t> &low_cutoff_frequencies,
                       const std::vector<uint32_t> &high_cutoff_frequencies,
                       const fftw_complex *fftw_output,
                       std::vector<double> *bars) const;

    void recalculate_cutoff_frequencies(
        uint32_t number_of_bars, std::vector<uint32_t> *low_cutoff_frequencies,
        std::vector<uint32_t> *high_cutoff_frequencies,
        std::vector<double> *freqconst_per_bin);

    /**
     * Applies the smoothing operations based on the settings in m_settings to
     * "bars"
     */
    void smooth_bars(std::vector<double> *bars);

    /**
     * Applies the falloff effect based on the settings in m_settings to "bars".
     * The old falloff from the previous run should be in "falloff_bars". The
     * new falloff values will be updated in-place in "falloff_bars"
     */
    void apply_falloff(const std::vector<double> &bars,
                       std::vector<double> *falloff_bars) const;

    /**
     * Calculates the moving average and the standard deviation for a given
     * range of elements.
     *
     * "new_value" is added to "old_values".
     *
     * If "old_values.size()" >  "max_number_of_elements" the first element of
     * old_values is erased.
     */
    void calculate_moving_average_and_std_dev(double new_value,
                                              size_t max_number_of_elements,
                                              std::vector<double> *old_values,
                                              double *moving_average,
                                              double *std_dev) const;

    /**
     * A long term and short term running average are kept of the max bar
     * heights for each frame. If the short term running average is very
     * different than the long term running average then the scaling size
     * should be reset to better suit the current music.
     *
     * This happens commonly if a there is a new song that is a lot quieter
     * or louder than the previous song.
     */
    void maybe_reset_scaling_window(double current_max_height,
                                    size_t max_number_of_elements,
                                    std::vector<double> *values,
                                    double *moving_average, double *std_dev);

    /**
     * Renders the spectrum bars to the screen.
     */
    virtual void draw_bars(const std::vector<double> &bars,
                           const std::vector<double> &bars_falloff,
                           int32_t win_height, bool flipped,
                           const std::wstring &bar_row_msg,
                           vis::NcursesWriter *writer);

    /**
     * Scaling the given vector of points "bars" to a fit a screen with a window
     * height of "height".
     */
    virtual void scale_bars(int32_t height, std::vector<double> *bars);

    /**
     * Creates the to be used for every section of the bar to be printed. For
     * example if the bar width is set to two, and the bar character to '#'.
     * Then the bar row msg would be "##";
     */
    std::wstring create_bar_row_msg(wchar_t character, uint32_t bar_width);

    /**
     * Savitzky-Golay smoothng. This type of smoothing is usually much faster
     * than monstercat.
     *
     *  https://en.wikipedia.org/wiki/Savitzky%E2%80%93Golay_filter
     */
    void sgs_smoothing(std::vector<double> *bars);

    /**
     * This type of smoothing is inspired by monstercat
     * (https://www.youtube.com/user/MonstercatMedia). The code was largely
     * taken from cava git@github.com:karlstav/cava.git
     */
    void monstercat_smoothing(std::vector<double> *bars);

    /** --- END MEMBER FUNCTIONS --- */
};
} // namespace vis

#endif
