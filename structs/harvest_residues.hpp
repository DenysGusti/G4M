#ifndef G4M_EUROPE_DG_HARVEST_RESIDUES_HPP
#define G4M_EUROPE_DG_HARVEST_RESIDUES_HPP

namespace g4m::structs {

    struct HarvestResidues {
        int simuid = 0;                             // ID of simulation unit
        int asID = 0;                               // ID of G4M cell
        int country = 0;                            // ID of G4M country
        int species = 0;
        float ftimber = 0;                          // tC -> m3 conversion
        float residuesSuit1_perha = 0;              // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/ha
        float residuesSuit2_perha = 0;              // amount of sustainably harvestable stumps, tC/ha
        float residuesSuit3_perha = 0;              // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/ha
        float residuesSuit4_notTaken_perha = 0;     // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        float residuesSuit1 = 0;                    // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/cell
        float residuesSuit2 = 0;                    // amount of sustainably harvestable stumps, stumps, tC/cell
        float residuesSuit3 = 0;                    // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/cell
        float residuesSuit4_notTaken = 0;           // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
        float costsSuit1 = 0;                       // costs of harvesting the residuesSuit1 emissions including the transportation costs
        float costsSuit2 = 0;                       // costs of harvesting the residuesSuit2 emissions including the transportation costs
        float costsSuit3 = 0;
        float costsSuit4_notTaken = 0;              // costs of harvesting the residuesSuit1 emissions including the transportation costs from planes which are not used for wood production
        float costsTotal = 0;                       // Total costs of harvesting the rresidues (costsSuit1+costsSuit2+costsSuit1_notTaken)
        float emissionsSuit1 = 0;                   // soil loss CO2 emissions in the cell associated with residuesSuit1
        float emissionsSuit2 = 0;                   // soil loss CO2 emissions in the cell associated with residuesSuit2
        float emissionsSuit3 = 0;                   // soil loss CO2 emissions in the cell associated with residuesSuit3
        float emissionsSuit4_notTaken = 0;          // soil loss CO2 emissions in the cell associated with residuesSuit1_notTaken
        float useSuit1 = 0;                         // switch to use the Suit1 residues
        float useSuit2 = 0;                         // switch to use the Suit2 residues
        float useSuit3 = 0;                         // switch to use the Suit2 residues
        float useSuit4 = 0;                         // switch to use the Suit2 residues
        int timeUseSust1 = 0;                       // time from beginning of extraction of the residues from the cell
        int timeUseSust2 = 0;                       // time from beginning of extraction of the residues from the cell
        int emissionsTimeFrame = 0;                 // period of residue extraction for estimation of emissions
        int usedForest = 0;                         // indicator = 0; if the forest is used for wood production (thinning>0) then usedForest=1 else usedForest=0
        int protect = 0;                            // protected cell

        float residuesSuit1_perha10 = 0;            // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/ha
        float residuesSuit2_perha10 = 0;            // amount of sustainably harvestable stumps, tC/ha
        float residuesSuit3_perha10 = 0;            // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/ha
        float residuesSuit4_notTaken_perha10 = 0;   // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        float residuesSuit1_10 = 0;                 // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/cell
        float residuesSuit2_10 = 0;                 // amount of sustainably harvestable stumps, stumps, tC/cell
        float residuesSuit3_10 = 0;                 // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/cell
        float residuesSuit4_notTaken10 = 0;         // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
        int timeUseSust1_10 = 0;                    // time from beginning of extraction of the residues from the cell
        int timeUseSust2_10 = 0;                    // time from beginning of extraction of the residues from the cell
        int usedForest10 = 0;                       // indicator = 0; if the forest is used for wood production (thinning>0) then usedForest=1 else usedForest=0

        float residuesSuit1_perha30 = 0;            // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/ha
        float residuesSuit2_perha30 = 0;            // amount of sustainably harvestable stumps, tC/ha
        float residuesSuit3_perha30 = 0;            // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/ha
        float residuesSuit4_notTaken_perha30 = 0;   // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        float residuesSuit1_30 = 0;                 // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/cell
        float residuesSuit2_30 = 0;                 // amount of sustainably harvestable stumps, stumps, tC/cell
        float residuesSuit3_30 = 0;                 // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/cell
        float residuesSuit4_notTaken30 = 0;         // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
        int timeUseSust1_30 = 0;                    // time from beginning of extraction of the residues from the cell
        int timeUseSust2_30 = 0;                    // time from beginning of extraction of the residues from the cell
        int usedForest30 = 0;                       // indicator = 0; if the forest is used for wood production (thinning>0) then usedForest=1 else usedForest=0


        bool em_harvRes_fcO = false;
        bool em_harvRes_thO = false;
        bool em_harvRes_fcN = false;
        bool em_harvRes_thN = false;
        bool em_harvRes_scO = false;
        bool em_harvRes_scN = false;

        bool em_harvRes_fcO10 = false;
        bool em_harvRes_thO10 = false;
        bool em_harvRes_scO10 = false;
        bool em_harvRes_fcO30 = false;
        bool em_harvRes_thO30 = false;
        bool em_harvRes_scO30 = false;
    };

}

#endif
