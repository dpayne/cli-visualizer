/*
 * ColorDefinition.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_COLOR_DEFINITION_H
#define _VIS_COLOR_DEFINITION_H

#include <string>
#include <vector>
#include "Domain/VisTypes.h"

namespace vis
{

class ColorDefinition
{
  public:
    ColorDefinition() = delete;

    ColorDefinition(ColorIndex color_index, ColorValue red, ColorValue green,
                    ColorValue blue);

    ColorDefinition(const ColorDefinition &&other) noexcept;

    ColorDefinition(const ColorDefinition &other) noexcept;

    virtual ~ColorDefinition();

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

    ColorDefinition &operator=(const ColorDefinition other) noexcept
    {
        this->m_color_index = other.get_color_index();
        this->m_red = other.get_red();
        this->m_green = other.get_green();
        this->m_blue = other.get_blue();
        return *this;
    }

    ColorDefinition &operator=(ColorDefinition &&other) noexcept
    {
        this->m_color_index = other.get_color_index();
        this->m_red = other.get_red();
        this->m_green = other.get_green();
        this->m_blue = other.get_blue();
        return *this;
    }

  private:
    ColorIndex m_color_index;

    ColorValue m_red;
    ColorValue m_green;
    ColorValue m_blue;
};
}

#endif
