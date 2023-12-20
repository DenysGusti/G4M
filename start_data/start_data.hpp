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
#include "../init/ffipols_country.hpp"

#include "../misc/concrete/ipol.hpp"
#include "../misc/concrete/ipolm.hpp"
#include "../misc/concrete/ffipol.hpp"

#include "../helper/string_hash.hpp"

#include "../constants.hpp"
#include "../arrays/arrays.hpp"
#include "../log.hpp"
#include "../increment/increment_tab.hpp"

#include "../increment/age_struct.hpp"
#include "../structs/dat.hpp"
#include "../structs/harvest_residues.hpp"

#include "../GLOBIOM_scenarios_data/datamaps/datamap_scenarios.hpp"

using namespace std;
using namespace g4m::helper;
using namespace g4m::init;
using namespace g4m::structs;
using namespace g4m::increment;
using namespace g4m::Arrays;
using namespace g4m::Constants;
using namespace g4m::GLOBIOM_scenarios_data;

namespace fs = filesystem;
namespace rv = ranges::views;

namespace g4m::StartData {
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
    CountryData countriesFmEmission_unfccc;

    vector<DataStruct> rawPlots;  // raw structure with data plots[<elNum>].<variable>[year]
    vector<DataStruct> commonPlots;  // structure with data plots[<elNum>].<variable>[year]

    // for quick plots search
    unordered_set <uint32_t> plotsSimuID;
    map <pair<uint32_t, uint32_t>, uint32_t> plotsXY_SimuID;  // <[x, y], simuID>

    map <pair<uint32_t, uint32_t>, string> nuts2id; // x,y,nuts2 (pair has no build hash, O(log(n)) look-up)

    DatamapScenarios datamapScenarios;

    DataGrid <string> nuts2grid;

    Ipol<double> sws;       // Schnittholzanteil an Vfm
    Ipol<double> hlv;       // 1-Ernteverluste Vornutzung
    Ipol<double> hle;       // 1-Ernteverluste Endnutzung
    IpolM<double> cov;      // costs Vornutzung
    IpolM<double> coe;      // costs Endnutzung
    IpolM<double> dov;      // do Vornutzung
    IpolM<double> doe;      // do Endnutzung
    Ipol<double> sdMaxH;    // sdMaxH
    Ipol<double> sdMinH;    // sdMinH

    FFIpol<double> ffsws; // sawn-wood share of harvested wood depending on dbh
    FFIpol<double> ffhlv; // 1-harvesting losses thinning (Vornutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
    FFIpol<double> ffhle; // 1-harvesting losses final felling (Endnutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
    FFIpol<double> ffsdMaxH; // stocking degree depending on max tree height
    FFIpol<double> ffsdMinH; // stocking degree depending on max (min?) tree height

    // Thinning costs (depending on d and removed volume per hectare) in relation to standing timber (Vorratsfestmeter)
    FFIpolM<double> ffcov;
    // Harvesting costs depending on d and vol
    FFIpolM<double> ffcoe;
    // Do thinning (depending on d and removed volume per hectare) in relation to standing timber (Vorratsfestmeter)
    FFIpolM<double> ffdov;
    // Do final felling (depending on d and stocking volume per hectare)
    FFIpolM<double> ffdoe;

    using simuIdType = unordered_map<uint32_t, Ipol < double> >;
    using heterSimuIdScenariosType = unordered_map<string, simuIdType, StringHash, equal_to<> >;

    heterSimuIdScenariosType globiomAfforMaxScenarios;         // Maximum allowed afforestation estimated from GLOBIOM natural land
    heterSimuIdScenariosType globiomLandScenarios;      // Country GLOBIOM land (kha) reserved for croplands, pastures and short rotation plantations (also wetlands and infrastructure)

    heterSimuIdScenariosType maiClimateShiftersScenarios;  // MAI climate shifters

    simuIdType commonDisturbWind;     // wood damage due to windfalls, m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023
    simuIdType commonDisturbFire;     // wood damage due to biotic agents (mostly bark beetle), m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023
    simuIdType commonDisturbBiotic;   // wood damage due to fire, m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023

    simuIdType commonDisturbWindExtreme;      // wood damage due to windfalls, m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023
    simuIdType commonDisturbBioticExtreme;    // wood damage due to biotic agents (mostly bark beetle), m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023
    simuIdType commonDisturbFireExtreme;      // wood damage due to fire, m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023

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

    vector<AgeStruct> commonCohort_all;
    vector<AgeStruct> commonNewCohort_all;
    vector<AgeStruct> commonCohort10_all;
    vector<AgeStruct> commonCohort30_all;
    vector<AgeStruct> commonCohort_primary_all;

    vector<Dat> commonDat_all;

    unordered_map<uint8_t, FFIpolsCountry> countriesFFIpols;
    // data related to harvesting the residues (residues amount, associated costs and soil loss emissions)
    unordered_map<uint8_t, vector<HarvestResidues> > harvestResiduesCountry;  // a vector of harvest residues data for all single countries

    unordered_map<string, binary_semaphore, StringHash, equal_to<> > signalZeroCToMainScenarios;
    unordered_map<string, vector<vector<double> >, StringHash, equal_to<> > biomassBauScenarios;
    unordered_map<string, vector<vector<double> >, StringHash, equal_to<> > NPVBauScenarios;
}

#endif
