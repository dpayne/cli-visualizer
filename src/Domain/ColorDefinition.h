/*
 * ColorDefinition.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_COLOR_DEFINITION_H
#define _VIS_COLOR_DEFINITION_H

#include "Domain/VisTypes.h"
#include <string>
#include <vector>

namespace vis
{

class ColorDefinition
{
  public:
    ColorDefinition() = delete;

    ColorDefinition(ColorIndex color_index, ColorValue red, ColorValue green,
                    ColorValue blue);

    ColorDefinition(ColorDefinition &&c) = default;

    ColorDefinition(const ColorDefinition &c) = default;

    ColorDefinition &operator=(const ColorDefinition &other) = default;

    ColorDefinition &operator=(ColorDefinition &&other) = default;

    ~ColorDefinition() = default;

    inline ColorIndex get_color_index() const
    {
        return m_color_index;
    }

    inline ColorValue get_red() const
    {
        return m_red;
    }

    inline ColorValue get_green() const
    {
        return m_green;
    }

    inline ColorValue get_blue() const
    {
        return m_blue;
    }

  private:
    ColorIndex m_color_index;

    ColorValue m_red;
    ColorValue m_green;
    ColorValue m_blue;
};
} // namespace vis

#endif
