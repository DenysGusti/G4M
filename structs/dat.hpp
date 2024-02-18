#ifndef G4M_EUROPE_DG_DAT_HPP
#define G4M_EUROPE_DG_DAT_HPP

#include <array>

#include "cell_forest.hpp"
#include "../init/species.hpp"
#include "../init/data_struct.hpp"

using namespace std;

using namespace g4m::init;

namespace g4m::structs {

    struct Dat {
        static constexpr size_t transitionPeriod = 211;
        double rotation = 0;
        double landAreaHa = 0;

        double forestShare = 0;
        double forestShare0 = 0;
        double OForestShare = 0;            // current forest share of old forest in each cell

        double prevOForShare = 0;           // forest share of all old forest one modelling step back in each cell

        CellForest U;                       // usual old forest
        CellForest N;                       // new forest
        CellForest O10;                     // old forest 10% policy
        CellForest O30;                     // old forest 30% policy
        CellForest P;                       // primary old-growth forest

        Species species = Species::NoTree;

        double harvestEfficiencyMultifunction = 0;  // Gained (additional) efficiency of collecting wood in multifunctional forests
        double constructedRoadsMultifunction = 0;   // Additional constructed road in multifunctional forest
        double currentYearRoadInvestment = 0;       // Investment in forest road construction in current year

        double savedCarbonPrev = 0;
        double gainedCarbonPrev = 0;
        double emissionsTotPrev = 0;
        double emissionsAfforPrev = 0;
        double prevPlantPhytHaBmGr = 0;     // new forest stem wood, tC/(cell area)
        double prevPlantPhytHaBmGrBef = 0;  // new forest above-ground biomass, tC/(cell area)
        double prevPlantPhytHaBmGrBef20b = 0;
        double prevPlantPhytHaBmGrBef20o = 0;
        array<double, transitionPeriod> prevPlantPhytHaBmGrBef20b_age{};
        array<double, transitionPeriod> prevPlantPhytHaBmGrBef20o_age{};
        double prevPlantPhytHaBlGr = 0;
        double deforestHaTot = 0;
        double afforestHaTot = 0;
        double emissionsProduct = 0;
        double emissionsLitter = 0;
        double emissionsSOC = 0;
        double emissionsSlashBurn = 0;
        double emissionsDeadBurn = 0;
        double emissionsCRootBurn = 0;
        double emissionsTot = 0;
        double EmLitterAffor = 0;
        double EmSOCAffor = 0;
        double emissionsAffor = 0;
        array<double, transitionPeriod> forestAgeShare{};
        array<double, transitionPeriod> bDeadA{};
        array<double, transitionPeriod> litterA{};
        array<double, transitionPeriod> SOCA{};
        array<double, transitionPeriod> SOCA1{};
        array<double, transitionPeriod> prodLongA{};
        array<double, transitionPeriod> prodShortA{};
        array<double, transitionPeriod> deforestA{};
        array<double, transitionPeriod> fineRootA{};
        array<double, transitionPeriod> litterAffor{};
        array<double, transitionPeriod> SOCAffor{};

        double abBiomassTotPrev = 0;
        double rotBiomass = 0;
        int iReportYear = 0;
        double stockingDegree = 0;
        double FMSink = 0;
        double FMSink_Bm = 0;
        double OBiomass0 = 0;               // modelled biomass at time 0
        double deforWoodTotM3 = 0;          // stem wood obtained from deforestation averaged for last 5 years
        array<double, 5> deforWoodArray{};  // stem wood obtained from deforestation for last 5 years
        array<double, 5> deforAreaArray{};  // deforested area for the last 5 years
        double deforestShare = 0;
        double afforestHaYear = 0;
        double deforestHaYear = 0;
        double harvestTot_all = 0;
        double harvestFcM3Ha = 0;
        double harvestThM3Ha = 0;
        double emissionsD_Bm = 0;
        double emissionsD_S = 0;
        double emissionsA_Bm = 0;
        double emissionsA_S = 0;
        double deforPrev = 0;
        double road = 0;

        double defBiomass = 0;              // tC per deforested ha (only stem!)
        double deforSW = 0;                 // tC per deforested ha
        double deforRW = 0;                 // defArea tC per deforested ha
        double slashBurn = 0;               // share of burnt wood at deforestation

        Dat() = default;

        Dat(const DataStruct &plot, const double rotation_, const double abBiomass0, const double cohort10Bm,
            const double cohort30Bm, const double cohortPrimaryBm, const double thinning) {
            rotation = rotation_;
            landAreaHa = plot.landArea * 100;
            forestShare = plot.getForestShare();
            forestShare0 = forestShare;
            OForestShare = forestShare;
            U.forestShare = plot.forest;
            O10.forestShare = plot.oldGrowthForest_ten;
            O30.forestShare = plot.oldGrowthForest_thirty;
            prevOForShare = OForestShare;       // forest share of all old forest one modelling step back in each cell
            U.prevForestShare = U.forestShare;     // forest share of "usual" old forest one modelling step back in each cell
            O10.prevForestShare = O10.forestShare;  // forest share of 10% policy forest one modelling step back in each cell
            O30.prevForestShare = O30.forestShare;  // forest share of 30% policy forest one modelling step back in each cell
            P.prevForestShare = forestShare;      // MG: Old forest share in the previous reporting year
            U.prevStemBiomass = abBiomass0;
            OBiomass0 = abBiomass0;             // Modelled biomass at time 0, tC/ha
            O10.prevStemBiomass = cohort10Bm;        // biomass of 10% policy forest on a previous step, tC/ha
            O30.prevStemBiomass = cohort30Bm;        // biomass of 30% policy forest on a previous step, tC/ha
            P.prevStemBiomass = cohortPrimaryBm;    // biomass of primary forest on a previous step, tC/ha
            U.totalBiomass = abBiomass0;
            O10.totalBiomass = O10.prevStemBiomass;
            O30.totalBiomass = O30.prevStemBiomass;
            P.totalBiomass = P.prevStemBiomass;
            rotBiomass = rotation;
            stockingDegree = thinning;
            species = plot.speciesType;
            deforPrev = plot.forLoss;
            road = plot.road.data.at(2000);
            slashBurn = plot.slashBurn.data.at(2000);
            U.deadwood = plot.forest > 0 ? plot.deadWood : 0;
            O10.deadwood = plot.oldGrowthForest_ten > 0 ? plot.deadWood : 0;
            O30.deadwood = plot.oldGrowthForest_thirty > 0 ? plot.deadWood : 0;
            P.deadwood = plot.strictProtected > 0 ? plot.deadWood : 0;
        }
    };
}

#endif
