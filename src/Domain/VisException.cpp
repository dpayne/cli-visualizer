#include "Domain/VisException.h"

#include <cstdarg>

vis::VisException::VisException( const char* format, ... ) throw ()
{
    char buffer[1024];

    std::va_list args;
    va_start( args, format );
    vsprintf( buffer, format, args );
    va_end( args );

    m_message = std::string{ buffer };
}

vis::VisException::~VisException() throw ()
{
}

