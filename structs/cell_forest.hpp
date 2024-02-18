#ifndef G4M_EUROPE_DG_FOREST_HPP
#define G4M_EUROPE_DG_FOREST_HPP

namespace g4m::structs {

    struct CellForest {
        double forestShare = 0;             // current forest share of forest in each cell
        double prevForestShare = 0;         // forest share of all forest one modelling step back in each cell
        double prevStemBiomass = 0;         // stem biomass of forest on a previous step, tC/ha; not for new forest!
        double CAI = 0;                     // current increment of forest
        double fellings = 0;                // fellings in forest
        double totalHarvest = 0;
        double totalBiomass = 0;
        double harvestFc_m3 = 0;            // wood removals from final cut of forest m^3 in the cell
        double harvestTh_m3 = 0;            // wood removals from thinning of forest m^3 in the cell
        double harvestSc_m3 = 0;            // wood removals from selective cut of unused forest m^3 in the cell
        double harvestConiferFc_m3 = 0;     // wood removals from final cut of coniferous forest m^3 in the cell
        double harvestConiferTh_m3 = 0;     // wood removals from thinning of coniferous forest m^3 in the cell
        double harvestConiferSc_m3 = 0;     // wood removals from selective cut of coniferous forest m^3 in the cell
        double harvestBroadleafFc_m3 = 0;   // wood removals from final cut of broadleaf forest m^3 in the cell
        double harvestBroadleafTh_m3 = 0;   // wood removals from thinning of broadleaf forest m^3 in the cell
        double harvestBroadleafSc_m3 = 0;   // wood removals from selective cut of broadleaf forest m^3 in the cell
        double deadwood = 0;                // dead wood in the forest in the cell, tC/ha
        double litter = 0;                  // litter in forest in the cell, tC/ha
        double deadwood_in = 0;             // dead wood input flow in forest in the cell, tC/ha year
        double litter_in = 0;               // litter input flow in forest in the cell, tC/ha year
        double deadwood_em = 0;             // dead wood emissions in forest in the cell, tC/ha year
        double litter_em = 0;               // litter emissions in forest in the cell, tC/ha year
        double extractedResidues = 0;       // The share of extracted harvest residues and harvest losses without RESIDUESUSESHARE
        double extractedStump = 0;          // The share of extracted stumps without RESIDUESUSESHARE
        double extractedCleaned = 0;        // The share of extracted cleaned stem-wood without (1-cleanedWoodUse) and RESIDUESUSESHARE
        double OAC = 0;                     // oldest age class in forest
        int timeCounterDeadwood = 0;        // Count time passed from changing the forest management from multifunctional to production or set-aside
        double burntDeadwood = 0;           // burnt deadwood due to fire damage, tC/ha year
        double burntLitter = 0;             // burnt litter due to fire damage, tC/ha year
    };
}

#endif
