/*
 * NcursesWriter.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _NCURSES_WRITER_H
#define _NCURSES_WRITER_H

#include "Writer/GenericWriter.h"

namespace vis
{

class NcursesWriter : public GenericWriter
{
  public:
    explicit NcursesWriter();

    virtual ~NcursesWriter();

    virtual void write(int32_t height, int32_t width, uint8_t color,
                       const std::string &msg) override;

    virtual void clear() override;

    virtual void flush() override;

  private:
    void setup_colors();
};
}

#endif
