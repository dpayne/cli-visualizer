/*
 * GenericWriter.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _GENERIC_WRITER_H
#define _GENERIC_WRITER_H

#include <string>

namespace vis
{

class GenericWriter
{
  public:
    explicit GenericWriter();

    virtual ~GenericWriter();

    virtual void write(int32_t height, int32_t width, uint8_t color,
                       const std::string &msg) = 0;

    virtual void clear() = 0;

    virtual void flush() = 0;

    virtual uint8_t to_color(int32_t number, int32_t max, bool wrap = true) const = 0;

};
}

#endif
