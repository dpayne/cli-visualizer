/*
 * FftwTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _FFTW_TRANSFORMER_H
#define _FFTW_TRANSFORMER_H

#include <memory>

#include "Domain/Settings.h"
#include "Transformer/GenericTransformer.h"
#include "Writer/NcursesWriter.h"
#include <fftw3.h>

namespace vis
{

class FftwTransformer : public GenericTransformer
{
  public:
    explicit FftwTransformer(
        const std::shared_ptr<const vis::Settings> settings,
        const std::string &name);

    FftwTransformer(const FftwTransformer &other) = delete;

    FftwTransformer(const FftwTransformer &&other) = delete;

    FftwTransformer &operator=(const FftwTransformer &v) = delete;

    FftwTransformer &operator=(FftwTransformer &&v) = delete;

    ~FftwTransformer() override;

  protected:
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

    /** --- END --- frequency cutoff calculations vars **/

    bool is_input_available(bool is_stereo, pcm_stereo_sample *buffer);

    void execute_fftw(bool is_stereo);

    void destroy_fftw(bool is_stereo);

    void generate_bands(uint32_t number_of_bands, size_t fftw_results,
                        const std::vector<uint32_t> &low_cutoff_frequencies,
                        const std::vector<uint32_t> &high_cutoff_frequencies,
                        bool boost_high_frequencies,
                        const fftw_complex *fftw_output,
                        std::vector<double> *bars) const;

    void recalculate_cutoff_frequencies(
        uint32_t number_of_bars, std::vector<uint32_t> *low_cutoff_frequencies,
        std::vector<uint32_t> *high_cutoff_frequencies,
        std::vector<double> *freqconst_per_bin);

    void apply_windowing(bool is_stereo, uint32_t sample_size,
                         vis::Windowing windowing_func, double *input_left,
                         double *input_right);

  private:
    const std::shared_ptr<const Settings> m_settings;

    /** --- BEGIN MEMBER VARIABLES --- */

    uint64_t m_silent_runs; // Used to determine if the transformer should sleep
                            // and wait for input

    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */

    /**
     * Copies the channel given by "channel_mode" to the fftw_input buffer
     */
    bool prepare_fft_input(pcm_stereo_sample *buffer, uint32_t sample_size,
                           double *fftw_input, ChannelMode channel_mode);

    void apply_blackman_windowing(uint32_t sample_size, double *input);

    /** --- END MEMBER FUNCTIONS --- */
};
} // namespace vis

#endif
