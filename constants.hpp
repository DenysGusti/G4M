#ifndef G4M_EUROPE_DG_VARS_HPP
#define G4M_EUROPE_DG_VARS_HPP

#include <string>

using namespace std;

namespace g4m::Constants {
    constexpr size_t numberOfCountries = 247;  // new country codes  // 18.07.2021 Added Kosovo
    constexpr size_t numberOfCountriesRegMix = 51;  // number of POLES regions mixed with EU27 countries
    constexpr size_t adjustLength = 31;  // 21.03.2023: a period over which G4M adjusts average FM emissions (31=2020-1990+1)
    constexpr uint16_t refYear = 2023; // C-tax policies start the next year // EUCLIMIT 2022:

    // resolution of model
    constexpr double gridStep = 0.5;    // step by latitude and longitude

    constexpr uint32_t modTimeStep = 1;

    constexpr string_view fileName_dat = "b2_euclimit6_bioclima_feu2020_nai2010_11052023.csv";  // as _30032023 but unfccc managed forest area increased for a number of countries
    constexpr string_view suffix = "_EUCLIMIT6_CTP_S2_CC_04072023_CRF2023_DisturbNoScaleClimTrendCS_climNoScale_"; // t4 setup; New baseline; // Reduced hurdle 0.9 // 22.06.2023: defShare_correction is introduced (calc v17) // 16.06.2023: Hurdle for BE, CY, DE, FI and NL is returned to o
    constexpr string_view bauScenario = "primes_ref_2020_REFERENCE_REFERENCE";

    constexpr string_view fileName_lp0 = "output_glo4myk_LandRent_myk_01042023.csv";
    constexpr string_view fileName_wp0 = "output_glo4myk_SupplyWood_myk_price_01042023.csv";
    constexpr string_view fileName_wd0 = "output_glo4myk_SupplyWood_CntDet_01042023.csv"; // 18 Jan 2021 NL corrected according to NFAP / a share of wood reported to FAOSTAT comes from non-forest sources (communication with NL experts in Dec 2020)
    constexpr string_view fileName_rd0 = "output_glo4myk_Residues_myk_01042023.csv"; // in m3

    constexpr string_view fileName_lp = "output_glo4myk_LandRent_myk_04072023.csv";
    constexpr string_view fileName_wp = "output_glo4myk_SupplyWood_myk_price_04072023.csv";
    constexpr string_view fileName_wd = "output_glo4myk_SupplyWood_CntDet_04072023.csv";
    constexpr string_view fileName_rd = "output_glo4myk_Residues_myk_04072023.csv"; // in th t

    // could be empty
    constexpr string_view fileName_gl_0 = "GLOBIOM_LC_MCS_BIOCLIMA_GRSCor9_devANBEXT_02052023_newClasses_updated_2000_2020.csv"; // GLOBIOM land cell scale
    constexpr string_view fileName_gl = "GLOBIOM_LC_MCS_BIOCLIMA_GRSCor9_FinJune11_11062023_newClasses_updated_CORRECTED.csv"; // GLOBIOM land cell scale
    constexpr string_view fileName_gl_country_0 = "GLOBIOM_LC_CountryLevel_Ref2070_05042023.csv"; // GLOBIOM land country scale
    constexpr string_view fileName_gl_country = "GLOBIOM_LC_CountryLevel_CTP_S2_CC_04072023_04072023.csv"; // GLOBIOM land country scale

    constexpr string_view fileName_co2p = "output_glo4myk_CO2price_myk_04072023.csv";
    constexpr string_view fileName_nuts2 = "nuts2_xy_05.csv";

    constexpr string_view fileName_maic = "shifters_g4m_v2.csv";  // MAI climate shifters
    constexpr string_view fileName_disturbance = "disturbances_g4m_m3ha_29062023.csv";  // disturbance damage
    constexpr string_view fileName_disturbanceExtreme = "disturbances_g4m_m3ha_29062023.csv";  // disturbance damage

    constexpr string_view fileName_ageStruct = "ageStructData_EUCLIMIT2020_15092022.csv";  // As ageStructData_EUCLIMIT2020_03012021, but IR and DE are corrected according to the countries' comments in 2022

    constexpr bool MAIClimateShift = true;      // Apply the MAI climate shifters starting from MAIClimateShiftYear
    constexpr bool scaleMAIClimate = true;      // Scaling the MAI climate shifters to the 2020 value (i.e., MAIShifter_year = MAIShifter_year/MAIShifter_2000, so the 2000 value = 1);
    constexpr bool disturbanceTrend = true;     // Do we take into account the trend in increasing damage from disturbances? If true, use the disturbance projections input data
    constexpr bool disturbanceExtreme = true;   // Do we simulate the worst historical disturbance in disturbanceExtremeYear?

    constexpr bool disturbanceClimateSensitive = true; // Do the disturbances depend on the RCP scenario? If True the damaged wood doubles for RCP7 and 2.5 for the RCP8. Works with disturbanceTrend = true;
    constexpr bool scaleDisturbance2020 = true;

    constexpr bool protectedNatLnd = true;  // Don't afforest Natura2000 natural vegetation and heathland to be protected in 2030 (only EU27); Check if GRLND_PROTECT is available in input data
    constexpr bool zeroProdAreaInit = true; // Let G4M to set up minimum necessary forest area used for satisfying wood demand

    //Get optimal rotation time (getTOpt and getTOptT)
    constexpr int optimMAI = 0;     // Highest average increment
    constexpr int optimMaxBm = 1;   // Maximum average Biomass
    constexpr int optimMaxAge = 2;  // Highest possible age
    constexpr int optimHarvFin = 3; // Maximum harvest at final cut
    constexpr int optimHarvAve = 4; // Average Maximum harvest at final cut

    constexpr double sdMinCoef = 1; // minimum stocking degree multiplier: sdMin = SD * sdMinCoef
    constexpr double sdMaxCoef = 1; // maximum stocking degree multiplier: sdMax = SD * sdMaxCoef

//    constexpr bool fmpol = true; // For testing FM response to C price incentive; requires bin files with BAU biomass and NPV

//    constexpr bool forest10_policyKey = true;   // true == set-aside
//    constexpr bool forest30_policyKey = false;
//    constexpr bool multifunction10Key = false;
//    constexpr bool multifunction30Key = false;
}

#endif
