#ifndef G4M_EUROPE_DG_DAT_HPP
#define G4M_EUROPE_DG_DAT_HPP

#include <array>

using namespace std;

namespace g4m::structs {

    struct Dat {
        int simUnit = -1;
        int Rotation = 0;
        double LandAreaHa = 0;
        double potHarvest = 0;
        double forestShare = 0;
        double forestShare0 = 0;
        double OforestShare = 0;            // current forest share of old forest in each cell
        double OforestShareU = 0;           // current "usual" forest share of old forest in each cell
        double OforestShare10 = 0;          // current forest share of old forest under forest 10 policy in each cell
        double OforestShare30 = 0;          // current forest share of old forest under forest 30 policy in each cell

        double OforestShareSC = 0;          // current forest share of old forest in case of species change simulation in each cell
        double OforestShareOnly = 0;        // current forest share of old forest without the new forest over 20 y.o. in each cell
        double AforestShare = 0;            // current forest share of new forest in each cell
        double prevOForShare = 0;           // forest share of all old forest one modelling step back in each cell
        double prevOForShareU = 0;          // forest share of "usual" old forest one modelling step back in each cell
        double prevOForShare10 = 0;         // forest share of 10% policy forest one modelling step back in each cell
        double prevOForShare30 = 0;         // forest share of 30% policy forest one modelling step back in each cell
        double prevOForShareOnly = 0;       // forest share of old forest  without the new forest over 20 y.o. one modelling step back in each cell
        double prevOForShareRP = 0;
        double prevAForShareRP = 0;
        double AforestSharePrev = 0;        // forest share of new forest one modelling step back in each cell
        double AforestShareAcc = 0;         // new forest share accumulated from 2001 till a current year
        double savedCarbonPrev = 0;
        double gainedCarbonPrev = 0;
        double EmissionsTotPrev = 0;
        double EmissionsAfforPrev = 0;
        double prevPlantPhytHaBmGr = 0;
        double prevPlantPhytHaBmGrBef = 0;
        double prevPlantPhytHaBmGrBef20b = 0;
        double prevPlantPhytHaBmGrBef20o = 0;
        array<double, 211> prevPlantPhytHaBmGrBef20b_age{};
        array<double, 211> prevPlantPhytHaBmGrBef20o_age{};
        double prevPlantPhytHaBlGr = 0;
        double deforestHaTot = 0;
        double afforestHaTot = 0;
        double EmissionsProduct = 0;
        double EmissionsLitter = 0;
        double EmissionsSOC = 0;
        double EmissionsSlashBurn = 0;
        double EmissionsDeadBurn = 0;
        double EmissionsCRootBurn = 0;
        double EmissionsTot = 0;
        double EmLitterAffor = 0;
        double EmSOCAffor = 0;
        double EmissionsAffor = 0;
        array<double, 211> forestAgeShare{};
        array<double, 211> BDeadA{};
        array<double, 211> LitterA{};
        array<double, 211> SOCA{};
        array<double, 211> SOCA1{};
        array<double, 211> ProdLongA{};
        array<double, 211> ProdShortA{};
        array<double, 211> deforestA{};
        array<double, 211> FineRootA{};
        array<double, 211> LitterAffor{};
        array<double, 211> SOCaffor{};
        double prevReportYear = 0;
        double ObiomassPrev = 0;            // Biomass of "usual" old forest on a previous step, tC/ha
        double ObiomassPrev10 = 0;          // Biomass of 10% policy forest on a previous step, tC/ha
        double ObiomassPrev30 = 0;          // Biomass of 30% policy forest on a previous step, tC/ha
        double ObiomassPrevP = 0;           // Biomass of primary forest on a previous step, tC/ha

        double abBiomassTotPrev = 0;
        int rotBiomass = 0;
        int ireportYear = 0;
        double SD = 0;
        double usedForest = 0;
        double FMsink = 0;
        double FMsink_Bm = 0;
        double Obiomass0 = 0;               // modelled biomass at time 0
        double deforWoodTotM3 = 0;          // stem wood obtained from deforestation averaged for last 5 years
        array<double, 5> deforWoodArray{};  // stem wood obtained from deforestation for last 5 years
        array<double, 5> deforAreaArray{};  // deforested area for the last 5 years
        double deforestShare = 0;
        double afforestShare = 0;
        double CAI = 0;                     // current increment of old "usual" forest
        double CAI_new = 0;                 // current increment of new "usual" forest
        double CAI10 = 0;                   // current increment of old 10% policy forest
        double CAI30 = 0;                   // current increment of old 30% policy forest
        double CAIP = 0;                    // current increment of old primary forest
        double afforestHaYear = 0;
        double deforestHaYear = 0;
        double harvestTot = 0;
        double harvestTot10 = 0;
        double harvestTot30 = 0;
        double fellingsO = 0;               // fellings in the old "usual" forest
        double fellingsN = 0;               // fellings in the new forest
        double fellings10 = 0;              // fellings in the 10% policy forest
        double fellings30 = 0;              // fellings in the 30% policy forest
        double harvestTot_all = 0;
        double harvestFcM3Ha = 0;
        double harvestThM3Ha = 0;
        double oforestBm = 0;
        double aforestBm = 0;
        double oforestBm10 = 0;
        double oforestBm30 = 0;
        double oforestBmP = 0;
        double emissionsD_Bm = 0;
        double emissionsD_S = 0;
        double emissionsA_Bm = 0;
        double emissionsA_S = 0;
        double R0 = 0.01;                   // discount rate at time 0
        double kR = 1;                      // a coefficient for modification the discount rate
        double deforPrev = 0;
        double road = 0;
        double deforRateCoeffCell = 0;
        double harvRes_fcO = 0;//
        double harvRes_thO = 0;
        double harvRes_fcN = 0;
        double harvRes_thN = 0;
        double harvRes_scO = 0;
        double harvRes_scN = 0;
        double harvestGSold = 0;            // harvested growing stock (volume of harvested stem, including harvest losses per harvested area), m3/ha (old forest)
        double hDBHold = 0;                 // diameter of trees in the oldest age class (average weighted by growing stock) (old forest)
        double hHold = 0;                   // height of trees in the oldest age class (average weighted by growing stock)
        double harvestGSnew = 0;            // harvested growing stock (volume of harvested stem, including harvest losses), m3/ha (new forest)
        double hDBHnew = 0;                 // diameter of trees in the oldest age class (average weighted by growing stock) (new forest)
        double hHnew = 0;                   // height of trees in the oldest age class (average weighted by growing stock)
        double harvestFcO_M3HaL = 0;        // wood removals from final cut of old forest m3/ha of land area in the cell
        double harvestThO_M3HaL = 0;        // wood removals from thinning of old forest m3/ha of land area in the cell
        double harvestFcN_M3HaL = 0;        // wood removals from final cut of new forest m3/ha of land area in the cell
        double harvestThN_M3HaL = 0;        // wood removals from thinning of new forest m3/ha of land area in the cell
        double harvestFcO_M3C = 0;          // wood removals from final cut of old forest m3 in the cell
        double harvestThO_M3C = 0;          // wood removals from thinning of old forest m3 in the cell
        double harvestFcN_M3C = 0;          // wood removals from final cut of new forest m3 in the cell
        double harvestThN_M3C = 0;          // wood removals from thinning of new forest m3 in the cell

        double harvestFc10_M3C = 0;         // wood removals from final cut of old forest m3 in the cell
        double harvestTh10_M3C = 0;         // wood removals from thinning of old forest m3 in the cell
        double harvestFc30_M3C = 0;         // wood removals from final cut of old forest m3 in the cell
        double harvestTh30_M3C = 0;         // wood removals from thinning of old forest m3 in the cell
        double harvestConiferSc10_M3C = 0;
        double harvestConiferSc30_M3C = 0;

        double harvestConiferFcO_M3C = 0;   // wood removals from final cut of old coniferous forest m3 in the cell
        double harvestConiferThO_M3C = 0;   // wood removals from thinning of old coniferous forest m3 in the cell
        double harvestConiferFcN_M3C = 0;   // wood removals from final cut of new coniferous forest m3 in the cell
        double harvestConiferThN_M3C = 0;   // wood removals from thinning of new coniferous forest m3 in the cell
        double harvestBroadleafFcO_M3C = 0; // wood removals from final cut of old broadleaf forest m3 in the cell
        double harvestBroadleafThO_M3C = 0; // wood removals from thinning of old broadleaf forest m3 in the cell
        double harvestBroadleafFcN_M3C = 0; // wood removals from final cut of new broadleaf forest m3 in the cell
        double harvestBroadleafThN_M3C = 0; // wood removals from thinning of new broadleaf forest m3 in the cell
        double thDBHold = 0;                // GS weighted average of diameter of trees in age classes that are thinned
        double thHold = 0;                  // GS weighted average of height of trees in age classes that are thinned
        double thDBHnew = 0;
        double thHnew = 0;

        double harvestScM3Ha = 0;
        double harvestConiferScO_M3C = 0;   // wood removals from selective cut of old coniferous forest m3 in the cell
        double harvestConiferScN_M3C = 0;   // wood removals from selective cut of new coniferous forest m3 in the cell
        double harvestBroadleafScO_M3C = 0; // wood removals from selective cut of old broadleaf forest m3 in the cell
        double harvestBroadleafScN_M3C = 0; // wood removals from selective cut of new broadleaf forest m3 in the cell
        double harvestScO_M3HaL = 0;        // wood removals from selective cut of old (unused) forest m3/ha of land area in the cell
        double harvestScN_M3HaL = 0;        // wood removals from selective cut of new (unused) forest m3/ha of land area in the cell
        double harvestScO_M3C = 0;          // wood removals from selective cut of old (unused) forest m3 in the cell
        double harvestScN_M3C = 0;          // wood removals from selective cut of new (unused) forest m3 in the cell
        double harvestGSoldSc = 0;          // harvested growing stock (volume of harvested stem, including harvest losses), m3/ha (old unused forest)
        double hDBHoldSc = 0;               // diameter of trees in the oldest age class (average weighted by growing stock) (old unused forest)
        double hHoldSc = 0;                 // height of trees in the oldest age class (average weighted by growing stock in unused forest)
        double harvestGSnewSc = 0;          // harvested growing stock (volume of harvested stem, including harvest losses), m3/ha (new unused forest)
        double hDBHnewSc = 0;               // diameter of trees in the oldest age class (average weighted by growing stock) (new unused forest)
        double hHnewSc = 0;                 // height of trees in the oldest age class (average weighted by growing stock in unused)

        double thDBHoldSc = 0;              // GS weighted average of diameter of trees in age classes that are thinned in unused forest
        double thHoldSc = 0;                // GS weighted average of height of trees in age classes that are thinned in unused forest
        double thDBHnewSc = 0;
        double thHnewSc = 0;

        double defBiomass = 0;              // tC/per deforested ha (only stem!)
        double deforSW = 0;                 // // tC per deforested ha
        double deforRW = 0;                 // / defArea;  // tC per deforested ha
        double slashBurn = 0;               // share of burnt wood at deforestation

        int species = 0;

        bool SpeciesChangeSwitch = false;

        double deadwood = 0;                // dead wood in the old forest in the cell, tC/ha
        double litter = 0;                  // litter in the old forest in the cell, tC/ha
        double deadwood_new = 0;            // dead wood in the new forest in the cell, tC/ha
        double litter_new = 0;              // litter in the new forest in the cell, tC/ha

        double deadwood10 = 0;              // dead wood in the old forest in the cell, tC/ha
        double litter10 = 0;                // litter in the old forest in the cell, tC/ha
        double deadwood30 = 0;              // dead wood in the old forest in the cell, tC/ha
        double litter30 = 0;                // litter in the old forest in the cell, tC/ha
        double deadwoodP = 0;               // dead wood in the old forest in the cell, tC/ha
        double litterP = 0;                 // litter in the old forest in the cell, tC/ha

        double deadwood_in = 0;             // dead wood input flow in the old forest in the cell, tC/ha year
        double litter_in = 0;               // litter input flow in the old forest in the cell, tC/ha year
        double deadwood_new_in = 0;         // dead wood input flow in the new forest in the cell, tC/ha
        double litter_new_in = 0;           // litter input flow in the new forest in the cell, tC/ha

        double deadwood_in10 = 0;           // dead wood input flow in the old forest in the cell, tC/ha year
        double litter_in10 = 0;             // litter input flow in the old forest in the cell, tC/ha year
        double deadwood_in30 = 0;           // dead wood input flow in the old forest in the cell, tC/ha year
        double litter_in30 = 0;             // litter input flow in the old forest in the cell, tC/ha year
        double deadwood_inP = 0;            // dead wood input flow in the old forest in the cell, tC/ha year
        double litter_inP = 0;              // litter input flow in the old forest in the cell, tC/ha year

        double deadwood_em = 0;             // dead wood emissions in the old forest in the cell, tC/ha year
        double litter_em = 0;               // litter emissions in the old forest in the cell, tC/ha year
        double deadwood_new_em = 0;         // dead wood emissions in the new forest in the cell, tC/ha
        double litter_new_em = 0;           // litter emissions in the new forest in the cell, tC/ha

        double deadwood10_em = 0;           // dead wood emissions in the old forest in the cell, tC/ha year
        double litter10_em = 0;             // litter emissions in the old forest in the cell, tC/ha year
        double deadwood30_em = 0;           // dead wood emissions in the old forest in the cell, tC/ha year
        double litter30_em = 0;             // litter emissions in the old forest in the cell, tC/ha year
        double deadwoodP_em = 0;            // dead wood emissions in the primary forest in the cell, tC/ha year
        double litterP_em = 0;              // litter emissions in the primary forest in the cell, tC/ha year


        double forest10 = 0;                // additional 10% of old growth forests (OLDGROWTH_TEN - PRIMARY)
        double forest30 = 0;                // additional 30% of old growth forests (OLDGROWTH_THIRTY - OLDGROWTH_TEN)

        double extractedResidues = 0;       // The share of extracted harvest residues and harvest losses without RESIDUESUSESHARE
        double extractedStump = 0;          // The share of extracted stumps without RESIDUESUSESHARE
        double extractedCleaned = 0;        // The share of extracted cleaned stem-wood without (1-cleanedWoodUse) and RESIDUESUSESHARE

        double extractedResidues10 = 0;     // The share of extracted harvest residues and harvest losses without RESIDUESUSESHARE
        double extractedStump10 = 0;        // The share of extracted stumps without RESIDUESUSESHARE
        double extractedCleaned10 = 0;      // The share of extracted cleaned stem-wood without (1-cleanedWoodUse) and RESIDUESUSESHARE
        double extractedResidues30 = 0;     // The share of extracted harvest residues and harvest losses without RESIDUESUSESHARE
        double extractedStump30 = 0;        // The share of extracted stumps without RESIDUESUSESHARE
        double extractedCleaned30 = 0;      // The share of extracted cleaned stem-wood without (1-cleanedWoodUse) and RESIDUESUSESHARE

        double OAC = 0;                     // oldest age cluss in old "usual" forest
        double OAC_new = 0;                 // oldest age cluss in new "usual" forest
        double OAC10 = 0;                   // oldest age cluss in 10% policy forest
        double OAC30 = 0;                   // oldest age cluss in 30% policy forest
        double OACP = 0;                    // oldest age cluss in old-growth and primary forest

        double harvestEfficiencyMultifunction = 0;  // Gained (additional) efficiency of collecting wood in multifunctional forests
        double constructedRoadsMultifunction = 0;   // Additional constructed road in multifunctional forest
        double currentYearRoadInvestment = 0;       // Investment in forest road construction in current year

        int timeCounterDW = 0;              // Count time passed from changing the forest management from multifunctional to production
        int timeCounterDW10 = 0;            // Count time passed from changing the forest management from multifunctional to set-aside in 10% policy forest
        int timeCounterDW30 = 0;            // Count time passed from changing the forest management from multifunctional to production in 30% policy forest

        double burntDeadwoodU = 0;          // Burnt deadwood  due to fire damage, tC/ha year
        double burntDeadwood30 = 0;         // Burnt deadwood  due to fire damage, tC/ha year
        double burntDeadwood10 = 0;         // Burnt deadwood  due to fire damage, tC/ha year
        double burntDeadwoodNew = 0;        // Burnt deadwood  due to fire damage, tC/ha year
        double burntDeadwoodP = 0;          // Burnt deadwood  due to fire damage, tC/ha year
        double burntLitterU = 0;            // Burnt  litter due to fire damage, tC/ha year
        double burntLitter30 = 0;           // Burnt  litter due to fire damage, tC/ha year
        double burntLitter10 = 0;           // Burnt  litter due to fire damage, tC/ha year
        double burntLitterNew = 0;          // Burnt  litter due to fire damage, tC/ha year
        double burntLitterP = 0;            // Burnt  litter due to fire damage, tC/ha year
    };
}

#endif
