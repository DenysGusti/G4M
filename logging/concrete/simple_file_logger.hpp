#ifndef G4M_EUROPE_DG_SIMPLE_LOGGER_HPP
#define G4M_EUROPE_DG_SIMPLE_LOGGER_HPP

#include <fstream>
#include <string>
#include <source_location>
#include <filesystem>

#include "../abstract/alogger.hpp"
#include "../log_level.hpp"
#include "../message_formatter.hpp"

using namespace std;
using namespace g4m::logging::abstract;

namespace g4m::logging::concrete {

    class SimpleFileLogger : public ALogger {
    public:
        explicit SimpleFileLogger(string_view file_name) : file{filesystem::path{file_name}} {}

    protected:
        ofstream file;

        void log(const LogLevel level, const string_view message, const source_location &source) override {
            if (!(level & filter))
                return;
            file << message_formatter.formatMessage(level, message, source) << '\n';
        }
    };
}

#endif
