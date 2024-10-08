#ifndef G4M_EUROPE_DG_VARS_HPP
#define G4M_EUROPE_DG_VARS_HPP

#include <string>
#include "../helper/string_hash.hpp"

using namespace std;

namespace g4m::Constants {
    constexpr size_t numberOfCountries = 247;  // new country codes  // 18.07.2021 Added Kosovo
    constexpr size_t numberOfCountriesRegMix = 51;  // number of POLES regions mixed with EU27 countries
    constexpr size_t adjustLength = 31;  // 21.03.2023: a period over which G4M adjusts average FM emissions (31=2020-1990+1)
    constexpr uint16_t refYear = 2023; // C-tax policies start the next year // EUCLIMIT 2022:

    // resolution of model
    constexpr double gridStep = 0.5;    // step by latitude and longitude

    constexpr uint32_t modTimeStep = 1;

    // t4 setup; New baseline; Reduced hurdle 0.9
    // 22.06.2023: defShare_correction is introduced (calc v17)
    // 16.06.2023: Hurdle for BE, CY, DE, FI and NL is returned to 0
    const string suffix = "_EUCLIMIT6_CTP_S2_CC_04072023_CRF2023_DisturbNoScaleClimTrendCS_climNoScale_";

    constexpr bool debugWithoutBigFiles = false;

    constexpr bool scaleMAIClimate = true;      // Scaling the MAI climate shifters to the 2020 value (i.e., MAIShifter_year = MAIShifter_year/MAIShifter_2000, so the 2000 value = 1);

    constexpr bool disturbanceClimateSensitive = false; // Do the disturbances depend on the RCP scenario? If True the damaged wood doubles for RCP7 and 2.5 for the RCP8. Works with disturbanceTrend = true;
    constexpr bool scaleDisturbance2020 = false;

    constexpr bool protectedNatLnd = true;  // Don't afforest Natura2000 natural vegetation and heathland to be protected in 2030 (only EU27); Check if GRLND_PROTECT is available in input data
    constexpr bool zeroProdAreaInit = true; // Let G4M to set up minimum necessary forest area used for satisfying wood demand

    constexpr double sdMinCoef = 1; // minimum stocking degree multiplier: sdMin = SD * sdMinCoef
    constexpr double sdMaxCoef = 1; // maximum stocking degree multiplier: sdMax = SD * sdMaxCoef

    constexpr double deflator = 0.8807;  // Deflator to convert 2000 USD prices to 1995 USD prices
    constexpr double molarRatio = 44. / 12.;  // M(CO2) / M(C)

    // policies before forPolicyYearBioclima
    constexpr bool commonForest10Policy = false;  // Policy on protecting 10% of EU forests (BIOCLIMA)
    constexpr bool commonForest30Policy = false;  // Policy on limiting the use of 30% (10% of strict protection and 20% of limited use) of EU forests (BIOCLIMA)
    constexpr bool commonMultifunction10 = false;  // Apply multifunctional forest management to the 10% forest
    constexpr bool commonMultifunction30 = false;  // Apply multifunctional forest management to the 30% forest

    constexpr uint16_t forPolicyYearBioClima = 2025;  // Nature restoration policies start this year / BIOCLIMA1 and EUCLIMIT6

    // policies after forPolicyYearBioClima
    constexpr bool commonForest10PolicyKey = true; // true == set-aside
    constexpr bool commonForest30PolicyKey = false;
    constexpr bool commonMultifunction10Key = false;
    constexpr bool commonMultifunction30Key = false;

    constexpr double cleanWoodUseShare10 = 0;  // A share of removal of wood from multifunctional forests under the 10% policy
    constexpr double cleanWoodUseShare30 = 1;  // A share of removal of wood from multifunctional forests under the 30% policy

    constexpr bool disturbanceTrend = true;     // Do we take into account the trend in increasing damage from disturbances? If true, use the disturbance projections input data
    constexpr bool disturbanceExtreme = true;   // Do we simulate the worst historical disturbance in disturbanceExtremeYear?

    constexpr uint16_t disturbanceExtremeYear = 2035;  // When the worst historical disturbance is projected

    constexpr uint16_t residueHarvYear = 1999;  // Extraction of residues start next year

    constexpr bool fmPol = true; // For testing FM response to C price incentive; requires bin files with BAU biomass and NPV

    constexpr bool binFilesOnDisk = false;  // bin files are already on disk, no need for generation in 0 C price app

    constexpr double priceIndexAvgEU27 = 2.8234442; //Average price index for the EU27, to be used with plot.priceIndex
    constexpr double scaleCostsFactorEs = 0.2; // costs adjustment for Estonia estimated such as to allow historical harvest
    constexpr double damageRiscCostsYear = 1;  //0.01; // costs that probably can be lost due to disturbances if rotation is extended, used in forNPV: damageRiscCostsYear*(MAIRot-Rot)^2

    constexpr double meanRoadDensityMultifunctional = 13.4;  // Mean forest road density in multifunctional forests [Recommendations for multifunctional forest management strategies. ARANGE project Deliverable D5.2, 2015]
    constexpr double meanRoadDensityProduction = 40;  // Average road density in production (commercial) forests in Austria Ref??
    constexpr double meanOptimalRoadDensityMultifunctional = 20.2;//Optimal forest road density in multifunctional forests [Recommendations for multifunctional forest management strategies. ARANGE project Deliverable D5.2, 2015]

    constexpr double exchangeRate = 1; // EUCLIMIT2; Euro -> USD exchange rate average for 2008 (all prices in the model are in USD)
    // Average costs of forest road construction Euro/m;
    // European Forest Institute, the average cost of forest road construction in Europe was 15,000€ per km in 2005
    // https://www.researchgate.net/publication/280528960_Forest_Land_Ownership_Change_in_Europe_COST_Action_FP1201_FACESMAP_Country_Reports_Joint_Volume
    constexpr double forestRoadConstructionCostsEuroM = 15 * exchangeRate * deflator;

    constexpr double tolerance = 0.05;  // 0.1; // tolerance of forest management adjustment to match domestic wood demand 11.05.2023
    constexpr double npvLoss = 0.25;    // 0.03 CHECK for SLOVENIA; share of NPV loss that we allow when adjusting RT 11.05.2023

    constexpr bool disturbanceSwitch = true;    // Switch simulation of the disturbance event in CZ

    // Adjusting (increasing) FM sink by disturbance management in "unmanaged" forests
    constexpr bool adjustFMSink = true; // switch ON adjusting of Rotation to better match the UNFCCC FM emissions

    constexpr bool cellInteract = true;
}

#endif
