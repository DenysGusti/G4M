#ifndef G4M_EUROPE_DG_TIMER_HPP
#define G4M_EUROPE_DG_TIMER_HPP

#include <chrono>
#include <string>

#include "../log.hpp"

using namespace std;

namespace g4m::diagnostics {

    class Timer {
        using clock = chrono::steady_clock;

    private:
        clock::time_point startTimePoint;
        string message;

    public:
        explicit Timer(const string_view msg) : startTimePoint{clock::now()}, message{msg} {}

        [[nodiscard]] auto elapsed() const {
            return chrono::duration_cast<chrono::milliseconds>(clock::now() - startTimePoint);
        }

        void reset() {
            startTimePoint = clock::now();
        }

        ~Timer() {
            INFO("{}: elapsed time: {}", message, elapsed());
        }
    };
}

#endif
