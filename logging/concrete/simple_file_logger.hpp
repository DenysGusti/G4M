#ifndef G4M_EUROPE_DG_SIMPLE_LOGGER_HPP
#define G4M_EUROPE_DG_SIMPLE_LOGGER_HPP

#include <fstream>
#include <string>
#include <source_location>
#include <filesystem>
#include <mutex>

#include "../abstract/alogger.hpp"
#include "../log_level.hpp"
#include "../message_formatter.hpp"

using namespace std;
using namespace g4m::logging::abstract;
namespace fs = filesystem;

namespace g4m::logging::concrete {

    class SimpleFileLogger : public ALogger {
    public:
        explicit SimpleFileLogger(const string_view file_name) : SimpleFileLogger(fs::path{file_name}) {}

        explicit SimpleFileLogger(const fs::path &file_path) : file{file_path} {}

    protected:
        ofstream file;
        mutex mtx;

        void log(const LogLevel level, const string_view message, const source_location &source) override {
            if (!(level & filter))
                return;
            string tmp = message_formatter.formatMessage(level, message, source);
            {
                scoped_lock<mutex> lock{mtx};  // if multiple apps with the same scenario write to the same file
                file << tmp << '\n';
            }
        }
    };
}

#endif
