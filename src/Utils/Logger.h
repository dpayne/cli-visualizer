/*
 * Logger.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_LOGGER_H
#define _VIS_LOGGER_H

#include "Domain/Settings.h"

/**
 * VIS_LOG is defined as a macro to force the log level check to happen at
 * compile time so that the expression is not evaluated if the log level
 * is turned off. For example,
 *      VIS_LOG( LogLevel::DEBUG,
 *          some_really_expensive_debug_function() )
 * In this case the, if the log level is ERROR, then the function will not be
 * called.
 */
#define VIS_LOG(log_level, message, args...)                                   \
    {                                                                          \
        if (vis::Logger::level() <= (log_level))                               \
        {                                                                      \
            vis::Logger::log(log_level, __FILE__, __LINE__, message, ##args);  \
        }                                                                      \
    }

namespace vis
{

enum class LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger
{
  public:
    static void log(vis::LogLevel level, const char *file, uint16_t line,
                    const char *format, ...);

    constexpr static vis::LogLevel level()
    {
#ifdef VIS_LOG_DEBUG
        return LogLevel::DEBUG;
#endif
        return LogLevel::ERROR;
    }

    static void initialize(std::string log_location);

    static void uninitialize();
};
} // namespace vis

#endif
