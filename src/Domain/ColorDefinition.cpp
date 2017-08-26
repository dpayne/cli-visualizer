/*
 * ColorDefinition.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Domain/ColorDefinition.h"
#include "Domain/VisConstants.h"

vis::ColorDefinition::ColorDefinition(ColorIndex color_index, ColorValue red,
                                      ColorValue green, ColorValue blue)
    : m_color_index{color_index}, m_red{red}, m_green{green}, m_blue{blue}
{
}
