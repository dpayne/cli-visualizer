/*
 * NcursesWriter.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <ncurses.h>
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

void vis::NcursesWriter::setup_color(const vis::ColorDefinition &color)
{
    init_color(color.get_color_index(), color.get_red(), color.get_green(),
               color.get_blue());

    // Set background to -1 to enable transparency
    init_pair(color.get_color_index(), color.get_color_index(), -1);
}

void vis::NcursesWriter::setup_colors()
{
    // setup basic colors
    for (auto pair : NcursesUtils::get_default_color_map())
    {
        init_pair(pair.second, pair.second, -1);
    }

    if (!m_settings->get_color_definitions().empty())
    {
        if (!m_settings->get_color_definitions().empty() &&
            can_change_color() == TRUE)
        {
            for (const auto &c : m_settings->get_color_definitions())
            {
                VIS_LOG(vis::LogLevel::DEBUG, "Setting up color %d %d %d %d",
                        c.get_color_index(), c.get_red(), c.get_green(),
                        c.get_blue());
                setup_color(c);
            }
        }
        else
        {
            VIS_LOG(vis::LogLevel::ERROR, "Terminal does not support change "
                                          "colors, using default colors.");
        }
    }
}

void vis::NcursesWriter::write(int32_t height, int32_t width,
                               vis::ColorIndex color, const std::wstring &msg)
{
    attron(COLOR_PAIR(color));

    move(height, width);
    addwstr(msg.c_str());

    attroff(COLOR_PAIR(color));
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

vis::ColorIndex vis::NcursesWriter::to_color(int32_t number, int32_t max,
                                             bool wrap) const
{
    const auto colors_size =
        static_cast<vis::ColorIndex>(m_settings->get_colors().size());
    const auto index = (number * colors_size) / (max + 1);

    return m_settings->get_colors()[static_cast<size_t>(
        wrap ? index % colors_size : std::min(index, colors_size - 1))];
}

vis::NcursesWriter::~NcursesWriter()
{
    endwin();
}
