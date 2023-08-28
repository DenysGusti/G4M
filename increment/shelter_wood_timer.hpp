#ifndef G4M_EUROPE_DG_SHELTER_WOOD_TIMER_HPP
#define G4M_EUROPE_DG_SHELTER_WOOD_TIMER_HPP

namespace g4m::increment {
    // Counting downtime to the second phase of final cut
    struct ShelterWoodTimer {
        double area = 0;    // Forest stand area in single age class that is left after the first phase and to be cut at the second phase
        int age = 0;        // age class in which the area is left after the first phase and to be cut in the second phase
        int timer = 0;      // years left to the second phase cut

        ShelterWoodTimer() = default;

        ShelterWoodTimer(const double area_, const int age_, const int timer_)
                : area{area_}, age{age_}, timer{timer_} {}
    };
}

#endif
