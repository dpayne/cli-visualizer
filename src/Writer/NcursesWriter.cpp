/*
 * NcursesWriter.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Writer/NcursesWriter.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"

vis::NcursesWriter::NcursesWriter(const vis::Settings *const settings)
    : m_settings{settings}
{
    initscr();
    curs_set(0); // sets the cursor to invisible
    setlocale(LC_ALL, "");

    if (has_colors() == TRUE)
    {
        start_color();        // turns on color
        use_default_colors(); // uses default colors of terminal, which allows
                              // transparency to work

        // initialize color pairs
        setup_colors();
    }
}

void vis::NcursesWriter::setup_colors()
{
    // initialize colors
    for (int16_t i = 0; i < COLORS; ++i)
    {
        init_pair(i, i, -1);

        // initialize colors as background, this is used in write_background to
        // create a
        // full block effect without using a custom font
        init_pair(static_cast<int16_t>(i + COLORS), i, i);
    }
}

void vis::NcursesWriter::write_background(int32_t height, int32_t width,
                                          vis::ColorIndex color,
                                          const std::wstring &msg)
{
    // Add COLORS which will set it to have the color as the background, see
    // "setup_colors"
    auto color_pair = COLOR_PAIR(color + COLORS);
    attron(color_pair);

    mvaddwstr(height, width, msg.c_str());

    attroff(color_pair);
}

void vis::NcursesWriter::write_foreground(int32_t height, int32_t width,
                               vis::ColorIndex color, const std::wstring &msg)
{
    attron(COLOR_PAIR(color));

    mvaddwstr(height, width, msg.c_str());

    attroff(COLOR_PAIR(color));
}

void vis::NcursesWriter::write(const int32_t row, const int32_t column,
                                     const vis::ColorIndex color,
                                     const std::wstring &msg,
                                     const wchar_t character)
{
    // This is a hack to achieve a solid bar look without using a custom font.
    // Instead of writing a real character, set the background to the color and
    // write a space
    if (character == VisConstants::k_space_wchar)
    {
        write_background(row, column, color, msg);
    }
    else
    {
        write_foreground(row, column, color, msg);
    }
}

void vis::NcursesWriter::clear()
{
    standend();
    erase();
}

void vis::NcursesWriter::flush()
{
    refresh();
}

int16_t vis::NcursesWriter::to_ansi_color_domain(const int16_t color)
{
    return static_cast<int16_t>(6.0 * static_cast<double>(color) / 256.0);
}

/**
 * Returns nearest supported 256-color an rgb value as an ansi color value
 * Inspired by the rainbow gem https://github.com/sickill/rainbow
 *
 * This is an approximate result. Actual results depend on what colors the
 * terminal has set.
 */
int16_t vis::NcursesWriter::to_ansi_color(const int16_t red,
                                          const int16_t green,
                                          const int16_t blue)
{
    return 16 + static_cast<int16_t>((to_ansi_color_domain(red) * 36.0) +
                                     (to_ansi_color_domain(green) * 6.0) +
                                     (to_ansi_color_domain(blue) * 1.0));
}

vis::ColorIndex vis::NcursesWriter::to_color_pair(int32_t number, int32_t max,
                                                  bool wrap) const
{
    const auto colors_size = static_cast<vis::ColorIndex>(
        m_settings->get_color_definitions().size());
    const auto index = (number * colors_size) / (max + 1);

    //no colors
    if (colors_size == 0)
    {
        return 0;
    }

    const auto color_definition =
        m_settings->get_color_definitions()[static_cast<size_t>(
            wrap ? index % colors_size : std::min(index, colors_size - 1))];

    return to_ansi_color(color_definition.get_red(),
                         color_definition.get_green(),
                         color_definition.get_blue());
}

vis::NcursesWriter::~NcursesWriter()
{
    endwin();
}
