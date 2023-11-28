#ifndef G4M_EUROPE_DG_OPT_ROT_TIMES_HPP
#define G4M_EUROPE_DG_OPT_ROT_TIMES_HPP

#include "ort.hpp"

namespace g4m::increment {
    // Optimal rotation time
    struct OptRotTimes {
        int maxInc = 0;       // Highest average increment
        int maxBm = 0;        // Maximum average Biomass
        int maxHarv = 0;      // Maximum harvest at final cut
        int maxAvgHarv = 0;   // Average Maximum harvest at final cut
        int maxAge = 0;       // Highest possible age

        int operator[](const ORT type) const noexcept {
            switch (type) {
                case ORT::MAI:
                    return maxInc;
                case ORT::MaxBm:
                    return maxBm;
                case ORT::MaxAge:
                    return maxAge;
                case ORT::HarvFin:
                    return maxHarv;
                case ORT::HarvAve:
                    return maxAvgHarv;
                default:
                    return numeric_limits<int>::max();
            }
        }
    };
}

#endif
