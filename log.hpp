#ifndef G4M_EUROPE_DG_LOG_HPP
#define G4M_EUROPE_DG_LOG_HPP

#include <memory>
#include <format>

#include "logging/concrete/simple_file_logger.hpp"
#include "logging/concrete/async_file_logger.hpp"

// #define ADD_NEW_LOGGER

using namespace std;
using namespace g4m::logging;
using namespace g4m::logging::abstract;
using namespace g4m::logging::concrete;

namespace g4m::Log {
    namespace {
        unique_ptr<ALogger> s_Logger;

#ifdef ADD_NEW_LOGGER
        unique_ptr<ILogger> s_LoggerNew;  // add new logger
#endif
    }

    static void Init() {
        s_Logger = make_unique<SimpleFileLogger>("info.txt");
        // s_Logger = make_unique<AsyncFileLogger>("info.txt");
        // s_Logger->setFormat("[%t] [%L]: %M");
        s_Logger->setFormat("[%L]: %M");
        // s_Logger->setFilter(LogLevel::Error);
        // output everything but debug or warning messages
        // s_Logger->setFilter(~(LogLevel::Debug | LogLevel::Warn));

#ifdef ADD_NEW_LOGGER
        s_LoggerNew = make_shared<Logger>("info_new.txt");
        s_LoggerNew->setFormat("%M");
#endif
    }

    inline ALogger &GetLogger() {
        return *s_Logger;
    }

#ifdef ADD_NEW_LOGGER
    inline ILogger &GetLoggerNew() {
        return *s_LoggerNew;
    }
#endif
}

#define TRACE(...) g4m::Log::GetLogger().trace(format(__VA_ARGS__))
#define DEBUG(...) g4m::Log::GetLogger().debug(format(__VA_ARGS__))
#define INFO(...) g4m::Log::GetLogger().info(format(__VA_ARGS__))
#define WARN(...) g4m::Log::GetLogger().warn(format(__VA_ARGS__))
#define ERROR(...) g4m::Log::GetLogger().error(format(__VA_ARGS__))
#define FATAL(...) g4m::Log::GetLogger().fatal(format(__VA_ARGS__))

#ifdef ADD_NEW_LOGGER
#define TRACE_NEW(...) g4m::Log::GetLoggerNew().trace(format(__VA_ARGS__))
#define DEBUG_NEW(...) g4m::Log::GetLoggerNew().debug(format(__VA_ARGS__))
#define INFO_NEW(...) g4m::Log::GetLoggerNew().info(format(__VA_ARGS__))
#define WARN_NEW(...) g4m::Log::GetLoggerNew().warn(format(__VA_ARGS__))
#define ERROR_NEW(...) g4m::Log::GetLoggerNew().error(format(__VA_ARGS__))
#define FATAL_NEW(...) g4m::Log::GetLoggerNew().fatal(format(__VA_ARGS__))
#endif

#endif
