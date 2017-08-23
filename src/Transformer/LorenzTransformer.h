/*
 * LorenzTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _LORENZ_TRANSFORMER_H
#define _LORENZ_TRANSFORMER_H

#include "Domain/Settings.h"
#include "Transformer/GenericTransformer.h"

namespace vis
{

class LorenzTransformer : public GenericTransformer
{
  public:
    explicit LorenzTransformer(const Settings *const settings);

    ~LorenzTransformer() override;

    void execute_mono(pcm_stereo_sample *buffer,
                      vis::NcursesWriter *writer) override;
    void execute_stereo(pcm_stereo_sample *buffer,
                        vis::NcursesWriter *writer) override;

  private:
    /** --- BEGIN MEMBER VARIABLES --- */

    const Settings *const m_settings;

    double m_rotation_count_left;

    double m_rotation_count_right;

    uint64_t m_max_color_index;

    std::vector<vis::ColorDefinition> m_precomputed_colors;

    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */

    /** --- END MEMBER FUNCTIONS --- */
};
}

#endif
