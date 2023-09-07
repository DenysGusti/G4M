#ifndef G4M_EUROPE_DG_HARVEST_RESIDUES_HPP
#define G4M_EUROPE_DG_HARVEST_RESIDUES_HPP

namespace g4m::structs {

    struct HarvestResidues {
        uint32_t simuId = 0;                        // ID of simulation unit
        size_t asID = 0;                            // ID of G4M cell
        uint8_t country = 0;                        // ID of G4M country
        uint8_t species = 0;
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
        int usedForest = 0;                         // indicator = 0; if the forest is used for wood production (thinning>0) then usedForest=1 else usedForest=0
        int protect = 0;                            // protected cell

        double residuesSuit1_perHa10 = 0;           // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/ha
        double residuesSuit2_perHa10 = 0;           // amount of sustainably harvestable stumps, tC/ha
        double residuesSuit3_perHa10 = 0;           // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/ha
        double residuesSuit4_notTaken_perHa10 = 0;  // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        double residuesSuit1_10 = 0;                // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/cell
        double residuesSuit2_10 = 0;                // amount of sustainably harvestable stumps, stumps, tC/cell
        double residuesSuit3_10 = 0;                // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/cell
        double residuesSuit4_notTaken10 = 0;        // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
        int timeUseSust1_10 = 0;                    // time from beginning of extraction of the residues from the cell
        int timeUseSust2_10 = 0;                    // time from beginning of extraction of the residues from the cell
        int usedForest10 = 0;                       // indicator = 0; if the forest is used for wood production (thinning>0) then usedForest=1 else usedForest=0

        double residuesSuit1_perHa30 = 0;           // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/ha
        double residuesSuit2_perHa30 = 0;           // amount of sustainably harvestable stumps, tC/ha
        double residuesSuit3_perHa30 = 0;           // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/ha
        double residuesSuit4_notTaken_perHa30 = 0;  // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        double residuesSuit1_30 = 0;                // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clearcut, tC/cell
        double residuesSuit2_30 = 0;                // amount of sustainably harvestable stumps, stumps, tC/cell
        double residuesSuit3_30 = 0;                // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clearcut, tC/cell
        double residuesSuit4_notTaken30 = 0;        // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
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
