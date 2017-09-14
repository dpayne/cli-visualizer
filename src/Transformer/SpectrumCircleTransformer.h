/*
 * SpectrumCircleTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _SPECTRUM_CIRCLE_TRANSFORMER_H
#define _SPECTRUM_CIRCLE_TRANSFORMER_H

#include <memory>

#include "Domain/Settings.h"
#include "Transformer/SpectrumTransformer.h"

namespace vis
{

class SpectrumCircleTransformer : public SpectrumTransformer
{
  public:
    explicit SpectrumCircleTransformer(
        const std::shared_ptr<const vis::Settings> settings,
        const std::string &name);

    SpectrumCircleTransformer(const SpectrumCircleTransformer &other) = delete;

    SpectrumCircleTransformer(const SpectrumCircleTransformer &&other) = delete;

    SpectrumCircleTransformer &
    operator=(const SpectrumCircleTransformer &v) = delete;

    SpectrumCircleTransformer &
    operator=(SpectrumCircleTransformer &&v) = delete;

    ~SpectrumCircleTransformer() override = default;

    void clear_colors() override
    {
        m_precomputed_colors.clear();
    }

  private:
    const std::shared_ptr<const Settings> m_settings;

    /** --- BEGIN MEMBER VARIABLES --- */

    // Pre-compute colors calculations to avoid duplicate work
    // Note: this is only re-computed when screen height changes
    std::vector<vis::ColorDefinition> m_precomputed_colors;

    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */

    void draw_bars(const std::vector<double> &bars,
                   const std::vector<double> &bars_falloff, int32_t win_height,
                   bool flipped, const std::wstring &bar_row_msg,
                   vis::NcursesWriter *writer) override;

    /** --- END MEMBER FUNCTIONS --- */
};
} // namespace vis

#endif
