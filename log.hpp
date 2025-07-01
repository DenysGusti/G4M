#ifndef G4M_EUROPE_DG_LOG_HPP
#define G4M_EUROPE_DG_LOG_HPP

#include <memory>
#include <format>
#include <filesystem>

#include "logging/concrete/simple_file_logger.hpp"
#include "logging/concrete/async_file_logger.hpp"

using namespace std;
using namespace g4m::logging;
using namespace g4m::logging::abstract;
using namespace g4m::logging::concrete;
namespace fs = filesystem;

namespace g4m::Log {
    namespace {
        const fs::path log_path{"log"};

        thread_local unique_ptr<ALogger> s_Logger;
    }

    void Init(const string_view file_name = "main") {
        if (!fs::exists(log_path))
            fs::create_directories(log_path);

        s_Logger = make_unique<SimpleFileLogger>(log_path / format("{}.txt", file_name));
        // s_Logger = make_unique<AsyncFileLogger>("info.txt");
        // s_Logger->setFormat("[%t] [%L]: %M");
        s_Logger->setFormat("[%L]: %M");
        // s_Logger->setFilter(LogLevel::Error);
        // output everything but debug or warning messages
        // s_Logger->setFilter(~(LogLevel::Debug | LogLevel::Warn));
    }

    ALogger &GetLogger() {
        return *s_Logger;
    }
}

#define TRACE(...) g4m::Log::GetLogger().trace(format(__VA_ARGS__))
#define DEBUG(...) g4m::Log::GetLogger().debug(format(__VA_ARGS__))
#define INFO(...) g4m::Log::GetLogger().info(format(__VA_ARGS__))
#define WARN(...) g4m::Log::GetLogger().warn(format(__VA_ARGS__))
#define ERROR(...) g4m::Log::GetLogger().error(format(__VA_ARGS__))
#define FATAL(...) g4m::Log::GetLogger().fatal(format(__VA_ARGS__))

#endif
