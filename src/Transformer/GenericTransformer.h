/*
 * GenericTransformer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_GENERIC_TRANSFORMER_H
#define _VIS_GENERIC_TRANSFORMER_H

#include "Domain/VisTypes.h"

namespace vis
{

class GenericTransformer
{
  public:
    explicit GenericTransformer();

    virtual ~GenericTransformer();

    virtual void execute(pcm_stereo_sample *buffer) = 0;
};
}

#endif
