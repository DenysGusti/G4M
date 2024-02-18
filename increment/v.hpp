#ifndef G4M_EUROPE_DG_V_HPP
#define G4M_EUROPE_DG_V_HPP

namespace g4m::increment {

    struct V {
        double area = 0;            // Area where management was done
        double sawnWood = 0;        // Sawn-wood [tC/Ha]
        double restWood = 0;        // Rest-wood [tC/Ha]
        double harvestCosts = 0;    // Costs [Costs/Ha]
        double bm = 0;              // Total cut biomass including harvesting losses [tC/Ha]
        // For final harvest the values are per hectare, for thinning they are total

        // (sawnWood + restWood) * area
        [[nodiscard]] double getTotalSalvageWoodRemoval() const noexcept {
            return (sawnWood + restWood) * area;
        }

        // bm * area
        [[nodiscard]] double getHarvestedBmForest() const noexcept {
            return bm * area;
        }
    };
}

#endif
