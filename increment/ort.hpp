#ifndef G4M_EUROPE_DG_ORT_HPP
#define G4M_EUROPE_DG_ORT_HPP

using namespace std;

namespace g4m::increment {
    // Get optimal rotation time (getTOpt and getTOptT)
    enum class ORT : uint8_t {
        MAI,       // Highest average increment
        MaxBm,     // Maximum average biomass
        MaxAge,    // Highest possible age
        HarvFin,   // Maximum harvest at final cut
        HarvAve    // Average Maximum harvest at final cut
    };
}

#endif
