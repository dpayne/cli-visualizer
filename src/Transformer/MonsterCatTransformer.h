/*
 * MonsterCatTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _MONSTER_CAT_TRANSFORMER_H
#define _MONSTER_CAT_TRANSFORMER_H

#include <fftw3.h>
#include "Transformer/GenericTransformer.h"
#include "Writer/NcursesWriter.h"
#include "Domain/Settings.h"

namespace vis
{

class MonsterCatTransformer : public GenericTransformer
{
  public:
    explicit MonsterCatTransformer(const Settings *const settings);

    ~MonsterCatTransformer() override;

    void execute_mono(pcm_stereo_sample *buffer,
                      vis::NcursesWriter *writer) override;
    void execute_stereo(pcm_stereo_sample *buffer,
                        vis::NcursesWriter *writer) override;

  protected:
    virtual void draw(int32_t win_height, int32_t win_width, bool flipped,
                      vis::NcursesWriter *writer);

  private:
    const Settings *const m_settings;

    size_t m_fftw_results;

    double *m_fftw_input;

    fftw_complex *m_fftw_output;

    fftw_plan m_fftw_plan;

    uint64_t m_silent_runs;

    std::vector<uint32_t> m_low_cutoff_frequencies;

    std::vector<uint32_t> m_high_cutoff_frequencies;

    int32_t m_previous_win_width;

    void execute_fftw_plan();

    std::vector<double>
    generate_bars(const uint32_t number_of_bars,
                  const fftw_complex *fftw_output, const size_t fftw_results,
                  const std::vector<uint32_t> &low_cutoff_frequencies,
                  const std::vector<uint32_t> &high_cutoff_frequencies) const;

    void recalculate_cutoff_frequencies(
        uint32_t number_of_bars, std::vector<uint32_t> *low_cutoff_frequencies,
        std::vector<uint32_t> *high_cutoff_frequencies);

    void draw_bars(const std::vector<double> &bars, int32_t win_height,
                   uint32_t number_of_bars, const bool flipped,
                   const std::wstring &bar_row_msg, vis::NcursesWriter *writer);

    std::vector<double>
    smooth_bars_pre_falloff(const std::vector<double> &bars) const;

    std::vector<double>
    smooth_bars_post_falloff(const std::vector<double> &bars) const;

    std::vector<double> apply_falloff(const std::vector<double> &bars) const;

    std::vector<double> scale_bars(const std::vector<double> &bars,
                                   const int32_t height) const;

    std::wstring create_bar_row_msg( const wchar_t character, uint32_t bar_width, uint32_t bar_spacing);
};
}

#endif
