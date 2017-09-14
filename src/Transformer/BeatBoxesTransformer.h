/*
 * BeatBoxesTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _BEATBOXES_TRANSFORMER_H
#define _BEATBOXES_TRANSFORMER_H

#include <deque>
#include <memory>

#include "Domain/Settings.h"
#include "Transformer/FftwTransformer.h"

namespace vis
{

class BeatBoxesTransformer : public FftwTransformer
{
  public:
    explicit BeatBoxesTransformer(
        const std::shared_ptr<const vis::Settings> settings,
        const std::string &name);

    BeatBoxesTransformer(const BeatBoxesTransformer &other) = delete;

    BeatBoxesTransformer(const BeatBoxesTransformer &&other) = delete;

    BeatBoxesTransformer &operator=(const BeatBoxesTransformer &v) = delete;

    BeatBoxesTransformer &operator=(BeatBoxesTransformer &&v) noexcept = delete;

    ~BeatBoxesTransformer() override;

    void execute_mono(pcm_stereo_sample *buffer,
                      vis::NcursesWriter *writer) override;

    void execute_stereo(pcm_stereo_sample *buffer,
                        vis::NcursesWriter *writer) override;

    void clear_colors() override
    {
        m_precomputed_colors.clear();
    }

  protected:
  private:
    /** --- BEGIN MEMBER VARIABLES --- */

    const std::shared_ptr<const Settings> m_settings;

    int32_t m_previous_number_of_sub_bands; // Used to determine if freq cutoffs
                                            // need to be re-calculated

    // Pre-compute colors calculations to avoid duplicate work
    // Note: this is only re-computed when screen height changes
    std::vector<vis::ColorDefinition> m_precomputed_colors;

    // Holds the values for each sub_band for the last N runs
    std::deque<std::vector<double>> m_sub_bands;

    /** --- END MEMBER VARIABLES --- */

    /** --- BEGIN MEMBER FUNCTIONS --- */
    void execute(pcm_stereo_sample *buffer, vis::NcursesWriter *writer,
                 bool is_stereo);

    void create_sub_bands(fftw_complex *fftw_output, const size_t fftw_results,
                          const uint32_t number_of_bars,
                          std::vector<double> *sub_bands);

    void draw_boxes(int32_t win_height, int32_t win_width, int32_t right_margin,
                    int32_t left_margin, int32_t top_margin,
                    int32_t bottom_margin,
                    const std::vector<double> &current_sub_bands_right,
                    const std::deque<std::vector<double>> &m_sub_bands,
                    vis::NcursesWriter *writer);

    /** --- END MEMBER FUNCTIONS --- */
};
} // namespace vis

#endif
