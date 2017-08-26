/*
 * VisTypes.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_TYPES_H
#define _VIS_TYPES_H

#include <cstdint>

namespace vis
{

// Structure read from mpd's fifo buffer
struct stereo_sample_frame
{
    int16_t l;
    int16_t r;
};

using pcm_stereo_sample = struct stereo_sample_frame;

using ColorIndex = int16_t;
using ColorValue = int16_t;

// Different smoothing modes used in spectrum visualizers
enum class SmoothingMode
{
    None = 0,
    MonsterCat,
    Sgs
};

// Different falloff modes used in spectrum visualizers
enum class FalloffMode
{
    None = 0,
    Fill,
    Top
};

// Channel modes for functions processing audio buffers
enum class ChannelMode
{
    Left = 0,
    Right,
    Both
};
} // namespace vis
#endif
