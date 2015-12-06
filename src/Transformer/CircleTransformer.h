/*
 * CircleTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _CIRCLE_TRANSFORMER_H
#define _CIRCLE_TRANSFORMER_H

#include <fftw3.h>
#include "Transformer/GenericTransformer.h"
#include "Transformer/SpectrumTransformer.h"
#include "Writer/NcursesWriter.h"
#include "Domain/Settings.h"

namespace vis
{

class CircleTransformer : public SpectrumTransformer
{
  public:
    explicit CircleTransformer(const Settings *const settings);

    ~CircleTransformer() override;

  protected:
    void draw(int32_t win_height, int32_t win_width, bool flipped,
                       vis::NcursesWriter *writer) override;
};
}

#endif

