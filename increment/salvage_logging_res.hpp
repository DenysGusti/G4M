#ifndef G4M_EUROPE_DG_SALVAGE_LOGGING_RES_HPP
#define G4M_EUROPE_DG_SALVAGE_LOGGING_RES_HPP

#include "FM_result.hpp"

namespace g4m::increment {
    struct SalvageLoggingRes {
        constexpr static double burntReduction = 0.85;  // reduction of biomass due to burning / expert assumption

        double realArea = 0;
        double harvestedWood = 0;  // harvest
        double bmH = 0;
        double damagedFire = 0;
        double harvestArea = 0;
        FMResult wind;
        FMResult fire;
        FMResult biotic;

        SalvageLoggingRes() = default;

        SalvageLoggingRes(const double realArea_, const FMResult &wind_, const FMResult &fire_, const FMResult &biotic_)
                : realArea{realArea_}, wind{wind_}, fire{fire_}, biotic{biotic_} {
            if (realArea <= 0)
                return;

            double reciprocalRealArea = 1. / realArea;
            harvestedWood = (wind.getTotalSalvageWoodRemoval() + fire.getTotalSalvageWoodRemoval() +
                             biotic.getTotalSalvageWoodRemoval()) * reciprocalRealArea;
            bmH = (wind.getHarvestedBmForest() + fire.getHarvestedBmForest() * burntReduction +
                   biotic.getHarvestedBmForest()) * reciprocalRealArea;
            damagedFire = fire.getHarvestedBmForest() * reciprocalRealArea;
            harvestArea = wind.area + fire.area + biotic.area;
            // TODO modTimeStep != 1
        }

        // (wind.getDBHArea() + fire.getDBHArea() + biotic.getDBHArea()) / harvestArea
        [[nodiscard]] inline double getDBHSlashCut() const noexcept {
            return (wind.getDBHArea() + fire.getDBHArea() + biotic.getDBHArea()) / harvestArea;
        }

        // (wind.getHArea() + fire.getHArea() + biotic.getHArea()) / harvestArea
        [[nodiscard]] inline double getHSlashCut() const noexcept {
            return (wind.getHArea() + fire.getHArea() + biotic.getHArea()) / harvestArea;
        }

        // harvestArea > 0 && realArea > 0 && harvest > 0 && bmH > 0
        [[nodiscard]] inline bool allPositive() const noexcept {
            return harvestArea > 0 && realArea > 0 && harvestedWood > 0 && bmH > 0;  // tC/ha
        }

        // bmH * realArea / harvestArea
        [[nodiscard]] inline double getGrowingStockCut() const noexcept {
            return bmH * realArea / harvestArea;
        }

        // harvest * (BEF - 2) + bmH
        [[nodiscard]] inline double getHarvestResiduesSalvageLogging(const double BEF) const noexcept {
            return harvestedWood * (BEF - 2) + bmH;
        }

        [[nodiscard]] inline bool positiveAreas() const noexcept {
            return harvestArea > 0 && realArea > 0;
        }

        // harvestArea / realArea
        [[nodiscard]] inline double getAreaRatio() const noexcept {
            return harvestArea / realArea;
        }
    };
}

#endif
