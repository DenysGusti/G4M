#ifndef G4M_EUROPE_DG_ALOGGER_HPP
#define G4M_EUROPE_DG_ALOGGER_HPP

#include <string>
#include <source_location>
#include "../log_level.hpp"
#include "../message_formatter.hpp"

using namespace std;

using namespace g4m::logging::concrete;

namespace g4m::logging::abstract {
    class ALogger {
    public:
        virtual ~ALogger() = default;

        void setFilter(const LogLevel mode) {
            filter = mode;
        }

        /*
        %T - long timestamp, %t - short timestamp
        %L - log level
        %S - source
        %M - message (always)
        Example: "[%T] [%L] [%S]: %M" (default format)
        */
        void setFormat(const string_view str) {
            message_formatter.setFormat(str);
        }

        void trace(const string_view message, const source_location source = source_location::current()) {
            log(LogLevel::Trace, message, source);
        }

        void debug(const string_view message, const source_location source = source_location::current()) {
            log(LogLevel::Debug, message, source);
        }

        void info(const string_view message, const source_location source = source_location::current()) {
            log(LogLevel::Info, message, source);
        }

        void warn(const string_view message, const source_location source = source_location::current()) {
            log(LogLevel::Warn, message, source);
        }

        void error(const string_view message, const source_location source = source_location::current()) {
            log(LogLevel::Error, message, source);
        }

        void fatal(const string_view message, const source_location source = source_location::current()) {
            log(LogLevel::Fatal, message, source);
        }
        // source_location::current() as a default parameter exists to pass information down the function calls

    protected:
        MessageFormatter message_formatter;
        LogLevel filter = LogLevel::All;

        virtual void log(LogLevel level, string_view message, const source_location &source) = 0;
    };
}

#endif
