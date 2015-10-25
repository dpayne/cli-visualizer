/*
 * SpectrumTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/SpectrumTransformer.h"
#include <cmath>

vis::SpectrumTransformer::SpectrumTransformer(const Settings *const settings)
    : m_settings{settings}
{
    m_fftw_results =
        (static_cast<size_t>(m_settings->get_sample_size()) / 2) + 1;
    m_freq_magnitudes.resize(m_fftw_results);
    m_fftw_input = static_cast<double *>(
        fftw_malloc(sizeof(double) * m_settings->get_sample_size()));
    m_fftw_output = static_cast<fftw_complex *>(
        fftw_malloc(sizeof(fftw_complex) * m_fftw_results));
    m_fftw_plan =
        fftw_plan_dft_r2c_1d(static_cast<int>(m_settings->get_sample_size()),
                             m_fftw_input, m_fftw_output, FFTW_ESTIMATE);
}

void vis::SpectrumTransformer::execute(pcm_stereo_sample *buffer)
{
    for (unsigned i = 0; i < m_settings->get_sample_size(); ++i)
    {
        m_fftw_input[i] = buffer[i].r + buffer[i].l;
    }

    fftw_execute(m_fftw_plan);

    for (size_t i = 0; i < m_fftw_results; ++i)
    {
        m_freq_magnitudes[i] =
            std::sqrt(m_fftw_output[i][0] * m_fftw_output[i][0] +
                      m_fftw_output[i][1] * m_fftw_output[i][1]);
    }

    // write magnitudes to terminal
    // multiple magnitude by 2e4*height
    // pass to writer
}

vis::SpectrumTransformer::~SpectrumTransformer()
{
    fftw_destroy_plan(m_fftw_plan);
    fftw_free(m_fftw_input);
    fftw_free(m_fftw_output);
}
