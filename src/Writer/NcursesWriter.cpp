/*
 * NcursesWriter.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */
#include <cmath>

#include "Writer/NcursesWriter.h"
#include "Utils/Logger.h"
#include "Domain/VisConstants.h"
#include "Utils/NcursesUtils.h"

#ifdef _LINUX
/* Ncurses version 6.0.20170401 introduced an issue with COLOR_PAIR which broke
 * setting more than 256 color pairs. Specifically it uses an A_COLOR macro
 * which uses a 8 bit mask. This will work for colors since only 256 colors are
 * supported but it breaks color pairs since 2^16 color pairs are supported.
 */
#define VIS_A_COLOR (NCURSES_BITS(((1U) << 16) - 1U, 0))
#define VIS_COLOR_PAIR(n) (NCURSES_BITS((n), 0) & VIS_A_COLOR)
#else
#define VIS_COLOR_PAIR(n) (COLOR_PAIR(n))
#endif

vis::NcursesWriter::NcursesWriter(const vis::Settings *const settings)
    : m_settings{settings}
{
    initscr();
    noecho();    // disable printing of pressed keys
    curs_set(0); // sets the cursor to invisible
    setlocale(LC_ALL, "");

    if (has_colors() == TRUE)
    {
        start_color();        // turns on color
        use_default_colors(); // uses default colors of terminal, which allows
                              // transparency to work

        if (NcursesUtils::is_extended_colors_supported())
        {
            // initialize color pairs
            //supports 32768 color pairs
            setup_extended_colors();
        }
        else
        {
            //only supports max 256 colors
            setup_colors();
        }
    }
}

void vis::NcursesWriter::setup_extended_colors()
{
    // assume their are 32768 color pairs available. Half for foreground and
    // half for background color pairs.
    for (int32_t color_index = 1; color_index <= VisConstants::k_max_extended_color; ++color_index)
    {
        init_extended_pair(color_index, color_index, -1);
        init_extended_pair(color_index + VisConstants::k_max_extended_color,
                           color_index, color_index);
    }
}

void vis::NcursesWriter::setup_colors()
{
    // initialize colors
    for (int16_t i = 1; i <= COLORS; ++i)
    {
        init_pair(i, i, -1);

        // initialize colors as background, this is used in write_background to
        // create a
        // full block effect without using a custom font
        init_pair(static_cast<int16_t>(i + COLORS), i, i);
    }
}

void vis::NcursesWriter::write_background(int32_t height, int32_t width,
                                          vis::ColorDefinition color,
                                          const std::wstring &msg)
{
    // Add COLORS which will set it to have the color as the background, see
    // "setup_colors"
    attron(VIS_COLOR_PAIR(color.get_color_index() + NcursesUtils::get_max_color()));

    mvaddwstr(height, width, msg.c_str());

    attroff(VIS_COLOR_PAIR(color.get_color_index() + NcursesUtils::get_max_color()));
}

void vis::NcursesWriter::write_foreground(int32_t height, int32_t width,
                                          vis::ColorDefinition color,
                                          const std::wstring &msg)
{
    attron(VIS_COLOR_PAIR(color.get_color_index()));

    mvaddwstr(height, width, msg.c_str());

    attroff(VIS_COLOR_PAIR(color.get_color_index()));
}

void vis::NcursesWriter::write(const int32_t row, const int32_t column,
                               const vis::ColorDefinition color,
                               const std::wstring &msg, const wchar_t character)
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

vis::ColorDefinition vis::NcursesWriter::to_color_pair(int32_t number, int32_t max,
                                                  bool wrap) const
{
    const auto colors_size =
        static_cast<vis::ColorIndex>(m_settings->get_colors().size());
    const auto index = (number * colors_size) / (max + 1);

    // no colors
    if (colors_size == 0)
    {
        return vis::ColorDefinition{0, 0, 0, 0};
    }

    const auto color = m_settings->get_colors()[static_cast<size_t>(
        wrap ? index % colors_size : std::min(index, colors_size - 1))];

    return color;
}

vis::NcursesWriter::~NcursesWriter()
{
    endwin();
}
