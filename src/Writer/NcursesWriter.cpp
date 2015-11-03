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
    VIS_LOG(vis::LogLevel::DEBUG, "Setting up default colors");
    for (auto pair : NcursesUtils::get_default_color_map())
    {
        init_pair(pair.second, pair.second, -1);
    }

    if (!m_settings->get_color_definitions().empty())
    {
        if (!m_settings->get_color_definitions().empty() &&
            can_change_color() == FALSE)
        {
            for (const auto &c : m_settings->get_color_definitions())
            {
                setup_color(c);
            }
        }
        else
        {
            VIS_LOG(vis::LogLevel::WARN, "Terminal does not support change "
                                         "colors, using default colors.");
        }
    }
}

void vis::NcursesWriter::write(int32_t height, int32_t width,
                               vis::ColorIndex color, const std::string &msg)
{
    attron(COLOR_PAIR(color));

    move(height, width);
    addstr(msg.c_str());

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
    const auto index = (number * colors_size) / max;

    return m_settings->get_colors()[static_cast<size_t>(
        wrap ? index % colors_size : std::min(index, colors_size - 1))];
}

vis::NcursesWriter::~NcursesWriter()
{
    endwin();
}
