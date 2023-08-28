#ifndef G4M_EUROPE_DG_COHORT_HPP
#define G4M_EUROPE_DG_COHORT_HPP

namespace g4m::increment {

    struct Cohort {
        double area = 0;    // Forest stand area (single age class)
        double bm = 0;      // Biomass (stem), tC/ha
        double d = 0;       // diameter, cm
        double h = 0;       // height, m

        void reset() noexcept {
            area = 0;
            bm = 0;
            d = 0;
            h = 0;
        }
    };
}

#endif
