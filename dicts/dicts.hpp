#ifndef G4M_EUROPE_DG_DICTS_HPP
#define G4M_EUROPE_DG_DICTS_HPP

#include "dicts_project_dependent.hpp"
#include "dicts_project_independent.hpp"

using namespace std;

namespace g4m::Dicts {
    unordered_set<uint16_t> years = yearsToConsider(1990, 2070);  // select years for results output
    unordered_set<uint8_t> countryRegList = countryRegionsToConsider();    // country and region mixture to be considered
    unordered_set<uint8_t> countriesFmCPol = countriesFmCPolToConsider();   // List of Annex-1 countries for FMCPol
    unordered_set<uint8_t> regions = regionsToConsider();
    unordered_set<uint8_t> countriesList = countriesToConsider();     // country to be considered

    // pairs of G4M country ID and GLOBIOM country name
    unordered_map<uint8_t, string> idCountryGLOBIOM = setIdCountryGLOBIOM();
    unordered_map<string, uint8_t, string_hash, equal_to<> > countryGLOBIOMId = setCountryGLOBIOMId();

    unordered_map<Species, IncrementTab, EnumHasher> species = defineSpecies();
    // countries and species where the stumps can be harvested as compiled by Fulvio 06.06
    set<pair<uint8_t, Species> > stumpHarvCountrySpecies = setCountrySpecies();  // [country, species]
}

#endif
