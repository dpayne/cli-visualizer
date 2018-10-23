/*
 * Logger.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Utils/Logger.h"
#include "Utils/Utils.h"
#include <cstdarg>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{
const uint64_t k_max_log_line = 4096;
const std::vector<const char *> log_level_names{"DEBUG", "INFO", "WARN",
                                                "ERROR"};
} // namespace

void vis::Logger::initialize(const std::string log_location)
{
    std::string log_path{log_location};

    // redirect stderr to log file
    if (std::freopen(log_path.c_str(), "w", stderr) == nullptr)
    {
        std::cerr << "Could not open log path " << log_path << std::endl;
    }
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
    vsnprintf(message_text_buf, k_max_log_line, format, args); // NOLINT
    fprintf(
        stderr, "%s:%d %s %s\n", file, line,
        log_level_names[std::min(static_cast<uint8_t>(level),
                                 static_cast<uint8_t>(vis::LogLevel::ERROR))],
        message_text_buf);
    va_end(args);
}
