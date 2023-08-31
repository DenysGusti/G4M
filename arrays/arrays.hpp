#ifndef G4M_EUROPE_DG_COMPILED_ARRAYS_HPP
#define G4M_EUROPE_DG_COMPILED_ARRAYS_HPP

#include <array>
#include <string>

#include "../constants.hpp"
#include "arrays_project_independent.hpp"
#include "arrays_project_dependent.hpp"

using namespace std;

using namespace g4m::Constants;

namespace g4m::Arrays {

    // compile time
    constexpr array<uint8_t, numberOfCountries> countryCodeOrder = initCountryCodeOrder();
    constexpr array<uint8_t, numberOfCountries> countryNwp = initCountryNwp();
    constexpr array<uint8_t, numberOfCountries> countryRegion = initCountryRegion();

    constexpr array<string_view, numberOfCountries> countryOrderISO = initCountryOrderISO();
    constexpr array<string_view, numberOfCountries> countryOrderISO2 = initCountryOrderISO2();
    constexpr array<string_view, numberOfCountriesRegMix> countryRegName = initCountryRegName();
    constexpr array<string_view, numberOfCountries> countryOrderName = initCountryOrderName();

    constexpr array<double, numberOfCountries> countryLosses = initCountryLosses();
    constexpr array<double, numberOfCountries> cleanedWoodUse = initCleanedWoodUse();  // share of cleaned  wood put to the market(thinned in forests that are not used for wood production in current year).
    constexpr array<double, numberOfCountries> countryLossesD = initCountryLossesD();
    constexpr array<double, numberOfCountries> defor_sl_em_unfccc_tCha = initDefor_sl_em_unfccc_tCha(); // deforestation soil emissions, tC/ha year, derived from countries; UNFCCC CRF tables 2020 submission; 1990-2018 average; DE, NO, SW had negative emissions, changed to 0.01
    constexpr array<double, numberOfCountries> defor_sl_em_unfccc_MtCO2 = initDefor_sl_em_unfccc_MtCO2(); // deforestation soil emissions, MtCO2/year, derived from countries; UNFCCC CRF tables 2020 submission; 2005-2018 average;
    constexpr array<double, numberOfCountries> defor_bm_em_unfccc_MtCO2 = initDefor_bm_em_unfccc_MtCO2(); // deforestation biomass emissions, MtCO2/year, derived from countries; UNFCCC CRF tables 2020 submission; 2005-2018 average;
    constexpr array<double, numberOfCountries> defor_dom_em_unfccc_MtCO2 = initDefor_dom_em_unfccc_MtCO2(); // deforestation DOM emissions, MtCO2/year, derived from countries; UNFCCC CRF tables 2020 submission; 2005-2018 average;
    constexpr array<double, numberOfCountries> harvestedWoodUse = initHarvestedWoodUse();
    constexpr array<double, numberOfCountries> maiCoefficients = initMaiCoefficients();

    constinit array<double, numberOfCountries> FM_sink_stat = initFM_sink_stat();
    constinit array<double, numberOfCountries> countryLandArea = initCountryLandArea();  // country terrestrial land area, kha, read in from the GLOBIOM country LC file
    constinit array<double, numberOfCountries> MAI_CountryUprotect = initMAI_CountryUprotect();

    constexpr array<uint8_t, 30> eu28OrderCode = initEu28OrderCode();  // country G4M code for EU28 countries + Norway + Switzerland for the FM sink data from the UNFCC
    constexpr array<uint8_t, 42> annex1OrderCode = initAnnex1OrderCode(); // country G4M code for the Annex-1 countries the FM sink data from the UNFCC
    constexpr array<uint16_t, 13> yearLP = initYearLP();
    constexpr array<uint16_t, 34> yearNwp = initYearNwp();

    constexpr array<array<double, 21>, 29> woodProdEUStats = initWoodProdEUStat();
    constexpr array<array<double, 32>, 30> fmEmission_unfccc_CRF = init_fmEmission_unfccc_CRF();
}

#endif
