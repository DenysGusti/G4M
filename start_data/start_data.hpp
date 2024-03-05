#ifndef G4M_EUROPE_DG_START_DATA_HPP
#define G4M_EUROPE_DG_START_DATA_HPP

#include <array>
#include <unordered_set>
#include <vector>
#include <filesystem>
#include <ranges>
#include <optional>
#include <memory>
#include <semaphore>

#include "../init/country_data.hpp"
#include "../init/data_struct.hpp"
#include "../init/data_grid.hpp"

#include "../helper/string_hash.hpp"

#include "../settings/constants.hpp"
#include "../settings/arrays/arrays.hpp"
#include "../log.hpp"
#include "../increment/increment_tab.hpp"

#include "../increment/age_struct.hpp"
#include "../structs/dat.hpp"
#include "../structs/harvest_residues.hpp"

#include "../GLOBIOM_scenarios_data/datamaps/datamap_scenarios.hpp"
#include "../GLOBIOM_scenarios_data/simu_ids/simu_id_scenarios.hpp"
#include "../GLOBIOM_scenarios_data/plots.hpp"
#include "../GLOBIOM_scenarios_data/NUTS2.hpp"
#include "../GLOBIOM_scenarios_data/age_struct_data.hpp"
#include "../GLOBIOM_scenarios_data/bau_scenarios.hpp"

#include "../parameters/FMP.hpp"

using namespace std;
using namespace g4m::helper;
using namespace g4m::init;
using namespace g4m::structs;
using namespace g4m::increment;
using namespace g4m::Arrays;
using namespace g4m::Constants;
using namespace g4m::GLOBIOM_scenarios_data;
using namespace g4m::parameters;

namespace fs = filesystem;
namespace rv = ranges::views;

namespace g4m::StartData {
    // Historic data on total wood production in EU countries (including Croatia) [th.m3/year, over bark); 1990-2010
//    CountryData countriesWoodProdStat;
    CountryData countriesFmEmission_unfccc;

    FMP fmp;

    Plots plots;
    NUTS2 nuts2;
    DatamapScenarios datamapScenarios;
    SimuIdScenarios simuIdScenarios;
    BauScenarios bauScenarios;
    AgeStructData asd;

    const size_t resLatitude = lround(180 / gridStep);

    // Initializing forest cover array by gridcells
    DataGrid<double> commonHarvestGrid{resLatitude};
    DataGrid<double> commonMaiForest{resLatitude};
    DataGrid<double> commonRotationForest{resLatitude};
    DataGrid<double> commonThinningForest{resLatitude};
    DataGrid<double> commonThinningForest10{resLatitude};
    DataGrid<double> commonThinningForest30{resLatitude};
    DataGrid<double> commonOForestShGrid{resLatitude};

    DataGrid <int8_t> commonDecisionGrid{resLatitude};
    DataGrid <int8_t> commonManagedForest{resLatitude};
    DataGrid <int8_t> commonManageChForest{resLatitude};
    DataGrid <int8_t> commonRotationType{resLatitude};
    DataGrid <int8_t> commonUnmanaged{resLatitude};

    vector<AgeStruct> commonCohortsU;
    vector<AgeStruct> commonCohortsN;
    vector<AgeStruct> commonCohorts10;
    vector<AgeStruct> commonCohorts30;
    vector<AgeStruct> commonCohortsP;

    vector<Dat> commonDats;

    // data related to harvesting the residues (residues amount, associated costs and soil loss emissions)
    // a vector of harvest residues data for all single countries
    unordered_map<uint8_t, vector<HarvestResidues> > commonHarvestResiduesCountry;

    unordered_map<string, binary_semaphore, StringHash, equal_to<> > signalZeroCToMainScenarios;
}

#endif
