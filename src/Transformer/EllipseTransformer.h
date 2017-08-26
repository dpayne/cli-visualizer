/*
 * EllipseTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _ELLIPSE_TRANSFORMER_H
#define _ELLIPSE_TRANSFORMER_H

#include <memory>

#include "Domain/Settings.h"
#include "Transformer/GenericTransformer.h"

namespace vis
{

class EllipseTransformer : public GenericTransformer
{
  public:
    explicit EllipseTransformer(
        const std::shared_ptr<const vis::Settings> settings,
        const std::string &name);

    EllipseTransformer(const EllipseTransformer &other) = delete;

    EllipseTransformer(const EllipseTransformer &&other) = delete;

    EllipseTransformer &operator=(const EllipseTransformer &v) = delete;

    EllipseTransformer &operator=(EllipseTransformer &&v) noexcept = delete;

    ~EllipseTransformer() override;

    void execute_mono(pcm_stereo_sample *buffer,
                      vis::NcursesWriter *writer) override;
    void execute_stereo(pcm_stereo_sample *buffer,
                        vis::NcursesWriter *writer) override;

    void clear_colors() override
    {
        m_precomputed_colors.clear();
    }

  protected:
    /**
     * The colors for the ellipse are calculated differently than most
     * other visualizers because the colors might be repeated several
     * timees depending on how many rings there are.
     */
    void recalculate_colors(size_t max,
                            const std::vector<ColorDefinition> &colors,
                            std::vector<ColorDefinition> *precomputed_colors,
                            const NcursesWriter *writer) override;

  private:
    const std::shared_ptr<const Settings> m_settings;

    /** --- BEGIN MEMBER VARIABLES --- */

    // Pre-compute colors calculations to avoid duplicate work
    // Note: this is only re-computed when screen height changes
    std::vector<vis::ColorDefinition> m_precomputed_colors;

    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */

    /** --- END MEMBER FUNCTIONS --- */
};
} // namespace vis

#endif
