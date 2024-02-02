#ifndef G4M_EUROPE_DG_COHORT_RES_HPP
#define G4M_EUROPE_DG_COHORT_RES_HPP

#include "../constants.hpp"
#include "v.hpp"
#include "../init/data_struct.hpp"

using namespace g4m::Constants;
using namespace g4m::init;

namespace g4m::increment {
    struct CohortRes {
        double harvestArea = 0;     // tC/ha
        double realArea = 0;        // tC/ha
        double DBH = 0;           // average DBH of trees in age classes that are clear-cut
        double H = 0;             // average height of trees in age classes that are clear-cut
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

        CohortRes() = default;

        CohortRes(const double realArea_, const pair<V, V> &res, const double DBH_, const double H_)
                : harvestArea{res.second.area}, realArea{realArea_}, DBH{DBH_}, H{H_} {
            if (realArea <= 0)
                return;

            double reciprocalRealArea = 1. / realArea;
            double areaRatio = harvestArea * reciprocalRealArea;  // harvestArea / realArea
            double reciprocalModTimeStep = 1. / modTimeStep;

            sawnWood = res.second.sawnWood * areaRatio;
            restWood = res.second.restWood * areaRatio;
            sawnThWood = res.first.sawnWood * reciprocalRealArea;
            restThWood = res.first.restWood * reciprocalRealArea;
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

        // sawnWood + restWood
        [[nodiscard]] double getFinalCutWood() const noexcept {
            return sawnWood + restWood;
        }

        // sawnThWood + restThWood
        [[nodiscard]] double getThinnedWood() const noexcept {
            return sawnThWood + restThWood;
        }

        [[nodiscard]] bool positiveAreas() const noexcept {
            return harvestArea > 0 && realArea > 0;
        }

        // harvestArea > 0 && getClearCutWood() > 0 && bmH > 0
        // tC/ha
        [[nodiscard]] bool if_fc() const noexcept {
            return positiveAreas() && getFinalCutWood() > 0 && bmH > 0;
        }

        // realArea > 0 && getThinnedWood() > 0 && bmTh > 0
        // tC/ha
        [[nodiscard]] bool if_th() const noexcept {
            return realArea > 0 && getThinnedWood() > 0 && bmTh > 0;
        }

        // harvestArea / realArea
        [[nodiscard]] double getAreaRatio() const noexcept {
            return harvestArea / realArea;
        }

        // bmH * (realArea / harvestArea)
        [[nodiscard]] double getResHarvestedBiomass() const noexcept {
            return bmH * realArea / harvestArea;
        }

        /*
        returns harvRes_sc, harvRes_sc_notTaken
        harvRes_scO = ((((sawnWlost + restWlost) * (BEF(int(iter->SPECIESTYPE[byear]) - 1, (bmHlost / harvAreaO * realAreaO), iter->FTIMBER[byear]) - 1) + (bmHlost - (sawnWlost + restWlost)))
            + ((sawnThWlost + restThWlost) * (BEF(int(iter->SPECIESTYPE[byear]) - 1, abBiomassO, iter->FTIMBER[byear]) - 1) + (bmThlost - (sawnThWlost + restThWlost))))
            * cleanedWoodUseCurrent); // we take residues only from the places where do we take the logs
        harvRes_scO_notTaken = (1 - cleanedWoodUse[country]) * (sawnWlost + restWlost) * BEF(int(iter->SPECIESTYPE[byear]) - 1, (bmHlost / harvAreaO * realAreaO), iter->FTIMBER[byear]) // Added 28 July 2017: self dying trees that are not used as logs
            + (1 - cleanedWoodUseCurrent) * (sawnThWlost + restThWlost) * BEF(int(iter->SPECIESTYPE[byear]) - 1, abBiomassO, iter->FTIMBER[byear]); //Added 28 July 2017: self dying trees that are not used as logs
        */

        [[nodiscard]] pair<double, double>
        harvResiduesSanitaryFellings(const DataStruct &plot, const double cleanedWoodUseShare,
                                     const double biomassExpansionThinning) const {
            if (realArea <= 0)
                return {0, 0};

            // we take residues only from the places where do we take the logs
            double lost_fc = getFinalCutWood() * plot.BEF(getResHarvestedBiomass());
            double lost_fc_d = bmH - 2 * getFinalCutWood();

            double lost_th = getThinnedWood() * biomassExpansionThinning;
            double lost_th_d = bmTh - 2 * getThinnedWood();

            // Added 28 July 2017: self thinned trees that are not used as logs
            double harvRes_sc = lost_th + lost_th_d;
            double harvRes_sc_notTaken = lost_th;

            // Added 28 July 2017: self dying trees that are not used as logs
            if (harvestArea > 0) {
                harvRes_sc += lost_fc + lost_fc_d;
                harvRes_sc_notTaken += lost_fc;
            }

            harvRes_sc *= cleanedWoodUseShare;
            harvRes_sc_notTaken *= 1 - cleanedWoodUseShare;
            return {harvRes_sc, harvRes_sc_notTaken};
        }
    };
}

#endif
