/*
 * VisException.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_VIS_EXCEPTION_H
#define _VIS_VIS_EXCEPTION_H

#include <string>
#include <exception>
#include <stdexcept>

namespace vis
{

class VisException : public std::exception
{
  public:
    explicit VisException(const char *format, ...) noexcept;

    VisException(const VisException &other) noexcept;

    ~VisException() noexcept override;

    const char *what() const noexcept override
    {
        return m_message.c_str();
    }

  private:
    std::string m_message;
};
}

#endif
