/*
 * EllipseTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _ELLIPSE_TRANSFORMER_H
#define _ELLIPSE_TRANSFORMER_H

#include "Transformer/GenericTransformer.h"
#include "Domain/Settings.h"

namespace vis
{

class EllipseTransformer : public GenericTransformer
{
  public:
    explicit EllipseTransformer(const Settings *const settings);

    ~EllipseTransformer() override;

    void execute_mono(pcm_stereo_sample *buffer,
                      vis::NcursesWriter *writer) override;
    void execute_stereo(pcm_stereo_sample *buffer,
                        vis::NcursesWriter *writer) override;

  protected:
    /**
     * The colors for the ellipse are calculated differently than most
     * other visualizers because the colors might be repeated several
     * timees depending on how many rings there are.
     */
    void recalculate_colors(const size_t max,
                            std::vector<ColorIndex> &precomputed_colors,
                            const NcursesWriter *writer) override;

  private:
    const Settings *const m_settings;

    /** --- BEGIN MEMBER VARIABLES --- */

    // Pre-compute colors calculations to avoid duplicate work
    // Note: this is only re-computed when screen height changes
    std::vector<vis::ColorIndex> m_precomputed_colors;

    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */

    /** --- END MEMBER FUNCTIONS --- */
};
}

#endif
