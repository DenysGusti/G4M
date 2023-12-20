#ifndef G4M_EUROPE_DG_DICTS_HPP
#define G4M_EUROPE_DG_DICTS_HPP

#include "dicts_project_dependent.hpp"
#include "dicts_project_independent.hpp"

using namespace std;

namespace g4m::Dicts {
    const unordered_set<uint16_t> years = yearsToConsider(1990, 2070);  // select years for results output
    const unordered_set<uint8_t> countryRegList = countryRegionsToConsider();    // country and region mixture to be considered
    const unordered_set<uint8_t> countriesFmCPol = countriesFmCPolToConsider();   // List of Annex-1 countries for FMCPol
    const unordered_set<uint8_t> regions = regionsToConsider();
    const unordered_set<uint8_t> countriesList = countriesToConsider();     // country to be considered

    // pairs of G4M country ID and GLOBIOM country name
    const unordered_map<uint8_t, string> idCountryGLOBIOM = setIdCountryGLOBIOM();
    const unordered_map<string, uint8_t, StringHash, equal_to<> > countryGLOBIOMId = setCountryGLOBIOMId();

    // non-const in order to use pointer in AgeStruct
    unordered_map<Species, IncrementTab, EnumHasher> species = defineSpecies();
    // countries and species where the stumps can be harvested as compiled by Fulvio 06.06
    const set<pair<uint8_t, Species> > stumpHarvCountrySpecies = setCountrySpecies();  // [country, species]
}

#endif
