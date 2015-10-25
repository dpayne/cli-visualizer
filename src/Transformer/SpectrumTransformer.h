/*
 * SpectrumTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _SPECTRUM_TRANSFORMER_H
#define _SPECTRUM_TRANSFORMER_H

#include <fftw3.h>
#include "Transformer/GenericTransformer.h"
#include "Domain/Settings.h"

namespace vis
{

class SpectrumTransformer : public GenericTransformer
{
  public:
    explicit SpectrumTransformer(const Settings *const settings);

    virtual ~SpectrumTransformer();

    virtual void execute(pcm_stereo_sample *buffer) override;

  private:
    const Settings *const m_settings;
    size_t m_fftw_results;
    double *m_fftw_input;
    fftw_complex *m_fftw_output;
    fftw_plan m_fftw_plan;

    std::vector<double> m_freq_magnitudes;
};
}

#endif
