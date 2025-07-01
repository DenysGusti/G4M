#ifndef G4M_EUROPE_DG_FOREST_HPP
#define G4M_EUROPE_DG_FOREST_HPP

#include <array>
#include <ranges>

using namespace std;

namespace g4m::structs {
    struct CellForest {
        static string csvHeader(const string_view forestType) {
            return format("forestShare_{0},stemBiomass_{0},fellings_{0},CAI_{0},OAC_{0},biomassChangeAb_{0},"
                          "biomassChangeTotal_{0},totalBiomass_{0},harvestFc_{0},harvestTh_{0},harvestSc_{0},"
                          "deadwood_{0},litter_{0},deadwoodInput_{0},litterInput_{0},"
                          "deadwoodEmissions_{0},litterEmissions_{0},burntDeadwood_{0},burntLitter_{0},"
                          "extractedResidues_{0},extractedStump_{0},extractedCleaned_{0}", forestType);
        }

        // historical variables
        array<double, 2> forestShare{}; // forest share of all forest in each cell
        array<double, 2> stemBiomass{}; // stem biomass of forest, tC / ha

        double fellings = 0;            // fellings in forest, m^3 / year
        double CAI = 0;                 // current annual increment of forest, m^3 / (ha * year)
        double OAC = 0;                 // oldest age class in forest, years
        double biomassChangeAb = 0;     // change in above-ground biomass, MtCO2 / (ha * year)
        double biomassChangeTotal = 0;  // change in above and below-ground biomass, MtCO2 / (ha * year)
        double totalBiomass = 0;        // total living biomass (above-ground + below-ground) of forest, tC / ha

        double harvestFc = 0;           // wood removals from final cut of forest in the cell, m^3 / year
        double harvestTh = 0;           // wood removals from thinning of forest in the cell, m^3 / year
        double harvestSc = 0;           // wood removals from selective cut of multifunctional forest in the cell, m^3 / year

        double deadwood = 0;            // deadwood in the forest in the cell, tC / ha
        double litter = 0;              // litter in forest in the cell, tC / ha
        double deadwoodInput = 0;       // deadwood input flow in forest in the cell, tC / (ha * year)
        double litterInput = 0;         // litter input flow in forest in the cell, tC / (ha * year)
        double deadwoodEmissions = 0;   // deadwood emissions in forest in the cell, tC / (ha * year)
        double litterEmissions = 0;     // litter emissions in forest in the cell, tC / (ha * year)
        double burntDeadwood = 0;       // burnt deadwood due to fire damage, tC / (ha * year)
        double burntLitter = 0;         // burnt litter due to fire damage, tC / (ha * year)

        double extractedResidues = 0;   // the share of extracted harvest residues and harvest losses without residuesUseShare
        double extractedStump = 0;      // the share of extracted stumps without residuesUseShare
        double extractedCleaned = 0;    // the share of extracted cleaned stem-wood without (1 - cleanedWoodUse) and residuesUseShare

        void update() {
            ranges::shift_left(forestShare, 1);
            forestShare.back() = forestShare.end()[-2];

            ranges::shift_left(stemBiomass, 1);
            stemBiomass.back() = 0;

            fellings = 0;
            CAI = 0;
            OAC = 0;
            biomassChangeAb = 0;
            biomassChangeTotal = 0;
            totalBiomass = 0;

            harvestFc = 0;
            harvestTh = 0;
            harvestSc = 0;

            deadwoodInput = 0;
            litterInput = 0;
            deadwoodEmissions = 0;
            litterEmissions = 0;
            burntDeadwood = 0;
            burntLitter = 0;
        }

        void resetExtracted() {
            extractedResidues = 0;
            extractedStump = 0;
            extractedCleaned = 0;
        }

        // total wood (from FM) of forest in the cell, m^3 / year
        [[nodiscard]] double totalHarvest() const {
            return harvestFc + harvestTh + harvestSc;
        }

        [[nodiscard]] string csv() const {
            return format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}",
                          forestShare.back(), stemBiomass.back(), fellings, CAI, OAC, biomassChangeAb,
                          biomassChangeTotal, totalBiomass, harvestFc, harvestTh, harvestSc, deadwood,
                          litter, deadwoodInput, litterInput, deadwoodEmissions, litterEmissions, burntDeadwood,
                          burntLitter, extractedResidues, extractedStump, extractedCleaned);
        }
    };
}

#endif
