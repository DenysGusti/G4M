#ifndef G4M_EUROPE_DG_LOG_LEVEL_HPP
#define G4M_EUROPE_DG_LOG_LEVEL_HPP

#include <string>
#include <unordered_map>

using namespace std;

namespace g4m::logging {

    enum class LogLevel : uint8_t {
        Trace = 1 << 0,
        Debug = 1 << 1,
        Info = 1 << 2,
        Warn = 1 << 3,
        Error = 1 << 4,
        Fatal = 1 << 5,
        None = 0,
        All = Trace | Debug | Info | Warn | Error | Fatal
    };

    using LogLevelType = underlying_type<LogLevel>::type;

    LogLevel operator|(const LogLevel lop, const LogLevel rop) {
        return LogLevel(static_cast<LogLevelType>(lop) | static_cast<LogLevelType>(rop));
    }

    LogLevel operator&(const LogLevel lop, const LogLevel rop) {
        return LogLevel(static_cast<LogLevelType>(lop) & static_cast<LogLevelType>(rop));
    }

    LogLevel operator^(const LogLevel lop, const LogLevel rop) {
        return LogLevel(static_cast<LogLevelType>(lop) ^ static_cast<LogLevelType>(rop));
    }

    LogLevel operator~(const LogLevel op) {
        return LogLevel(~static_cast<LogLevelType>(op));
    }

    bool operator!(const LogLevel op) {
        return !static_cast<LogLevelType>(op);
    }

    struct EnumHasher {
        template<typename T>
        size_t operator()(const T t) const noexcept {
            return static_cast<size_t>(t);
        }
    };

    const static unordered_map<LogLevel, string, EnumHasher> convert_level = {
            {LogLevel::Trace, "TRACE"},
            {LogLevel::Debug, "DEBUG"},
            {LogLevel::Info,  "INFO"},
            {LogLevel::Warn,  "WARN"},
            {LogLevel::Error, "ERROR"},
            {LogLevel::Fatal, "FATAL"}
    };
}

#endif
