/*
 * SpectrumTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _SPECTRUM_TRANSFORMER_H
#define _SPECTRUM_TRANSFORMER_H

#include "Transformer/GenericTransformer.h"
#include "Domain/Settings.h"

namespace vis
{

class SpectrumTransformer : public GenericTransformer
{
  public:
    explicit SpectrumTransformer(const Settings * const settings);

    virtual ~SpectrumTransformer();

  private:
    const Settings * const m_settings;
};
}

#endif
