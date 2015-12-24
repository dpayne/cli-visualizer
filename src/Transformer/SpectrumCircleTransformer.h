/*
 * SpectrumCircleTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _SPECTRUM_CIRCLE_TRANSFORMER_H
#define _SPECTRUM_CIRCLE_TRANSFORMER_H

#include "Transformer/SpectrumTransformer.h"
#include "Domain/Settings.h"

namespace vis
{

class SpectrumCircleTransformer : public SpectrumTransformer
{
  public:
    explicit SpectrumCircleTransformer(const Settings *const settings);

    ~SpectrumCircleTransformer() override;

  private:
    const Settings *const m_settings;

    /** --- BEGIN MEMBER VARIABLES --- */

    // Pre-compute colors calculations to avoid duplicate work
    // Note: this is only re-computed when screen height changes
    std::vector<vis::ColorIndex> m_precomputed_colors;

    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */

    void draw_bars(const std::vector<double> &bars,
                   const std::vector<double> &bars_falloff, int32_t win_height,
                   const bool flipped, const std::wstring &bar_row_msg,
                   vis::NcursesWriter *writer) override;

    /** --- END MEMBER FUNCTIONS --- */
};
}

#endif
