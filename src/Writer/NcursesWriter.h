/*
 * NcursesWriter.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _NCURSES_WRITER_H
#define _NCURSES_WRITER_H

#include "Writer/GenericWriter.h"

class NcursesWriter : public GenericWriter
{
  public:
    explicit NcursesWriter();

    ~NcursesWriter();
};

#endif
