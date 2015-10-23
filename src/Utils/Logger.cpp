/*
 * Logger.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <cstdarg>
#include "Utils/Logger.h"

namespace
{
const static uint64_t k_max_log_line = 4096;
}

vis::Logger::Logger()
{
}

void vis::Logger::log(vis::LogLevel level, const char *message, ...)
{
    char message_text_buf[k_max_log_line];
    std::va_list args;
    va_start(args, message);
    vsnprintf(message_text_buf, k_max_log_line, message, args);
}

vis::Logger::~Logger()
{
}
