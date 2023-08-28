#ifndef G4M_EUROPE_DG_TIMER_HPP
#define G4M_EUROPE_DG_TIMER_HPP

#include <iostream>
#include <chrono>

using namespace std;

namespace g4m::diagnostics {

    class Timer {
        using clock = chrono::steady_clock;

    private:
        clock::time_point startTimePoint;

    public:
        Timer() : startTimePoint{clock::now()} {}

        [[nodiscard]] auto elapsed() const {
            return chrono::duration_cast<chrono::milliseconds>(clock::now() - startTimePoint);
        }

        void reset() {
            startTimePoint = clock::now();
        }

        ~Timer() {
            // cout << "Elapsed time: " << elapsed() << '\n';
            INFO("Elapsed time: {}", elapsed());
        }
    };
}

#endif
