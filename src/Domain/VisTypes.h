/*
 * VisTypes.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_TYPES_H
#define _VIS_TYPES_H

namespace vis
{
struct stereo_sample_frame
{
    short l;
    short r;
};

using pcm_stereo_sample = struct stereo_sample_frame;
}
#endif
