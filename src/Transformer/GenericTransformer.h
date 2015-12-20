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
    explicit GenericTransformer(const Settings *const settings);

    virtual ~GenericTransformer();

    virtual void execute_stereo(pcm_stereo_sample *buffer,
                                vis::NcursesWriter *writer) = 0;
    virtual void execute_mono(pcm_stereo_sample *buffer,
                              vis::NcursesWriter *writer) = 0;
};
}

#endif
