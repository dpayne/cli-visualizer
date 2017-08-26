/*
 * VisException.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_VIS_EXCEPTION_H
#define _VIS_VIS_EXCEPTION_H

#include <exception>
#include <stdexcept>
#include <string>

namespace vis
{

class VisException : public std::exception
{
  public:
    explicit VisException(const char *format, ...) noexcept;

    VisException(const VisException &other) noexcept;

    VisException(const VisException &&other) noexcept;

    VisException &operator=(const VisException &v) = default;

    VisException &operator=(VisException &&v) = default;

    ~VisException() noexcept override;

    const char *what() const noexcept override
    {
        return m_message.c_str();
    }

  private:
    std::string m_message;
};
} // namespace vis

#endif
