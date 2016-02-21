/*
 * Logger.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <cstdarg>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Utils/Logger.h"
#include "Utils/Utils.h"

namespace
{
const static uint64_t k_max_log_line = 4096;
const static std::vector<const char *> log_level_names{"DEBUG", "INFO", "WARN",
                                                       "ERROR"};
}

vis::Logger::Logger()
{
}

void vis::Logger::initialize(const std::string log_location)
{
    std::string log_path{log_location};

    // redirect stderr to log file
    std::freopen(log_path.c_str(), "w", stderr);
}

void vis::Logger::uninitialize()
{
    std::fclose(stderr);
}

void vis::Logger::log(vis::LogLevel level, const char *file, uint16_t line,
                      const char *format, ...)
{
    char message_text_buf[k_max_log_line];
    std::va_list args;
    va_start(args, format);
    vsnprintf(message_text_buf, k_max_log_line, format, args);
    fprintf(
        stderr, "%s:%d %s %s\n", file, line,
        log_level_names[std::min(static_cast<uint8_t>(level),
                                 static_cast<uint8_t>(vis::LogLevel::ERROR))],
        message_text_buf);
    va_end(args);
}

vis::Logger::~Logger()
{
}
