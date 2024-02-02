#ifndef G4M_EUROPE_DG_HARVEST_RESIDUES_HPP
#define G4M_EUROPE_DG_HARVEST_RESIDUES_HPP

#include "../init/data_struct.hpp"
#include "../init/species.hpp"

namespace g4m::structs {

    struct HarvestResidues {
        uint32_t simuId = 0;                        // ID of simulation unit
        size_t asID = 0;                            // ID of G4M cell
        uint8_t country = 0;                        // ID of G4M country
        Species species = Species::NoTree;
        double fTimber = 0;                         // tC -> m3 conversion
        double residuesSuit1_perHa = 0;             // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/ha
        double residuesSuit2_perHa = 0;             // amount of sustainably harvestable stumps, tC/ha
        double residuesSuit3_perHa = 0;             // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/ha
        double residuesSuit4_notTaken_perHa = 0;    // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        double residuesSuit1 = 0;                   // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/cell
        double residuesSuit2 = 0;                   // amount of sustainably harvestable stumps, stumps, tC/cell
        double residuesSuit3 = 0;                   // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/cell
        double residuesSuit4_notTaken = 0;          // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
        double costsSuit1 = 0;                      // costs of harvesting the residuesSuit1 emissions including the transportation costs
        double costsSuit2 = 0;                      // costs of harvesting the residuesSuit2 emissions including the transportation costs
        double costsSuit3 = 0;
        double costsSuit4_notTaken = 0;             // costs of harvesting the residuesSuit1 emissions including the transportation costs from planes which are not used for wood production
        double costsTotal = 0;                      // Total costs of harvesting the residues (costsSuit1+costsSuit2+costsSuit1_notTaken)
        double emissionsSuit1 = 0;                  // soil loss CO2 emissions in the cell associated with residuesSuit1
        double emissionsSuit2 = 0;                  // soil loss CO2 emissions in the cell associated with residuesSuit2
        double emissionsSuit3 = 0;                  // soil loss CO2 emissions in the cell associated with residuesSuit3
        double emissionsSuit4_notTaken = 0;         // soil loss CO2 emissions in the cell associated with residuesSuit1_notTaken
        double useSuit1 = 0;                        // switch to use the Suit1 residues
        double useSuit2 = 0;                        // switch to use the Suit2 residues
        double useSuit3 = 0;                        // switch to use the Suit2 residues
        double useSuit4 = 0;                        // switch to use the Suit2 residues
        int timeUseSust1 = 0;                       // time from beginning of extraction of the residues from the cell
        int timeUseSust2 = 0;                       // time from beginning of extraction of the residues from the cell
        int emissionsTimeFrame = 0;                 // period of residue extraction for estimation of emissions
        bool usedForest = false;                    // indicator = false; true if the forest is used for wood production (thinning > 0) otherwise false
        bool protect = false;                            // protected cell

        double residuesSuit1_perHa10 = 0;           // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clear-cut, tC/ha
        double residuesSuit2_perHa10 = 0;           // amount of sustainably harvestable stumps, tC/ha
        double residuesSuit3_perHa10 = 0;           // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clear-cut, tC/ha
        double residuesSuit4_notTaken_perHa10 = 0;  // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        double residuesSuit1_10 = 0;                // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clear-cut, tC/cell
        double residuesSuit2_10 = 0;                // amount of sustainably harvestable stumps, stumps, tC/cell
        double residuesSuit3_10 = 0;                // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clear-cut, tC/cell
        double residuesSuit4_notTaken10 = 0;        // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
        int timeUseSust1_10 = 0;                    // time from beginning of extraction of the residues from the cell
        int timeUseSust2_10 = 0;                    // time from beginning of extraction of the residues from the cell
        bool usedForest10 = false;                  // indicator = false; true if the forest is used for wood production (thinning > 0) otherwise false

        double residuesSuit1_perHa30 = 0;           // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clear-cut, tC/ha
        double residuesSuit2_perHa30 = 0;           // amount of sustainably harvestable stumps, tC/ha
        double residuesSuit3_perHa30 = 0;           // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clear-cut, tC/ha
        double residuesSuit4_notTaken_perHa30 = 0;  // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        double residuesSuit1_30 = 0;                // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clear-cut, tC/cell
        double residuesSuit2_30 = 0;                // amount of sustainably harvestable stumps, stumps, tC/cell
        double residuesSuit3_30 = 0;                // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clear-cut, tC/cell
        double residuesSuit4_notTaken30 = 0;        // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
        int timeUseSust1_30 = 0;                    // time from beginning of extraction of the residues from the cell
        int timeUseSust2_30 = 0;                    // time from beginning of extraction of the residues from the cell
        bool usedForest30 = false;                  // indicator = false; true if the forest is used for wood production (thinning > 0) otherwise false

        bool em_harvRes_fcO = false;
        bool em_harvRes_thO = false;
        bool em_harvRes_fcN = false;
        bool em_harvRes_thN = false;
        bool em_harvRes_scO = false;
        bool em_harvRes_scN = false;

        bool em_harvRes_fc10 = false;
        bool em_harvRes_th10 = false;
        bool em_harvRes_sc10 = false;
        bool em_harvRes_fc30 = false;
        bool em_harvRes_th30 = false;
        bool em_harvRes_sc30 = false;

        HarvestResidues() = default;

        explicit HarvestResidues(const DataStruct &plot) {
            simuId = plot.simuID;
            asID = plot.asID;
            country = plot.country;
            fTimber = plot.fTimber.data.at(2000);
            costsSuit1 = plot.residuesUseCosts;
            costsSuit2 = plot.residuesUseCosts + 10;
            costsSuit3 = plot.residuesUseCosts;
            costsSuit4_notTaken = plot.residuesUseCosts * 10;
        }

        void initTotalCost() noexcept {
            costsTotal = 0;
            if (residuesSuit1_perHa > 0 || residuesSuit2_perHa > 0 || residuesSuit3_perHa > 0 ||
                residuesSuit4_notTaken_perHa > 0
                //|| residuesSuit1_perHa10 > 0 || residuesSuit2_perHa10 > 0 || residuesSuit3_perHa10 > 0 ||
                // residuesSuit4_notTaken_perHa10 > 0
                || residuesSuit1_perHa30 > 0 || residuesSuit2_perHa30 > 0 || residuesSuit3_perHa30 > 0 ||
                residuesSuit4_notTaken_perHa30 > 0) {

                double numerator =
                        // costsSuit1 * (residuesSuit1 + residuesSuit1_10 + residuesSuit1_30)
                        // + costsSuit2 * (residuesSuit2 + residuesSuit2_10 + residuesSuit2_30)
                        // + costsSuit3 * (residuesSuit3 + residuesSuit3_10 + residuesSuit3_30)
                        costsSuit1 * (residuesSuit1 + residuesSuit1_30)
                        + costsSuit2 * (residuesSuit2 + residuesSuit2_30)
                        + costsSuit3 * (residuesSuit3 + residuesSuit3_30)
                        //+ costsSuit4_notTaken * (residuesSuit4_notTaken + residuesSuit4_notTaken10 + residuesSuit4_notTaken30)
                        + costsSuit4_notTaken * (residuesSuit4_notTaken + residuesSuit4_notTaken30);

                double denominator =
                        residuesSuit1 + residuesSuit2 + residuesSuit3 + residuesSuit4_notTaken
                        // + residuesSuit1_10 + residuesSuit2_10 + residuesSuit3_10 + residuesSuit4_notTaken10
                        + residuesSuit1_30 + residuesSuit2_30 + residuesSuit3_30 + residuesSuit4_notTaken30;

                costsTotal = numerator / denominator;
            }
        }
    };

}

#endif
