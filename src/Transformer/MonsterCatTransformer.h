/*
 * MonsterCatTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _MONSTER_CAT_TRANSFORMER_H
#define _MONSTER_CAT_TRANSFORMER_H

#include <fftw3.h>
#include "Transformer/GenericTransformer.h"
#include "Transformer/SpectrumTransformer.h"
#include "Writer/NcursesWriter.h"
#include "Domain/Settings.h"

namespace vis
{

class MonsterCatTransformer : public SpectrumTransformer
{
  public:
    explicit MonsterCatTransformer(const Settings *const settings);

    ~MonsterCatTransformer() override;

  protected:
    void draw(int32_t win_height, int32_t win_width, bool flipped,
                       vis::NcursesWriter *writer) override;

    std::vector<double> smooth(const std::vector<double> & frequencies);

    int32_t normalize_height(const int32_t win_height,
                             const double bins_per_bar,
                             const double bar_height);
};
}

#endif


