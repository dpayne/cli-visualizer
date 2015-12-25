/*
 * GenericTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_GENERIC_TRANSFORMER_H
#define _VIS_GENERIC_TRANSFORMER_H

#include "Writer/NcursesWriter.h"
#include "Domain/VisTypes.h"

namespace vis
{

class GenericTransformer
{
  public:
    explicit GenericTransformer();

    virtual ~GenericTransformer();

    virtual void execute_stereo(pcm_stereo_sample *buffer,
                                vis::NcursesWriter *writer) = 0;
    virtual void execute_mono(pcm_stereo_sample *buffer,
                              vis::NcursesWriter *writer) = 0;

  protected:
    /**
     * Helper method to recalculate colors for a given range. This is used
     * mainly as a performance tweak since re-calculating colors can be
     * somewhat costly if done on every run.
     *
     * Colors are re-calculated if max != precomputed_colors.size()
     */
    virtual void recalculate_colors(const size_t max,
                                    std::vector<ColorIndex> &precomputed_colors,
                                    const NcursesWriter *writer);
};
}

#endif
