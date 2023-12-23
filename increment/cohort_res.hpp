#ifndef G4M_EUROPE_DG_COHORT_RES_HPP
#define G4M_EUROPE_DG_COHORT_RES_HPP

#include "../constants.hpp"
#include "v.hpp"

using namespace g4m::Constants;

namespace g4m::increment {
    struct CohortRes {
        // MG: get harvestable sawn-wood for the forest tC/ha for final cut
        double sawnWood = 0;
        // MG: get harvestable rest-wood for the forest tC/ha for final cut
        double restWood = 0;
        // MG: get harvestable sawn-wood for the forest tC/ha for thinning
        double sawnThWood = 0;
        // MG: get harvestable rest-wood for the forest tC/ha for thinning
        double restThWood = 0;
        // MG: get total harvestable biomass including harvest losses for the forest tC/ha for final cut
        double bmH = 0;
        // MG: get total harvestable biomass including harvest losses for the forest tC/ha for thinning
        double bmTh = 0;
        // MG: harvest residues for the forest tC/ha
        double harvLosses = 0;

        CohortRes() = default;

        CohortRes(const double realArea, const pair<V, V> &res) {
            if (realArea <= 0)
                return;

            double reciprocalRealArea = 1. / realArea;
            double areaRatio = res.second.area * reciprocalRealArea;  // harvArea / realArea
            double reciprocalModTimeStep = 1. / modTimeStep;

            sawnWood = res.second.sw * areaRatio;
            restWood = res.second.rw * areaRatio;
            sawnThWood = res.first.sw * reciprocalRealArea;
            restThWood = res.first.rw * reciprocalRealArea;
            bmH = res.second.bm * areaRatio;
            // MG: get total harvestable biomass including harvest losses for the forest tC/ha for thinning
            bmTh = res.first.bm * reciprocalRealArea;
            // harvestW, bmH, bmTh

            sawnWood *= reciprocalModTimeStep;
            restWood *= reciprocalModTimeStep;
            sawnThWood *= reciprocalModTimeStep;
            restThWood *= reciprocalModTimeStep;
            bmH *= reciprocalModTimeStep;
            bmTh *= reciprocalModTimeStep;
        }

        // sawnWood + restWood + sawnThWood + restThWood
        [[nodiscard]] double getTotalWoodRemoval() const noexcept {
            return sawnWood + restWood + sawnThWood + restThWood;
        }

        // bmH + bmTh
        [[nodiscard]] double getTotalHarvestedBiomass() const noexcept {
            return bmH + bmTh;
        }

        // totalHarvestedBiomass - totalWoodRemoval
        [[nodiscard]] double getHarvestLosses() const noexcept {
            return getTotalHarvestedBiomass() - getTotalWoodRemoval();
        }
    };
}

#endif
