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
    : m_settings{settings}, m_visualizer_window_width{0}, m_visualizer_window_height{0}, m_info_window_width{0}, m_info_window_height{0}, m_visualizer_window{nullptr}, m_info_window{nullptr}
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

    //Setup windows
    int32_t max_width, max_height;
    getmaxyx(stdscr, max_height, max_width);

    m_visualizer_window_width = max_width / 2;
    m_visualizer_window_height = max_height;

    m_info_window_width = max_width - m_visualizer_window_width;
    m_info_window_height = max_height;

    m_visualizer_window =
        newwin(m_visualizer_window_height, m_visualizer_window_width, 0, 0);

    m_info_window = newwin(m_info_window_height, m_info_window_width,
                           0, m_visualizer_window_width);
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

void vis::NcursesWriter::write_info(int32_t height, int32_t width,
                               vis::ColorIndex color, const std::wstring &msg)
{
    write(m_info_window, height, width, color, msg);
}

void vis::NcursesWriter::write_visualizer(int32_t height, int32_t width,
                               vis::ColorIndex color, const std::wstring &msg)
{
    write(m_visualizer_window, height, width, color, msg);
}

void vis::NcursesWriter::write(WINDOW * win, int32_t height, int32_t width,
                               vis::ColorIndex color, const std::wstring &msg)
{
    wattron(win, COLOR_PAIR(color));

    wmove(win, height, width);
    waddwstr(win, msg.c_str());

    wattroff(win, COLOR_PAIR(color));
}

void vis::NcursesWriter::clear_info()
{
    clear(m_info_window);
}

void vis::NcursesWriter::clear_visualizer()
{
    clear(m_visualizer_window);
}

void vis::NcursesWriter::clear(WINDOW *win)
{
    wstandend(win);
    werase(win);
}

void vis::NcursesWriter::flush_info()
{
    flush(m_info_window);
}

void vis::NcursesWriter::flush_visualizer()
{
    flush(m_visualizer_window);
}

void vis::NcursesWriter::flush(WINDOW * win)
{
    wrefresh(win);
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
    delwin(m_visualizer_window);
    delwin(m_info_window);
    endwin();
}
