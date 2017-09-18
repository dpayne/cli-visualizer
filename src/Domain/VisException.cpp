#include "Domain/VisException.h"

#include <cstdarg>

vis::VisException::VisException(const VisException &other) noexcept
{
    this->m_message = other.m_message;
}

vis::VisException::VisException(const VisException &&other) noexcept
{
    this->m_message = other.m_message;
}

vis::VisException::VisException(const char *format, ...) noexcept
{
    char buffer[1024];

    std::va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args); // NOLINT
    va_end(args);

    m_message = std::string{buffer};
}

vis::VisException::~VisException() noexcept = default;
