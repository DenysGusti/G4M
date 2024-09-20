#ifndef G4M_EUROPE_DG_COHORT_RES_HPP
#define G4M_EUROPE_DG_COHORT_RES_HPP

#include "../settings/constants.hpp"
#include "FM_result.hpp"
#include "../init/data_struct.hpp"

using namespace g4m::Constants;
using namespace g4m::init;

namespace g4m::increment {
    // Calculate total harvestable stem wood for forest tC/ha (final cut + thinning) and
    // total wood (including harvest losses) at final felling and thinning
    struct CohortRes {
        double realArea = 0;        // tC/ha
        // MG: get harvestable sawn-wood for the forest tC/ha for final cut
        double sawnWood = 0;
        // MG: get harvestable rest-wood for the forest tC/ha for final cut
        double restWood = 0;
        // MG: get harvestable sawn-wood for the forest tC/ha for thinning
        double sawnThinningWood = 0;
        // MG: get harvestable rest-wood for the forest tC/ha for thinning
        double restThinningWood = 0;
        // MG: get total harvestable biomass including harvest losses for the forest tC/ha for final cut
        double biomassHarvest = 0;
        // MG: get total harvestable biomass including harvest losses for the forest tC/ha for thinning
        double biomassThinning = 0;

        FMResult thinning;
        FMResult finalCut;  // harvestArea = area

        CohortRes() = default;

        CohortRes(const double realArea_, const FMResult &thinning_, const FMResult &finalCut_)
                : realArea{realArea_}, thinning{thinning_}, finalCut{finalCut_} {
            if (realArea <= 0)
                return;

            double reciprocalRealArea = 1. / realArea;
            double areaRatio = finalCut.area * reciprocalRealArea;  // harvestArea / realArea
            double reciprocalModTimeStep = 1. / modTimeStep;

            sawnWood = finalCut.sawnWood * areaRatio;
            restWood = finalCut.restWood * areaRatio;
            sawnThinningWood = thinning.sawnWood * reciprocalRealArea;
            restThinningWood = thinning.restWood * reciprocalRealArea;
            biomassHarvest = finalCut.biomass * areaRatio;
            // MG: get total harvestable biomass including harvest losses for the forest tC/ha for thinning
            biomassThinning = thinning.biomass * reciprocalRealArea;
            // harvestW, bmH, bmTh

            sawnWood *= reciprocalModTimeStep;
            restWood *= reciprocalModTimeStep;
            sawnThinningWood *= reciprocalModTimeStep;
            restThinningWood *= reciprocalModTimeStep;
            biomassHarvest *= reciprocalModTimeStep;
            biomassThinning *= reciprocalModTimeStep;
        }

        // sawnWood + restWood + sawnThinningWood + restThinningWood
        [[nodiscard]] inline double getTotalWoodRemoval() const noexcept {
            return sawnWood + restWood + sawnThinningWood + restThinningWood;
        }

        // biomassHarvest + biomassThinning
        [[nodiscard]] inline double getTotalHarvestedBiomass() const noexcept {
            return biomassHarvest + biomassThinning;
        }

        // totalHarvestedBiomass - totalWoodRemoval
        [[nodiscard]] inline double getHarvestLosses() const noexcept {
            return getTotalHarvestedBiomass() - getTotalWoodRemoval();
        }

        // sawnWood + restWood
        [[nodiscard]] inline double getFinalCutWood() const noexcept {
            return sawnWood + restWood;
        }

        // sawnThinningWood + restThinningWood
        [[nodiscard]] inline double getThinnedWood() const noexcept {
            return sawnThinningWood + restThinningWood;
        }

        // biomassHarvest - (sawnWood + restWood)
        [[nodiscard]] inline double getFinalCutHarvestLosses() const noexcept {
            return biomassHarvest - getFinalCutWood();
        }

        // biomassThinning - (sawnThinningWood + restThinningWood)
        [[nodiscard]] inline double getThinningHarvestLosses() const noexcept {
            return biomassThinning - getThinnedWood();
        }

        [[nodiscard]] inline bool positiveAreas() const noexcept {
            return finalCut.area > 0 && realArea > 0;
        }

        // positiveAreas() && getFinalCutWood() > 0 && biomassHarvest > 0
        // tC/ha
        [[nodiscard]] inline bool if_fc() const noexcept {
            return positiveAreas() && getFinalCutWood() > 0 && biomassHarvest > 0;
        }

        // realArea > 0 && getThinnedWood() > 0 && biomassThinning > 0
        // tC/ha
        [[nodiscard]] inline bool if_th() const noexcept {
            return realArea > 0 && getThinnedWood() > 0 && biomassThinning > 0;
        }

        // harvestArea / realArea
        [[nodiscard]] inline double getAreaRatio() const noexcept {
            return finalCut.area / realArea;
        }

        // biomassHarvest * (realArea / harvestArea)
        [[nodiscard]] inline double getHarvestGrowingStock() const noexcept {
            return biomassHarvest * realArea / finalCut.area;
        }

        // getFinalCutWood() * (BEF - 2) + biomassHarvest
        [[nodiscard]] inline double getHarvestResiduesFinalCut(const double BEF) const noexcept {
            return getFinalCutWood() * (BEF - 2) + biomassHarvest;
        }

        // getThinnedWood() * (BEF - 2) + biomassThinning
        [[nodiscard]] inline double getHarvestResiduesThinning(const double BEF) const noexcept {
            return getThinnedWood() * (BEF - 2) + biomassThinning;
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
        harvestResiduesSanitaryFellings(const double BEF, const double cleanedWoodUseShare,
                                        const double biomassExpansionThinning) const {
            if (realArea <= 0)
                return {0, 0};

            // we take residues only from the places where do we take the logs
            double lost_fc = getFinalCutWood() * BEF;
            double lost_fc_d = biomassHarvest - 2 * getFinalCutWood();

            double lost_th = getThinnedWood() * biomassExpansionThinning;
            double lost_th_d = biomassThinning - 2 * getThinnedWood();

            // Added 28 July 2017: self thinned trees that are not used as logs
            double harvRes_sc = lost_th + lost_th_d;
            double harvRes_sc_notTaken = lost_th;

            // Added 28 July 2017: self dying trees that are not used as logs
            if (finalCut.area > 0) {
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
