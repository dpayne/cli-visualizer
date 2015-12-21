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
