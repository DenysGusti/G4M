#ifndef G4M_EUROPE_DG_FOREST_HPP
#define G4M_EUROPE_DG_FOREST_HPP

namespace g4m::structs {

    struct CellForest {
        vector<double> forestShare;         // forest share of all forest in each cell
        vector<double> stemBiomass;         // stem biomass of forest, tC/ha; for new forest N.forestShare * tC/ha
        vector<double> fellings;            // fellings in forest
        vector<double> CAI;                 // current annual increment of forest, m^3 / (ha *year)
        vector<double> OAC;                 // oldest age class in forest
        vector<double> deadwoodInput;       // dead wood input flow in forest in the cell, tC/ha year
        vector<double> litterInput;         // litter input flow in forest in the cell, tC/ha year
        vector<double> biomassChange_ab;    // change in above-ground biomass, MtCO2 / (ha * year)
        vector<double> biomassChange_total; // change in above and below-ground biomass, MtCO2 / (ha * year)

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
        double deadwood_em = 0;             // deadwood emissions in forest in the cell, tC/ha year
        double litter_em = 0;               // litter emissions in forest in the cell, tC/ha year
        double extractedResidues = 0;       // The share of extracted harvest residues and harvest losses without RESIDUESUSESHARE
        double extractedStump = 0;          // The share of extracted stumps without RESIDUESUSESHARE
        double extractedCleaned = 0;        // The share of extracted cleaned stem-wood without (1-cleanedWoodUse) and RESIDUESUSESHARE
        int timeCounterDeadwood = 0;        // Count time passed from changing the forest management from multifunctional to production or set-aside
        double burntDeadwood = 0;           // burnt deadwood due to fire damage, tC/ha year
        double burntLitter = 0;             // burnt litter due to fire damage, tC/ha year

        CellForest() {
            forestShare.reserve(128);
            stemBiomass.reserve(128);
            fellings.reserve(128);
            CAI.reserve(128);
            OAC.reserve(128);
            deadwoodInput.reserve(128);
            litterInput.reserve(128);
            biomassChange_ab.reserve(128);
            biomassChange_total.reserve(128);
        }
    };
}

#endif
