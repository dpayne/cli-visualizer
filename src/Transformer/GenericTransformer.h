/*
 * GenericTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_GENERIC_TRANSFORMER_H
#define _VIS_GENERIC_TRANSFORMER_H

#include "Domain/VisTypes.h"
#include "Writer/NcursesWriter.h"

namespace vis
{

class GenericTransformer
{
  public:
    explicit GenericTransformer(std::string name);

    GenericTransformer(const GenericTransformer &other) = delete;

    GenericTransformer(const GenericTransformer &&other) = delete;

    GenericTransformer &operator=(const GenericTransformer &v) = default;

    GenericTransformer &operator=(GenericTransformer &&v) = default;

    virtual ~GenericTransformer() = default;

    virtual void execute_stereo(pcm_stereo_sample *buffer,
                                vis::NcursesWriter *writer) = 0;
    virtual void execute_mono(pcm_stereo_sample *buffer,
                              vis::NcursesWriter *writer) = 0;

    virtual void clear_colors() = 0;

    const inline std::string &get_name()
    {
        return m_name;
    }

  protected:
    std::string m_name;

    /**
     * Helper method to recalculate colors for a given range. This is used
     * mainly as a performance tweak since re-calculating colors can be
     * somewhat costly if done on every run.
     *
     * Colors are re-calculated if max != precomputed_colors.size()
     */
    virtual void
    recalculate_colors(size_t max, const std::vector<ColorDefinition> &colors,
                       std::vector<ColorDefinition> *precomputed_colors,
                       const NcursesWriter *writer);
};
} // namespace vis

#endif
