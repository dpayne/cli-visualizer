/*
 * NcursesWriter.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <ncurses.h>
#include "Writer/NcursesWriter.h"

vis::NcursesWriter::NcursesWriter()
{
    initscr();
}

void vis::NcursesWriter::write(int32_t height, int32_t width,
                               const std::string &msg)
{
    move(height, width);
    addnstr(msg.c_str(), static_cast<int>(msg.size()));
}

void vis::NcursesWriter::clear()
{
    erase();
}

void vis::NcursesWriter::flush()
{
    refresh();
}

vis::NcursesWriter::~NcursesWriter()
{
    endwin();
}
