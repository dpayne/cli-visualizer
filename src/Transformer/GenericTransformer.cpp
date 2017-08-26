/*
 * GenericTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/GenericTransformer.h"

#include <utility>

vis::GenericTransformer::GenericTransformer(std::string name)
    : m_name{std::move(name)}
{
}

void vis::GenericTransformer::recalculate_colors(
    const size_t max, const std::vector<ColorDefinition> &colors,
    std::vector<ColorDefinition> *precomputed_colors,
    const NcursesWriter *writer)
{
    if (max != precomputed_colors->size())
    {
        precomputed_colors->reserve(max);
        for (size_t i = 0ul; i < max; ++i)
        {
            (*precomputed_colors)[i] =
                writer->to_color_pair(static_cast<int32_t>(i),
                                      static_cast<int32_t>(max), colors, true);
        }
    }
}
