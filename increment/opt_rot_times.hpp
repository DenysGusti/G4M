#ifndef G4M_EUROPE_DG_OPT_ROT_TIMES_HPP
#define G4M_EUROPE_DG_OPT_ROT_TIMES_HPP

namespace g4m::increment {
    // Optimal rotation time
    struct OptRotTimes {
        // Get optimal rotation time (getTOpt and getTOptT)
        enum class Mode : uint8_t {
            MAI,       // Highest average increment
            MaxBm,     // Maximum average biomass
            MaxAge,    // Highest possible age
            HarvFin,   // Maximum harvest at final cut
            HarvAve    // Average Maximum harvest at final cut
        };

        int maxInc = 0;       // Highest average increment
        int maxBm = 0;        // Maximum average Biomass
        int maxHarv = 0;      // Maximum harvest at final cut
        int maxAvgHarv = 0;   // Average Maximum harvest at final cut
        int maxAge = 0;       // Highest possible age

        [[nodiscard]] int operator()(const Mode type) const noexcept {
            switch (type) {
                case Mode::MAI:
                    return maxInc;
                case Mode::MaxBm:
                    return maxBm;
                case Mode::MaxAge:
                    return maxAge;
                case Mode::HarvFin:
                    return maxHarv;
                case Mode::HarvAve:
                    return maxAvgHarv;
                default:
                    return numeric_limits<int>::max();
            }
        }
    };
}

#endif
