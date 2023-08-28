#ifndef G4M_EUROPE_DG_START_DATA_HPP
#define G4M_EUROPE_DG_START_DATA_HPP

#include <array>
#include <unordered_set>
#include <vector>
#include <filesystem>
#include <ranges>
#include <optional>

#include "../init/settings.hpp"
#include "../init/country_species.hpp"
#include "../init/country_data.hpp"
#include "../init/coef_struct.hpp"
#include "../init/data_struct.hpp"
#include "../init/data_grid.hpp"

#include "../misc/concrete/ipol.hpp"

#include "../diagnostics/heterogeneous_lookup.hpp"

#include "../constants.hpp"
#include "../arrays/arrays.hpp"
#include "../log.hpp"

using namespace std;

using namespace g4m::init;
using namespace g4m::Arrays;
using namespace g4m::Constants;
namespace fs = filesystem;
namespace rv = ranges::views;

namespace g4m::StartData {
    // runtime
    Settings settings;
    CoefStruct coef;

    unordered_set <uint16_t> years;  // select years for results output
    unordered_set <uint8_t> regions;
    unordered_set <uint8_t> countryRegList;    // country and region mixture to be considered
    unordered_set <uint8_t> countriesFmcpol;   // List of Annex-1 countries for FMcpol
    unordered_set <uint8_t> countriesList;     // country to be considered

    CountryData countriesNforCover;
    CountryData countriesNforTotC;
    CountryData countriesAfforHaYear;
    CountryData countriesAfforCYear;
    CountryData countriesAfforCYear_ab;
    CountryData countriesAfforCYear_bl;
    CountryData countriesAfforCYear_biom;
    CountryData countriesAfforCYear_dom;
    CountryData countriesAfforCYear_soil;
    CountryData countriesOforCover; // all old forest (existing in 2000)
    CountryData countriesDeforHaYear;
    CountryData countriesOfor_ab_C;
    CountryData countriesOforC_biom;
    CountryData countriesDeforCYear;
    CountryData countriesDeforCYear_bl;
    CountryData countriesDeforCYear_ab;
    CountryData countriesDeforCYear_biom;
    CountryData countriesDeforCYear_dom;
    CountryData countriesDeforCYear_soil;
    CountryData countriesWoodHarvestM3Year;
    CountryData countriesWoodHarvestPlusM3Year;
    CountryData countriesWoodHarvestFmM3Year;
    CountryData countriesWoodHarvestDfM3Year;
    CountryData countriesWoodLoosCYear;
    CountryData countriesHarvLossesYear;
    CountryData countriesManagedForHa;
    CountryData countriesManagedCount;
    CountryData countriesMAI;
    CountryData countriesCAI;
    CountryData countriesCAI_new;
    CountryData countriesFM;
    CountryData countriesFMbm;
    CountryData countriesProfit;  // profit due to selling  harvested wood
    // Historic data on total wood production in EU countries (including Croatia) [th.m3/year, over bark); 1990-2010
    CountryData countriesWoodProdStat;
    CountryData countriesfmEmission_unfccc;

    vector<CountrySpecies> stumpHarvCountrySpecies;  // countries and species where the stumps can be harvested as compiled by Fulvio 06.06
    // pairs of G4M country ID and GLOBIOM country name
    unordered_map<uint8_t, string> idCountryGLOBIOM;
    unordered_map<string, uint8_t, string_hash, equal_to<>> countryGLOBIOMId;

    vector<DataStruct> plots;  // structure with data plots[<elNum>].<variable>[year]
    map<pair<size_t, size_t >, string> nuts2id; // x,y,nuts2 (pair has no build hash, O(log(n)) look-up)

    using datamapType = unordered_map<uint8_t, Ipol < double>>;
    using heterDatamapScenariosType = unordered_map<string, datamapType, string_hash, equal_to<>>;

    datamapType histLandPrice;  // datamap for land price corrections for current price scenario (GLOBIOM)
    datamapType histWoodPrice; // datamap for wood price corrections for current price scenario (GLOBIOM)
    datamapType histWoodDemand;  // datamap for wood demand (GLOBIOM)
    datamapType histResiduesDemand;  // datamap for residues demand (GLOBIOM)

    heterDatamapScenariosType globiomAfforMaxCountryScenarios;  // Country maximum allowed afforestation estimated from GLOBIOM natural land (kha)
    heterDatamapScenariosType globiomLandCountryScenarios;  // Country GLOBIOM land (kha) reserved for croplands, pastures and short rotation plantations (also wetlands and infrastructure)

    heterDatamapScenariosType landPriceScenarios;
    heterDatamapScenariosType woodPriceScenarios;
    heterDatamapScenariosType woodDemandScenarios;
    heterDatamapScenariosType residuesDemandScenarios;

    heterDatamapScenariosType CO2PriceScenarios;

    DataGrid<string> nuts2grid;
}

#endif
