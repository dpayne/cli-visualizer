/*
 * Logger.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_LOGGER_H
#define _VIS_LOGGER_H

#include "Domain/Settings.h"

#define VIS_LOG(log_level, message, args...)                                                                           \
    {                                                                                                                  \
        if (vis::Logger::level() <= log_level)                                                                         \
        {                                                                                                              \
            vis::Logger::log(log_level, __FILE__, __LINE__, message, ##args);                                          \
        }                                                                                                              \
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
    explicit Logger();

    virtual ~Logger();

    static void log(vis::LogLevel level, const char *message, ...);

    static vis::LogLevel level()
    {
#ifdef VIS_LOG_LEVEL
        return static_cast<vis::LogLevel>(VIS_LOG_LEVEL);
#endif
        return LogLevel::ERROR;
    }
};
}

#endif
