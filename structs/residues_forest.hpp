#ifndef G4M_EUROPE_DG_RESIDUES_FOREAT_HPP
#define G4M_EUROPE_DG_RESIDUES_FOREAT_HPP

namespace g4m::structs {

    struct ResiduesForest {
        // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clear-cut, tC/ha
        double residuesSuit1_perHa = 0;
        // amount of sustainably harvestable stumps, tC/ha
        double residuesSuit2_perHa = 0;
        // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clear-cut, tC/ha
        double residuesSuit3_perHa = 0;
        // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/ha
        double residuesSuit4_notTaken_perHa = 0;
        // amount of sustainably harvestable residues (branches and harvest losses) in the cell with clear-cut, tC/cell
        double residuesSuit1 = 0;
        // amount of sustainably harvestable stumps, stumps, tC/cell
        double residuesSuit2 = 0;
        // amount of sustainably harvestable residues (branches and harvest losses) in the cell without clear-cut, tC/cell
        double residuesSuit3 = 0;
        // all above-ground wood extracted from cleaned forest that is not taken for wood production, tC/cell
        double residuesSuit4_notTaken = 0;

        int timeUseSust1 = 0;   // time from beginning of extraction of the residues from the cell
        int timeUseSust2 = 0;   // time from beginning of extraction of the residues from the cell

        // indicator = false; true if the forest is used for wood production (thinning > 0) otherwise false
        bool usedForest = false;
    };
}

#endif
