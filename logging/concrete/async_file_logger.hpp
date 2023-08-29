#ifndef G4M_EUROPE_DG_ASYNC_FILE_LOGGER_HPP
#define G4M_EUROPE_DG_ASYNC_FILE_LOGGER_HPP

#include <fstream>
#include <string>
#include <queue>
#include <source_location>
#include <functional>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../abstract/alogger.hpp"
#include "../log_level.hpp"
#include "../message_formatter.hpp"

using namespace std;
using namespace g4m::logging::abstract;
namespace fs = filesystem;

namespace g4m::logging::concrete {

    class AsyncFileLogger final : public ALogger {
    public:
        explicit AsyncFileLogger(const string_view file_name) : AsyncFileLogger(fs::path{file_name}) {}

        explicit AsyncFileLogger(const fs::path &file_path) :
                file{file_path}, logger_thread{bind_front(&AsyncFileLogger::fileWorker, this)} {}

        ~AsyncFileLogger() override {
            logger_thread.request_stop();
            cv.notify_one();
            logger_thread.join();
        }

    protected:
        ofstream file;

        mutex mtx;
        condition_variable cv;
        jthread logger_thread;
        queue <string> log_queue;

        void log(const LogLevel level, const string_view message, const source_location &source) final {
            if (!(level & filter))
                return;
            string tmp = message_formatter.formatMessage(level, message, source);
            // push message
            {
                scoped_lock<mutex> lock{mtx};
                log_queue.push(tmp);
            }
            cv.notify_one();
        }

        void fileWorker(const stop_token &stoken) {
            for (string message; true; file << message << '\n') {
                // pop message
                unique_lock<mutex> lock{mtx};
                cv.wait(lock, [&]() { return !log_queue.empty() || stoken.stop_requested(); });
                if (log_queue.empty() && stoken.stop_requested())
                    return;
                message = log_queue.front();
                log_queue.pop();
            }
        }
    };
}

#endif
