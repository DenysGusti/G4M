#ifndef G4M_EUROPE_DG_FOREST_GUI_EUROPE_PARAM_DW_5_3_DG_HPP
#define G4M_EUROPE_DG_FOREST_GUI_EUROPE_PARAM_DW_5_3_DG_HPP

#include <iostream>
#include <array>
#include <string>
#include <set>
#include <unordered_set>
#include <numbers>
#include <functional>

#include "../log.hpp"
#include "../diagnostics/debugging.hpp"
#include "../diagnostics/timer.hpp"

#include "../start_data/start_data.hpp"
#include "../settings/dicts/dicts.hpp"

#include "../structs/lw_price.hpp"
#include "../structs/harvest_residues.hpp"

#include "../misc/concrete/ffipolm.hpp"

#include "../init/species.hpp"

#include "../GLOBIOM_scenarios_data/datamaps/datamaps.hpp"
#include "../GLOBIOM_scenarios_data/simu_ids/simu_ids.hpp"
#include "../GLOBIOM_scenarios_data/result_files.hpp"

using namespace std;

using namespace numbers;
using namespace g4m::structs;
using namespace g4m::misc::concrete;
using namespace g4m::increment;
using namespace g4m::init;
using namespace g4m::diagnostics;
using namespace g4m::StartData;
using namespace g4m::Dicts;
using namespace g4m::GLOBIOM_scenarios_data;

namespace g4m::application {
    class Forest_GUI_Europe_param_dw_5_3 {
    public:
        explicit Forest_GUI_Europe_param_dw_5_3(const span<const string> args)
                : appName{format("{}_{}_{}_{}", args[0], args[1], args[2], args[3])},
                  c_scenario{{args[0], args[1], args[2]}}, inputPriceC{stoi(args[3])},
                  dms{datamapScenarios, full_scenario, inputPriceC},
                  sis{simuIdScenarios, full_scenario, c_scenario[1]},
                  rf{settings.outputPath, local_suffix} {
            // TODO initialize earlier
            Log::Init(appName);
            INFO("Scenario to read in & GL: {}", full_scenario);
            correctBelgium();
            applyMAIClimateShifters();
            initGlobiomLandLocal();
            applyCorruptionModifyCO2Price();
            toAdjust.reserve(256);
            countriesNoFmCPol.reserve(256);
            doneList.reserve(256);

            rf.addHeadersToBuffers(Dat::csvHeader());

            if constexpr (fmPol && !binFilesOnDisk)
                if (inputPriceC == 0) {
                    scoped_lock<mutex> lock{zeroC_mutex};
                    signalZeroCToMainScenarios.emplace(suffix0, 0);
                }
        }

        ~Forest_GUI_Europe_param_dw_5_3() {
            if constexpr (fmPol && !binFilesOnDisk)
                if (inputPriceC == 0) {
                    scoped_lock<mutex> lock{zeroC_mutex};
                    signalZeroCToMainScenarios.at(suffix0).release();
                }

            rf.saveFiles();     // in destructor to output something if exception was thrown
        }

        // start calculations
        void Run() {
            INFO("Application {} is running", appName);

            for (const auto &plot: appPlots)
                rf.cellInfoBuffer += format("{},{},{},{},{},{},{}\n", plot.asID, plot.simuID,
                                            idCountryGLOBIOM.at(plot.country), coef.bYear - 1,
                                            speciesName.at(plot.speciesType), vegetationTypeName.at(plot.potVeg),
                                            appDats[plot.asID].csv());

            // loop by years
            for (uint16_t year = coef.bYear; year <= coef.eYear; ++year) {
                INFO("Processing year {}", year);

                if (year == forPolicyYearBioClima) {
                    appForest10Policy = commonForest10PolicyKey;
                    appForest30Policy = commonForest30PolicyKey;
                    appMultifunction10 = commonMultifunction10Key;
                    appMultifunction30 = commonMultifunction30Key;
                }

                if (MAIClimateShift && year > 2020)
                    for (const auto &plot: appPlots) {
                        // Max mean annual increment (tC/ha) of existing forest (with uniform age structure and managed with rotation length maximizing MAI)
                        // Max mean annual increment of new forest (with uniform age structure and managed with rotation length maximizing MAI)
                        double MAI = max(0., plot.getForestShare() > 0 ? plot.MAIE(year) : plot.MAIN(year));

                        appMaiForest(plot.x, plot.y) = MAI;

                        appCohortsU[plot.asID].setMaiAndAvgMai(MAI);
                        appCohorts10[plot.asID].setMaiAndAvgMai(MAI);
                        appCohorts30[plot.asID].setMaiAndAvgMai(MAI);
                        appCohortsP[plot.asID].setMaiAndAvgMai(MAI);
                        appCohortsN[plot.asID].setMaiAndAvgMai(MAI);
                    }

                INFO("Adjusting FM...");
                adjustManagedForest(year);

                for (int country = 0; country < numberOfCountries; ++country) {
                    countriesNForestCover1990.setVal(country, year, 0);
                    countriesOForestCover.setVal(country, year, 0);
                }
                // processing data from all plots
                rf.biomassBau.emplace_back();
                auto &tmpBm = rf.biomassBau.back();
                tmpBm.reserve(appPlots.size());

                INFO("Starting cell calculations...");
                for (const auto &plot: appPlots) {
                    calc(plot, year);
                    const Dat &cell = appDats[plot.asID];

                    rf.cellInfoBuffer += format("{},{},{},{},{},{},{}\n", plot.asID, plot.simuID,
                                                idCountryGLOBIOM.at(plot.country), year,
                                                speciesName.at(plot.speciesType), vegetationTypeName.at(plot.potVeg),
                                                cell.csv());

                    if (!cell.checkLastForestShares()) {
                        FATAL("Negative forest share in year = {}, asID = {}, country = {}", year, plot.asID,
                              plot.country);
                        throw runtime_error{"Negative forest share!"};
                    }
                    if constexpr (fmPol && !binFilesOnDisk)
                        if (inputPriceC == 0 && year > refYear)
                            tmpBm.push_back(cell.U.stemBiomass.back());

                    appOForestShGrid.update1YearForward();

                    const auto [x, y] = plot.getGeographicCoordinates();
                    string nuts2_id;
                    if (auto it_nuts2 = nuts2.nuts2id.find({plot.x, plot.y}); it_nuts2 != nuts2.nuts2id.end())
                        nuts2_id = it_nuts2->second;
                    else
                        ERROR("NUTS2 was not found! x = {}, y = {}", x, y);

                    const double sd = appThinningForest(plot.x, plot.y);
                    const IncrementTab::TableMode mode = IncrementTab::getTableModeBySD(sd);
                    const double rotMAI = species.at(plot.speciesType).getTOpt(mode, appMaiForest(plot.x, plot.y),
                                                                               OptRotTimes::Mode::MAI, abs(sd));
                    const double rotMaxBm = species.at(plot.speciesType).getTOpt(mode, appMaiForest(plot.x, plot.y),
                                                                                 OptRotTimes::Mode::MaxBm, abs(sd));
                    // TODO rotMAI, rotMaxBm in cell, special formula, in calc?
                    // TODO rotMAI, rotMaxBm?
                    // TODO cell.U.totalHarvest?
                    // TODO obsolete forest?
                    // TODO harvestFcM3Ha?
                    // TODO check columns and wished data
                    // TODO add 0 columns to cellInfo
                    // TODO add Harvest details for Fulvio in CanopyLayer?
                    // TODO harvestFc_m3_U has FS
                    // TODO cell.U.harvestFc() / cell.landAreaHa?
//                    if (year > residueHarvYear)
//                        rf.harvestDetailsBuffer +=
//                                format("{},{}\n",
//                                       x, y, plot.simuID, plot.country, nuts2_id, year, cell.SD, cell.rotation,
//                                       cell.forestShareOld(-1) * cell.landAreaHa,
//                                       cell.N.forestShare.back() * cell.landAreaHa, cell.U.CAI, cell.U.totalHarvest,
//                                       cell.U.harvestFc(), cell.U.harvestTh() /*???*/, cell.U.totalBiomass,
//                                       cell.N.totalBiomass /*columns*/, speciesName.at(plot.speciesType),
//                                       cell.U.harvestFc() / cell.landAreaHa, cell.U.harvestTh() / cell.landAreaHa,
//                                       cell.N.harvestFc() / cell.landAreaHa, cell.N.harvestTh() / cell.landAreaHa,
//                                       cell.U.harvestFc(), cell.U.harvestTh(), cell.N.harvestFc(), cell.N.harvestTh(),
//                                       cell.U.harvestConiferFc, cell.U.harvestConiferTh, cell.N.harvestConiferFc,
//                                       cell.N.harvestConiferTh, cell.U.harvestBroadleafFc, cell.U.harvestBroadleafTh,
//                                       cell.N.harvestBroadleafFc, cell.N.harvestBroadleafTh,
//                                       cell.U.harvestSc() /*???*/ /*columns*/,
//                                       cell.U.harvestSc() / cell.landAreaHa, cell.N.harvestSc() / cell.landAreaHa,
//                                       cell.U.harvestSc(), cell.N.harvestSc(), cell.U.harvestConiferSc,
//                                       cell.N.harvestConiferSc, cell.U.harvestBroadleafSc, cell.N.harvestBroadleafSc,
//                                       rotMAI, rotMaxBm, cell.U.deadwood, cell.N.deadwood, cell.U.litter,
//                                       cell.N.litter, cell.U.deadwoodInput, cell.N.deadwoodInput, cell.U.litterInput,
//                                       cell.N.litterInput, cell.U.deadwoodEmissions, cell.N.deadwoodEmissions,
//                                       cell.U.litterEmissions, cell.N.litterEmissions);

                    rf.bioclimaDetailsBuffer +=
                            format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},"
                                   "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},"
                                   "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}\n",
                                   x, y, plot.simuID, idCountryGLOBIOM.at(plot.country), nuts2_id, year,
                                   cell.landAreaHa, speciesName.at(plot.speciesType), plot.protect,
                                   cell.U.forestShare.back() * cell.landAreaHa,
                                   cell.O10.forestShare.back() * cell.landAreaHa,
                                   cell.O30.forestShare.back() * cell.landAreaHa,
                                   cell.P.forestShare.back() * cell.landAreaHa,
                                   cell.N.forestShare.back() * cell.landAreaHa, cell.SD,
                                   appCohorts10[plot.asID].getStockingDegree(),
                                   appCohorts30[plot.asID].getStockingDegree(), rotMAI, rotMaxBm, cell.rotation,
                                   appCohorts10[plot.asID].getU(), appCohorts30[plot.asID].getU(),
                                   appCohortsP[plot.asID].getU(),
                                   cell.U.CAI * cell.U.forestShare.back() * cell.landAreaHa,
                                   cell.O10.CAI * cell.O10.forestShare.back() * cell.landAreaHa,
                                   cell.O30.CAI * cell.O30.forestShare.back() * cell.landAreaHa,
                                   cell.P.CAI * cell.P.forestShare.back() * cell.landAreaHa,
                                   cell.N.CAI * cell.N.forestShare.back() * cell.landAreaHa,
                                   cell.U.totalBiomass, cell.O10.totalBiomass, cell.O30.totalBiomass,
                                   cell.P.totalBiomass, cell.N.totalBiomass, cell.U.totalHarvest(),
                                   cell.O10.totalHarvest(), cell.O30.totalHarvest(), cell.N.totalHarvest(),
                                   cell.U.fellings, cell.O10.fellings, cell.O30.fellings, cell.N.fellings,
                                   cell.U.deadwood, cell.O10.deadwood, cell.O30.deadwood, cell.P.deadwood,
                                   cell.N.deadwood,
                                   cell.U.deadwoodInput, cell.O10.deadwoodInput,
                                   cell.O30.deadwoodInput, cell.P.deadwoodInput, cell.N.deadwoodInput,
                                   cell.U.deadwoodEmissions, cell.O10.deadwoodEmissions, cell.O30.deadwoodEmissions,
                                   cell.P.deadwoodEmissions, cell.N.deadwoodEmissions,
                                   cell.U.litter, cell.O10.litter, cell.O30.litter, cell.P.litter, cell.N.litter,
                                   cell.U.litterInput, cell.O10.litterInput, cell.O30.litterInput, cell.P.litterInput,
                                   cell.N.litterInput,
                                   cell.U.litterEmissions, cell.O10.litterEmissions, cell.O30.litterEmissions,
                                   cell.P.litterEmissions, cell.N.litterEmissions,
                                   appForest10Policy, appForest30Policy,
                                   cell.U.OAC, cell.N.OAC, cell.O10.OAC, cell.O30.OAC, cell.P.OAC);
                }
            }
            rf.sortCellInfo();
        }

    protected:
        string appName;
        array<string, 3> c_scenario;
        int inputPriceC;

        Timer timer{appName};

        string full_scenario = c_scenario[0] + '_' + c_scenario[1] + '_' + c_scenario[2];
        string local_suffix = suffix + full_scenario + (inputPriceC == 0 ? "_Pco2_0" : "");
        string suffix0 = c_scenario[0] + '_' + c_scenario[1];

        // Apply the MAI climate shifters starting from MAIClimateShiftYear
        bool MAIClimateShift = c_scenario[1].contains("RCP");

        vector<DataStruct> appPlots = plots.filteredPlots;

        Datamaps dms;
        SimuIds sis;
        ResultFiles rf;

        // wood from outside forests in Belgium to cover the inconsistency between FAOSTAT removals and Forest Europe increment and felling
        Ipol<double> woodSupplement;

        // Initializing forest cover array by grid-cells
        DataGrid<double> thinningForestNew{resLatitude};
        DataGrid<double> rotationForestNew{resLatitude};
        DataGrid<double> maxNPVGrid{resLatitude};
        DataGrid<double> salvageLogging{resLatitude}; // salvage logging wood

        DataGrid<double> appHarvestGrid = commonHarvestGrid;
        DataGrid<double> appMaiForest = commonMaiForest;
        DataGrid<double> appRotationForest = commonRotationForest;
        DataGrid<double> appThinningForest = commonThinningForest;
        DataGrid<double> appThinningForest10 = commonThinningForest10;
        DataGrid<double> appThinningForest30 = commonThinningForest30;
        DataGrid<double> appOForestShGrid = commonOForestShGrid;

        DataGrid<int8_t> appDecisionGrid = commonDecisionGrid;
        DataGrid<int8_t> appManagedForest = commonManagedForest;
        DataGrid<int8_t> appManageChForest = commonManageChForest;
        DataGrid<int8_t> appRotationType = commonRotationType;
        DataGrid<int8_t> appUnmanaged = commonUnmanaged;

        vector<AgeStruct> appCohortsU = commonCohortsU;
        vector<AgeStruct> appCohortsN = commonCohortsN;
        vector<AgeStruct> appCohorts10 = commonCohorts10;
        vector<AgeStruct> appCohorts30 = commonCohorts30;
        vector<AgeStruct> appCohortsP = commonCohortsP;

        vector<Dat> appDats = commonDats;

        bool appForest10Policy = commonForest10Policy;
        bool appForest30Policy = commonForest30Policy;
        bool appMultifunction10 = commonMultifunction10;
        bool appMultifunction30 = commonMultifunction30;

        unordered_set<uint8_t> toAdjust;            // country where FM to be adjusted
        unordered_set<uint8_t> countriesNoFmCPol;   // List of countries where it's impossible to match demanded wood production in current year
        unordered_set<uint8_t> doneList;            // countries already adjusted

        unordered_map<uint8_t, vector<HarvestResidues> > appHarvestResiduesCountry = commonHarvestResiduesCountry;

        CountryData countryRegMaxHarvest;
        CountryData countryRegWoodProd;
        CountryData countryRegWoodHarvestDfM3Year;

        CountryData countryFMCPolMaxNPV;
        CountryData countryFMCPolCurrNPV;

        CountryData countryRotation;
        CountryData countryRotationUsed;
        CountryData countryRotationUnused;

        CountryData countriesResiduesExtract_m3;        // sustainably extracted harvest residuals, m3
        CountryData countriesResiduesDemand_m3;         // harvest residues demand recalculated to m3
        CountryData countriesResExtSoilEm_MtCO2Year;    // soil loss emissions due to sustainably extracted harvest residuals, MtCO2/year

        CountryData countriesWoodHarvestM3Year;
        CountryData countriesWoodHarvest10M3Year;   // Harvest from 10% forest
        CountryData countriesWoodHarvest30M3Year;   // Harvest from 30% forest
        CountryData countriesWoodHarvestFmM3Year;
        CountryData countriesWoodHarvestDfM3Year;

        CountryData countriesNForestCover1990;
        CountryData countriesNForestCover;
        CountryData countriesOForestCover;

        CountryData countriesAfforestationHaYear1990;
        CountryData countriesAfforestationHaYear;
        CountryData countriesAfforestationAccumulationHa;

        CountryData countriesNForestTotalC;
        CountryData countriesNForestStemC;
        CountryData countriesNForestAbC;
        CountryData countriesAfforestationCYear;
        CountryData countriesAfforestationCYear_ab;
        CountryData countriesAfforestationCYear_bl;
        CountryData countriesAfforestationCYear_biomass;
        CountryData countriesAfforestationCYear_dom;
        CountryData countriesAfforestationCYear_soil;
        CountryData countriesAfforestationCYear_soil_altern;
        CountryData countriesAfforestationCover20;
        CountryData countriesAfforestationTotalC20;
        CountryData countriesAfforestationCYear_biomass20;
        CountryData countriesAfforestationCYear_biomass20os;
        CountryData countriesAfforestationCYear_biomass20o;
        CountryData countriesAfforestationCYear_biomass20b;
        CountryData countriesAfforestationCYear_biomassGL;
        CountryData countriesAfforestationCYear_dom20b;
        CountryData countriesAfforestationCYear_soil20b;

        CountryData countriesOForestCoverU;
        CountryData countriesOForestCover10;
        CountryData countriesOForestCover30;
        CountryData countriesOForestCoverP;

        CountryData countriesOForestCoverFAWSOld;

        CountryData countriesDeforestationHaYear;
        CountryData countriesOForest_stem_C;
        CountryData countriesOForest_ab_C;
        CountryData countriesOForest10_ab_C;
        CountryData countriesOForest30_ab_C;
        CountryData countriesOForestP_ab_C;

        CountryData countriesOForestC_biomass;
        CountryData countriesDeforestationCYear;
        CountryData countriesDeforestationCYear_ab;
        CountryData countriesDeforestationCYear_bl;
        CountryData countriesDeforestationCYear_biomass;
        CountryData countriesDeforestationCYear_dom;
        CountryData countriesDeforestationCYear_soil;
        CountryData countriesDeforestationCYear_soil_altern;

        CountryData countriesWoodHarvestFc_oldM3Year;
        CountryData countriesWoodHarvestTh_oldM3Year;
        CountryData countriesWoodHarvestLostFc_oldM3Year;
        CountryData countriesWoodHarvestLostTh_oldM3Year;
        CountryData countriesWoodHarvestSc_oldM3Year;

        CountryData countriesWoodHarvestSalvage_oldM3Year;
        CountryData countriesWoodHarvestFc_newM3Year;
        CountryData countriesWoodHarvestTh_newM3Year;
        CountryData countriesWoodHarvestSc_newM3Year;
        CountryData countriesHarvLossesYear;
        CountryData countriesHarvLossesOldYear;
        CountryData countriesHarvLossesOldFcYear;
        CountryData countriesHarvLossesOldThYear;
        CountryData countriesFellingsNAIYear;

        CountryData countriesManagedCount;
        CountryData countriesManagedForHa;
        CountryData countriesManagedForOldHa;
        CountryData countriesManagedNewHa;
        CountryData countriesGCAI_mng;
        CountryData countriesNCAI_mng;
        CountryData countriesGCAI_new_mng;
        CountryData countriesNCAI_new_mng;

        CountryData countriesMAI;
        CountryData countriesCAI;
        CountryData countriesCAI_FAWS_old;
        CountryData countriesCAI_FAWS_all;
        CountryData countriesCAI_ALL_old;
        CountryData countriesCAI_ALL_all;
        CountryData countriesCAI_new;
        CountryData countriesNAI_old;
        CountryData countriesGCAI;
        CountryData countriesNCAI;
        CountryData countriesGCAI_new;
        CountryData countriesNCAI_new;
        CountryData countriesFM;
        CountryData countriesFMbm;
        CountryData countriesFMbmFAWS_old;

        CountryData countriesProfit;
        CountryData countriesOForestCoverUNFCCC;
        CountryData countriesFMbmUNFCCC;

        CountryData countriesWoodHarvBroadleafFc_oldM3Year;
        CountryData countriesWoodHarvBroadleafTh_oldM3Year;
        CountryData countriesWoodHarvBroadleafSc_oldM3Year;

        CountryData countriesWoodHarvBroadleafFc_newM3Year;
        CountryData countriesWoodHarvBroadleafTh_newM3Year;
        CountryData countriesWoodHarvBroadleafSc_newM3Year;

        CountryData countriesAreaUsedBroadleaf_oldHa;
        CountryData countriesAreaUsedBroadleaf_newdHa;
        CountryData countriesAreaHarvBroadleafFc_oldHaYear;
        CountryData countriesAreaHarvBroadleafFc_newHaYear;
        CountryData countriesAreaScBroadleaf_oldHa;
        CountryData countriesAreaScBroadleaf_newHa;

        CountryData countriesWoodHarvConiferFc_oldM3Year;
        CountryData countriesWoodHarvConiferTh_oldM3Year;
        CountryData countriesWoodHarvConiferSc_oldM3Year;

        CountryData countriesWoodHarvConiferFc_newM3Year;
        CountryData countriesWoodHarvConiferTh_newM3Year;
        CountryData countriesWoodHarvConiferSc_newM3Year;

        CountryData countriesAreaUsedConifer_oldHa;
        CountryData countriesAreaUsedConifer_newdHa;
        CountryData countriesAreaHarvConiferFc_oldHaYear;
        CountryData countriesAreaHarvConiferFc_newHaYear;
        CountryData countriesAreaScConifer_oldHa;
        CountryData countriesAreaScConifer_newHa;

        CountryData countriesDeadwood_old_tCha;
        CountryData countriesDeadwood_new_tCha;
        CountryData countriesLitter_old_tCha;
        CountryData countriesLitter_new_tCha;
        CountryData countriesDeadwoodEm_old_mtco2year;
        CountryData countriesDeadwoodEm_new_mtco2year;
        CountryData countriesLitterEm_old_mtco2year;
        CountryData countriesLitterEm_new_mtco2year;

        DataGrid<double> forestryValueDPrevGrid{resLatitude};

        void correctBelgium() noexcept {
            woodSupplement = dms.woodDemand.at(20);
            // 05.04.2023: we assume that 14% of round-wood comes from outside forest
            // the Forest Europe net increment and felling values are less than FAOSTAT round-wood
            const double forestWood = 0.86;
            dms.woodDemand[20] *= forestWood;  // Belgium
            woodSupplement *= 1 - forestWood;
        }

        // Alteration of mean annual increment with time in each grid cell with values read in a dedicated file,
        // e.g. caused by projected climate change.
        void applyMAIClimateShifters() noexcept {
            if (!MAIClimateShift) {
                INFO("MAIClimateShift is turned off");
                return;
            }

            for (auto &plot: appPlots) {
                const double maie = plot.MAIE.data.at(2000);
                const double main = plot.MAIN.data.at(2000);
                const double npp = plot.NPP.data.at(2000);

                plot.MAIE.data[2020] = maie;
                plot.MAIN.data[2020] = main;
                plot.NPP.data[2020] = npp;

                if (sis.maiClimateShifters.contains(plot.simuID))
                    for (const auto [year, value]: sis.maiClimateShifters.at(plot.simuID).data)
                        if (year >= 2030) {
                            plot.MAIE.data[year] = maie * value;
                            plot.MAIN.data[year] = main * value;
                            plot.NPP.data[year] = npp * value;
                        }
            }
        }

        // Synchronize land use with the GLOBIOM model
        void initGlobiomLandLocal() noexcept {
            // file_globiom_LC_correction is not implemented
            double sumGrLnd_protect = 0;

            for (auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect))) {
                // we leave the previous values if in current dataset this cell is absent
                if (!sis.GLOBIOM_AfforMax[plot.simuID].data.empty())
                    for (const auto &[year, value]: sis.GLOBIOM_AfforMax[plot.simuID].data)
                        if (year > 2000)
                            plot.afforMax.data[year] = max(0., value + plot.natLnd_correction);

                // we leave the previous values if in current dataset this cell is absent
                if (!sis.GLOBIOM_Land[plot.simuID].data.empty())
                    for (const auto &[year, value]: sis.GLOBIOM_Land[plot.simuID].data)
                        // dfor correction was ahead of time and value of GLOBIOM_reserved is probably negative
                        // max(0, value + plot.GL_correction) =>
                        // max(plot.GLOBIOM_reserved.data[year],
                        // value + plot.GL_correction + plot.GLOBIOM_reserved.data[year])
                        // plot.GLOBIOM_reserved.data[year] exists only if (opt_dfor)
                        plot.GLOBIOM_reserved.data[year] = max(plot.GLOBIOM_reserved.data[year],
                                                               value + plot.GL_correction +
                                                               plot.GLOBIOM_reserved.data[year]);

                else if (!dms.GLOBIOM_LandCountry[plot.country].data.empty() &&
                         !dms.GLOBIOM_AfforMaxCountry[plot.country].data.empty()) {

                    if constexpr (protectedNatLnd)
                        if (plot.grLnd_protect > 0) {
                            plot.afforMax.data[2000] = max(0., plot.afforMax.data.at(2000) + plot.grLnd_protect);
                            sumGrLnd_protect += plot.grLnd_protect;
                        }

                    for (auto &[year, GL]: plot.GLOBIOM_reserved.data)
                        if (year > 2000 && GL > 0) {
                            double &natLnd = plot.afforMax.data[year];  // no interpolation, I suppose data is already there
                            double dGL = (dms.GLOBIOM_LandCountry[plot.country].data.at(year) -
                                          dms.GLOBIOM_LandCountry[plot.country](year - modTimeStep)) /
                                         countryLandArea[plot.country];
                            dGL = clamp(dGL, -GL, natLnd);
                            // reference variables
                            GL = max(0., GL + dGL);
                            natLnd = max(0., natLnd - dGL);
                        }
                }
            }

            if constexpr (protectedNatLnd)
                if (sumGrLnd_protect <= 0)
                    ERROR("GRLND_PROTECT is 0. Check spatial input data (plots) and if flag debugWithoutBigFiles is disabled!!!");
        }

        // apply corruption and populate country CO2Price if inputPriceC > 0
        void applyCorruptionModifyCO2Price() noexcept {
            if (inputPriceC == 0)
                return;

            // CO2 price different for regions: set priceC to negative value then the price is defined in the function carbonPriceRegion
            if (inputPriceC > 0) {
                double priceC = inputPriceC * deflator * molarRatio;  // * molarRatio if input price is $/tCO2

                for (auto &[country, ipol]: dms.CO2Price)
                    ipol.data[refYear] = priceC;
            }

            unordered_map<uint8_t, double> countryCorruption;
            countryCorruption.reserve(numberOfCountries);
            for (const auto &plot: appPlots)
                countryCorruption[plot.country] = plot.corruption;

            for (const auto &[country, corruption]: countryCorruption)
                dms.CO2Price[country] *= corruption;
        }

        // Adjust forest management in each grid cell to harvest the demanded amount of wood on a country scale every
        // time step. Disturbance damage and respective salvage logging is implemented here as well.
        void adjustManagedForest(const uint16_t year) {
            double stockingDegree = 1.3;    // test for Belgium
            bool harvControl = true;        // Additional information to control output of the fmCPol module
            bool NPV_postControl_0 = false; // Control of old forest management NPV at 0 C price: Use only for testing!!!!
            bool NPV_postControl = true;    // Control of old forest management NPV at non-zero C price
            // Country code for deep analysis of G4M FM algorithm; nullopt - no output of the deep analysis
            // optional<uint8_t> country_to_check = nullopt;  // corresponds Null from python
            toAdjust = countriesList;

            if (year == coef.bYear)
                for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect))) {
                    thinningForestNew(plot.x, plot.y) = appCohortsN[plot.asID].getStockingDegree();
                    rotationForestNew(plot.x, plot.y) = appCohortsN[plot.asID].getU();

                    double rotation = appRotationForest(plot.x, plot.y);
                    double SD = appThinningForest(plot.x, plot.y);

                    appThinningForest30(plot.x, plot.y) = SD;
                    appCohorts30[plot.asID].setU(rotation);
                    appCohorts30[plot.asID].setStockingDegree(SD * sdMinCoef);
                    appCohorts30[plot.asID].setStockingDegreeMax(SD * sdMaxCoef);
                }

            if (year == coef.bYear + 1)
                for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                    if (appThinningForest(plot.x, plot.y) < 0)
                        appUnmanaged(plot.x, plot.y) = 1;

            // Apply the forest 10% and forest 30% policies
            if (year > 2020) {
                if (appForest10Policy)
                    switch2Conserved(10, appCohorts10, appMultifunction10);
                if (appForest30Policy)
                    switch2Conserved(30, appCohorts30, appMultifunction30);
            }

            array<double, numberOfCountries> residueHarvest{};  // Extraction of harvest residues, tC
            // Calculate salvage logging additional to historical
            if (disturbanceTrend && year > 2020 || disturbanceExtreme && year == disturbanceExtremeYear)
                residueHarvest = disturbanceDamageHarvest(year);

            // Gradually adjust thinning
            for (const auto &plot: appPlots) {
                // for the most productive forest (MAI = 6) the forest cleaning time is about 20 years,
                // and for the least productive (MAI close to 0) the cleaning time is 50 years
                double SD_conv = (stockingDegree - 1) / 20 - (6 - appMaiForest(plot.x, plot.y)) * 0.0015;

                double thinningTmp = appThinningForest(plot.x, plot.y);
                if (thinningTmp > 1 && appManageChForest(plot.x, plot.y) > 0) {
                    thinningTmp = max(1., thinningTmp - SD_conv * modTimeStep);
                    appThinningForest(plot.x, plot.y) = thinningTmp;
                    appCohortsU[plot.asID].setStockingDegreeMin(thinningTmp * sdMinCoef);
                    appCohortsU[plot.asID].setStockingDegreeMax(thinningTmp * sdMaxCoef);
                }

                double thinningTmpNew = thinningForestNew(plot.x, plot.y);
                if (thinningTmpNew > 1 && appManageChForest(plot.x, plot.y) > 0) {
                    thinningTmpNew = max(1., thinningTmpNew - SD_conv * modTimeStep);
                    thinningForestNew(plot.x, plot.y) = thinningTmpNew;
                    appCohortsN[plot.asID].setStockingDegreeMin(thinningTmpNew * sdMinCoef);
                    appCohortsN[plot.asID].setStockingDegreeMax(thinningTmpNew * sdMaxCoef);
                }
            }

            if (!appForest30Policy)  // TODO write separate loop for no forest10 policy
                forest30GeneralFM();

            if (fmPol && year > refYear && inputPriceC != 0) {
                // populate list of countries where it's impossible to match demanded wood production in current year
                countriesNoFmCPol.clear();
                for (double tmpTimeStep = modTimeStep / 100.; const auto country: countriesList)
                    if (countryRegMaxHarvest.getVal(country, year - 1) <
                        (0.8 - tmpTimeStep) * countryRegWoodProd.getVal(country, year - 1) ||
                        countryRegWoodHarvestDfM3Year.getVal(country, year - 1) >
                        (1.1 + tmpTimeStep) * countryRegWoodProd.getVal(country, year - 1) ||
                        dms.CO2Price.at(country)(year) <= 0)
                        countriesNoFmCPol.insert(country);

                DEBUG("countriesNoFmCPol:");
                for (const auto country: countriesNoFmCPol)
                    DEBUG("{}", int{country});

                findMaxWoodAndCNPV(year);
                array<double, numberOfCountries> woodHarvest = fmCPol(1, year, true);
                array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, true);
                array<double, numberOfCountries> maxDiff0 = harvDiff;

                doneList.clear();
                for (uint8_t maxDiffCountry = 0;
                     doneList.size() < countriesList.size(); doneList.insert(maxDiffCountry)) {
                    // TODO maybe heap, improve?
                    while (doneList.contains(maxDiffCountry)) {
                        harvDiff[maxDiffCountry] = 0;
                        maxDiffCountry = distance(harvDiff.begin(), ranges::max_element(harvDiff));
                    }

                    double maxDiff = harvDiff[maxDiffCountry];
                    if (maxDiff <= 0.01)
                        break;

                    toAdjust = {maxDiffCountry};

                    double fm_hurdle = 1;
                    double diffMaxDiff = 1;
                    TRACE("maxDiff = {}\tfm_hurdle = {}\tdiffMaxDiff = {}", maxDiff, fm_hurdle, diffMaxDiff);

                    for (int i = 1; maxDiff > 0.01 && fm_hurdle > -1'000'000 && i <= 100; ++i) {
                        if (diffMaxDiff <= 0.001) {
                            if (fm_hurdle < 2 && fm_hurdle > 0)
                                fm_hurdle += 0.2;
                            else if (fm_hurdle < 10 && fm_hurdle >= 2)
                                fm_hurdle += 1;
                            else if (fm_hurdle < 100 && fm_hurdle >= 10)
                                fm_hurdle += 10;
                            else if (fm_hurdle < 1'000 && fm_hurdle >= 100)
                                fm_hurdle += 100;
                            else if (fm_hurdle < 100'000 && fm_hurdle >= 1'000)
                                fm_hurdle += 50'000;
                            else if (fm_hurdle < 1'000'000 && fm_hurdle >= 100'000)
                                fm_hurdle += 500'000;
                            else if (fm_hurdle < 10'000'000 && fm_hurdle >= 1'000'000)
                                fm_hurdle += 5'000'000;
                            else if (fm_hurdle < 100'000'000 && fm_hurdle >= 10'000'000)
                                fm_hurdle += 50'000'000;
                            else if (fm_hurdle < 1'000'000'000 && fm_hurdle >= 100'000'000)
                                fm_hurdle += 500'000'000;
                            else if (fm_hurdle >= 1'000'000'000)
                                fm_hurdle = -1;    // Try for Ireland
                            else if (fm_hurdle >= -1 && fm_hurdle < 0)
                                fm_hurdle = -2;
                            else if (fm_hurdle >= -2 && fm_hurdle < 0)
                                fm_hurdle = -10;
                            else if (fm_hurdle >= -100'000 && fm_hurdle < 0)
                                fm_hurdle = -1'000'000;
                        }
                        auto _ = fmCPol(fm_hurdle, year, true);

                        double maxDiffPrev = maxDiff0[maxDiffCountry];

                        diffMaxDiff = maxDiffPrev - maxDiff;
                        maxDiffPrev = maxDiff;

                        TRACE("maxDiff = {}\tfm_hurdle_loop = {}\tdiffMaxDiff = {}", maxDiff, fm_hurdle, diffMaxDiff);
                    }
                }

                INFO("Start adjust residues in FMCPol");
                array<double, numberOfCountries> harvestResiduesSoilEmissions = adjustResidues(year, residueHarvest);

                INFO("-------------------------------");
                if (year > residueHarvYear) {
                    for (const auto country: countriesList)
                        if (dms.woodDemand.at(country)(year) > 0) {
                            double residuesDemand = dms.residuesDemand.at(country)(year);
                            double residuesDiff =
                                    residuesDemand > 0 ? abs(residueHarvest[country] / residuesDemand - 1) : 0;
                            TRACE("{}: residuesDiff = {}\tresiduesDemand = {}\tresiduesHarvest = {}\tharvestResiduesSoilEmissions = {}",
                                  country, residuesDiff, residuesDemand, residueHarvest[country],
                                  harvestResiduesSoilEmissions[country]);
                        }
                }

                INFO("-----NPV post control-----");
                if (NPV_postControl)
                    for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect))) {
                        bool used = appThinningForest(plot.x, plot.y) > 0;
                        double NPVTmp = npvCalc(plot, appCohortsU[plot.asID], year,
                                                appRotationForest(plot.x, plot.y), used).first;
                        countryFMCPolMaxNPV.inc(plot.country, year, maxNPVGrid(plot.x, plot.y));
                        countryFMCPolCurrNPV.inc(plot.country, year, NPVTmp);
                    }
            } else {
                // 0 - path
                array<double, numberOfCountries> woodHarvest = fmCPol(1, year, false);

                INFO("Start adjust residues");
                array<double, numberOfCountries> harvestResiduesSoilEmissions = adjustResidues(year, residueHarvest);

                INFO("-----NPV post control-----");
                if constexpr (fmPol)
                    if (year > refYear && NPV_postControl_0)
                        for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect))) {
                            bool used = appThinningForest(plot.x, plot.y) > 0;
                            double NPVTmp = npvCalc(plot, appCohortsU[plot.asID], year,
                                                    appRotationForest(plot.x, plot.y), used).first;
                        }

                if constexpr (adjustFMSink)
                    if (year == coef.bYear + 1) {
                        array<double, 4> FMs_diff2 = {1000, 0, 0, 0};
                        // change back to ranges later
                        for (int count = 0;
                             (FMs_diff2[0] != FMs_diff2[1] || FMs_diff2[1] != FMs_diff2[2] ||
                              FMs_diff2[2] != FMs_diff2[3]) && FMs_diff2[0] > 100 && count < 150; ++count) {
                            ranges::shift_right(FMs_diff2, 1);  // shift right 1
                            FMs_diff2[0] = adjustFMSinkFunc();
                            DEBUG("count = {}\t", count, FMs_diff2[0]);
                            for (size_t i = 0; i < FMs_diff2.size(); ++i)
                                DEBUG("FMs_diff[{}] = {:e}", i, FMs_diff2[i]);
                        }
                        forest30GeneralFM();
                    }
            }
        }

        // 17 August 2022
        // Implementing conservation policy to the 10% or 30% forest
        void switch2Conserved(const int forestConcerned, const span<AgeStruct> cohortVec, const bool multifunction) {
            if (forestConcerned != 10 && forestConcerned != 30) {
                ERROR("Unknown forestConcerned = {}", forestConcerned);
                return;
            }

            for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect))) {
                const Dat &cell = appDats[plot.asID];
                double forestShareConcerned =
                        forestConcerned == 10 ? cell.O10.forestShare.back() : cell.O30.forestShare.back();

                if (forestShareConcerned > 0) {
                    double SD = cohortVec[plot.asID].getStockingDegree();
                    double biomassRot = 0;
                    double rotMaxBm = 0;

                    double biomass =
                            forestConcerned == 10 ? cell.O10.stemBiomass.back() : cell.O30.stemBiomass.back();

                    if (biomass > 0 && plot.CAboveHa > 0 && appMaiForest(plot.x, plot.y) > 0) {
                        double biomassTmp = max(biomass, cell.U_stemBiomass0);
                        // rotation time to get current biomass (without thinning)
                        biomassRot = species.at(plot.speciesType).getU(biomassTmp, appMaiForest(plot.x, plot.y));
                        rotMaxBm = species.at(plot.speciesType).getTOpt(appMaiForest(plot.x, plot.y),
                                                                        OptRotTimes::Mode::MaxBm);
                    }

                    //  TO BE CHANGED to specific cleanedWoodUse10[...] / cleanedWoodUse30[...]
                    const auto [_, cleanedWoodUseCurrent10, cleanedWoodUseCurrent30, baseValue] =
                            getCleanedWoodUseCurrent(plot);
                    double cleanedWoodUseNew =
                            forestConcerned == 10 ? cleanedWoodUseCurrent10 : cleanedWoodUseCurrent30;

                    if (biomass > 0 && appMaiForest(plot.x, plot.y) > 0 && SD != 0) {
                        double harvestTmp = appHarvestGrid(plot.x, plot.y);
                        double harvestO = cohortVec[plot.asID].cohortRes().getTotalWoodRemoval();
                        double lostHarvestTmp =
                                baseValue * harvestO * forestShareConcerned * cell.landAreaHa * plot.fTimber;
                        appHarvestGrid(plot.x, plot.y) = max(0., harvestTmp - lostHarvestTmp);

                        if (SD < 0) {
                            if (cohortVec[plot.asID].getU() < biomassRot)
                                cohortVec[plot.asID].setU(biomassRot);
                        } else {
                            cohortVec[plot.asID].setU(rotMaxBm);
                            cohortVec[plot.asID].setStockingDegree(-1);

                            forestConcerned == 10 ?
                                    appThinningForest10(plot.x, plot.y) = -1 : appThinningForest30(plot.x,
                                                                                                   plot.y) = -1;
                        }

                        if (multifunction) {
                            harvestTmp = appHarvestGrid(plot.x, plot.y);
                            harvestO = cohortVec[plot.asID].cohortRes().getTotalWoodRemoval();
                            double harvestNewTmp =
                                    cleanedWoodUseNew * harvestO * forestShareConcerned * cell.landAreaHa *
                                    plot.fTimber;
                            appHarvestGrid(plot.x, plot.y) = max(0., harvestTmp + harvestNewTmp);
                        }
                    }
                }
            }
        }

        // Calculate the harvest of wood damaged by disturbances.
        // The damaged amount is informed for each disturbance agent, grid cell and year by a special file.
        // Apply the harvest taking into account forest stand vulnerability criteria like tree species, Dbh, height, age
        // returns residueHarvest
        [[nodiscard]] array<double, numberOfCountries> disturbanceDamageHarvest(const uint16_t year) {
            array<double, numberOfCountries> residueHarvest{};  // extraction of harvest residues, tC

            for (const auto &plot: appPlots) {
                Dat &cell = appDats[plot.asID];

                double reciprocalFTimber = 1 / plot.fTimber;

                double damagedWind = 0;
                double damagedFire = 0;
                double damagedBiotic = 0;

                if constexpr (disturbanceTrend) {
                    damagedWind = sis.disturbWind[plot.simuID](year) * reciprocalFTimber;
                    damagedFire = sis.disturbFire[plot.simuID](year) * reciprocalFTimber;
                    damagedBiotic = sis.disturbBiotic[plot.simuID](year) * reciprocalFTimber;
                } else if constexpr (disturbanceExtreme)
                    if (year == disturbanceExtremeYear) {
                        damagedWind = sis.disturbWindExtreme[plot.simuID](year) * reciprocalFTimber;
                        damagedFire = sis.disturbFireExtreme[plot.simuID](year) * reciprocalFTimber;
                        damagedBiotic = sis.disturbBioticExtreme[plot.simuID](year) * reciprocalFTimber;
                    }

                const auto [cleanedWoodUseCurrent, cleanedWoodUseCurrent10, cleanedWoodUseCurrent30, _] =
                        getCleanedWoodUseCurrent(plot);

                double harvestableFire = 0;
                double harvestableWind = 0;
                double harvestableBiotic = 0;

                if (!plot.protect) {
                    harvestableFire = 0.25;  // after consulting CBM database, email by Viorel Blujdea 26.07.2023 // Uncertain
                    harvestableWind = 0.7;
                    harvestableBiotic = 0.95;
                }

                SalvageLoggingRes slrU =
                        cell.U.forestShare.back() > 0 ? appCohortsU[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                                       damagedFire,
                                                                                                       damagedBiotic,
                                                                                                       harvestableWind,
                                                                                                       harvestableFire,
                                                                                                       harvestableBiotic)
                                                      : SalvageLoggingRes{};

                SalvageLoggingRes slrO30 =
                        cell.O30.forestShare.back() > 0 ? appCohorts30[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                                          damagedFire,
                                                                                                          damagedBiotic,
                                                                                                          harvestableWind,
                                                                                                          harvestableFire,
                                                                                                          harvestableBiotic)
                                                        : SalvageLoggingRes{};

                SalvageLoggingRes slrO10 =
                        cell.O10.forestShare.back() > 0 ? appCohorts10[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                                          damagedFire,
                                                                                                          damagedBiotic,
                                                                                                          harvestableWind,
                                                                                                          harvestableFire,
                                                                                                          harvestableBiotic)
                                                        : SalvageLoggingRes{};

                // ---- we don't account for the deadwood accumulation at the moment (to be improved) ---
                SalvageLoggingRes slrN =
                        cell.N.forestShare.back() > 0 ? appCohortsN[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                                       damagedFire,
                                                                                                       damagedBiotic,
                                                                                                       harvestableWind,
                                                                                                       harvestableFire,
                                                                                                       harvestableBiotic)
                                                      : SalvageLoggingRes{};

                // ---- we don't clean salvage in the primary forest and don't account for the deadwood accumulation at the moment (to be improved) ---
                SalvageLoggingRes slrP =
                        cell.P.forestShare.back() > 0 ? appCohortsP[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                                       damagedFire,
                                                                                                       damagedBiotic,
                                                                                                       harvestableWind,
                                                                                                       harvestableFire,
                                                                                                       harvestableBiotic)
                                                      : SalvageLoggingRes{};

                const double forestShareApplied = cell.U.forestShare.back() - cell.deforestationShare;
                salvageLogging(plot.x, plot.y) =
                        (slrU.harvestedWood * forestShareApplied + cell.N.forestShare.back() * slrN.harvestedWood +
                         slrO30.harvestedWood * cleanedWoodUseCurrent30 * cell.O30.forestShare.back() +
                         slrO10.harvestedWood * cleanedWoodUseCurrent10 * cell.O10.forestShare.back()) * plot.fTimber *
                        cell.landAreaHa;

                if (slrU.damagedFire + slrO30.damagedFire + slrO10.damagedFire + slrN.damagedFire +
                    slrP.damagedFire <= 0)
                    residueHarvest[plot.country] += adjustResiduesDisturbed(plot, year, slrU, slrN, slrO30);

                const auto [deadwoodPoolIn, litterPoolIn] =
                        cell.U.forestShare.back() > 0 ? deadwoodPoolDisturbanceCalcFunc(plot, slrU,
                                                                                        cell.U.extractedResidues,
                                                                                        cell.U.extractedStump)
                                                      : pair{0., 0.};
                // theoretically, year > 2020, so cell.N.forestShare.end()[-2] must exist
                const auto [deadwoodPoolIn_new, litterPoolIn_new] =
                        cell.N.forestShare.back() > 0 ? deadwoodPoolDisturbanceCalcFunc(plot, slrN,
                                                                                        cell.U.extractedResidues,
                                                                                        cell.U.extractedStump)
                                                      : pair{0., 0.};

                // because the whole structure is transferred to the function,
                // we need to modify values here and restore them after
                // TODO should be harvestO10 * cleanedWoodUseCurrent10 or harvestO10 * (1 - cleanedWoodUseCurrent10)?
                double tmp_hwO10 = slrO10.harvestedWood;
                slrO10.harvestedWood *= cleanedWoodUseCurrent10;
                const auto [deadwoodPoolIn10, litterPoolIn10] =
                        cell.O10.forestShare.back() > 0 ? deadwoodPoolDisturbanceCalcFunc(plot, slrO10,
                                                                                          cell.O10.extractedResidues,
                                                                                          cell.O10.extractedStump)
                                                        : pair{0., 0.};
                slrO10.harvestedWood = tmp_hwO10;

                const auto [deadwoodPoolIn30, litterPoolIn30] =
                        cell.O30.forestShare.back() > 0 ? deadwoodPoolDisturbanceCalcFunc(plot, slrO30,
                                                                                          cell.O30.extractedResidues,
                                                                                          cell.O30.extractedStump)
                                                        : pair{0., 0.};

                // because the whole structure is transferred to the function,
                // we need to modify values here and restore them after
                double tmp_hwP = exchange(slrP.harvestedWood, 0);
                const auto [deadwoodPoolInP, litterPoolInP] =
                        cell.P.forestShare.back() > 0 ? deadwoodPoolDisturbanceCalcFunc(plot, slrP,
                                                                                        cell.P.extractedResidues,
                                                                                        cell.P.extractedStump)
                                                      : pair{0., 0.};
                slrP.harvestedWood = tmp_hwP;

                cell.U.burntDeadwood = min(slrU.damagedFire, 0.9 * cell.U.deadwood);
                cell.O30.burntDeadwood = min(slrO30.damagedFire, 0.9 * cell.O30.deadwood);
                cell.O10.burntDeadwood = min(slrO10.damagedFire, 0.9 * cell.O10.deadwood);
                cell.N.burntDeadwood = min(slrN.damagedFire, 0.9 * cell.N.deadwood);
                cell.P.burntDeadwood = min(slrP.damagedFire, 0.9 * cell.P.deadwood);

                cell.U.burntLitter = min(slrU.damagedFire, 0.9 * cell.U.litter);
                cell.O30.burntLitter = min(slrO30.damagedFire, 0.9 * cell.O30.litter);
                cell.O10.burntLitter = min(slrO10.damagedFire, 0.9 * cell.O10.litter);
                cell.N.burntLitter = min(slrN.damagedFire, 0.9 * cell.N.litter);
                cell.P.burntLitter = min(slrP.damagedFire, 0.9 * cell.P.litter);

                cell.U.deadwood += deadwoodPoolIn;
                cell.N.deadwood += deadwoodPoolIn_new;
                cell.O10.deadwood += deadwoodPoolIn10;
                cell.O30.deadwood += deadwoodPoolIn30;
                cell.P.deadwood += deadwoodPoolInP;

                cell.U.litter += litterPoolIn;
                cell.N.litter += litterPoolIn_new;
                cell.O10.litter += litterPoolIn10;
                cell.O30.litter += litterPoolIn30;
                cell.P.litter += litterPoolInP;
            }

            return residueHarvest;
        }

        /*
        Modified 12 May 2020 according to suggestion by Fulvio Di Fulvio:
        First collect branches and harvest losses at the clear-cut areas, then,
        if necessary, collect stumps at the clear-cut areas,
        then, if necessary, collect branches and harvest losses at the low-intensity harvest areas
        then, if necessary, collect dead trees in the low-intensity management forests
        returns residueHarvestDist
         */
        [[nodiscard]] double adjustResiduesDisturbed(const DataStruct &plot, const uint16_t year,
                                                     const SalvageLoggingRes &slrU,
                                                     const SalvageLoggingRes &slrN,
                                                     const SalvageLoggingRes &slrO30) {
            if (year <= residueHarvYear || plot.protect || plot.residuesUseShare <= 0)
                return 0;

            const Dat &cell = appDats[plot.asID];
            // TODO if (!appForest30Policy)?
            const double forestShareApplied = cell.U.forestShare.back() - cell.deforestationShare;
            const double residueUse30 = !appForest30Policy;

            //---- Estimation of harvest residues (branches and leaves) per grid in tC ------------------------------------
            double harvRes_fcU = 0;
            double harvRes_fcN = 0;
            double harvRes_fc30 = 0;
//            double harvRes_thO = 0;

//            double harvRes_thN = 0;
//            double harvRes_scU = 0;  // residues extracted from cleaned (old) forest that is taken for wood production
//            double harvRes_scN = 0;  // residues extracted from cleaned (new) forest that is taken for wood production
//            double harvRes_scN_notTaken = 0;  // all wood extracted from (new) cleaned forest that is not taken for wood production
//            double harvRes_scU_notTaken = 0;  // all wood extracted from (old) cleaned forest that is not taken for wood production

            if (slrU.allPositive())  // tC/ha
                harvRes_fcU = slrU.getHarvestResiduesSalvageLogging(plot.BEF(slrU.getGrowingStockCut()));

            if (slrN.allPositive())  // tC/ha
                harvRes_fcN = slrN.getHarvestResiduesSalvageLogging(plot.BEF(slrN.getGrowingStockCut()));

            if (slrO30.allPositive())  // tC/ha
                harvRes_fc30 = slrO30.getHarvestResiduesSalvageLogging(plot.BEF(slrO30.getGrowingStockCut()));

            double stumpU = 0;       // stumps of old forest // tC/ha
            double stumpN = 0;   // stumps of new forest// tC/ha
            double stump30 = 0;     // stumps of old forest // tC/ha
//            double stump_df = 0;
//            double stump10 = 0;     // stumps of old forest // tC/ha
//            double stump_df10 = 0;

//            double stump_df30 = 0;

            // calculate amount of stumps + big roots for final felling, tC/ha
            if (stumpHarvCountrySpecies.contains({plot.country, plot.speciesType}) && plot.fTimber > 0) {
                if (appThinningForest(plot.x, plot.y) > 0) {
                    double hDBHOld = slrU.getDBHSlashCut();
                    double hHOld = slrU.getHSlashCut();
                    double hDBHNew = slrN.getDBHSlashCut();
                    double hHNew = slrN.getHSlashCut();

                    if (slrU.positiveAreas() && hDBHOld > 0 && hHOld > 0) {
                        double harvestGSOld = slrU.getGrowingStockCut();
                        stumpU = plot.DBHHToStump(hDBHOld, hHOld, harvestGSOld) * slrU.getAreaRatio();
                    }

                    if (slrN.positiveAreas() && hDBHNew > 0 && hHNew > 0) {
                        double harvestGSNew = slrN.getGrowingStockCut();
                        stumpN = plot.DBHHToStump(hDBHNew, hHNew, harvestGSNew) * slrN.getAreaRatio();
                    }
                }

                if (appThinningForest30(plot.x, plot.y) > 0 && !appForest30Policy) {
                    double hDBHOld30 = slrO30.getDBHSlashCut();
                    double hHOld30 = slrO30.getHSlashCut();

                    if (slrO30.positiveAreas() && hDBHOld30 > 0 && hHOld30 > 0) {
                        double harvestGSOld30 = slrO30.getGrowingStockCut();
                        stump30 = plot.DBHHToStump(hDBHOld30, hHOld30, harvestGSOld30) * slrO30.getAreaRatio();
                    }
                }
            }

            double residuesSuit1_perHa = plot.residuesUseShare * (harvRes_fcU + harvRes_fcN);   // tC/ha
            double residuesSuit2_perHa = plot.residuesUseShare * (stumpU + stumpN);           // tC/ha

            double residuesSuit1_perHa30 = plot.residuesUseShare * harvRes_fc30;               // tC/ha
            double residuesSuit2_perHa30 = plot.residuesUseShare * stump30 * residueUse30;      // tC/ha

            double residuesSuit1 = plot.residuesUseShare *
                                   (harvRes_fcU * forestShareApplied + harvRes_fcN * cell.N.forestShare.back()) *
                                   cell.landAreaHa;
            double residuesSuit2 =
                    plot.residuesUseShare * (stumpU * forestShareApplied + stumpN * cell.N.forestShare.back()) *
                    cell.landAreaHa;

            double residuesSuit1_30 = residuesSuit1_perHa30 * cell.O30.forestShare.back() * cell.landAreaHa;
            double residuesSuit2_30 = residuesSuit2_perHa30 * cell.O30.forestShare.back() * cell.landAreaHa;

            double residueHarvestDist =
                    (residuesSuit1 + residuesSuit2 + residuesSuit1_30 + residuesSuit2_30) * plot.fTimber;

            countriesResiduesExtract_m3.inc(plot.country, year, residueHarvestDist);
            return residueHarvestDist;
        }

        // Deadwood input (d > 10cm) in the cell, tC/ha, in the old forest
        // Litter input (d <= 10cm) in the cell, tC/ha, in the old forest
        // returns pair<deadwoodPoolIn, litterPoolIn>
        [[nodiscard]] static pair<double, double>
        deadwoodPoolDisturbanceCalcFunc(const DataStruct &plot, const SalvageLoggingRes &slr,
                // wood harvested in the production forest, m3/ha
                // pure harvested area
                // total harvested stem biomass at final felling in the production forest and multifunction forest
                                        const double extractedResidues, // share of extracted logging residues
                                        const double extractedStump) // share of extracted stump
        {
            const double shareLargeBranches = 0.3; // share of branches greater than 10 cm in diameter
            // Dead wood
//            double deadwood_fc_unmanaged = 0;       // deadwood (stem) of trees dying in each age cohort and at the end of lifespan in the old forests not used for intensive wood production
//            double deadwood_fc_managed = 0;         // deadwood (stem) of trees dying in each age cohort and at the end of lifespan in the old forests used for intensive wood production
//
//            double deadwood_mort = 0;           // deadwood (stem) due to mortality in old forest that is not classified as potentially merchantable
//            double litter_mort = 0;             // litter (stem) due to mortality in old forest that is not classified as potentially merchantable

//            double deadwoodBranches_mort = 0;
//            double litterBranches_mort = 0;
//            double deadwoodStump_mort = 0;
//            double litterStump_mort = 0;

            double fcDBH = slr.getDBHSlashCut();
            double fcH = slr.getHSlashCut();
            double fcGS = slr.getGrowingStockCut();

            double deadwoodStump_fc = 0;
//            double deadWoodStump_th = 0;
            // tC/ha in the cell
            double deadwoodBranches_fc = 0;

            if (slr.positiveAreas()) {
                deadwoodStump_fc = plot.DBHHToStump(fcDBH, fcH, fcGS) * slr.getAreaRatio();
                deadwoodBranches_fc = shareLargeBranches * slr.biomassHarvest * (plot.BEF(fcGS) - 1);
            }

            double litterBranches_fc = deadwoodBranches_fc * (1 / shareLargeBranches - 1);
//            double litterBranches_th = 0;

//            double deadwoodBranches_th = 0;

            const double harvLossShare = 0.25;

            double harvL = max(0., slr.biomassHarvest - slr.harvestedWood);
            // deadwood from harvest losses in intensively managed old forest
            double deadwood_hRes_mng = (1 - harvLossShare) * harvL;
            // litter from harvest losses in intensively managed old forest
            double litter_hRes_mng = harvLossShare * harvL;

//            double thDBH_old = 0;
//            double thH_old = 0;

            // branches from harvested trees
            double residuesLeftShare = 1 - extractedResidues * plot.residuesUseShare;
            double stumpLeftShare = 1 - extractedStump * plot.residuesUseShare;

            double deadwoodPoolIn = (deadwoodBranches_fc + deadwood_hRes_mng) * residuesLeftShare +
                                    deadwoodStump_fc * stumpLeftShare;

            double litterPoolIn = (litterBranches_fc + litter_hRes_mng) * residuesLeftShare;

            return {deadwoodPoolIn, litterPoolIn};
        }

        void findMaxWoodAndCNPV(const uint16_t year) {
            for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                // Don't do initial disturbance for countries which cannot produce demanded amount of wood due to lack of forest resources or very high deforestation
                if (countriesNoFmCPol.contains(plot.country) && toAdjust.contains(plot.country))
                    if (appMaiForest(plot.x, plot.y) > 0 && appDats[plot.asID].forestShareOld(-1) > 0) {

                        if (appManagedForest(plot.x, plot.y) > 0) {
                            const double rotMAI = species.at(plot.speciesType).getTOptT(
                                    appMaiForest(plot.x, plot.y), OptRotTimes::Mode::MAI);
                            const double rotMaxBmTh = species.at(plot.speciesType).getTOptT(
                                    appMaiForest(plot.x, plot.y), OptRotTimes::Mode::MaxBm);
                            const auto [rotMaxNPV, maxNPV, harvestMaxNPV] =
                                    maxNPVRotation(plot, year, true, rotMAI, rotMaxBmTh);

                            maxNPVGrid(plot.x, plot.y) = maxNPV;
                            appRotationForest(plot.x, plot.y) = rotMaxNPV;
//                            appHarvestGrid(plot.x, plot.y) = harvestMaxNPV;
                            appCohortsU[plot.asID].setU(rotMaxNPV);
                            appCohortsN[plot.asID].setU(rotMaxNPV);

                            if (!appForest10Policy && appThinningForest10(plot.x, plot.y) > 0)
                                appCohorts10[plot.asID].setU(rotMaxNPV);

                            if (!appForest30Policy && appThinningForest30(plot.x, plot.y) > 0)
                                appCohorts30[plot.asID].setU(rotMaxNPV);

                        } else {  // if managed
                            double rotMaxBm = species.at(plot.speciesType).getTOpt(appMaiForest(plot.x, plot.y),
                                                                                   OptRotTimes::Mode::MaxBm);
                            double maxNPV = npvCalc(plot, appCohortsU[plot.asID].createSetU(rotMaxBm), year,
                                                    rotMaxBm, false).first;
                            maxNPVGrid(plot.x, plot.y) = maxNPV;
                            appRotationForest(plot.x, plot.y) = rotMaxBm;
                            appCohortsU[plot.asID].setU(rotMaxBm);
                            appCohortsN[plot.asID].setU(rotMaxBm);

                            if (appThinningForest10(plot.x, plot.y) < 0)
                                appCohorts10[plot.asID].setU(rotMaxBm);

                            if (appThinningForest30(plot.x, plot.y) < 0)
                                appCohorts30[plot.asID].setU(rotMaxBm);
                        }
                    }
        }

        // returns newHarvestTmp, newHarvestTmp10, newHarvestTmp30
        [[nodiscard]] array<double, 3> calculatePlotHarvest(const DataStruct &plot) const {
            const Dat &cell = appDats[plot.asID];

            const double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
            const double harvestO10 = appCohorts10[plot.asID].cohortRes().getTotalWoodRemoval();
            const double harvestO30 = appCohorts30[plot.asID].cohortRes().getTotalWoodRemoval();
            const double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

            const auto [cleanedWoodUseCurrent, cleanedWoodUseCurrent10, cleanedWoodUseCurrent30, _] =
                    getCleanedWoodUseCurrent(plot);

            const double forestShareApplied = cell.U.forestShare.back() - cell.deforestationShare;

            // Total current harvested wood in the cell, m3
            double newHarvestTmp =
                    (harvestO * forestShareApplied + harvestN * cell.N.forestShare.back()) * cell.landAreaHa *
                    plot.fTimber + cell.deforestationWoodTotalM3;

            if (appThinningForest(plot.x, plot.y) <= 0)
                newHarvestTmp *= cleanedWoodUseCurrent;

            double newHarvestTmp10 = harvestO10 * cell.O10.forestShare.back() * cell.landAreaHa * plot.fTimber;

            if (appThinningForest10(plot.x, plot.y) <= 0)
                newHarvestTmp10 *= cleanedWoodUseCurrent10;

            double newHarvestTmp30 = harvestO30 * cell.O30.forestShare.back() * cell.landAreaHa * plot.fTimber;

            if (appThinningForest30(plot.x, plot.y) <= 0)
                newHarvestTmp30 *= cleanedWoodUseCurrent30;

            return {newHarvestTmp, newHarvestTmp10, newHarvestTmp30};
        }

        // Control (calculate) the detailed amount of harvested wood after adjusting forest management.
        // Used together with printHarvestDiagnostics to check how do the adjustment functions work.
        // Not necessary for release.
        // woodHarvestDetailed[country]: 0 - woodHarvestTotal, 1 - UN, 2 - O10, 3 - O30, 4 - salvageLogging
        // returns woodHarvestDetailed, harvestGridLocal
        [[nodiscard]] pair<array<array<double, numberOfCountries>, 5>, DataGrid<double>>

        calculateHarvestDetailed(const bool toAdjustOnly) const {
            // 0 - woodHarvestTotal, 1 - UN, 2 - O10, 3 - O30, 4 - salvageLogging
            array<array<double, numberOfCountries>, 5> woodHarvestDetailed{};
            DataGrid<double> harvestGridLocal{resLatitude};

            for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                if (!toAdjustOnly || toAdjust.contains(plot.country)) {  // using Karnaugh map
                    const auto [newHarvestTmp, newHarvestTmp10, newHarvestTmp30] = calculatePlotHarvest(plot);

                    double woodHarvestTotal =
                            newHarvestTmp + newHarvestTmp10 + newHarvestTmp30 + salvageLogging(plot.x, plot.y);
                    woodHarvestDetailed[0][plot.country] += woodHarvestTotal;
                    woodHarvestDetailed[1][plot.country] += newHarvestTmp;
                    woodHarvestDetailed[2][plot.country] += newHarvestTmp10;
                    woodHarvestDetailed[3][plot.country] += newHarvestTmp30;
                    woodHarvestDetailed[4][plot.country] += salvageLogging(plot.x, plot.y);
                    harvestGridLocal(plot.x, plot.y) = newHarvestTmp + newHarvestTmp30;

//                    DEBUG("asID = {}\tcountry = {}\twoodHarvestTotal = {}\tnewHarvestTmp = {}\tnewHarvestTmp10 = {}\tnewHarvestTmp30 = {}\tsalvageLogging = {}",
//                          plot.asID, idCountryGLOBIOM.at(plot.country), woodHarvestTotal,
//                          newHarvestTmp, newHarvestTmp10, newHarvestTmp30, salvageLogging(plot.x, plot.y));
                }

            return {woodHarvestDetailed, harvestGridLocal};
        }

        // Control (output) the amount of harvested wood after adjusting forest management.
        // Used together with calculateHarvestDetailed to check how do the adjustment functions work.
        // Not necessary for release.
        void printHarvestDiagnostics(const array<array<double, numberOfCountries>, 5> &woodHarvestDetailed,
                                     const span<const double> harvDiff, const uint16_t year) const {
            if (harvDiff.size() != numberOfCountries) {
                FATAL("harvDiff.size() is wrong: {} != ", harvDiff.size(), numberOfCountries);
                throw runtime_error{"harvDiff.size() is wrong!"};
            }
            TRACE("Wood Harvest Post Control:");

            constexpr int width = 15;
            constexpr int precision = 6;

            // https://docs.python.org/3/library/string.html
            for (const auto country: countriesList) {
                TRACE("i = {2:3}\tharvDiff = {3:{0}.{1}f}\twoodDemand = {4:{0}.{1}f}\twoodHarvestTotal = {5:{0}.{1}f}\tUN = {6:{0}.{1}f}\tO10 = {7:{0}.{1}f}\tO30 = {8:{0}.{1}f}\tsalvageLogging = {9:{0}.{1}f}",
                      width, precision, country, harvDiff[country], dms.woodDemand.at(country)(year),
                      woodHarvestDetailed[0][country], woodHarvestDetailed[1][country], woodHarvestDetailed[2][country],
                      woodHarvestDetailed[3][country], woodHarvestDetailed[4][country]);
//                TRACE("harvestCalcPrev = {}\t\tprevUN = {}\t\tprevO10 = {}\t\tprevO30 = {}",
//                      countriesWoodHarvestM3Year.getVal(country, year - 1), countriesWoodHarvestUNM3Year.getVal(country, year - 1),
//                      countriesWoodHarvest10M3Year.getVal(country, year - 1),
//                      countriesWoodHarvest30M3Year.getVal(country, year - 1));
            }
        }

        // Adjust forest management in the case of a non-zero or zero carbon price of the carbon in tree biomass.
        // CPol = true - non-zero carbon price
        // Wood and land prices by countries
        // returns woodHarvest
        [[nodiscard]] array<double, numberOfCountries>
        fmCPol(const double fm_hurdle, const uint16_t year, const bool CPol) {
            array<double, numberOfCountries> woodHarvest{};
            {   // toAdjustOnly = CPol
                const auto [woodHarvestDetailed, harvestGridLocal] = calculateHarvestDetailed(CPol);
                woodHarvest = woodHarvestDetailed[0];
                appHarvestGrid = harvestGridLocal;
            }

            // ------- Zero pass = Adjust thinning if population density changed --------------------
            const double stockingDegree = 1.3;
            if (year > 2000 && year % 10 == 0) {
                INFO("Start Zero pass = adjust thinning if population density changed");
                adjustSD(year, 0.12, woodHarvest, stockingDegree, fm_hurdle, CPol);
            }

            {   // toAdjustOnly = CPol
                const auto [woodHarvestDetailed, _] = calculateHarvestDetailed(CPol);
                array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
            }

            // ---- First pass = adjust rotation time -------
            INFO("Start First pass = adjust rotation time");
            adjustRT(year, 0.01, woodHarvest, fm_hurdle, CPol, false);

            {   // toAdjustOnly = CPol
                const auto [woodHarvestDetailed, _] = calculateHarvestDetailed(CPol);
                array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
            }

            //----Second pass = adjust rotation time -------
            INFO("Start Second pass = adjust rotation time");
            adjustRT(year, 0.02, woodHarvest, fm_hurdle, CPol, true);

            {   // toAdjustOnly = CPol
                const auto [woodHarvestDetailed, _] = calculateHarvestDetailed(CPol);
                array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
            }

            //----Third pass = adjust rotation time -------
            INFO("Start Third pass = adjust thinning");
            adjustSD(year, 0.01, woodHarvest, stockingDegree, fm_hurdle, CPol);

            {   // toAdjustOnly = CPol
                const auto [woodHarvestDetailed, _] = calculateHarvestDetailed(CPol);
                array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
            }

            //----Forth pass = adjust rotation time -------
            INFO("Start Forth pass = adjust rotation time");
            adjustRT(year, 0.01, woodHarvest, fm_hurdle, CPol, false);

            {   // toAdjustOnly = CPol
                const auto [woodHarvestDetailed, _] = calculateHarvestDetailed(CPol);
                array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
            }

            //----Fifth pass = adjust rotation time -------
            INFO("Start Fifth pass = adjust rotation time");
            adjustRT(year, 0.02, woodHarvest, fm_hurdle, CPol, true);

            {   // toAdjustOnly = CPol
                const auto [woodHarvestDetailed, _] = calculateHarvestDetailed(CPol);
                array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
            }

            // adjustHarvestEfficiency was deprecated
            //**************** Adjust Salvage logging ************************

            if (disturbanceSwitch && toAdjust.contains(57)) // currently only for the Czech Republic
                //	if (year > 2019 && year < 2038)         // salvage logging in Czech Black scenario (NFAP, 2019)
                if (year > 2018 && year < 2051) {            // salvage logging in Czech Red scenario (NFAP, 2019)
                    INFO("Adjust salvage logging");
                    salvageLoggingCZ(year, 0.02, stockingDegree, woodHarvest, 1, CPol);
                }

            {   // toAdjustOnly = CPol
                const auto [woodHarvestDetailed, _] = calculateHarvestDetailed(CPol);
                array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
            }

            if (!appForest30Policy)  // TODO write separate loop for no forest10 policy
                forest30GeneralFM();
            return woodHarvest;
        }

        // TEMPORAL SHORTCUT FOR forest10 and forest30 when there are no the forest10 and forest30 policies
        void forest30GeneralFM() {
            for (const auto &plot: appPlots) {
                appCohorts30[plot.asID].setU(appRotationForest(plot.x, plot.y));
                double SD = appThinningForest(plot.x, plot.y);
                appThinningForest30(plot.x, plot.y) = SD;
                appCohorts30[plot.asID].setStockingDegreeMin(SD * sdMinCoef);
                appCohorts30[plot.asID].setStockingDegreeMax(SD * sdMaxCoef);
            }
        }

        // Calculate rotation that maximizes NPV
        // maiV computes internally
        // returns RotMaxNPV, MaxNPV, harvestMaxNPV
        [[nodiscard]] array<double, 3>
        maxNPVRotation(const DataStruct &plot, const uint16_t year, const bool used,
                       const double MAIRot, const double BmMaxRot) {
            const double OForestShare = appDats[plot.asID].forestShareOld(-1);

            const auto [NPV_maiRot, harv_maiRot] = npvCalc(plot, appCohortsU[plot.asID].createSetU(MAIRot), year,
                                                           MAIRot, used);

            const auto [NPV_bmMaxRot, harv_bmMaxRot] = npvCalc(plot, appCohortsU[plot.asID].createSetU(BmMaxRot),
                                                               year, MAIRot, used);

            double rotTmp1 = lerp(MAIRot, BmMaxRot, 2 - phi);
            double rotTmp2 = lerp(MAIRot, BmMaxRot, phi - 1);
            double rotTmp3 = lerp(MAIRot, BmMaxRot, 0.05);

            auto [NPV_rotTmp1, harv_rotTmp1] = npvCalc(plot, appCohortsU[plot.asID].createSetU(rotTmp1), year, MAIRot,
                                                       used);

            auto [NPV_rotTmp2, harv_rotTmp2] = npvCalc(plot, appCohortsU[plot.asID].createSetU(rotTmp2), year, MAIRot,
                                                       used);

            const auto [NPV_rotTmp3, harv_rotTmp3] = npvCalc(plot, appCohortsU[plot.asID].createSetU(rotTmp3), year,
                                                             MAIRot, used);

            double RotMaxNPV = 0;
            double MaxNPV = 0;
            double harvestMaxNPV = 0;

            if (NPV_maiRot >= max({NPV_bmMaxRot, NPV_rotTmp1, NPV_rotTmp2, NPV_rotTmp3})) {
                RotMaxNPV = MAIRot;
                MaxNPV = NPV_maiRot;
                harvestMaxNPV = harv_maiRot;
            } else if (NPV_bmMaxRot >= max({NPV_maiRot, NPV_rotTmp1, NPV_rotTmp2, NPV_rotTmp3})) {
                // we take the shortest rotation with max NPV
                MaxNPV = NPV_bmMaxRot;
                if (NPV_bmMaxRot == NPV_rotTmp1) {
                    RotMaxNPV = rotTmp1;
                    harvestMaxNPV = harv_rotTmp1;
                } else if (NPV_bmMaxRot == NPV_rotTmp2) {
                    RotMaxNPV = rotTmp2;
                    harvestMaxNPV = harv_rotTmp2;
                } else if (NPV_bmMaxRot == NPV_rotTmp3) {
                    RotMaxNPV = rotTmp3;
                    harvestMaxNPV = harv_rotTmp3;
                } else {
                    RotMaxNPV = BmMaxRot;
                    harvestMaxNPV = harv_bmMaxRot;
                }
            } else {
                double dRot = 100, rotTmpL = MAIRot, rotTmpR = BmMaxRot;
                for (int NStep = 0; dRot > 1 && NStep < 15; ++NStep) {
                    if (NPV_rotTmp1 >= NPV_rotTmp2) {
                        rotTmpR = rotTmp2;
                        rotTmp2 = rotTmp1;
                        rotTmp1 = lerp(rotTmpL, rotTmpR, 2 - phi);
                        NPV_rotTmp2 = NPV_rotTmp1;
                        tie(NPV_rotTmp1, harvestMaxNPV) = npvCalc(plot, appCohortsU[plot.asID].createSetU(rotTmp1),
                                                                  year, MAIRot, used);
                        RotMaxNPV = rotTmp1;
                        MaxNPV = NPV_rotTmp1;
                    } else {
                        rotTmpL = rotTmp1;
                        rotTmp1 = rotTmp2;
                        rotTmp2 = lerp(rotTmpL, rotTmpR, phi - 1);
                        NPV_rotTmp1 = NPV_rotTmp2;
                        tie(NPV_rotTmp2, harvestMaxNPV) = npvCalc(plot, appCohortsU[plot.asID].createSetU(rotTmp2),
                                                                  year, MAIRot, used);
                        RotMaxNPV = rotTmp2;
                        MaxNPV = NPV_rotTmp2;
                    }
                    dRot = rotTmpR - rotTmpL;
                }
            }
            return {RotMaxNPV, MaxNPV, harvestMaxNPV};
        }

        // Calculate NPV for the period from current year till end of the modeling
        // cohortTmp now isn't a lvalue reference, use AgeStruct::createSetU for encapsulation
        // maiV computes internally
        // returns npvSum, harvestTmp
        [[nodiscard]] pair<double, double>
        npvCalc(const DataStruct &plot, AgeStruct cohortTmp, const uint16_t year, const double rotation,
                const bool used, const double wpMult = 1, const bool zeroC = false) const {
            double maiV = appMaiForest(plot.x, plot.y) * plot.fTimber;
            double rotMAI = species.at(plot.speciesType).getTOptT(maiV / plot.fTimber, OptRotTimes::Mode::MAI);
            double costsScaling = plot.priceIndex / priceIndexAvgEU27;
            if (plot.country == 69)
                costsScaling *= scaleCostsFactorEs;
            double damageRiscCosts = max(0., (rotation - rotMAI) * damageRiscCostsYear * costsScaling);
            double harvestTmp = 0;
            double harvestedArea = 0;

            double npvSum = 0;

            uint16_t maxYear = coef.eYear;
            if (!zeroC) {
                size_t n = bauScenarios.biomassBauScenarios.at(suffix0).size();
                maxYear = min(maxYear, static_cast<uint16_t>(refYear + n * modTimeStep));
            }
            for (int j = 0; year + j + modTimeStep <= maxYear; j += modTimeStep) {
                CohortRes cohort_res = cohortTmp.aging();
                double realAreaO = clamp(cohort_res.realArea, 0., 1.);  // cohortTmp.getArea()
                double bm = realAreaO > 0 && !zeroC ? cohortTmp.getBm() / realAreaO : 0;

                if (used) {
                    harvestTmp = cohort_res.getTotalWoodRemoval() * plot.fTimber;
                    harvestedArea = cohort_res.finalCut.area; // cohortTmp.getArea(size_t{0})
                }

                double NPVOne = forNPV_fdc(plot, year + j, harvestedArea, harvestTmp, zeroC ? nan("") : bm, wpMult,
                                           !zeroC);
                if (!used && wpMult > 1)
                    NPVOne += wpMult - 1;
                NPVOne -= damageRiscCosts;

                npvSum += NPVOne * pow(1 + plot.R(year), -j);
            }
            return {npvSum, harvestTmp};
        }

        // Calculate value of one rotation
        // fd if fdcFlag is false
        // maiV computes internally
        [[nodiscard]] double
        forNPV_fdc(const DataStruct &plot, const uint16_t year, const double harvestedArea, const double harvestedW,
                   const double biomassCur, const double wpMult = 1, const bool fdcFlag = true) const {
            double priceHarvest = 0;  // Price to harvest the timber
            // Beside harvesting costs also thinning costs, branch-removal,... can be considered plot.harvCost[year];
            // Costs to plant 1 ha of forest
            // return plantingCosts0 * priceIndex(year) / priceIndex0;
            // Maybe these costs do not occur on the second rotation interval
            // because of natural regeneration coppice forests

            // Costs to plant 1 ha of forest
            double plantingCosts = 0;
            double maiV = appMaiForest(plot.x, plot.y) * plot.fTimber;
            if (harvestedArea > 0) {
                double plantRate = clamp((maiV - 3) / 6., 0., 1.);
                plantingCosts =
                        plantRate * coef.plantingCostsR * harvestedArea * plot.priceIndex / coef.priceIndexE;
            }

            double priceWExt = 0;

            if (harvestedW > 0) {
                const double sFor = (1 - appDats[plot.asID].forestShareOld(-1)) * 9 + 1;
                // MG: use internal G4M wood price
                // MG: Changed to external SawnLogsPrice
                double c4 = (coef.priceTimberMaxR - coef.priceTimberMinR) / 99;
                double c3 = coef.priceTimberMinR - c4;
                priceWExt = (c3 + c4 * plot.sPopDens(year) * sFor) * plot.priceIndex / coef.priceIndexE *
                            dms.woodPrice.at(plot.country)(year) / dms.woodPrice.at(plot.country).data.at(2000) *
                            wpMult;
            }

            // extension to total biomass
            double CBenefit = 0;
            if (fdcFlag) {
                size_t idx = (year - refYear) / modTimeStep - 1;
                double biomassBauTmp = bauScenarios.biomassBauScenarios.at(suffix0)[idx][plot.asID];
                CBenefit = 1.2 * dms.CO2Price.at(plot.country)(year) * (biomassCur - biomassBauTmp);
            }

            //MG: Value of Forestry during one rotation External // Changed to 1 year!!!!
            return (priceWExt - priceHarvest) * harvestedW - plantingCosts + CBenefit;
        }

        // Adjust forest management type depending on wood demand and the state of the forests
        // wood and land prices are by countries!
        void adjustSD(const uint16_t year, const double woodProdTolerance, const span<double> woodHarvest,
                      const double stockingDegree, const double fm_hurdle, const bool CPol) {
            const double adjustTolerance = 2;

            auto CPolPart = [&](const DataStruct &plot, const double rotation, const double countryHarvestTmp,
                                const double countryWoodDemand, const double newHarvestTmp,
                                const double rotationForestTmp, const double thinningForestTmp,
                                const double rotationForestTmpNew, const double thinningForestTmpNew,
                                const int8_t managedForestTmp, const double fm_hurdle) -> void {
                if (CPol) {
                    double NPVTmp = 1e20;

                    if (countriesFmCPol.contains(plot.country) &&
                        !countriesNoFmCPol.contains(plot.country)) {

                        bool used = appThinningForest(plot.x, plot.y) > 0;
                        NPVTmp = npvCalc(plot, appCohortsU[plot.asID], year, rotation, used, fm_hurdle).first;
                    }

                    if (abs(countryHarvestTmp - countryWoodDemand) < (1 + tolerance * adjustTolerance) *
                                                                     abs(woodHarvest[plot.country] -
                                                                         countryWoodDemand) &&
                        NPVTmp >= maxNPVGrid(plot.x, plot.y)) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;
                        // usage of useChange was reconsidered
                        appManageChForest(plot.x, plot.y) = 1;

                        if (!appForest30Policy) {
                            appThinningForest30(plot.x, plot.y) = -1;
                            appCohorts30[plot.asID].setU(rotation);
                            appCohorts30[plot.asID].setStockingDegreeMin(-sdMinCoef);
                            appCohorts30[plot.asID].setStockingDegreeMax(-sdMaxCoef);
                        }

                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appThinningForest(plot.x, plot.y) = thinningForestTmp;
                        appManagedForest(plot.x, plot.y) = managedForestTmp;
                        appCohortsU[plot.asID].setU(rotationForestTmp);
                        appCohortsU[plot.asID].setStockingDegreeMin(thinningForestTmp * sdMinCoef);
                        appCohortsU[plot.asID].setStockingDegreeMax(thinningForestTmp * sdMaxCoef);

                        rotationForestNew(plot.x, plot.y) = rotationForestTmpNew;
                        thinningForestNew(plot.x, plot.y) = thinningForestTmpNew;
                        appCohortsN[plot.asID].setU(rotationForestTmpNew);
                        appCohortsN[plot.asID].setStockingDegreeMin(
                                thinningForestTmpNew * sdMinCoef);
                        appCohortsN[plot.asID].setStockingDegreeMax(
                                thinningForestTmpNew * sdMaxCoef);
                    }
                } else {
                    if (abs(countryHarvestTmp - countryWoodDemand) < (1 + tolerance * adjustTolerance) *
                                                                     abs(woodHarvest[plot.country] -
                                                                         countryWoodDemand)) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;
                        appManageChForest(plot.x, plot.y) = 1;

                        if (!appForest30Policy) {
                            appThinningForest30(plot.x, plot.y) = -1;
                            appCohorts30[plot.asID].setU(rotation);
                            appCohorts30[plot.asID].setStockingDegreeMin(-sdMinCoef);
                            appCohorts30[plot.asID].setStockingDegreeMax(-sdMaxCoef);
                        }
                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appThinningForest(plot.x, plot.y) = thinningForestTmp;
                        appManagedForest(plot.x, plot.y) = managedForestTmp;
                        appCohortsU[plot.asID].setU(rotationForestTmp);
                        appCohortsU[plot.asID].setStockingDegreeMin(thinningForestTmp * sdMinCoef);
                        appCohortsU[plot.asID].setStockingDegreeMax(thinningForestTmp * sdMaxCoef);

                        rotationForestNew(plot.x, plot.y) = rotationForestTmpNew;
                        thinningForestNew(plot.x, plot.y) = thinningForestTmpNew;
                        appCohortsN[plot.asID].setU(rotationForestTmpNew);
                        appCohortsN[plot.asID].setStockingDegreeMin(
                                thinningForestTmpNew * sdMinCoef);
                        appCohortsN[plot.asID].setStockingDegreeMax(
                                thinningForestTmpNew * sdMaxCoef);
                    }
                }
            };

            for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                if (toAdjust.contains(plot.country))
                    if (appDats[plot.asID].U.forestShare.back() > 0 && appMaiForest(plot.x, plot.y) > 0) {
                        Dat &cell = appDats[plot.asID];

                        const double countryWoodDemand = dms.woodDemand.at(plot.country)(year);

                        const double rotationForestTmp = appRotationForest(plot.x, plot.y);
                        const int8_t managedForestTmp = appManagedForest(plot.x, plot.y);
                        const double thinningForestTmp = appThinningForest(plot.x, plot.y);

                        const double rotationForestTmpNew = appCohortsU[plot.asID].getU();
                        const double thinningForestTmpNew = appCohortsN[plot.asID].getStockingDegree();

                        if (woodHarvest[plot.country] < (1 - woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) <= 0) {
                                double rotMAI = 0;
                                double rotMaxBmTh = 0;
                                double biomassRotTh2 = 0;

                                if (cell.U.stemBiomass.back() > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0) {
                                    // rotation time to get current biomass (with thinning)
                                    biomassRotTh2 = species.at(plot.speciesType).getUSdTab(
                                            cell.U_stemBiomass0, appMaiForest(plot.x, plot.y), stockingDegree);
                                    rotMAI = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                                   OptRotTimes::Mode::MAI);
                                    rotMaxBmTh = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                                       OptRotTimes::Mode::MaxBm);
                                } else if (cell.N.stemBiomass.back() > 0) {
                                    rotMAI = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                                   OptRotTimes::Mode::MAI);
                                    rotMaxBmTh = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                                       OptRotTimes::Mode::MaxBm);
                                    biomassRotTh2 = rotMaxBmTh;
                                }

                                double rotation = max(rotMAI, biomassRotTh2 + 1);

                                if (appManagedForest(plot.x, plot.y) == 0) {
                                    appManagedForest(plot.x, plot.y) = 3;
                                    appRotationType(plot.x, plot.y) = 1;
                                } else {
                                    appManagedForest(plot.x, plot.y) = 2;
                                    appRotationType(plot.x, plot.y) = 3;
                                }

                                appRotationForest(plot.x, plot.y) = rotation;
                                rotationForestNew(plot.x, plot.y) = rotation;
                                appThinningForest(plot.x, plot.y) = stockingDegree;

                                appCohortsU[plot.asID].setU(rotation);
                                appCohortsU[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                appCohortsU[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);

                                appCohortsN[plot.asID].setU(rotation);

                                if (appCohortsN[plot.asID].getActiveAge() >= rotation) {
                                    thinningForestNew(plot.x, plot.y) = stockingDegree;
                                    appCohortsN[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                    appCohortsN[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);
                                }

                                double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
                                double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

                                double forestShareApplied = cell.U.forestShare.back() - cell.deforestationShare;
                                if (!appForest30Policy)
                                    forestShareApplied += cell.O30.forestShare.back();

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp =
                                        (harvestO * forestShareApplied + harvestN * cell.N.forestShare.back()) *
                                        cell.landAreaHa * plot.fTimber + cell.deforestationWoodTotalM3;
                                double countryHarvestTmp =
                                        woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                double costsScaling = plot.priceIndex / priceIndexAvgEU27;
                                double roadMultifunctional = meanRoadDensityMultifunctional;
                                if (plot.country == 69) {
                                    costsScaling *= scaleCostsFactorEs;
                                    roadMultifunctional = meanOptimalRoadDensityMultifunctional;
                                }

                                if (CPol) {
                                    double NPVTmp = 1e32;

                                    if (countriesFmCPol.contains(plot.country) &&
                                        !countriesNoFmCPol.contains(plot.country)) {

                                        bool used = appThinningForest(plot.x, plot.y) > 0;
                                        NPVTmp = npvCalc(plot, appCohortsU[plot.asID], year, rotation, used,
                                                         fm_hurdle).first;

                                        if (cell.constructedRoadsMultifunction < 0 &&
                                            appManageChForest(plot.x, plot.y) == 0)
                                            NPVTmp -= cell.currentYearRoadInvestment +
                                                      (meanRoadDensityProduction - roadMultifunctional -
                                                       cell.constructedRoadsMultifunction) *
                                                      forestRoadConstructionCostsEuroM * costsScaling;
                                    }

                                    if (abs(countryHarvestTmp - countryWoodDemand) < (1 + tolerance * adjustTolerance) *
                                                                                     abs(woodHarvest[plot.country] -
                                                                                         countryWoodDemand) &&
                                        NPVTmp >= maxNPVGrid(plot.x, plot.y)) {
                                        woodHarvest[plot.country] = countryHarvestTmp;
                                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;
                                        // usage of useChange was reconsidered
                                        appManageChForest(plot.x, plot.y) = 1;
                                        cell.constructedRoadsMultifunction =
                                                meanRoadDensityProduction - roadMultifunctional;
                                        cell.harvestEfficiencyMultifunction = 0;

                                        if (!appForest30Policy) {
                                            appThinningForest30(plot.x, plot.y) = stockingDegree;
                                            appCohorts30[plot.asID].setU(rotation);
                                            appCohorts30[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                            appCohorts30[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);
                                        }

                                    } else { // return old values
                                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                                        appThinningForest(plot.x, plot.y) = thinningForestTmp;
                                        appManagedForest(plot.x, plot.y) = managedForestTmp;
                                        appCohortsU[plot.asID].setU(rotationForestTmp);
                                        appCohortsU[plot.asID].setStockingDegreeMin(thinningForestTmp * sdMinCoef);
                                        appCohortsU[plot.asID].setStockingDegreeMax(thinningForestTmp * sdMaxCoef);

                                        rotationForestNew(plot.x, plot.y) = rotationForestTmpNew;
                                        thinningForestNew(plot.x, plot.y) = thinningForestTmpNew;
                                        appCohortsN[plot.asID].setU(rotationForestTmpNew);
                                        appCohortsN[plot.asID].setStockingDegreeMin(
                                                thinningForestTmpNew * sdMinCoef);
                                        appCohortsN[plot.asID].setStockingDegreeMax(
                                                thinningForestTmpNew * sdMaxCoef);
                                    }
                                } else {
                                    double NPVTmp = npvCalc(plot, appCohortsU[plot.asID], year, rotation, true, 1,
                                                            true).first;

                                    // to simplify? because meanRoadDensityProduction = roadMultifunctional
                                    // => 0 = meanRoadDensityProduction - roadMultifunctional
                                    if (cell.constructedRoadsMultifunction < 0 &&
                                        appManageChForest(plot.x, plot.y) == 0)
                                        NPVTmp -= cell.currentYearRoadInvestment +
                                                  (meanRoadDensityProduction - roadMultifunctional -
                                                   cell.constructedRoadsMultifunction) *
                                                  forestRoadConstructionCostsEuroM * costsScaling;

                                    if (abs(countryHarvestTmp - countryWoodDemand) < (1 + tolerance * adjustTolerance) *
                                                                                     abs(woodHarvest[plot.country] -
                                                                                         countryWoodDemand) &&
                                        NPVTmp > 0) {
                                        woodHarvest[plot.country] = countryHarvestTmp;
                                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;
                                        appManageChForest(plot.x, plot.y) = 1;

                                        cell.constructedRoadsMultifunction =
                                                meanRoadDensityProduction - roadMultifunctional;
                                        cell.harvestEfficiencyMultifunction = 0;

                                        if (!appForest30Policy) {
                                            appThinningForest30(plot.x, plot.y) = stockingDegree;
                                            appCohorts30[plot.asID].setU(rotation);
                                            appCohorts30[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                            appCohorts30[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);
                                        }
                                    } else { // return old values
                                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                                        appThinningForest(plot.x, plot.y) = thinningForestTmp;
                                        appManagedForest(plot.x, plot.y) = managedForestTmp;
                                        appCohortsU[plot.asID].setU(rotationForestTmp);
                                        appCohortsU[plot.asID].setStockingDegreeMin(thinningForestTmp * sdMinCoef);
                                        appCohortsU[plot.asID].setStockingDegreeMax(thinningForestTmp * sdMaxCoef);

                                        rotationForestNew(plot.x, plot.y) = rotationForestTmpNew;
                                        thinningForestNew(plot.x, plot.y) = thinningForestTmpNew;
                                        appCohortsN[plot.asID].setU(rotationForestTmpNew);
                                        appCohortsN[plot.asID].setStockingDegreeMin(
                                                thinningForestTmpNew * sdMinCoef);
                                        appCohortsN[plot.asID].setStockingDegreeMax(
                                                thinningForestTmpNew * sdMaxCoef);
                                    }
                                }
                            }

                        } else if (woodHarvest[plot.country] > (1 + woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) > 0 && appManagedForest(plot.x, plot.y) < 3) {
                                double rotMaxBm = 0;
                                if (cell.U.stemBiomass.back() > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || cell.N.stemBiomass.back() > 0)
                                    rotMaxBm = species.at(plot.speciesType).getTOpt(appMaiForest(plot.x, plot.y),
                                                                                    OptRotTimes::Mode::MaxBm);

                                double rotation = max(rotMaxBm, appRotationForest(plot.x, plot.y));

                                if (appManagedForest(plot.x, plot.y) == 2) {
                                    appManagedForest(plot.x, plot.y) = -1;
                                    appRotationType(plot.x, plot.y) = 1;
                                } else {
                                    appManagedForest(plot.x, plot.y) = -2;
                                    appRotationType(plot.x, plot.y) = 3;
                                }

                                appRotationForest(plot.x, plot.y) = rotation;
                                appThinningForest(plot.x, plot.y) = -1;
                                appCohortsU[plot.asID].setU(rotation);
                                appCohortsU[plot.asID].setStockingDegree(-1);
                                rotationForestNew(plot.x, plot.y) = rotation;
                                appCohortsN[plot.asID].setU(rotation);

                                // New forest age > rotation -> change FM for the new forest
                                if (appCohortsN[plot.asID].getActiveAge() > rotation && thinningForestTmpNew > 0) {
                                    thinningForestNew(plot.x, plot.y) = -1;
                                    appCohortsN[plot.asID].setStockingDegree(-1);
                                }

                                double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
                                double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

                                double forestShareApplied = cell.U.forestShare.back() - cell.deforestationShare;

                                if (!appForest30Policy)
                                    forestShareApplied += cell.O30.forestShare.back();

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp =
                                        (cleanedWoodUse[plot.country] + cell.harvestEfficiencyMultifunction) *
                                        (harvestO * forestShareApplied + harvestN * cell.N.forestShare.back()) *
                                        cell.landAreaHa * plot.fTimber + cell.deforestationWoodTotalM3;

                                double countryHarvestTmp =
                                        woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                         rotationForestTmp, thinningForestTmp, rotationForestTmpNew,
                                         thinningForestTmpNew, managedForestTmp, fm_hurdle);
                            }
                        }
                    }

            for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                if (toAdjust.contains(plot.country))
                    if (appDats[plot.asID].U.forestShare.back() > 0) {
                        const Dat &cell = appDats[plot.asID];

                        const double countryWoodDemand = dms.woodDemand.at(plot.country)(year);

                        const double rotationForestTmp = appRotationForest(plot.x, plot.y);
                        const int8_t managedForestTmp = appManagedForest(plot.x, plot.y);
                        const double thinningForestTmp = appThinningForest(plot.x, plot.y);

                        const double rotationForestTmpNew = appCohortsU[plot.asID].getU();
                        const double thinningForestTmpNew = appCohortsN[plot.asID].getStockingDegree();

                        if (woodHarvest[plot.country] > (1 - woodProdTolerance) * countryWoodDemand)
                            if (appManagedForest(plot.x, plot.y) > 0) {
                                double rotMaxBm = 0;
                                if (cell.U.stemBiomass.back() > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || cell.N.stemBiomass.back() > 0)
                                    rotMaxBm = species.at(plot.speciesType).getTOpt(appMaiForest(plot.x, plot.y),
                                                                                    OptRotTimes::Mode::MaxBm);

                                double rotation = max(rotMaxBm, appRotationForest(plot.x, plot.y));

                                switch (appManagedForest(plot.x, plot.y)) {
                                    case 3:
                                        appManagedForest(plot.x, plot.y) = 0;
                                        appRotationType(plot.x, plot.y) = 1;
                                        break;
                                    case 2:
                                        appManagedForest(plot.x, plot.y) = -1;
                                        appRotationType(plot.x, plot.y) = 1;
                                        break;
                                    default:
                                        appManagedForest(plot.x, plot.y) = -2;
                                        appRotationType(plot.x, plot.y) = 3;
                                }

                                appRotationForest(plot.x, plot.y) = rotation;
                                appThinningForest(plot.x, plot.y) = -1;
                                appCohortsU[plot.asID].setU(rotation);
                                appCohortsU[plot.asID].setStockingDegree(-1);
                                rotationForestNew(plot.x, plot.y) = rotation;
                                appCohortsN[plot.asID].setU(rotation);

                                // New forest age > rotation -> change FM for the new forest
                                if (appCohortsN[plot.asID].getActiveAge() > rotation && thinningForestTmpNew > 0) {
                                    thinningForestNew(plot.x, plot.y) = -1;
                                    appCohortsN[plot.asID].setStockingDegree(-1);
                                }

                                double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
                                double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

                                double forestShareApplied = cell.U.forestShare.back() - cell.deforestationShare;

                                if (!appForest30Policy)
                                    forestShareApplied += cell.O30.forestShare.back();

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp =
                                        (cleanedWoodUse[plot.country] + cell.harvestEfficiencyMultifunction) *
                                        (harvestO * forestShareApplied + harvestN * cell.N.forestShare.back()) *
                                        cell.landAreaHa * plot.fTimber + cell.deforestationWoodTotalM3;

                                double countryHarvestTmp =
                                        woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                         rotationForestTmp, thinningForestTmp, rotationForestTmpNew,
                                         thinningForestTmpNew, managedForestTmp, fm_hurdle);
                            }
                    }
        }

        // Adjust rotation time depending on wood demand and the state of the forests
        // CPol = True - used for non-zero C price
        // allMng = False - shorten rotation in more productive forests if we need to increase wood harvest
        //                  and extend rotation in less productive forests if we need to decrease wood harvest
        // allMng = True - adjust rotation in forests
        void adjustRT(const uint16_t year, const double woodProdTolerance, const span<double> woodHarvest,
                      const double fm_hurdle, const bool CPol, const bool allMng) {

            auto CPolPart = [&](const DataStruct &plot, const double rotation, const double countryHarvestTmp,
                                const double countryWoodDemand, const double newHarvestTmp,
                                const double rotationForestTmp, const double NPVTmp0) -> void {
                if (CPol) {
                    double NPVTmp = 1e20;

                    if (countriesFmCPol.contains(plot.country) &&
                        !countriesNoFmCPol.contains(plot.country)) {

                        bool used = appThinningForest(plot.x, plot.y) > 0;
                        // fmHurdle as wpMult in the future
                        NPVTmp = npvCalc(plot, appCohortsU[plot.asID], year, rotation,
                                         used, fm_hurdle).first;
                    }

                    if (abs(countryHarvestTmp - countryWoodDemand) <
                        (1 + tolerance) * abs(woodHarvest[plot.country] - countryWoodDemand) &&
                        NPVTmp >= maxNPVGrid(plot.x, plot.y)) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;

                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appCohortsU[plot.asID].setU(rotationForestTmp);
                        appCohortsN[plot.asID].setU(rotationForestTmp);
                    }
                } else {
                    double NPVTmp1 = npvCalc(plot, appCohortsU[plot.asID], year, rotation, true, 1, true).first;
                    if (abs(countryHarvestTmp - countryWoodDemand) <
                        (1 + tolerance) * abs(woodHarvest[plot.country] - countryWoodDemand) &&
                        NPVTmp1 > 0 && NPVTmp1 >= (1 - npvLoss) * NPVTmp0) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;

                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appCohortsU[plot.asID].setU(rotationForestTmp);
                        appCohortsN[plot.asID].setU(rotationForestTmp);
                    }
                }
            };

            for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                if (toAdjust.contains(plot.country))
                    if (appDats[plot.asID].U.forestShare.back() > 0) {
                        const Dat &cell = appDats[plot.asID];

                        const double countryWoodDemand = dms.woodDemand.at(plot.country)(year);

                        const double rotationForestTmp = appRotationForest(plot.x, plot.y);

                        if (woodHarvest[plot.country] < (1 - woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) >= 2 && !allMng ||
                                appManagedForest(plot.x, plot.y) > 0 && allMng) {

                                double rotMAI = 0;
                                if (cell.U.stemBiomass.back() > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || cell.N.stemBiomass.back() > 0)
                                    rotMAI = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                                   OptRotTimes::Mode::MAI);

                                if (appRotationForest(plot.x, plot.y) != rotMAI) {
                                    double NPVTmp0 = CPol ? 0 : npvCalc(plot, appCohortsU[plot.asID], year,
                                                                        appRotationForest(plot.x, plot.y), true, 1,
                                                                        true).first;

                                    double rotation = 0;
                                    if (rotMAI < appRotationForest(plot.x, plot.y)) // TEST for AT
                                        rotation = max(appRotationForest(plot.x, plot.y) - 5, rotMAI);
                                    else if (rotMAI > appRotationForest(plot.x, plot.y))
                                        rotation = min(appRotationForest(plot.x, plot.y) + 5, rotMAI);

                                    appRotationForest(plot.x, plot.y) = rotation;
                                    appCohortsU[plot.asID].setU(rotation);
                                    appCohortsN[plot.asID].setU(rotation);

                                    double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
                                    double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

                                    double forestShareApplied = cell.U.forestShare.back() - cell.deforestationShare;

                                    if (!appForest30Policy)
                                        forestShareApplied += cell.O30.forestShare.back();

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp =
                                            (harvestO * forestShareApplied + harvestN * cell.N.forestShare.back()) *
                                            cell.landAreaHa * plot.fTimber + cell.deforestationWoodTotalM3;

                                    double countryHarvestTmp =
                                            woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                    CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                             rotationForestTmp, NPVTmp0);
                                }
                            }
                        } else if (woodHarvest[plot.country] > (1 + woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) > 0 &&
                                (appManagedForest(plot.x, plot.y) <= 2 && !allMng || allMng)) {

                                double rotMaxBmTh = 0;
                                if (cell.U.stemBiomass.back() > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || cell.N.stemBiomass.back() > 0)
                                    rotMaxBmTh = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                                       OptRotTimes::Mode::MaxBm);

                                if (appRotationForest(plot.x, plot.y) < rotMaxBmTh) {
                                    double NPVTmp0 = CPol ? 0 : npvCalc(plot, appCohortsU[plot.asID], year,
                                                                        appRotationForest(plot.x, plot.y), true, 1,
                                                                        true).first;

                                    double rotation = min(appRotationForest(plot.x, plot.y) + 5, rotMaxBmTh);

                                    appRotationForest(plot.x, plot.y) = rotation;
                                    appCohortsU[plot.asID].setU(rotation);
                                    appCohortsN[plot.asID].setU(rotation);

                                    double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
                                    double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

                                    double forestShareApplied = cell.U.forestShare.back() - cell.deforestationShare;

                                    if (!appForest30Policy)
                                        forestShareApplied += cell.O30.forestShare.back();

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp =
                                            (harvestO * forestShareApplied + harvestN * cell.N.forestShare.back()) *
                                            cell.landAreaHa * plot.fTimber + cell.deforestationWoodTotalM3;

                                    double countryHarvestTmp =
                                            woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                    CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                             rotationForestTmp, NPVTmp0);
                                }
                            }
                        }
                    }
        }

        // Force logging of the damaged wood.
        // 1. Mark the grid cells which are infested following vulnerability criteria
        // 2. Do salvage logging
        // 3. Adjust forest management in the grid cells, which we re not infested
        // TODO deprecated
        void salvageLoggingCZ(const uint16_t year, const double woodProdTolerance, const double stockingDegree,
                              const span<double> woodHarvest, const double fm_hurdle, const bool CPol) {
            array<double, 32> allHarvestedUsed = initAllHarvestedUsed();  // CZ:  m3 total harvested wood used by the industry
            // according to NIR2022 95% of wood removals was from salvage logging
            allHarvestedUsed[0] = dms.woodDemand.at(57)(2019) * 1e-6;
            // according to NIR2022 95% of wood removals was from salvage logging
            allHarvestedUsed[1] = dms.woodDemand.at(57)(2020) * 1e-6;
            array<double, 32> salvageHarvest{};

            const uint16_t yearSalvage = year - 2019;
            const double rotationSalvage = 30;

            unordered_set<uint8_t> infested;  // mark cells with infested forest
            infested.reserve(appPlots.size());

            // For all countries, values are only for years in multiples of 10, others are interpolated.
            // Here the value must be written to 2051 before everything is changed for the 57th country.
            if (yearSalvage == 0)
                dms.woodDemand.at(57).data[2051] = dms.woodDemand.at(57)(2051);

            if (year > 2020) // now we have real data by 2020
                dms.woodDemand.at(57).data[year] = allHarvestedUsed[yearSalvage] * 1e6;

            if (yearSalvage >= 0 && yearSalvage < salvageLoggingTotal.size()) {
                // additionally filtered by countriesList (see filter plots)
                for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                    if (toAdjust.contains(plot.country) && plot.speciesType == Species::Spruce)
                        if (salvageHarvest[yearSalvage] <
                            (1 - woodProdTolerance) * salvageLoggingTotal[yearSalvage] * 1e6) {
                            const Dat &cell = appDats[plot.asID];

                            if (appManagedForest(plot.x, plot.y) <= 0) {
                                appManagedForest(plot.x, plot.y) = appManagedForest(plot.x, plot.y) == 0 ? 3 : 2;

                                appThinningForest(plot.x, plot.y) = stockingDegree;
                                appCohortsU[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                appCohortsU[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);

                                // if (newCohort_all[asID]->getActiveAge() >= Rotation_salvage)
                                thinningForestNew(plot.x, plot.y) = stockingDegree;
                                appCohortsN[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                appCohortsN[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);

                                appManageChForest(plot.x, plot.y) = 1;
                            }

                            appRotationForest(plot.x, plot.y) = rotationSalvage;
                            appCohortsU[plot.asID].setU(rotationSalvage);
                            appCohortsN[plot.asID].setU(rotationSalvage);

                            CohortRes resO;
                            if (cell.forestShareOld(-1) > 0)
                                resO = appCohortsU[plot.asID].cohortRes();

                            CohortRes resN;
                            if (cell.N.forestShare.back() > 0)
                                resN = appCohortsN[plot.asID].cohortRes();

                            const double forestShareApplied = cell.forestShareOld(-1) - cell.deforestationShare;

                            // Total current harvested wood in the cell, m3
                            double newHarvestTmp = (resO.getTotalWoodRemoval() * forestShareApplied +
                                                    resN.getTotalWoodRemoval() * cell.N.forestShare.back()) *
                                                   cell.landAreaHa * plot.fTimber;

                            woodHarvest[plot.country] += newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                            appHarvestGrid(plot.x, plot.y) = newHarvestTmp;

                            // total salvage felling in the cell m3
                            double salvageFelled = (resO.getTotalHarvestedBiomass() * forestShareApplied +
                                                    resN.getTotalHarvestedBiomass() * cell.N.forestShare.back()) *
                                                   cell.landAreaHa * plot.fTimber;
                            salvageHarvest[yearSalvage] += salvageFelled;

                            infested.insert(plot.asID);
                        }

                TRACE("PostControl 1");
                {   // toAdjustOnly = CPol
                    array<array<double, numberOfCountries>, 5> woodHarvestDetailed{};
                    array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                    ranges::copy(woodHarvest, woodHarvestDetailed[0].begin());
                    printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
                }

                // Not all felled salvage is used
                woodHarvest[57] -= (salvageLoggingTotal[yearSalvage] - salvageLoggingUsed[yearSalvage]) * 1e6;

                for (const auto tolerance: {0.02, 0.02, 0.01, 0.01, 0.01, 0.01, 0.01})
                    adjustRT_allMngSalvage(year, tolerance, woodHarvest, fm_hurdle, infested, CPol);

                TRACE("PostControl 2");
                {   // toAdjustOnly = CPol
                    array<array<double, numberOfCountries>, 5> woodHarvestDetailed{};
                    array<double, numberOfCountries> harvDiff = getHarvDiff(year, woodHarvest, CPol);
                    ranges::copy(woodHarvest, woodHarvestDetailed[0].begin());
                    printHarvestDiagnostics(woodHarvestDetailed, harvDiff, year);
                }
            } else
                WARN("salvageYear > available salvage data");
        }

        [[nodiscard]] array<double, numberOfCountries>
        getHarvDiff(const uint16_t year, const span<const double> woodHarvest, const bool toAdjustOnly) const {
            array<double, numberOfCountries> harvDiff{};

            for (const auto country: countriesList)
                if ((!toAdjustOnly || toAdjust.contains(country) && !countriesNoFmCPol.contains(country)) &&
                    dms.woodDemand.at(country)(year) > 0)
                    harvDiff[country] = abs(woodHarvest[country] / dms.woodDemand.at(country)(year) - 1);

            return harvDiff;
        }

        // TODO deprecated, made for all countries
        // Adjust rotation time for the forest that is not infested
        // wood and land prices are by countries!
        // 18 February 2019: As _3_01 but the bug with region wood prices applied to countries is corrected
        void
        adjustRT_allMngSalvage(const uint16_t year, const double woodProdTolerance, const span<double> woodHarvest,
                               const double fm_hurdle, const unordered_set<uint8_t> &infested,
                               const bool CPol = false) {
            auto CPolPart = [&](const DataStruct &plot, const double rotation, const double countryHarvestTmp,
                                const double countryWoodDemand, const double newHarvestTmp,
                                const double rotationForestTmp, const double NPVTmp0, const bool checkNPV) -> void {
                if (CPol) {
                    double NPVTmp = 1e20;

                    if (countriesFmCPol.contains(plot.country) &&
                        !countriesNoFmCPol.contains(plot.country)) {

                        bool used = appThinningForest(plot.x, plot.y) > 0;
                        // fmHurdle as wpMult in the future
                        NPVTmp = npvCalc(plot, appCohortsU[plot.asID], year, rotation, used, fm_hurdle).first;
                    }

                    if (abs(countryHarvestTmp - countryWoodDemand) <
                        (1 + tolerance) * abs(woodHarvest[plot.country] - countryWoodDemand) &&
                        NPVTmp >= maxNPVGrid(plot.x, plot.y)) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;

                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appCohortsU[plot.asID].setU(rotationForestTmp);
                        appCohortsN[plot.asID].setU(rotationForestTmp);
                    }
                } else {
                    double NPVTmp1 = npvCalc(plot, appCohortsU[plot.asID], year, rotation, true, 1, true).first;
                    if (abs(countryHarvestTmp - countryWoodDemand) <
                        (1 + tolerance) * abs(woodHarvest[plot.country] - countryWoodDemand) &&
                        (checkNPV && NPVTmp1 > 0 && NPVTmp1 >= (1 - npvLoss) * NPVTmp0 || !checkNPV)) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;

                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appCohortsU[plot.asID].setU(rotationForestTmp);
                        appCohortsN[plot.asID].setU(rotationForestTmp);
                    }
                }
            };

            for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                if (toAdjust.contains(plot.country))
                    if (plot.country == 57 && !infested.contains(plot.asID)) {
                        const Dat &cell = appDats[plot.asID];

                        double countryWoodDemand = dms.woodDemand.at(plot.country)(year);

                        double rotationForestTmp = appRotationForest(plot.x, plot.y);

                        if (woodHarvest[plot.country] < (1 - woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) > 0) {

                                double rotMAI = 0;
                                if (cell.U.stemBiomass.back() > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || cell.N.stemBiomass.back() > 0)
                                    rotMAI = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                                   OptRotTimes::Mode::MAI);

                                if (appRotationForest(plot.x, plot.y) != rotMAI) {
                                    double NPVTmp0 = CPol ? 0 : npvCalc(plot, appCohortsU[plot.asID], year,
                                                                        appRotationForest(plot.x, plot.y), true, 1,
                                                                        true).first;

                                    double rotation = 0;
                                    if (rotMAI < appRotationForest(plot.x, plot.y)) // TEST for AT
                                        rotation = max(appRotationForest(plot.x, plot.y) - 5, rotMAI);
                                    else if (rotMAI > appRotationForest(plot.x, plot.y))
                                        rotation = min(appRotationForest(plot.x, plot.y) + 5, rotMAI);

                                    appRotationForest(plot.x, plot.y) = rotation;
                                    appCohortsU[plot.asID].setU(rotation);
                                    appCohortsN[plot.asID].setU(rotation);

                                    double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
                                    double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

                                    const double forestShareApplied = cell.forestShareOld(-1) - cell.deforestationShare;

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp =
                                            (harvestO * forestShareApplied + harvestN * cell.N.forestShare.back()) *
                                            cell.landAreaHa * plot.fTimber + cell.deforestationWoodTotalM3;

                                    double countryHarvestTmp =
                                            woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                    CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                             rotationForestTmp, NPVTmp0, true);
                                }
                            }
                        } else if (woodHarvest[plot.country] > (1 + woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) > 0) {

                                double rotMaxBmTh = 0;
                                if (cell.U.stemBiomass.back() > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || cell.N.stemBiomass.back() > 0)
                                    rotMaxBmTh = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                                       OptRotTimes::Mode::MaxBm);

                                if (appRotationForest(plot.x, plot.y) < rotMaxBmTh) {
                                    double NPVTmp0 = CPol ? 0 : npvCalc(plot, appCohortsU[plot.asID], year,
                                                                        appRotationForest(plot.x, plot.y), true, 1,
                                                                        true).first;

                                    double rotation = min(appRotationForest(plot.x, plot.y) + 5, rotMaxBmTh);

                                    appRotationForest(plot.x, plot.y) = rotation;
                                    appCohortsU[plot.asID].setU(rotation);
                                    appCohortsN[plot.asID].setU(rotation);

                                    double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
                                    double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

                                    const double forestShareApplied = cell.forestShareOld(-1) - cell.deforestationShare;

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp =
                                            (harvestO * forestShareApplied + harvestN * cell.N.forestShare.back()) *
                                            cell.landAreaHa * plot.fTimber + cell.deforestationWoodTotalM3;

                                    double countryHarvestTmp =
                                            woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                    CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                             rotationForestTmp, NPVTmp0, false);
                                }
                            }
                        }

                        if (appCohortsU[plot.asID].getU() < 20)
                            DEBUG("asID_RT2 = {}\tcountry = {}\tyear = {}\trotation = {}", plot.asID,
                                  idCountryGLOBIOM.at(plot.country), year, appCohortsU[plot.asID].getU());
                    }
        }

        // Modified 12 May 2020 according to suggestion by Fulvio Di Fulvio:
        // First collect branches and harvest losses at the clear-cut areas, then,
        // if necessary, collect stumps at the clear-cut areas,
        // then, if necessary, collect branches and harvest losses at the low-intensity harvest areas
        // then, if necessary, collect dead trees in the low-intensity management forests
        // returns harvestResiduesSoilEmissions
        [[nodiscard]] array<double, numberOfCountries>
        adjustResidues(const uint16_t year, const span<double> residueHarvest) {
            if (year <= residueHarvYear)
                return {};

            const double residueUse30 = !appForest30Policy;

            for (const auto &plot: appPlots | rv::filter(not_fn(&DataStruct::protect)))
                if (plot.residuesUseShare > 0) {
                    const Dat &cell = appDats[plot.asID];

                    const auto [cleanedWoodUseCurrent, cleanedWoodUseCurrent10, cleanedWoodUseCurrent30, _] =
                            getCleanedWoodUseCurrent(plot);

                    CohortRes resU =
                            cell.U.forestShare.back() > 0 ? appCohortsU[plot.asID].cohortRes() : CohortRes{};
                    CohortRes resN =
                            cell.N.forestShare.back() > 0 ? appCohortsN[plot.asID].cohortRes() : CohortRes{};
                    CohortRes res30 =
                            cell.O30.forestShare.back() > 0 ? appCohorts30[plot.asID].cohortRes() : CohortRes{};

                    double harvRes_fcU = 0, harvRes_thU = 0;
                    double harvRes_fcN = 0, harvRes_thN = 0;
                    double harvRes_scU = 0, harvRes_scU_notTaken = 0;
                    double harvRes_scN = 0, harvRes_scN_notTaken = 0;

                    if (appThinningForest(plot.x, plot.y) > 0) {
                        // harvRes_fcO = ((sawnW + restW) * (BEF(int(iter->SPECIESTYPE[byear]) - 1, (bmH / harvAreaO * realAreaO), iter->FTIMBER[byear]) - 1) + (bmH - (sawnW + restW)));
                        if (resU.if_fc())
                            harvRes_fcU = resU.getHarvestResiduesFinalCut(plot.BEF(resU.getHarvestGrowingStock()));
                        if (resU.if_th())
                            harvRes_thU = resU.getHarvestResiduesThinning(plot.BEF(cell.U.stemBiomass.back()));
                        if (resN.if_fc())
                            harvRes_fcN = resN.getHarvestResiduesFinalCut(plot.BEF(resN.getHarvestGrowingStock()));
                        // harvRes_thN = ((sawnThWnew + restThWnew) * (((PlantPhytHaBmGrBef - PlantPhytHaBmGr) / PlantPhytHaBmGr) - 1) + (bmThnew - (sawnThWnew + restThWnew)));
                        // For the forest planted after 2000 we calculate above-ground biomass, PlantPhytHaBmGrBef,
                        // for each age cohort in calc
                        if (resN.if_th())
                            harvRes_thN = resN.getHarvestResiduesThinning(
                                    cell.N_abovegroundBiomass(-1) / cell.N.stemBiomass.back() - 1);
                    } else if (appThinningForest(plot.x, plot.y) < 0) {
                        tie(harvRes_scU, harvRes_scU_notTaken)
                                = resU.harvestResiduesSanitaryFellings(plot.BEF(resU.getHarvestGrowingStock()),
                                                                       cleanedWoodUseCurrent,
                                                                       plot.BEF(cell.U.stemBiomass.back()));
                        tie(harvRes_scN, harvRes_scN_notTaken)
                                = resN.harvestResiduesSanitaryFellings(plot.BEF(resN.getHarvestGrowingStock()),
                                                                       cleanedWoodUseCurrent,
                                                                       cell.N_abovegroundBiomass(-1) /
                                                                       cell.N.stemBiomass.back() - 1);
                    }
                    double harvRes_fc30 = 0, harvRes_th30 = 0;
                    double harvRes_sc30 = 0, harvRes_sc30_notTaken = 0;

                    if (appThinningForest30(plot.x, plot.y) > 0) {
                        if (res30.if_fc())
                            harvRes_fc30 = res30.getHarvestResiduesFinalCut(plot.BEF(res30.getHarvestGrowingStock()));
                        if (res30.if_th())
                            harvRes_th30 = res30.getHarvestResiduesThinning(plot.BEF(cell.O30.stemBiomass.back()));
                    } else if (appThinningForest30(plot.x, plot.y) < 0 && residueUse30 > 0)
                        tie(harvRes_sc30, harvRes_sc30_notTaken)
                                = res30.harvestResiduesSanitaryFellings(plot.BEF(res30.getHarvestGrowingStock()),
                                                                        cleanedWoodUseCurrent30,
                                                                        plot.BEF(cell.O30.stemBiomass.back()));

                    double stumpU = 0;       // stumps of old forest, tC/ha
                    double stumpN = 0;   // stumps of new forest, tC/ha
                    double stump30 = 0;     // stumps of old forest, tC/ha

                    if (stumpHarvCountrySpecies.contains({plot.country, plot.speciesType})) {
                        if (appThinningForest(plot.x, plot.y) > 0) {
                            // calculate amount of stumps + big roots for final felling, tC/ha
                            if (resU.positiveAreas())
                                stumpU = plot.DBHHToStump(resU.finalCut.DBH, resU.finalCut.H,
                                                          resU.getHarvestGrowingStock());
                            if (resN.positiveAreas())
                                stumpN = plot.DBHHToStump(resN.finalCut.DBH, resN.finalCut.H,
                                                          resN.getHarvestGrowingStock());
                        }
                        if (appThinningForest30(plot.x, plot.y) > 0 && residueUse30 > 0) {
                            // calculate amount of stumps + big roots for final felling, tC/ha
                            if (res30.positiveAreas())
                                stump30 = plot.DBHHToStump(res30.finalCut.DBH, res30.finalCut.H,
                                                           res30.getHarvestGrowingStock());
                        }
                    }
                    /*
					WORKING ON
					if (dat_all[asID].defBiomass>0) {
                     float Dbh_avg = cohortTmp.getDBHthinned();
                     float h_avg = cohortTmp.getHthinned();
                     if (harvAreaO>0 && realAreaO>0 && iter->FTIMBER[2000]>0 && Dbh_avg>0 && h_avg>0)
                     stump_df = DbhhToStump((int)iter->SPECIESTYPE[2000]-1,
                     Dbh_avg,h_avg,dat_all[asID].defBiomass,iter->FTIMBER[2000])*dat_all[asID].deforestShare/realAreaO;
                    }
					*/
                    // Locate the struct with asID==asID within the country. Binary search O(log n)
                    auto it_hr = lower_bound(appHarvestResiduesCountry.at(plot.country).begin(),
                                             appHarvestResiduesCountry.at(plot.country).end(), plot.asID,
                                             [](const HarvestResidues &hr, const size_t asID) -> bool {
                                                 return hr.asID < asID;
                                             });
                    if (it_hr == appHarvestResiduesCountry.at(plot.country).end() || it_hr->asID != plot.asID) {
                        FATAL("country = {}, asID = {} not found in harvestResiduesCountry", plot.country, plot.asID);
                        throw runtime_error{"Missing asID harvestResidues"};
                    }

                    auto &hr = *it_hr;
                    hr.protect = false;
                    hr.species = plot.speciesType;

                    hr.U.residuesSuit1_perHa = harvRes_fcU + harvRes_thU + harvRes_fcN + harvRes_thN;
                    hr.U.residuesSuit2_perHa = stumpU + stumpN;
                    hr.U.residuesSuit3_perHa = harvRes_scU + harvRes_scN;
                    hr.U.residuesSuit4_notTaken_perHa = harvRes_scU_notTaken + harvRes_scN_notTaken;

                    hr.U.residuesSuit1_perHa *= plot.residuesUseShare;
                    hr.U.residuesSuit2_perHa *= plot.residuesUseShare;
                    hr.U.residuesSuit3_perHa *= plot.residuesUseShare;
                    hr.U.residuesSuit4_notTaken_perHa *= plot.residuesUseShare;

                    hr.O30.residuesSuit1_perHa = harvRes_fc30 + harvRes_th30;
                    hr.O30.residuesSuit2_perHa = stump30 * residueUse30;
                    hr.O30.residuesSuit3_perHa = harvRes_sc30 * residueUse30;
                    hr.O30.residuesSuit4_notTaken_perHa = harvRes_sc30_notTaken;

                    hr.O30.residuesSuit1_perHa *= plot.residuesUseShare;
                    hr.O30.residuesSuit2_perHa *= plot.residuesUseShare;
                    hr.O30.residuesSuit3_perHa *= plot.residuesUseShare;
                    hr.O30.residuesSuit4_notTaken_perHa *= plot.residuesUseShare;

                    hr.U.residuesSuit1 = (harvRes_fcU + harvRes_thU) * cell.U.forestShare.back() +
                                         (harvRes_fcN + harvRes_thN) * cell.N.forestShare.back();
                    hr.U.residuesSuit2 = stumpU * cell.U.forestShare.back() + stumpN * cell.N.forestShare.back();
                    hr.U.residuesSuit3 =
                            harvRes_scU * cell.U.forestShare.back() + harvRes_scN * cell.N.forestShare.back();
                    hr.U.residuesSuit4_notTaken = harvRes_scU_notTaken * cell.U.forestShare.back() +
                                                  harvRes_scN_notTaken * cell.N.forestShare.back();

                    double useShareArea = plot.residuesUseShare * cell.landAreaHa;
                    hr.U.residuesSuit1 *= useShareArea;
                    hr.U.residuesSuit2 *= useShareArea;
                    hr.U.residuesSuit3 *= useShareArea;
                    hr.U.residuesSuit4_notTaken *= useShareArea;

                    double shareArea30 = cell.O30.forestShare.back() * cell.landAreaHa;
                    hr.O30.residuesSuit1 = hr.O30.residuesSuit1_perHa * shareArea30;
                    hr.O30.residuesSuit2 = hr.O30.residuesSuit2_perHa * shareArea30;
                    hr.O30.residuesSuit3 = hr.O30.residuesSuit3_perHa * shareArea30;
                    hr.O30.residuesSuit4_notTaken = hr.O30.residuesSuit4_notTaken_perHa * shareArea30;

                    // branches and harvest losses of intensive clear-cut areas
                    hr.costsSuit1 =
                            hr.U.residuesSuit1_perHa > 0 || hr.O30.residuesSuit1 > 0 ? plot.residuesUseCosts : 1000;
                    // stumps of intensive clear-cut areas
                    hr.costsSuit2 = hr.U.residuesSuit2_perHa > 0 || hr.O30.residuesSuit2 > 0 ? hr.costsSuit1 + 10 : 0;
                    // branches and harvest losses of non-intensive selective logging areas
                    hr.costsSuit3 =
                            hr.U.residuesSuit3_perHa > 0 || hr.O30.residuesSuit3 > 0 ? plot.residuesUseCosts : 0;
                    // dead trees of non-intensively managed areas
                    hr.costsSuit4_notTaken = hr.U.residuesSuit4_notTaken_perHa > 0 || hr.O30.residuesSuit4_notTaken > 0
                                             ? plot.residuesUseCosts : 0;

                    hr.initTotalCost();

                    hr.U.usedForest = appThinningForest(plot.x, plot.y) > 0;
                    hr.O30.usedForest = appThinningForest30(plot.x, plot.y) > 0;

                    hr.em_harvRes_fcU = harvRes_fcU > 0;
                    hr.em_harvRes_thU = harvRes_thU > 0;
                    hr.em_harvRes_fcN = harvRes_fcN > 0;
                    hr.em_harvRes_thN = harvRes_thN > 0;
                    hr.em_harvRes_scU = harvRes_scU > 0;
                    hr.em_harvRes_scN = harvRes_scN > 0;

                    hr.em_harvRes_fc30 = harvRes_fc30 > 0;
                    hr.em_harvRes_th30 = harvRes_th30 > 0;
                    hr.em_harvRes_sc30 = harvRes_sc30 > 0;

                    hr.useSuit1 = 0;
                    hr.useSuit2 = 0;
                    hr.useSuit3 = 0;
                    hr.useSuit4 = 0;
                }

            // Soil loss emissions resulting from extraction of harvest residues, MtCO2/year
            array<double, numberOfCountries> harvestResiduesSoilEmissions{};

            // calculate minimum costs residues use
            // go to each country
            for (const auto country: countriesList) {
                if (!commonHarvestResiduesCountry.at(country).empty()) {
                    // sort the cells by costs of residue extraction
                    sort(commonHarvestResiduesCountry.at(country).begin(),
                         commonHarvestResiduesCountry.at(country).end(),
                         [](const HarvestResidues &lop, const HarvestResidues &rop) -> bool {
                             return lop.costsSuit1 < rop.costsSuit1;
                         });

                    // Harvest residues and branches of logged trees from production forest

                    // Usual forest
                    // first check the amount of residues from branches and harvest losses in the intensive
                    // clear-cut areas and estimate respective soil loss emissions
                    for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                        if (residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                            break;

                        if (hr.U.usedForest) {
                            double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                            double lastShare = min(resDeficit / hr.U.residuesSuit1, 1.);
                            residueHarvest[country] += lastShare * hr.U.residuesSuit1;
                            appDats[hr.asID].U.extractedResidues = lastShare;
                            hr.U.timeUseSust1++;

                            double em_fU = 0, em_fN = 0;
                            if (hr.em_harvRes_fcU || hr.em_harvRes_thU)
                                em_fU = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                        appDats[hr.asID].U.forestShare.back() * appDats[hr.asID].landAreaHa;
                            if (hr.em_harvRes_fcN || hr.em_harvRes_thN)
                                em_fN = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                        appDats[hr.asID].N.forestShare.back() * appDats[hr.asID].landAreaHa;

                            hr.emissionsSuit1 = lastShare * (em_fU + em_fN);
                            harvestResiduesSoilEmissions[country] += hr.emissionsSuit1;
                            hr.useSuit1 = lastShare;
                        }
                    }

                    // Forest 30
                    // first check the amount of residues from branches and harvest losses in the intensive
                    // clear-cut areas and estimate respective soil loss emissions
                    if (!appForest30Policy)
                        for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                            if (residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                                break;

                            if (hr.U.usedForest) {
                                double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                                double lastShare = min(resDeficit / hr.O30.residuesSuit1, 1.);
                                residueHarvest[country] += lastShare * hr.O30.residuesSuit1;
                                appDats[hr.asID].O30.extractedResidues = lastShare;
                                hr.O30.timeUseSust1++;

                                double em_fU = 0;
                                if (hr.em_harvRes_fc30 || hr.em_harvRes_th30)
                                    em_fU = hr.lerpERUS(emissionsResUseSust1, hr.O30.timeUseSust1) *
                                            appDats[hr.asID].O30.forestShare.back() * appDats[hr.asID].landAreaHa;

                                hr.emissionsSuit1 = lastShare * em_fU;
                                harvestResiduesSoilEmissions[country] += hr.emissionsSuit1;
                                hr.useSuit1 = lastShare;
                            }
                        }

                    // stumps of logged trees from production forests
                    // usual forest
                    for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                        if (!stumpHarvCountrySpecies.contains({country, hr.species}) ||
                            residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                            break;

                        if (hr.U.usedForest) {
                            double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                            double lastShare = min(resDeficit / hr.U.residuesSuit2, 1.);
                            residueHarvest[country] += lastShare * hr.U.residuesSuit2;
                            appDats[hr.asID].U.extractedStump = lastShare;
                            hr.U.timeUseSust2++;

                            double em_fU = 0, em_fN = 0, em_fU_sust1 = 0, em_fN_sust1 = 0;
                            if (hr.em_harvRes_fcU || hr.em_harvRes_thU) {
                                em_fU = hr.lerpERUS(emissionsResUseSust2, hr.U.timeUseSust2) *
                                        appDats[hr.asID].U.forestShare.back() * appDats[hr.asID].landAreaHa;
                                em_fU_sust1 = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust2) *
                                              appDats[hr.asID].U.forestShare.back() * appDats[hr.asID].landAreaHa;
                            }
                            if (hr.em_harvRes_fcN || hr.em_harvRes_thN) {
                                em_fN = hr.lerpERUS(emissionsResUseSust2, hr.U.timeUseSust2) *
                                        appDats[hr.asID].N.forestShare.back() * appDats[hr.asID].landAreaHa;
                                em_fN_sust1 = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust2) *
                                              appDats[hr.asID].N.forestShare.back() * appDats[hr.asID].landAreaHa;
                            }

                            hr.emissionsSuit2 = lastShare * (em_fU + em_fN - em_fU_sust1 - em_fN_sust1);
                            harvestResiduesSoilEmissions[country] += hr.emissionsSuit2;
                            hr.useSuit2 = lastShare;
                        }
                    }

                    // stumps of logged trees from production forests
                    // forest 30
                    if (!appForest30Policy)     // no residueUse30 = !appForest30Policy
                        for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                            if (!stumpHarvCountrySpecies.contains({country, hr.species}) ||
                                residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                                break;

                            if (hr.O30.usedForest) {
                                double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                                double lastShare = min(resDeficit / hr.O30.residuesSuit2, 1.);
                                residueHarvest[country] += lastShare * hr.O30.residuesSuit2;
                                appDats[hr.asID].O30.extractedStump = lastShare;
                                hr.O30.timeUseSust2++;

                                double em_fO30 = 0, em_fO30_sust1 = 0;
                                if (hr.em_harvRes_fc30 || hr.em_harvRes_th30) {
                                    em_fO30 = hr.lerpERUS(emissionsResUseSust2, hr.O30.timeUseSust2) *
                                              appDats[hr.asID].O30.forestShare.back() * appDats[hr.asID].landAreaHa;
                                    em_fO30_sust1 = hr.lerpERUS(emissionsResUseSust1, hr.O30.timeUseSust2) *
                                                    appDats[hr.asID].O30.forestShare.back() *
                                                    appDats[hr.asID].landAreaHa;
                                }

                                hr.emissionsSuit2 = lastShare * (em_fO30 - em_fO30_sust1);
                                harvestResiduesSoilEmissions[country] += hr.emissionsSuit2;
                                hr.useSuit2 = lastShare;
                            }
                        }

                    // if there is deficit of residues extracted from the intensively managed areas,
                    // harvest the residues from branches and harvest losses in the non-intensively managed areas and
                    // estimate respective soil loss emissions

                    // harvest residues and branches of logged trees from multifunctional forests
                    // usual forest
                    for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                        if (residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                            break;

                        if (hr.U.usedForest) {
                            double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                            double lastShare = min(resDeficit / hr.U.residuesSuit3, 1.);
                            residueHarvest[country] += lastShare * hr.U.residuesSuit3;
                            appDats[hr.asID].U.extractedResidues = lastShare;
                            hr.U.timeUseSust1++;

                            double em_scU = 0, em_scN = 0;
                            if (hr.em_harvRes_scU)
                                em_scU = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                         appDats[hr.asID].U.forestShare.back() * appDats[hr.asID].landAreaHa;
                            if (hr.em_harvRes_scN)
                                em_scN = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                         appDats[hr.asID].N.forestShare.back() * appDats[hr.asID].landAreaHa;

                            double cleanedWoodUseCurrent = hr.protect ? 0 : cleanedWoodUse[country];
                            em_scU *= cleanedWoodUseCurrent;
                            em_scN *= cleanedWoodUseCurrent;

                            hr.emissionsSuit3 = lastShare * (em_scU + em_scN);
                            harvestResiduesSoilEmissions[country] += hr.emissionsSuit3;
                            hr.useSuit3 = lastShare;
                        }
                    }

                    // harvest residues and branches of logged trees from multifunctional forests
                    // forest 30
                    if (!appForest30Policy)
                        for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                            if (residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                                break;

                            if (hr.O30.usedForest) {
                                double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                                double lastShare = min(resDeficit / hr.O30.residuesSuit3, 1.);
                                residueHarvest[country] += lastShare * hr.O30.residuesSuit3;
                                appDats[hr.asID].O30.extractedResidues = lastShare;
                                hr.O30.timeUseSust1++;

                                double em_scU = 0;
                                if (hr.em_harvRes_scU)
                                    em_scU = hr.lerpERUS(emissionsResUseSust1, hr.O30.timeUseSust1) *
                                             appDats[hr.asID].O30.forestShare.back() * appDats[hr.asID].landAreaHa;

                                double cleanedWoodUseCurrent30 = hr.protect ? 0 : cleanedWoodUse[country] +
                                                                                  appDats[hr.asID].harvestEfficiencyMultifunction;
                                em_scU *= cleanedWoodUseCurrent30;

                                hr.emissionsSuit3 = lastShare * em_scU;
                                harvestResiduesSoilEmissions[country] += hr.emissionsSuit3;
                                hr.useSuit3 = lastShare;
                            }
                        }

                    // whole (above-ground) trees from multifunctional forests that die but are not harvested
                    // usual forest
                    for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                        if (residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                            break;

                        if (hr.U.usedForest) {
                            double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                            double lastShare = min(resDeficit / hr.U.residuesSuit4_notTaken, 1.);
                            residueHarvest[country] += lastShare * hr.U.residuesSuit4_notTaken;
                            appDats[hr.asID].U.extractedCleaned = lastShare;
                            hr.U.timeUseSust1++;

                            double em_scU_notTaken = 0, em_scN_notTaken = 0;
                            if (hr.em_harvRes_scU)
                                em_scU_notTaken = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                                  appDats[hr.asID].U.forestShare.back() * appDats[hr.asID].landAreaHa;
                            if (hr.em_harvRes_scN)
                                em_scN_notTaken = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                                  appDats[hr.asID].N.forestShare.back() * appDats[hr.asID].landAreaHa;

                            double cleanedWoodUseCurrent = hr.protect ? 0 : cleanedWoodUse[country];
                            em_scU_notTaken *= 1 - cleanedWoodUseCurrent;
                            em_scN_notTaken *= 1 - cleanedWoodUseCurrent;

                            hr.emissionsSuit4_notTaken = lastShare * (em_scU_notTaken + em_scN_notTaken);
                            harvestResiduesSoilEmissions[country] += hr.emissionsSuit4_notTaken;
                            hr.useSuit4 = lastShare;
                        }
                    }

                    // whole (above-ground) trees from multifunctional forests that die but are not harvested
                    // forest 30
                    if (!appForest30Policy)
                        for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                            if (residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                                break;

                            if (hr.O30.usedForest) {
                                double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                                double lastShare = min(resDeficit / hr.O30.residuesSuit4_notTaken, 1.);
                                residueHarvest[country] += lastShare * hr.O30.residuesSuit4_notTaken;
                                appDats[hr.asID].O30.extractedCleaned = lastShare;
                                hr.O30.timeUseSust1++;

                                double em_scU_notTaken = 0, em_scN_notTaken = 0;
                                if (hr.em_harvRes_scU)
                                    em_scU_notTaken = hr.lerpERUS(emissionsResUseSust1, hr.O30.timeUseSust1) *
                                                      appDats[hr.asID].O30.forestShare.back() *
                                                      appDats[hr.asID].landAreaHa;

                                double cleanedWoodUseCurrent30 = hr.protect ? 0 : cleanedWoodUse[country] +
                                                                                  appDats[hr.asID].harvestEfficiencyMultifunction;
                                em_scU_notTaken *= 1 - cleanedWoodUseCurrent30;

                                hr.emissionsSuit4_notTaken = lastShare * em_scU_notTaken;
                                harvestResiduesSoilEmissions[country] += hr.emissionsSuit4_notTaken;
                                hr.useSuit4 = lastShare;
                            }
                        }
                }
                for (auto &hr: commonHarvestResiduesCountry.at(country))
                    hr.setTimeUseSust();

                for (const auto &hr: commonHarvestResiduesCountry.at(country))
                    rf.residueExtractDetailsBuffer +=
                            format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}\n",
                                   hr.simuId, idCountryGLOBIOM.at(country), year, hr.U.usedForest,
                                   appDats[hr.asID].U.forestShare.back() * appDats[hr.asID].landAreaHa * 1e-3,
                                   hr.U.residuesSuit1_perHa * hr.fTimber, hr.U.residuesSuit2_perHa * hr.fTimber,
                                   hr.U.residuesSuit3_perHa * hr.fTimber,
                                   hr.U.residuesSuit4_notTaken_perHa * hr.fTimber, hr.U.residuesSuit1 * hr.fTimber,
                                   hr.U.residuesSuit2 * hr.fTimber, hr.U.residuesSuit3 * hr.fTimber,
                                   hr.U.residuesSuit4_notTaken * hr.fTimber, hr.emissionsSuit1 * 1e-6,
                                   hr.emissionsSuit2 * 1e-6, hr.emissionsSuit3 * 1e-6,
                                   hr.emissionsSuit4_notTaken * 1e-6, hr.costsSuit1, hr.costsSuit2, hr.costsSuit3,
                                   hr.costsSuit4_notTaken, hr.costsTotal, hr.useSuit1, hr.useSuit2, hr.useSuit3,
                                   hr.useSuit4);

                // TODO fTimber by cells!
                double fTimber = commonHarvestResiduesCountry.at(country)[0].fTimber;
                countriesResiduesDemand_m3.setVal(country, year, dms.residuesDemand.at(country)(year) * fTimber);
                countriesResiduesExtract_m3.inc(country, year, residueHarvest[country] * fTimber);
                countriesResExtSoilEm_MtCO2Year.setVal(country, year, harvestResiduesSoilEmissions[country] * 1e-6);
            }
            return harvestResiduesSoilEmissions;
        }

        // returns FMs_diff2
        double adjustFMSinkFunc() {
            const double adjustToleranceFM = 0.01;
            const uint16_t adjustEndYear = 2020;
            array<double, numberOfCountries> FMs{};
            const double dRotation = 5;

            vector<double> OForestShares(appPlots.size());
            for (const auto &plot: appPlots)
                // or appDats[plot.asID].forestShareAll() - plot.oldGrowthForest_ten - plot.strictProtected
                OForestShares[plot.asID] = plot.oldGrowthForest_thirty + plot.forest;

            vector<double> bmBEF0s(appPlots.size());
            vector<double> FM_init(appPlots.size());
            for (const auto &plot: appPlots)
                if (plot.managed_UNFCCC) {
                    AgeStruct cohortTmp = appCohortsU[plot.asID];
                    const double bm0 = cohortTmp.getBm();
                    const double bmBEF0 = bm0 * plot.BEF(bm0);

                    int count = 0;
                    for (; count < adjustLength && coef.bYear + count <= adjustEndYear; ++count)
                        auto _ = cohortTmp.aging();

                    const double bm = cohortTmp.getBm();
                    const double FM_tmp =
                            (bm * plot.BEF(bm) - bmBEF0) * OForestShares[plot.asID] * appDats[plot.asID].landAreaHa *
                            molarRatio * 0.001 / count;
                    bmBEF0s[plot.asID] = bmBEF0;
                    FM_init[plot.asID] = FM_tmp;
                }

            for (const auto &plot: appPlots)
                if (plot.managed_UNFCCC)
                    FMs[plot.country] += FM_init[plot.asID];

            for (const auto &plot: appPlots)
                if (plot.managed_UNFCCC && appThinningForest(plot.x, plot.y) > 0) {
                    const Dat &cell = appDats[plot.asID];

                    if (FMs[plot.country] < (1 - adjustToleranceFM) * FM_sink_stat[plot.country]) {
                        double biomassRot = 0;
                        double rotMaxBm = 0;

                        if (OForestShares[plot.asID] > 0 && plot.CAboveHa > 0 && appMaiForest(plot.x, plot.y) > 0) {
                            biomassRot = species.at(plot.speciesType).getU(cell.U_stemBiomass0,
                                                                           appMaiForest(plot.x, plot.y));
                            rotMaxBm = species.at(plot.speciesType).getTOpt(appMaiForest(plot.x, plot.y),
                                                                            OptRotTimes::Mode::MaxBm);
                        }

                        double rotation = appRotationForest(plot.x, plot.y);
                        if (rotation < rotMaxBm) {
                            rotation = min(rotation + dRotation, rotMaxBm);

                            appCohortsU[plot.asID].setU(rotation);
                            appCohortsN[plot.asID].setU(rotation);
                            appRotationForest(plot.x, plot.y) = rotation;
                            rotationForestNew(plot.x, plot.y) = rotation;

                            AgeStruct cohortTmp = appCohortsU[plot.asID];

                            int count = 0;
                            for (; count < adjustLength && coef.bYear + count <= adjustEndYear; ++count)
                                auto _ = cohortTmp.aging();

                            const double bm = cohortTmp.getBm();
                            const double FM_tmp = (bm * plot.BEF(bm) - bmBEF0s[plot.asID]) * OForestShares[plot.asID] *
                                                  cell.landAreaHa * molarRatio * 0.001 / count;
                            const double dFM = FM_tmp - FM_init[plot.asID];
                            FMs[plot.country] += dFM;
                        }

                    } else if (FMs[plot.country] > (1 + adjustToleranceFM) * FM_sink_stat[plot.country]) {
                        double biomassRot = 0;
                        double rotMAI = 0;

                        if (OForestShares[plot.asID] > 0 && plot.CAboveHa > 0 && appMaiForest(plot.x, plot.y) > 0) {
                            biomassRot = species.at(plot.speciesType).getU(cell.U_stemBiomass0,
                                                                           appMaiForest(plot.x, plot.y));
                            rotMAI = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                           OptRotTimes::Mode::MAI);
                        }

                        double rotation = appRotationForest(plot.x, plot.y);
                        if (rotation > rotMAI) {
                            rotation = max(rotation - dRotation, rotMAI);
                            appCohortsU[plot.asID].setU(rotation);
                            AgeStruct cohortTmp = appCohortsU[plot.asID];

                            int count = 0;
                            for (; count < adjustLength && coef.bYear + count <= adjustEndYear; ++count)
                                auto _ = cohortTmp.aging();

                            const double bm = cohortTmp.getBm();
                            const double FM_tmp = (bm * plot.BEF(bm) - bmBEF0s[plot.asID]) * OForestShares[plot.asID] *
                                                  cell.landAreaHa * molarRatio * 0.001 / count;
                            const double dFM = FM_tmp - FM_init[plot.asID];
                            FMs[plot.country] += dFM;
                        }
                    }
                }

            double FMs_diff2 = 0;
            for (const auto country: countriesList) // test only some countries
                FMs_diff2 += pow(FMs[country] - FM_sink_stat[country], 2);
            return FMs_diff2;
        }

        [[nodiscard]] array<double, 4> getCleanedWoodUseCurrent(const DataStruct &plot) const noexcept {
            double cleanedWoodUseCurrent = 0;
            double cleanedWoodUseCurrent10 = 0;
            double cleanedWoodUseCurrent30 = 0;
            double baseValue = cleanedWoodUse[plot.country] + appDats[plot.asID].harvestEfficiencyMultifunction;
            if (!plot.protect) {
                cleanedWoodUseCurrent = appThinningForest(plot.x, plot.y) < 0 ? baseValue : 0;
                if (appThinningForest10(plot.x, plot.y) < 0) {
                    if (!appForest10Policy)
                        cleanedWoodUseCurrent10 = baseValue;
                    else if (appMultifunction10)
                        cleanedWoodUseCurrent10 = cleanWoodUseShare10 * baseValue;
                }
                if (appThinningForest30(plot.x, plot.y) < 0) {
                    if (!appForest30Policy)
                        cleanedWoodUseCurrent30 = baseValue;
                    else if (appMultifunction30)
                        cleanedWoodUseCurrent30 = cleanWoodUseShare30 * baseValue;
                }
            }
            return {cleanedWoodUseCurrent, cleanedWoodUseCurrent10, cleanedWoodUseCurrent30, baseValue};
        }

        void calc(const DataStruct &plot, const uint16_t year) {
            Dat &cell = appDats[plot.asID];
            cell.update();

            constexpr bool roadInfo = true;
            constexpr bool deforestationInit = true;

            cell.salvageLogging = salvageLogging(plot.x, plot.y);

            cell.rotation = appRotationForest(plot.x, plot.y);
            // Initialise cohorts
            if (year == coef.bYear) {
                double SD = appThinningForest(plot.x, plot.y);
                appCohortsU[plot.asID].setU(cell.rotation);
                appCohortsN[plot.asID].setU(cell.rotation);
                appCohorts30[plot.asID].setStockingDegreeMin(SD * sdMinCoef);
                appCohorts30[plot.asID].setStockingDegreeMax(SD * sdMaxCoef);

                appThinningForest30(plot.x, plot.y) = SD;
                thinningForestNew(plot.x, plot.y) = SD;
                rotationForestNew(plot.x, plot.y) = cell.rotation;
            }
            CohortRes resU;
            CohortRes resULost;
            {
                CohortRes tmp = appCohortsU[plot.asID].aging();
                if (double SD = appThinningForest(plot.x, plot.y); cell.U.forestShare.back() > 0 && SD != 0) {
                    if (SD > 0)
                        resU = tmp;
                    else
                        resULost = tmp;
                }
            }
            CohortRes resN;
            CohortRes resNLost;
            {
                CohortRes tmp = appCohortsN[plot.asID].aging();
                if (double SD = appThinningForest(plot.x, plot.y); cell.N.forestShare.back() > 0 && SD != 0) {
                    if (SD > 0)
                        resN = tmp;
                    else
                        resNLost = tmp;
                }
            }
            CohortRes res10;
            CohortRes res10Lost;
            {
                CohortRes tmp = appCohorts10[plot.asID].aging();
                if (double SD = appThinningForest10(plot.x, plot.y); cell.O10.forestShare.back() > 0 && SD != 0) {
                    if (SD > 0)
                        res10 = tmp;
                    else
                        res10Lost = tmp;
                }
            }
            CohortRes res30;
            CohortRes res30Lost;
            {
                CohortRes tmp = appCohorts30[plot.asID].aging();
                if (double SD = appThinningForest30(plot.x, plot.y); cell.O30.forestShare.back() > 0 && SD != 0) {
                    if (SD > 0)
                        res30 = tmp;
                    else
                        res30Lost = tmp;
                }
            }
            CohortRes resPLost;
            {
                CohortRes tmp = appCohortsP[plot.asID].aging();
                resPLost = cell.P.forestShare.back() > 0 ? tmp : CohortRes{};
            }

            cell.U.stemBiomass.back() = resU.realArea > 0 || resULost.realArea > 0 ? appCohortsU[plot.asID].getBm() : 0;
            cell.O10.stemBiomass.back() =
                    res10.realArea > 0 || res10Lost.realArea > 0 ? appCohorts10[plot.asID].getBm() : 0;
            cell.O30.stemBiomass.back() =
                    res30.realArea > 0 || res30Lost.realArea > 0 ? appCohorts30[plot.asID].getBm() : 0;
            cell.P.stemBiomass.back() = resPLost.realArea > 0 ? appCohortsP[plot.asID].getBm() : 0;
            // new forest stem biomass is calculated differently

            const auto [cleanedWoodUseCurrent, cleanedWoodUseCurrent10, cleanedWoodUseCurrent30, _] =
                    getCleanedWoodUseCurrent(plot);

            // Total current harvested wood in the cell, m3
            // Total current "lost" wood in the cell, tC (in remote forests)
            // Total current harvested wood in the cell which is not used for intensive wood production, m3
            // harvestable wood (m3/ha) including 50% of residues
            double harvestWoodU = harvestedWoodUse[plot.country] * resU.getTotalWoodRemoval() * plot.fTimber;
            double harvestWoodLU = cleanedWoodUseCurrent * resULost.getTotalWoodRemoval() * plot.fTimber;
            double harvestWoodPlusU = harvestWoodU + harvestWoodLU;

            double harvestWoodN = harvestedWoodUse[plot.country] * resN.getTotalWoodRemoval() * plot.fTimber;
            double harvestWoodLN = cleanedWoodUseCurrent * resNLost.getTotalWoodRemoval() * plot.fTimber;
            double harvestWoodPlusN = harvestWoodN + harvestWoodLN;

            double harvestWood10 = harvestedWoodUse[plot.country] * res10.getTotalWoodRemoval() * plot.fTimber;
            double harvestWoodL10 = cleanedWoodUseCurrent10 * res10Lost.getTotalWoodRemoval() * plot.fTimber;
            double harvestWoodPlus10 = harvestWood10 + harvestWoodL10;

            double harvestWood30 = harvestedWoodUse[plot.country] * res30.getTotalWoodRemoval() * plot.fTimber;
            double harvestWoodL30 = cleanedWoodUseCurrent30 * res30Lost.getTotalWoodRemoval() * plot.fTimber;
            double harvestWoodPlus30 = harvestWood30 + harvestWoodL30;

            double rotMAI = 0;
            double harvMAI = 0;
            double timberPrice = 0;
            double forestryValue = 0;
            double agriculturalValue = 0;
            double forestryValuePlus = 0;
            double plantingCosts = 0;

            if (!plot.protect) {
                // Rotation time fitted to get certain biomass under certain MAI (w/o thinning)
                if (cell.forestShareAll(-1) > 0 && plot.CAboveHa > 0 && appMaiForest(plot.x, plot.y) > 0) {
                    rotMAI = species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y),
                                                                   OptRotTimes::Mode::MAI);
                    rotMAI = max(0., rotMAI);
                }
                if (cell.rotation <= 1) {
                    ERROR("harvestWoodU = {}", harvestWoodU);
                    harvestWoodU = 0;
                }
                harvestWoodU = max(0., harvestWoodU);

                harvMAI = appMaiForest(plot.x, plot.y) * plot.fTimber * (1 - coef.harvLoos);

                DIMA decision{
                        year, plot.NPP, plot.sPopDens, plot.sAgrSuit, plot.priceIndex, coef.priceIndexE, plot.R,
                        coef.priceC, coef.plantingCostsR, coef.priceLandMinR, coef.priceLandMaxR,
                        coef.maxRotInter,
                        coef.minRotInter, coef.decRateL, coef.decRateS, plot.fracLongProd, coef.baseline,
                        plot.fTimber, coef.priceTimberMaxR, coef.priceTimberMinR, coef.fCUptake, plot.GDP,
                        coef.harvLoos, cell.forestShareOld(-1),
                        dms.woodPrice.at(plot.country), rotMAI, harvMAI};

                timberPrice = decision.priceTimberComb();   // MG: use Combined G4M + external timberPrice
                forestryValue = decision.forValComb();      // Combined G4M + external forestryValue
                plantingCosts = decision.plantingCosts();
                // We bypass the "policies" above for calibration!
                forestryValueDPrevGrid(plot.x, plot.y) = forestryValue;
                // MG: use G4M & GLOBIOM prices
                agriculturalValue = decision.agrVal() * dms.landPrice.at(plot.country)(year) /
                                    dms.landPrice.at(plot.country).data.at(2000);

                if constexpr (cellInteract) {
                    // minimum of forest share in the neighbour cells within a country
                    double minForestShare = appOForestShGrid.getMinCountry(plot.x, plot.y, 1);
                    // higher agriculture value if closer to recently deforested places
                    // agriculture is more attractive if neighbor cells are deforested
                    agriculturalValue *= 2 - ((plot.potVeg == VegetationType::TropicalEvergreenForest ||
                                               plot.potVeg == VegetationType::TropicalDeciduousForest) ?
                                              minForestShare : min(minForestShare, cell.forestShareOld(-1)));
                    // prevForestShareOld = forestShareOld at this point
                }
                // presence of road increases the probability of deforestation
                if constexpr (roadInfo)
                    if (plot.road.data.at(2000) > 0)
                        agriculturalValue *= 1.2 + plot.road.data.at(2000) * 0.0044;
                // higher agriculture value if closer to recently deforested places
                if constexpr (deforestationInit)
                    if ((plot.potVeg == VegetationType::TropicalEvergreenForest ||
                         plot.potVeg == VegetationType::TropicalDeciduousForest) && cell.deforestationPrev > 0.00014)
                        agriculturalValue *= 2;

                double maxForestShare = plot.getMaxForestShare(year);
                double afforMax = plot.afforMax(year);

                if (plot.country == 78)
                    afforMax *= 0.45;
                else if (plot.country != 71 && plot.country != 177)
                    afforMax *= 0.5;

                double afforMaxCountry = countryLandArea[plot.country] * 500;           // 0.5 * 1000
                double globiomReservedCountry = afforMaxCountry;

                if (!dms.GLOBIOM_LandCountry[plot.country].data.empty() &&
                    !dms.GLOBIOM_AfforMaxCountry[plot.country].data.empty()) {
                    globiomReservedCountry = dms.GLOBIOM_AfforMaxCountry[plot.country](year) * 1000;
                    afforMaxCountry = dms.GLOBIOM_AfforMaxCountry[plot.country](year) * 500;
                }
                double gdp = 1644;
                if (plot.popDens(year) > 0) {
                    gdp = 200'000 * plot.GDP(year) / plot.popDens(year);
                    gdp *= deflator; // Global GDP deflator GDP(1995) / GDP(2000) = 8.807 / 10 (World Bank)
                }
                if (plot.country == 162)
                    gdp = min(gdp, 25'000.);

                if (double forestShareAll = cell.forestShareAll(-1); forestShareAll > maxForestShare) {
                    const double dFor = forestShareAll - maxForestShare;
                    cell.U.forestShare.back() = max(0., cell.U.forestShare.back() - dFor);
                    // TODO TO BE IMPROVED!
                }
                const double OForestShareTmp = cell.forestShareOld(-1);

                double deforestationRate = 0;
                if (plot.agrSuit > 0 && cell.U.forestShare.back() > 0) {
                    const double popDens = plot.popDens(year);
                    deforestationRate = 0.05 / (1. + exp(1.799e+00 + 2.200e-01 / cell.forestShareOld(-1) +
                                                         1.663e-01 / plot.agrSuit +
                                                         (-4.029e-02 + 5.305e-04 * popDens) * popDens +
                                                         1.282e-04 * gdp));
                    if constexpr (deforestationInit)
                        if (plot.potVeg == VegetationType::TropicalEvergreenForest ||
                            plot.potVeg == VegetationType::TropicalDeciduousForest) {
                            if (year == coef.bYear) {
                                if (deforestationRate > 1.e-17) {
                                    // To match observed deforestation rate in the tropics
                                    // cell.deforestPrev = plot.forLoss at this point
                                    cell.deforestationRateCoefCell =
                                            max(cell.deforestationPrev, 0.000139) / deforestationRate;
                                    // less than 0.00014, the threshold of increased agriculture attractiveness
                                }
                            } else
                                deforestationRate *= cell.deforestationRateCoefCell;
                        }
                    if (year <= 2005) {
                        if (year <= 1995) {
                            if (year == 1990)
                                deforestationRate *= coef_df_mod1990[plot.country];
                            else
                                deforestationRate *= coef_df_mod1995[plot.country];
                        } else {
                            if (year <= 2000)
                                deforestationRate *= coef_df_mod2000[plot.country];
                            else
                                deforestationRate *= coef_df_mod2005[plot.country];
                        }
                    } else {
                        if (year <= 2015) {
                            if (year <= 2010)
                                deforestationRate *= coef_df_mod2010[plot.country];
                            else
                                deforestationRate *= coef_df_mod2015[plot.country];
                        } else if (year <= 2020)
                            deforestationRate *= coef_df_mod2020[plot.country];
                    }
                    double deforestationRateCorrection = 1;
                    // (year - 2020) / 30. = (year - 2020) / (2050 - 2020)
                    if (year > 2020 && year <= 2050 && coef_df_mod2020[plot.country] != 1)
                        deforestationRateCorrection = lerp(coef_df_mod2020[plot.country], 1, (year - 2020) / 30.);

                    deforestationRate *= deforestationRateCorrection * deforRate_opt[plot.country];
                    // 11.04.2023 smoothing deforestation for Romania to keep it within the historical range
                    if (plot.country == 183 && year > 2020 && year < 2040) {
                        // (year - 2020) / 35. = (year - 2020) / (2055 - 2020)
                        const double kdf = max(0., lerp(coef_df_mod2020[plot.country], 1, (year - 2020) / 35.));
                        deforestationRate *= kdf;
                    }
                    // MG: End of gdp definition
                    // pay if carbon gets to air (Harvest goes to products)
                    // MG: mistake: must be DECRATES but not DECRATEL
                    // Income from selling the deforested wood, taking into account the carbon tax
                    const double R = plot.R(year);
                    const double CO2Price = inputPriceC != 0 ? dms.CO2Price.at(plot.country)(year) : 0;

                    const double pDeforestationIncome
                            = plot.CAboveHa * (timberPrice * plot.fTimber * (1 - coef.harvLoos) -
                                               CO2Price * (1 + R) *
                                               (plot.fracLongProd * coef.decRateL / (coef.decRateL + R) +
                                                plot.fracLongProd * coef.decRateS / (coef.decRateS + R)));
                    const double shareLargeBranches = 0.3; // share of branches greater than 10 cm in diameter
                    const double pDeforestationLoss
                            = -CO2Price * (1 + R) *
                              (plot.CLitterHa * (shareLargeBranches * plot.decWood / (plot.decWood + R) +
                                                 (1 - shareLargeBranches) * plot.decHerb / (plot.decHerb + R)) +
                               plot.CBelowHa * shareLargeBranches * plot.decHerb / (plot.decHerb + R) +
                               plot.SOCHa * plot.decSOC / (plot.decSOC + R));
                    // immediate pay if deforested (Slash and Burn)
                    const double sDeforestationIncome =
                            plot.CAboveHa * (timberPrice * plot.fTimber * (1 - coef.harvLoos) - CO2Price);
                    const double sDeforestationLoss = -CO2Price * (plot.CDeadHa + plot.CBelowHa * 0.7);

                    double deforestationIncome =
                            lerp(pDeforestationIncome, sDeforestationIncome, plot.slashBurn) + pDeforestationLoss +
                            sDeforestationLoss;
                    // MG: adjust the multiplier to account for a forest saving policy in some countries
                    if (agriculturalValue + deforestationIncome > forestryValue * hurdle_opt[plot.country]) {
                        cell.U.forestShare.back() -= deforestationRate * modTimeStep; // decrease forest share
                        cell.U.forestShare.back() = max(0., cell.U.forestShare.back());
                        // TODO TO BE IMPROVED!
                    }
                    if (double forestShareAll = cell.forestShareAll(-1); forestShareAll > maxForestShare) {
                        const double dFor = forestShareAll - maxForestShare;
                        cell.U.forestShare.back() = max(0., cell.U.forestShare.back() - dFor);
                        // TODO TO BE IMPROVED!
                    }
                }
                // policy to deforest 43 kha in 2020-2023 for building railway according to MS comments to EUCLIMIT2 in February 2016
                if (plot.country == 130 && year >= 2020 && year <= 2023 &&
                    OForestShareTmp - cell.forestShareOld(-1) < 0.003255846)
                    cell.U.forestShare.back() = max(0., cell.U.forestShare.back() - 0.001706064);

                double afforestationRate = 0;
                if (OForestShareTmp + cell.N.forestShare.back() < maxForestShare &&
                    cell.N.forestShare.back() < afforMax &&
                    countriesNForestCover1990.getVal(plot.country, year) < afforMaxCountry &&
                    countriesNForestCover1990.getVal(plot.country, year) +
                    countriesOForestCover.getVal(plot.country, year) +
                    globiomReservedCountry < countryLandArea[plot.country] * 1'000) {
                    // MG: We afforest only places, where potential vegetation is forest and savanna
                    if (0 < static_cast<uint8_t>(plot.potVeg) && static_cast<uint8_t>(plot.potVeg) <= 9 &&
                        plot.NPP(year) > 0) {
                        const double R = plot.R(year);
                        const double CO2Price = inputPriceC != 0 ? dms.CO2Price.at(plot.country)(year) : 0;

                        afforestationRate = 0.01 / (1 + exp(1 + 0.1 / plot.agrSuit + 1 / (0.001 * gdp)));
                        if (afforestationRate < 1e-6 / plot.landArea)
                            afforestationRate = 0; // minimum one tree (1m^2)
                        else
                            afforestationRate *= afforRate_opt[plot.country];
                        forestryValuePlus = forestryValue + CO2Price *
                                                            (((plot.SOCHa * 0.4 / (8 * decision.rotInter())) +
                                                              5 / (1.053 * decision.rotInter())) / R);
                        // afforestation grants in UK till 2020, approx 180 GBP/ha year during 20 years,
                        // exchange rate GBP/USD for 2000-2016 approx 1.7
                        if (plot.country == 78 && year < 2021) {
                            const double R_year = plot.R(year);
                            // sum of geometric series, b_0 = 180 * 1.7 = 306, q = 1 / (1 + plot.R(year)), n = 20
                            const double capUK = 306 * (1 - pow(1 + R_year, -20)) * (1 + 1 / R_year);
                            forestryValuePlus += capUK;
                        }
                        if (year <= 2005) {
                            if (year <= 1995) {
                                if (year == 1990)
                                    afforestationRate *= coef_af_mod1990[plot.country];
                                else
                                    afforestationRate *= coef_af_mod1995[plot.country];
                            } else {
                                if (year <= 2000)
                                    afforestationRate *= coef_af_mod2000[plot.country];
                                else
                                    afforestationRate *= coef_af_mod2005[plot.country];
                            }
                        } else {
                            if (year <= 2015) {
                                if (year <= 2010)
                                    afforestationRate *= coef_af_mod2010[plot.country];
                                else
                                    afforestationRate *= coef_af_mod2015[plot.country];
                            } else if (year <= 2020)
                                afforestationRate *= coef_af_mod2020[plot.country];
                        }
                        switch (plot.country) {
                            case 20:
                                if (year <= 2040) {
                                    if (year > 2020)
                                        afforestationRate *= 1 - 0.6 * (year - 2020) / 20.;
                                } else if (year <= 2045)
                                    afforestationRate *= 1 - 0.6 + 0.6 * (year - 2040) / 5.;
                                break;
                            case 61:
                                if (year > 2020 && year <= 2070)
                                    afforestationRate *= 0.6;
                                break;
                            case 71:    // Finland
                                // limit total afforestation to 350 kha as discussed with A.Deppermann 01.03.2021
                                if (year > 2020 && countriesNForestCover.getVal(71, year - 1) >= 350'000)
                                    afforestationRate = 0;
                                else if (year >= 2047 && year <= 2070)  // limit afforestation 9 Jan 2021
                                    afforestationRate *= 1 - 0.7 * (year - 2047) / 23.;
                                // no break!
                            case 58:
                            case 204:
                                if (year <= 2030) {
                                    if (year > 2020)
                                        afforestationRate *= 1 - 0.5 * (year - 2020) / 10.;
                                } else if (year <= 2040)
                                    afforestationRate *= 1 - 0.5 + 0.5 * (year - 2030) / 10.;
                                break;
                            case 103:   // Ireland
                                // limit afforestation to 9 kha / year after 2030, request by national experts and Rene, December 2020
                                if (year <= 2040) {
                                    if (year > 2020)
                                        afforestationRate *= 1 - 0.4 * (year - 2020) / 20.;
                                } else if (year <= 2070)
                                    afforestationRate *= 1 - 0.4;
                                break;
                            case 161:   // Netherlands
                                if (year <= 2026) {
                                    if (year > 2020)
                                        afforestationRate *= 1 - 0.8 * (year - 2020) / 9.;
                                } else if (year <= 2035) {
                                    if (year == 2027)
                                        afforestationRate *= 0.25;
                                    else
                                        afforestationRate *= 0.2;
                                } else if (year <= 2040)
                                    afforestationRate *= 1 - 0.8 + 0.8 * (year - 2035) / 5.;
                                break;
                        }
                        if (agriculturalValue < forestryValuePlus * hurdle_opt[plot.country]) {
                            cell.N.forestShare.back() += afforestationRate * modTimeStep;
                            // MG: 20 April 2018: added to limit afforestation to a share of other natural land
                            cell.N.forestShare.back() = min(min(afforMax, maxForestShare - OForestShareTmp),
                                                            cell.N.forestShare.back());
                        }
                    } else {
                        // MG: added 20 April 2018
                        cell.N.forestShare.back() = min(afforMax, cell.N.forestShare.back());

                        if (double forestShareAll = cell.forestShareAll(-1); forestShareAll > maxForestShare) {
                            const double dFor = forestShareAll - maxForestShare;
                            cell.U.forestShare.back() = max(0., cell.U.forestShare.back() - dFor);
                            // TODO TO BE IMPROVED!
                        }
                    }
                }
            }
            cell.correctForestShareDynamics();

            const ptrdiff_t age = (year - coef.bYear + 1) / modTimeStep;
            // Limit afforestation speed to 2% of cell area (0.5deg) per year Hannes Bottcher personal communication, 2013
            cell.afforestationShareTimeA[age] = clamp(cell.N.forestShare.back() - cell.N.forestShare.end()[-2],
                                                      0., 0.02 * modTimeStep);
            cell.N.forestShare.back() = cell.N.forestShare.end()[-2] + cell.afforestationShareTimeA[age];

            countriesNForestCover1990.inc(plot.country, year, cell.N.forestShare.back() * cell.landAreaHa);
            countriesAfforestationHaYear1990.inc(plot.country, year,
                                                 cell.afforestationShareTimeA[age] * cell.landAreaHa / modTimeStep);

            // limit deforestation speed to 5% of cell area (0.5deg) per year (see Kindermann et al. 2007)
            cell.deforestationShareTimeA[age] = clamp(cell.forestShareOld(-2) - cell.forestShareOld(-1), 0.,
                                                      0.05 * modTimeStep);
            cell.U.forestShare.back() = cell.U.forestShare.end()[-2] - cell.deforestationShareTimeA[age];

            // MG: We start deforestation after 2000, because we have initial data for 2000 (discussion with Hannes Bottcher 10.09.09)
            if (year <= 2000)
                cell.restoreForestShares();
            // 31.03.2023 all afforested before 2000 is already in the initial LC therefore we afforest after 2000
            appCohortsN[plot.asID].afforest(cell.afforestationShareTimeA[age]);

            FMResult resultDeforestation;
            if (cell.deforestationShareTimeA[age] > 0 && (resU.realArea > 0 || resULost.realArea > 0)) {
                // MG: Correcting the bug causing relAreaO approaching zero before real deforestation starts
                // MG: We start deforestation after 2000, because we have initial data for 2000 (discussion with Hannes Bottcher 10.09.09)
                if (year <= 2000)
                    resultDeforestation = AgeStruct{appCohortsU[plot.asID]}.deforest(
                            cell.deforestationShareTimeA[age]);
                else
                    resultDeforestation = appCohortsU[plot.asID].deforest(cell.deforestationShareTimeA[age]);
            }
            cell.deforestHaYear = cell.deforestationShareTimeA[age] * cell.landAreaHa;
            cell.deforestHaTotal += cell.deforestHaYear;
            cell.afforestHaYear = cell.afforestationShareTimeA[age] * cell.landAreaHa;
            // 10.03.2023 adjusting to Ireland Fl-Fl reporting
            if (!(plot.country == 103 && year < 2001))
                cell.afforestHaTotal += cell.afforestHaYear;

            cell.deforestationWoodyProductsLongLivedA[age] =
                    resultDeforestation.biomass * plot.fracLongProd * (1 - coef.harvLoos) * (1 - plot.slashBurn) *
                    cell.deforestHaYear;
            cell.deforestationWoodyProductsShortLivedA[age] = cell.deforestationWoodyProductsLongLivedA[age];
            // MG:BEF: we assume that the non-stem above-ground biomass goes to the litter pool
            cell.deforestationLitterTimeA[age] =
                    (plot.CLitterHa + (plot.BEF(resultDeforestation.biomass) - 1) * resultDeforestation.biomass) *
                    cell.deforestHaYear;
            cell.fineRootA[age] = plot.CBelowHa * 0.3 * cell.deforestHaYear;
            // According to Karjalainen & Liski 1997, p.?? about 38% of SOC is in the "fast" SOC1 pool
            cell.deforestationSoilTimeA1[age] = 0.38 * plot.SOCHa * cell.deforestHaYear;
            cell.deforestationSoilTimeA[age] = 0.62 * plot.SOCHa * cell.deforestHaYear;
            cell.afforestationSoilTimeA[age] = plot.SOCHa * cell.afforestHaYear;
            cell.afforestationLitterTimeA[age] = plot.CLitterHa * cell.afforestHaYear;

            // Emissions from deforestation
            const double emissionsProductCur =
                    (ranges::fold_left(cell.deforestationWoodyProductsLongLivedA, 0., plus{}) * coef.decRateL +
                     ranges::fold_left(cell.deforestationWoodyProductsShortLivedA, 0., plus{}) * coef.decRateS) *
                    modTimeStep;

            // a share of decomposed litter and SOM transferred to the pool below according to SOM2 model in Karjalainen & Liski 1997, p.56
            const double p = 0.115;   // p in equation
            // litter pools of foliage and branches (fine roots and coarse roots as well) are approximately of the same size according to Karjalainen and Liski 1997, p.69
            const double emissionsLitterCur =
                    (ranges::fold_left(cell.deforestationLitterTimeA, 0., plus{}) *
                     midpoint(plot.decWood, plot.decHerb) +
                     ranges::fold_left(cell.fineRootA, 0., plus{}) * plot.decHerb) * (1 - p) * modTimeStep;

//            for (const auto value: cell.deforestationLitterTimeA)
//                println("{}", value);
//            FATAL("Negative forest share in year = {}, asID = {}, cell: {}", year, plot.asID, cell.str());

            // 05.06.2023: Testing: deadwood is a mixture of branches and stems (to be estimated better!)
            const double decStem = plot.decWood * 0.5;
            // we make it to correspond to SOC1 decomposition in Karjalainen & Liski 1997, p.56
            const double decSOC1 = 0.9 * decStem;
            // to make it to correspond to SOC2  decomposition in Karjalainen & Liski 1997, p.56
            const double decSOC = plot.decSOC / 3;
            // deforestation emissions from soil, tCO2/year
            double emissionsSOCCur = 0;
            for (const auto [SOCA, SOCA1, deforestA]:
                    rv::zip(cell.deforestationSoilTimeA, cell.deforestationSoilTimeA1,
                            cell.deforestationShareTimeA))
                if (SOCA + SOCA1 > 0.6 * plot.SOCHa * deforestA * cell.landAreaHa)
                    emissionsSOCCur += SOCA * decSOC + SOCA1 * decSOC1 * (1 - p);
            emissionsSOCCur *= modTimeStep;
            // alternative estimate of soil carbon emissions at deforestation [tC/ha year]
            // alternative estimation of SOC change (tC/ha year) due to conversion from forest to farmland
            // [Deng et al. (2016) Global patterns of the effects of land-use changes on soil carbon stocks.
            // Global Ecology and Conservation N5, pp. 127-138]
            double emissionsSOCCur1 = 0;
            // alternative estimate of soil carbon emissions at deforestation [tC/ha year] based on average UNFCCC emissions for countries
            double emissionsSOCCur2 = 0;
            if (plot.SOCHa > 0) {
                for (const auto [i, deforestA]: cell.deforestationShareTimeA | rv::enumerate) {
                    double denominator = static_cast<double>(i) * modTimeStep;
                    double tmp = plot.SOCHa - (-0.53 * plot.SOCHa - 0.25 * denominator + 19.22);
                    if (deforestA > 0 && tmp <= 21.05)
                        emissionsSOCCur1 += deforestA * tmp / (i > 0 ? denominator : 1);
                }
                for (const auto [i, deforestA]: cell.deforestationShareTimeA | rv::enumerate |
                                                rv::drop(max(ptrdiff_t{0}, age - 20)))
                    if (deforestA > 0)
                        emissionsSOCCur2 += defor_sl_em_unfccc_tCha[plot.country] * 10 * 0.2 *
                                            exp(-0.22 * static_cast<double>(age - i) * modTimeStep) * deforestA;

                emissionsSOCCur1 *= cell.landAreaHa;
                emissionsSOCCur2 *= cell.landAreaHa;
            }
            // MG: Recalculate carbon pools
            const double decRateL_dec = max(0., 1 - coef.decRateL * modTimeStep);
            const double decRateS_dec = max(0., 1 - coef.decRateS * modTimeStep);
            // corresponds to foliage in Karjalainen & Liski 1997, p.56
            const double decHerb_dec = max(0., 1 - plot.decHerb * modTimeStep);
            // corresponds to branches of birch in Karjalainen & Liski 1997, p.56
            const double decWood_dec = max(0., 1 - plot.decWood * modTimeStep);
            const double decHerb_decWood_dec_avg = midpoint(decHerb_dec, decWood_dec);
            const double decSOC1_dec = max(0., 1 - decSOC1 * modTimeStep);
            // corresponds to SOC2 in Karjalainen & Liski 1997, p.56
            const double decSOC_dec = max(0., 1 - decSOC * modTimeStep);

            for (auto &value: cell.deforestationWoodyProductsLongLivedA)
                value *= decRateL_dec;
            for (auto &value: cell.deforestationWoodyProductsShortLivedA)
                value *= decRateS_dec;
            for (auto &value: cell.deforestationLitterTimeA)
                value *= decHerb_decWood_dec_avg;
            for (auto &value: cell.fineRootA)
                value *= decHerb_dec;

            // litter pools of foliage and branches (fine roots and coarse roots as well) are approximately of the same size according to Karjalainen and Liski 1997, p.69
            vector<double> emissionsLitterCurTmp;
            emissionsLitterCurTmp.reserve(age + 1);
            for (double decWood_decHerb_avg = midpoint(plot.decWood, plot.decHerb); const auto [litterA, fineRootA]:
                    rv::zip(cell.deforestationLitterTimeA, cell.fineRootA))
                emissionsLitterCurTmp.push_back(
                        (litterA * decWood_decHerb_avg + fineRootA * plot.decHerb) * (1 - p) * modTimeStep);

            for (auto &&[SOCA, SOCA1, deforestA, emissionsLitterCurA]:
                    rv::zip(cell.deforestationSoilTimeA, cell.deforestationSoilTimeA1,
                            cell.deforestationShareTimeA, emissionsLitterCurTmp))
                if (SOCA + SOCA1 > 0.6 * plot.SOCHa * deforestA * cell.landAreaHa) {
                    SOCA1 = SOCA1 * decSOC1_dec + p * emissionsLitterCurA;
                    SOCA = SOCA * decSOC_dec + p * SOCA1 * decSOC1 * modTimeStep;
                }
            // Emissions from deforestation in current cell for current year caused by burning
            const double emissionsSlashBurnCur = cell.U.stemBiomass.back() * plot.slashBurn * cell.deforestHaYear;
            const double emissionsDeadBurnCur = plot.CDeadHa * cell.deforestHaYear;
            const double emissionsCRootBurnCur = 0.7 * plot.CBelowHa * cell.deforestHaYear;
            // total emissions in current cell for current year
            const double emissionsCur =
                    emissionsProductCur + emissionsLitterCur + emissionsSOCCur +
                    emissionsSlashBurnCur + emissionsDeadBurnCur + emissionsCRootBurnCur;
            // *************** END Emissions from deforestation ****************
            // *************** Afforestation "negative" emissions block ********
            // alternative estimate of soil carbon emissions at afforestation [tC/ha year]
            // Initialisation of alternative SOC emissions (sink) at afforestation
            double emissionsSOCAfforCur1 = cell.afforestationShareTimeA[age] > 0 ?
                                           (-5.76 - 0.52 * modTimeStep) * cell.afforestationShareTimeA[age] : 0;

            double emissionsBmAfforCur_gl = 0;      // afforestation biomass emissions in forest stands Gain-Loss method
            double emissionsBmAfforCur_age20o = 0;  // afforestation biomass emissions in forest stands of age over 20 years old Gain-Loss method
            double emissionsBmAfforCur_age20b = 0;  // afforestation biomass emissions in forest stands of age <= 20 years old Gain-Loss method
            double plantArea20_rel = 0;
            double emissionsSOCAfforCur = 0;
            double emissionsSOCAfforCur20o = 0;
            double emissionsSOCAfforCur20b = 0;
            double emissionsSOCAfforCur20 = 0;
            double emissionsLitterAfforCur = 0;
            double emissionsLitterAfforCur20 = 0;
            double emissionsLitterAfforCur20b = 0;

            if (cell.N.forestShare.back() > 0) {
                double reciprocalN_FS = 1 / cell.N.forestShare.back();

                for (const auto [i, afforestA]: cell.afforestationShareTimeA | rv::enumerate) {
                    const double cohortArea = appCohortsN[plot.asID].getArea(
                            static_cast<size_t>(age - i));   // made by idx

                    if (afforestA > 0 || cohortArea > 0) {
                        // correct cohorts area to the final cut harvest
                        // we ignore the replanting thus underestimating the sink in the forest stands moved to the category FL-FL
                        // afforested (stem) biomass of age (Age-ia) * modTimeStep per ha
                        double abovePhCur = appCohortsN[plot.asID].getBm(static_cast<double>(age - i) * modTimeStep);

                        if ((age - i) * modTimeStep <= 20) {    // age cohorts younger than 21 year
                            // a share of the age cohort has been replanted after final cut
                            if (afforestA < cohortArea) {
                                // afforested biomass of age (Age-ia) * modTimeStep per ha * afforShare[ia]
                                cell.N_abovegroundBiomassBelow20yoAge.back()[age - i] =
                                        abovePhCur * plot.BEF(abovePhCur) * afforestA * reciprocalN_FS;
                                // afforested biomass of age (Age-ia) * modTimeStep per ha * afforShare[ia]
                                cell.N_abovegroundBiomassOver20yoAge.back()[age - i] =
                                        abovePhCur * plot.BEF(abovePhCur) * (cohortArea - afforestA) * reciprocalN_FS;
                            } else {  //  a share of the age cohort has been under final cut or no final cut in current age cohort
                                // afforested biomass of age (Age-ia) * modTimeStep per ha * afforShare[ia]
                                cell.N_abovegroundBiomassBelow20yoAge.back()[age - i] =
                                        abovePhCur * plot.BEF(abovePhCur) * cohortArea * reciprocalN_FS;
                            }
                        } else {    // age cohorts older than 20 years
                            // afforested biomass of age (Age-ia) * modTimeStep per ha * afforShare[ia]
                            cell.N_abovegroundBiomassOver20yoAge.back()[age - i] =
                                    abovePhCur * plot.BEF(abovePhCur) * cohortArea * reciprocalN_FS;
                        }
                        // increment of biomass in the forest stand of age (Age-ia) * modTimeStep per ha * afforShare[ia] per modTimeStep period
                        double emissionsBmAfforCur_age = 0;
                        if (i < age) {
                            double abovePhPrev = appCohortsN[plot.asID].getBm(
                                    static_cast<double>(age - i - 1) * modTimeStep);
                            emissionsBmAfforCur_age =
                                    (abovePhCur * plot.BEF(abovePhCur) - abovePhPrev * plot.BEF(abovePhPrev)) *
                                    cohortArea;
                        }
                        abovePhCur *= cohortArea;   // afforested (stem) biomass per ha * afforShare[ia]
                        cell.N.stemBiomass.back() += abovePhCur; // stem biomass of planted forest, tC in the cell

                        emissionsBmAfforCur_gl += emissionsBmAfforCur_age;  // biomass increment of all stands Gain-Loss method
                        if ((age - i) * modTimeStep > 20) {    // we track area and phytomass of new forest over 20 y.o.
                            if (year > 2000)    // 13.04.2023 adjusting to Ireland Fl-Fl reporting
                                plantArea20_rel += afforestA;   // relative area of forest over 20 y.o.
                            // biomass increment of all stands of age over 20 years old per ha * afforShare[ia] per modTimeStep period Gain-Loss method
                            emissionsBmAfforCur_age20o += emissionsBmAfforCur_age;
                        } else  // we track area and phytomass of new forest below 20 y.o.
                            // biomass increment of all  stands of age <= 20 years old per ha * afforShare[ia] per modTimeStep period Gain-Loss method
                            emissionsBmAfforCur_age20b += emissionsBmAfforCur_age;
                        if (afforestA > 0) {
                            if (cell.afforestationLitterTimeA[i] < 5 * afforestA * cell.landAreaHa) {
                                double curEmissionsLitterAfforCur = cell.afforestationLitterInput(i);
                                emissionsLitterAfforCur += curEmissionsLitterAfforCur;
                                cell.afforestationLitterTimeA[i] += curEmissionsLitterAfforCur;

                                if ((age - i) * modTimeStep >
                                    20)  // we track litter emissions of new forest over 20 y.o.
                                    emissionsLitterAfforCur20 += curEmissionsLitterAfforCur;
                                else
                                    emissionsLitterAfforCur20b += curEmissionsLitterAfforCur;
                            }
                            if (cell.afforestationSoilTimeA[i] <=
                                1.4 * plot.SOCHa * afforestA * cell.landAreaHa) {
                                double asi = cell.afforestationSoilInput(i) * plot.afforestationSoilInputCoef();
                                emissionsSOCAfforCur += asi;
                                cell.afforestationSoilTimeA[i] += asi;
                                // we track area and phytomass or soil emissions of new forest over 20 y.o.
                                if ((age - i) * modTimeStep > 20)
                                    emissionsSOCAfforCur20o += asi;
                                else
                                    emissionsSOCAfforCur20b += asi;
                                // alternative estimation of SOC change (tC/ha year) due to conversion from farmland to forest
                                // [Deng et al. (2016) Global patterns of the effects of land-use changes on soil carbon stocks.
                                // Global Ecology and Conservation N5, pp. 127-138]
                                emissionsSOCAfforCur1 += 0.52 * modTimeStep * afforestA;
                            }
                        }
                    }
                }
                emissionsSOCAfforCur1 *= cell.landAreaHa;   // tC/year per cell
                cell.N.stemBiomass.back() *= reciprocalN_FS;
                // below-ground phytomass
                cell.N_belowgroundBiomass.back() = cell.N_abovegroundBiomass(-1) * plot.coefBL();
            }
            // above-ground phytomass increment in the cell, tC / (ha * modTimeStep)
            double curPlantPhytBmGr = (cell.N_abovegroundBiomass(-1) - cell.N_abovegroundBiomass(-2));
            // stem phytomass increment in the cell, tC / (ha * modTimeStep)
            double curPlantPhytStem = (cell.N.stemBiomass.back() - cell.N.stemBiomass.end()[-2]);
            // below-ground phytomass increment in the cell, tC / (ha * modTimeStep)
            double curPlantPhytBlGr = (cell.N_belowgroundBiomass.back() - cell.N_belowgroundBiomass.end()[-2]);

            // total (negative) emissions from afforestation for current year and current cell
            double emissionsAfforCur =
                    (curPlantPhytBmGr + curPlantPhytBlGr) * cell.N.forestShare.back() * cell.landAreaHa +
                    emissionsLitterAfforCur + emissionsSOCAfforCur;
            // END Afforestation "negative" emissions block
            // wood obtained at deforestation, m^3 / ha
            double harvestDfM3Ha = 0;
            // harvested new and old wood (from FM) in the cell
            double harvestFmTotalM3 = 0;
            // total wood (from FM and deforestation) in the cell
            double harvestTotalAllM3 = 0;

            if (!plot.protect) {
                harvestDfM3Ha = resultDeforestation.getWood() * (1 - plot.slashBurn) * plot.fTimber;
                cell.deforestationWoodTotalM3 = harvestDfM3Ha * cell.deforestHaYear;

                cell.U.harvestFc = harvestedWoodUse[plot.country] * resU.getFinalCutWood() * plot.fTimber *
                                   cell.U.forestShare.back() * cell.landAreaHa;
                cell.U.harvestTh = harvestedWoodUse[plot.country] * resU.getThinnedWood() * plot.fTimber *
                                   cell.U.forestShare.back() * cell.landAreaHa;
                cell.U.harvestSc = harvestWoodLU * cell.U.forestShare.back() * cell.landAreaHa;

                cell.N.harvestFc = harvestedWoodUse[plot.country] * resN.getFinalCutWood() * plot.fTimber *
                                   cell.N.forestShare.end()[-2] * cell.landAreaHa;
                cell.N.harvestTh = harvestedWoodUse[plot.country] * resN.getThinnedWood() * plot.fTimber *
                                   cell.N.forestShare.end()[-2] * cell.landAreaHa;
                cell.N.harvestSc = harvestWoodLN * cell.N.forestShare.end()[-2] * cell.landAreaHa;

                cell.O10.harvestFc = harvestedWoodUse[plot.country] * res10.getFinalCutWood() * plot.fTimber *
                                     cell.O10.forestShare.back() * cell.landAreaHa;
                cell.O10.harvestTh = harvestedWoodUse[plot.country] * res10.getThinnedWood() * plot.fTimber *
                                     cell.O10.forestShare.back() * cell.landAreaHa;
                cell.O10.harvestSc = harvestWoodL10 * cell.O10.forestShare.back() * cell.landAreaHa;

                cell.O30.harvestFc = harvestedWoodUse[plot.country] * res30.getFinalCutWood() * plot.fTimber *
                                     cell.O30.forestShare.back() * cell.landAreaHa;
                cell.O30.harvestTh = harvestedWoodUse[plot.country] * res30.getThinnedWood() * plot.fTimber *
                                     cell.O30.forestShare.back() * cell.landAreaHa;
                cell.O30.harvestSc = harvestWoodL30 * cell.O30.forestShare.back() * cell.landAreaHa;

                harvestFmTotalM3 = cell.U.totalHarvest() + cell.N.totalHarvest() + cell.O10.totalHarvest() +
                                   cell.O30.totalHarvest();
                harvestTotalAllM3 = harvestFmTotalM3 + cell.deforestationWoodTotalM3 + cell.salvageLogging;

                cell.U.fellings = (resU.getTotalHarvestedBiomass() +
                                   cleanedWoodUseCurrent * resULost.getTotalHarvestedBiomass()) * plot.fTimber *
                                  cell.U.forestShare.back() * cell.landAreaHa;
                cell.N.fellings = (resN.getTotalHarvestedBiomass() +
                                   cleanedWoodUseCurrent * resNLost.getTotalHarvestedBiomass()) * plot.fTimber *
                                  cell.N.forestShare.end()[-2] * cell.landAreaHa;
                cell.O10.fellings = (res10.getTotalHarvestedBiomass() +
                                     cleanedWoodUseCurrent10 * res30Lost.getTotalHarvestedBiomass()) *
                                    plot.fTimber * cell.O10.forestShare.back() * cell.landAreaHa;
                cell.O30.fellings = (res30.getTotalHarvestedBiomass() +
                                     cleanedWoodUseCurrent30 * res30Lost.getTotalHarvestedBiomass()) *
                                    plot.fTimber * cell.O30.forestShare.back() * cell.landAreaHa;
            }
            appHarvestGrid(plot.x, plot.y) = harvestTotalAllM3;

            cell.U.CAI = (cell.U.stemBiomass.back() - cell.U.stemBiomass.end()[-2]) / modTimeStep * plot.fTimber;
            cell.O10.CAI = (cell.O10.stemBiomass.back() - cell.O10.stemBiomass.end()[-2]) / modTimeStep * plot.fTimber;
            cell.O30.CAI = (cell.O30.stemBiomass.back() - cell.O30.stemBiomass.end()[-2]) / modTimeStep * plot.fTimber;
            cell.P.CAI = (cell.P.stemBiomass.back() - cell.P.stemBiomass.end()[-2]) / modTimeStep * plot.fTimber;

            if (!plot.protect) {
                cell.U.CAI += (resU.getTotalHarvestedBiomass() +
                               cleanedWoodUseCurrent * resULost.getTotalHarvestedBiomass()) * plot.fTimber;
                cell.O10.CAI += (res10.getTotalHarvestedBiomass() +
                                 cleanedWoodUseCurrent10 * res10Lost.getTotalHarvestedBiomass()) * plot.fTimber;
                cell.O30.CAI += (res30.getTotalHarvestedBiomass() +
                                 cleanedWoodUseCurrent30 * res30Lost.getTotalHarvestedBiomass()) * plot.fTimber;
            }
            // TODO / modTimeStep?
            cell.N.CAI = (curPlantPhytStem + resN.getTotalHarvestedBiomass() +
                          cleanedWoodUseCurrent * resNLost.getTotalHarvestedBiomass()) * plot.fTimber;
            // 22.12.2015 Net Annual Increment
            double NAI_old_m3ha = ((cell.U.stemBiomass.back() - cell.U.stemBiomass.end()[-2]) / modTimeStep +
                                   resU.getTotalHarvestedBiomass() +
                                   cleanedWoodUseCurrent * resULost.getTotalHarvestedBiomass()) * plot.fTimber;
            // Gross Current Annual Increment, m3/ha
            double grossCAI_m3ha = max(resU.thinning.grossInc, resULost.thinning.grossInc) * plot.fTimber;
            // Net Current Annual Increment, m3/ha
            double netCAI_m3ha = max(resU.thinning.netInc, resULost.thinning.netInc) * plot.fTimber;
            // Gross Current Annual Increment, m3/ha
            double grossCAI_new_m3ha = max(resN.thinning.grossInc, resNLost.thinning.grossInc) * plot.fTimber;
            // Net Current Annual Increment, m3/ha
            double netCAI_new_m3ha = max(resN.thinning.netInc, resNLost.thinning.netInc) * plot.fTimber;
            // TODO could be problems with / modTimeStep
            const double unitConvCoef = molarRatio * 1e-6 / modTimeStep;

            cell.U.biomassChangeAb = (cell.U.stemBiomass.back() * plot.BEF(cell.U.stemBiomass.back()) -
                                      cell.U.stemBiomass.end()[-2] * plot.BEF(cell.U.stemBiomass.end()[-2])) *
                                     unitConvCoef;
            cell.U.biomassChangeTotal = cell.U.biomassChangeAb * (1 + plot.coefBL());

            cell.O10.biomassChangeAb = (cell.O10.stemBiomass.back() * plot.BEF(cell.O10.stemBiomass.back()) -
                                        cell.O10.stemBiomass.end()[-2] *
                                        plot.BEF(cell.O10.stemBiomass.end()[-2])) *
                                       unitConvCoef;
            cell.O10.biomassChangeTotal = cell.O10.biomassChangeAb * (1 + plot.coefBL());

            cell.O30.biomassChangeAb = (cell.O30.stemBiomass.back() * plot.BEF(cell.O30.stemBiomass.back()) -
                                        cell.O30.stemBiomass.end()[-2] *
                                        plot.BEF(cell.O30.stemBiomass.end()[-2])) *
                                       unitConvCoef;
            cell.O30.biomassChangeTotal = cell.O30.biomassChangeAb * (1 + plot.coefBL());

            cell.P.biomassChangeAb = (cell.P.stemBiomass.back() * plot.BEF(cell.P.stemBiomass.back()) -
                                      cell.P.stemBiomass.end()[-2] * plot.BEF(cell.P.stemBiomass.end()[-2])) *
                                     unitConvCoef;
            cell.P.biomassChangeTotal = cell.P.biomassChangeAb * (1 + plot.coefBL());

            if (appThinningForest(plot.x, plot.y) < 0)
                cell.rotationBiomass =
                        species.at(plot.speciesType).getU(cell.U.stemBiomass.back(), appMaiForest(plot.x, plot.y));
            else
                cell.rotationBiomass =
                        species.at(plot.speciesType).getUSdTab(cell.U.stemBiomass.back(), appMaiForest(plot.x, plot.y),
                                                               appThinningForest(plot.x, plot.y));

            double rotMaxBm = species.at(plot.speciesType).getTOpt(appMaiForest(plot.x, plot.y),
                                                                   OptRotTimes::Mode::MaxBm);
            cell.emissionsDeforestationBiomass =
                    (resultDeforestation.biomass * plot.BEF(resultDeforestation.biomass) + plot.CBelowHa) *
                    unitConvCoef;

            if (cell.U.forestShare.back() > 0)
                cell.emissionsDeforestationSoil =
                        emissionsSOCCur * unitConvCoef / (cell.U.forestShare.back() * cell.landAreaHa);

            if (cell.N.forestShare.back() > 0)
                cell.emissionsAfforestationSoil =
                        emissionsSOCAfforCur * unitConvCoef / (cell.N.forestShare.back() * cell.landAreaHa);

            rf.detailsBuffer +=
                    format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},"
                           "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}"
                           "\n",
                           plot.asID, plot.simuID, idCountryGLOBIOM.at(plot.country), year, cell.forestShareOld(-1),
                           cell.N.forestShare.back(), plot.CAboveHa,
                           cell.U.stemBiomass.back() * plot.BEF(cell.U.stemBiomass.back()),
                           cell.N_abovegroundBiomass(-1), appMaiForest(plot.x, plot.y) * plot.fTimber, rotMAI,
                           resU.getThinnedWood() * plot.fTimber, resU.getFinalCutWood() * plot.fTimber,
                           resN.getThinnedWood() * plot.fTimber, resN.getFinalCutWood() * plot.fTimber,
                           harvestTotalAllM3, harvMAI, netCAI_m3ha, grossCAI_m3ha, cell.rotationBiomass,
                           forestryValuePlus * hurdle_opt[plot.country], forestryValue, agriculturalValue,
                           cell.rotation, rotationForestNew(plot.x, plot.y), timberPrice,
                           appThinningForest(plot.x, plot.y), appCohortsN[plot.asID].getStockingDegree(),
                           species.at(plot.speciesType).getTOptT(appMaiForest(plot.x, plot.y), OptRotTimes::Mode::MAI),
                           resU.realArea, cell.deforestationShareTimeA.back(), resultDeforestation.area,
                           plot.BEF(cell.U.stemBiomass.back()), plot.BEF(cell.U.stemBiomass.end()[-2]), NAI_old_m3ha,
                           NAI_old_m3ha * cell.U.forestShare.back() * cell.landAreaHa * 0.001,
                           cell.U.fellings * 0.001,
                           (resU.getTotalHarvestedBiomass() +
                            cleanedWoodUseCurrent * resULost.getTotalHarvestedBiomass()) * plot.fTimber / NAI_old_m3ha,
                           (NAI_old_m3ha - (resU.getTotalHarvestedBiomass() + resULost.getTotalHarvestedBiomass()) *
                                           plot.fTimber) * cell.U.forestShare.back() * cell.landAreaHa,
                           (cell.U.stemBiomass.back() - cell.U.stemBiomass.end()[-2]) * plot.fTimber *
                           cell.U.forestShare.back() * cell.landAreaHa,
                           (cell.emissionsDeforestationBiomass * cell.deforestHaYear +
                            cell.U.biomassChangeTotal * cell.U.forestShare.back() * cell.landAreaHa -
                            (cell.U.stemBiomass.back() * plot.BEF(cell.U.stemBiomass.back()) *
                             cell.U.forestShare.back() -
                             cell.U.stemBiomass.end()[-2] * plot.BEF(cell.U.stemBiomass.end()[-2]) *
                             cell.U.forestShare.end()[-2]) * cell.landAreaHa * unitConvCoef) * (1 + plot.coefBL()),
                           resULost.getTotalHarvestedBiomass() * plot.fTimber * cell.U.forestShare.back() *
                           cell.landAreaHa, cell.U.stemBiomass.back() * cell.U.forestShare.back() * cell.landAreaHa,
                           cell.U.stemBiomass.end()[-2] * cell.U.forestShare.end()[-2] * cell.landAreaHa,
                           cell.U.stemBiomass.back(), cell.U.stemBiomass.end()[-2], cell.U.forestShare.back(),
                           cell.U.forestShare.end()[-2], cell.landAreaHa,
                           cell.U.biomassChangeAb * cell.U.forestShare.back() * cell.landAreaHa,
                           cell.U.biomassChangeTotal * cell.U.forestShare.back() * cell.landAreaHa,
                           rotMaxBm, appCohortsU[plot.asID].getActiveAge(), appCohortsN[plot.asID].getActiveAge(),
                           cleanedWoodUseCurrent * resULost.getFinalCutWood() * plot.fTimber *
                           cell.U.forestShare.back() * cell.landAreaHa * 0.001,
                           cleanedWoodUseCurrent * resULost.getThinnedWood() * plot.fTimber *
                           cell.U.forestShare.back() * cell.landAreaHa * 0.001,
                           (cell.U.biomassChangeTotal -
                            (NAI_old_m3ha - (harvestWoodPlusU +
                                             (resU.getHarvestLosses() + resULost.getHarvestLosses()) * plot.fTimber)) *
                            plot.BEF(cell.U.stemBiomass.back()) *
                            unitConvCoef * (1 + plot.coefBL()) / plot.fTimber) *
                           cell.U.forestShare.back() * cell.landAreaHa,
                           plot.GLOBIOM_reserved(year), plot.afforMax(year),
                           cell.forestShareAll(-1) + plot.GLOBIOM_reserved(year)
                    );
            // output results for countries
            countriesNForestCover.inc(plot.country, year, cell.N.forestShare.back() * cell.landAreaHa);
            countriesAfforestationHaYear.inc(plot.country, year, cell.afforestHaYear / modTimeStep);
            countriesAfforestationAccumulationHa.inc(plot.country, year, cell.afforestHaTotal);

            cell.N.totalBiomass = cell.N_abovegroundBiomass(-1) + cell.N_belowgroundBiomass.back();
            countriesNForestTotalC.inc(plot.country, year,
                                       cell.N.totalBiomass * cell.N.forestShare.back() * cell.landAreaHa);
            // accumulated living stem biomass of planted forest, tC
            countriesNForestStemC.inc(plot.country, year,
                                      cell.N.stemBiomass.back() * cell.N.forestShare.back() * cell.landAreaHa);
            // accumulated living above-ground biomass of planted forest, tC
            countriesNForestAbC.inc(plot.country, year,
                                    cell.N_abovegroundBiomass(-1) * cell.N.forestShare.back() * cell.landAreaHa);
            // TODO / modTimeStep?
            cell.N.biomassChangeAb = curPlantPhytBmGr * unitConvCoef;
            cell.N.biomassChangeTotal = (curPlantPhytBmGr + curPlantPhytBlGr) * unitConvCoef;
            // ????? per year or modTimeStep???
            countriesAfforestationCYear.inc(plot.country, year, emissionsAfforCur * unitConvCoef);

            countriesAfforestationCYear_ab.inc(plot.country, year,
                                               cell.N.biomassChangeAb * cell.N.forestShare.back() *
                                               cell.landAreaHa);
            // ????? per year or modTimeStep???
            countriesAfforestationCYear_bl.inc(plot.country, year,
                                               (cell.N.biomassChangeTotal - cell.N.biomassChangeAb) *
                                               cell.N.forestShare.back() * cell.landAreaHa);
            // ????? per year or modTimeStep???
            countriesAfforestationCYear_biomass.inc(plot.country, year, cell.N.biomassChangeTotal *
                                                                        cell.N.forestShare.back() * cell.landAreaHa);

            countriesAfforestationCYear_dom.inc(plot.country, year, emissionsLitterAfforCur * unitConvCoef);
            countriesAfforestationCYear_soil.inc(plot.country, year, emissionsSOCAfforCur * unitConvCoef);
            countriesAfforestationCYear_soil_altern.inc(plot.country, year, emissionsSOCAfforCur1 * unitConvCoef);
            // ????? per year or modTimeStep???
            countriesAfforestationCover20.inc(plot.country, year, plantArea20_rel * cell.landAreaHa);
            countriesAfforestationTotalC20.inc(plot.country, year,
                                               cell.N_abovegroundBiomassBelow20yo(-1) * cell.N.forestShare.back() *
                                               cell.landAreaHa);
            double emAffBm20bs = 0;
            for (size_t i = 0; i < cell.N_abovegroundBiomassBelow20yoAge.end()[-2].size(); ++i)
                emAffBm20bs += cell.N_abovegroundBiomassBelow20yoAge.back()[i + 1] -
                               cell.N_abovegroundBiomassBelow20yoAge.end()[-2][i];
            emAffBm20bs *= (1 + plot.coefBL()) * cell.N.forestShare.back() * cell.landAreaHa * unitConvCoef;

            countriesAfforestationCYear_biomass20.inc(plot.country, year, emAffBm20bs);
            double emAffBm20o2 =
                    cell.N.biomassChangeTotal * cell.N.forestShare.back() * cell.landAreaHa - emAffBm20bs;
            countriesAfforestationCYear_biomass20os.inc(plot.country, year, emAffBm20o2);
            countriesAfforestationCYear_biomass20o.inc(plot.country, year, emissionsBmAfforCur_age20o *
                                                                           (1 + plot.coefBL()) * unitConvCoef *
                                                                           cell.landAreaHa);
            countriesAfforestationCYear_biomass20b.inc(plot.country, year, emissionsBmAfforCur_age20b *
                                                                           (1 + plot.coefBL()) * unitConvCoef *
                                                                           cell.landAreaHa);
            countriesAfforestationCYear_biomassGL.inc(plot.country, year, emissionsBmAfforCur_gl * (1 + plot.coefBL())
                                                                          * unitConvCoef * cell.landAreaHa);
            countriesAfforestationCYear_dom20b.inc(plot.country, year, emissionsLitterAfforCur20b * unitConvCoef);
            countriesAfforestationCYear_soil20b.inc(plot.country, year, emissionsSOCAfforCur20b * unitConvCoef);

            countriesOForestCover.inc(plot.country, year, cell.forestShareOld(-1) * cell.landAreaHa);
            countriesOForestCoverU.inc(plot.country, year, cell.U.forestShare.back() * cell.landAreaHa);
            countriesOForestCover10.inc(plot.country, year, cell.O10.forestShare.back() * cell.landAreaHa);
            countriesOForestCover30.inc(plot.country, year, cell.O30.forestShare.back() * cell.landAreaHa);
            countriesOForestCoverP.inc(plot.country, year, cell.P.forestShare.back() * cell.landAreaHa);

            countriesOForestCoverFAWSOld.inc(plot.country, year, !plot.protect ?
                                                                 (cell.U.forestShare.back() +
                                                                  cell.O10.forestShare.back() +
                                                                  cell.O30.forestShare.back()) * cell.landAreaHa : 0);

            countriesDeforestationHaYear.inc(plot.country, year, cell.deforestHaYear / modTimeStep);  // ha/year
            countriesOForest_stem_C.inc(plot.country, year,
                                        cell.U.stemBiomass.back() * cell.U.forestShare.back() * cell.landAreaHa);
            countriesOForest_ab_C.inc(plot.country, year,
                                      cell.U.stemBiomass.back() * plot.BEF(cell.U.stemBiomass.back()) *
                                      cell.U.forestShare.back() * cell.landAreaHa);
            countriesOForest10_ab_C.inc(plot.country, year,
                                        cell.O10.stemBiomass.back() * plot.BEF(cell.O10.stemBiomass.back()) *
                                        cell.O10.forestShare.back() * cell.landAreaHa);
            countriesOForest30_ab_C.inc(plot.country, year,
                                        cell.O30.stemBiomass.back() * plot.BEF(cell.O30.stemBiomass.back()) *
                                        cell.O30.forestShare.back() * cell.landAreaHa);
            countriesOForestP_ab_C.inc(plot.country, year,
                                       cell.P.stemBiomass.back() * plot.BEF(cell.P.stemBiomass.back()) *
                                       cell.P.forestShare.back() * cell.landAreaHa);
            // TODO why plot.CBelowHa?
            if (cell.U.stemBiomass.back() > 0)
                cell.U.totalBiomass = cell.U.stemBiomass.back() * plot.BEF(cell.U.stemBiomass.back()) + plot.CBelowHa;

            if (cell.O10.stemBiomass.back() > 0)
                cell.O10.totalBiomass =
                        cell.O10.stemBiomass.back() * plot.BEF(cell.O10.stemBiomass.back()) + plot.CBelowHa;

            if (cell.O30.stemBiomass.back() > 0)
                cell.O30.totalBiomass =
                        cell.O30.stemBiomass.back() * plot.BEF(cell.O30.stemBiomass.back()) + plot.CBelowHa;

            if (cell.P.stemBiomass.back() > 0)
                cell.P.totalBiomass = cell.P.stemBiomass.back() * plot.BEF(cell.P.stemBiomass.back()) + plot.CBelowHa;

            countriesOForestC_biomass.inc(plot.country, year,
                                          cell.U.totalBiomass * cell.U.forestShare.back() * cell.landAreaHa);
            countriesDeforestationCYear.inc(plot.country, year, emissionsCur * unitConvCoef);  // tC/year
            countriesDeforestationCYear_ab.inc(plot.country, year,
                                               resultDeforestation.biomass * plot.BEF(resultDeforestation.biomass) *
                                               cell.deforestHaYear * unitConvCoef);
            countriesDeforestationCYear_bl.inc(plot.country, year, plot.CBelowHa * cell.deforestHaYear * unitConvCoef);
            // tC/ha/year
            countriesDeforestationCYear_biomass.inc(plot.country, year,
                                                    cell.emissionsDeforestationBiomass * cell.deforestHaYear);
            countriesDeforestationCYear_dom.inc(plot.country, year,
                                                (emissionsDeadBurnCur + emissionsLitterCur) * unitConvCoef);
            countriesDeforestationCYear_soil.inc(plot.country, year, emissionsSOCCur * unitConvCoef);
            countriesDeforestationCYear_soil_altern.inc(plot.country, year, emissionsSOCCur2 * unitConvCoef);

            countriesWoodHarvestM3Year.inc(plot.country, year, harvestTotalAllM3);
            countriesWoodHarvestFmM3Year.inc(plot.country, year, harvestFmTotalM3);
            countriesWoodHarvestDfM3Year.inc(plot.country, year, cell.deforestationWoodTotalM3);
            countriesWoodHarvest10M3Year.inc(plot.country, year, cell.O10.totalHarvest());
            countriesWoodHarvest30M3Year.inc(plot.country, year, cell.O30.totalHarvest());

            countriesWoodHarvestFc_oldM3Year.inc(
                    plot.country, year,
                    (resU.getFinalCutWood() * cell.U.forestShare.back() +
                     res10.getFinalCutWood() * cell.O10.forestShare.back() +
                     res30.getFinalCutWood() * cell.O30.forestShare.back()) * plot.fTimber * cell.landAreaHa);
            countriesWoodHarvestTh_oldM3Year.inc(
                    plot.country, year,
                    (resU.getThinnedWood() * cell.U.forestShare.back() +
                     res10.getThinnedWood() * cell.O10.forestShare.back() +
                     res30.getThinnedWood() * cell.O30.forestShare.back()) * plot.fTimber * cell.landAreaHa);
            countriesWoodHarvestLostFc_oldM3Year.inc(
                    plot.country, year,
                    (cleanedWoodUseCurrent * resULost.getFinalCutWood() * cell.U.forestShare.back() +
                     cleanedWoodUseCurrent10 * res10Lost.getFinalCutWood() * cell.O10.forestShare.back() +
                     cleanedWoodUseCurrent30 * res30Lost.getFinalCutWood() * cell.O30.forestShare.back()) *
                    plot.fTimber * cell.landAreaHa);
            countriesWoodHarvestLostTh_oldM3Year.inc(
                    plot.country, year,
                    (cleanedWoodUseCurrent * resULost.getThinnedWood() * cell.U.forestShare.back() +
                     cleanedWoodUseCurrent10 * res10Lost.getThinnedWood() * cell.O10.forestShare.back() +
                     cleanedWoodUseCurrent30 * res30Lost.getThinnedWood() * cell.O30.forestShare.back()) *
                    plot.fTimber * cell.landAreaHa);
            countriesWoodHarvestSc_oldM3Year.inc(
                    plot.country, year,
                    (cleanedWoodUseCurrent * resULost.getTotalWoodRemoval() * cell.U.forestShare.back() +
                     cleanedWoodUseCurrent10 * res10Lost.getTotalWoodRemoval() * cell.O10.forestShare.back() +
                     cleanedWoodUseCurrent30 * res30Lost.getTotalWoodRemoval() * cell.O30.forestShare.back()) *
                    plot.fTimber * cell.landAreaHa);

            countriesWoodHarvestSalvage_oldM3Year.inc(plot.country, year, cell.salvageLogging);
            // corrected 21.08.2013
            countriesWoodHarvestFc_newM3Year.inc(
                    plot.country, year,
                    resN.getFinalCutWood() * plot.fTimber * cell.N.forestShare.end()[-2] * cell.landAreaHa);
            // corrected 21.08.2013
            countriesWoodHarvestTh_newM3Year.inc(
                    plot.country, year,
                    resN.getThinnedWood() * plot.fTimber * cell.N.forestShare.end()[-2] * cell.landAreaHa);
            countriesWoodHarvestSc_newM3Year.inc(
                    plot.country, year,
                    cleanedWoodUseCurrent * resN.getTotalWoodRemoval() * plot.fTimber * cell.N.forestShare.end()[-2] *
                    cell.landAreaHa);
            countriesHarvLossesYear.inc(
                    plot.country, year, (resU.getHarvestLosses() * cell.U.forestShare.back() +
                                         resN.getHarvestLosses() * cell.N.forestShare.end()[-2]) * plot.fTimber *
                                        cell.landAreaHa);
            countriesHarvLossesOldYear.inc(
                    plot.country, year,
                    (resU.getHarvestLosses() + resN.getHarvestLosses()) * cell.U.forestShare.back() * plot.fTimber *
                    cell.landAreaHa);
            countriesHarvLossesOldFcYear.inc(
                    plot.country, year, (resU.getFinalCutHarvestLosses() + resULost.getFinalCutHarvestLosses()) *
                                        cell.U.forestShare.back() * plot.fTimber * cell.landAreaHa);
            countriesHarvLossesOldThYear.inc(
                    plot.country, year, (resU.getThinningHarvestLosses() + resULost.getThinningHarvestLosses()) *
                                        cell.U.forestShare.back() * plot.fTimber * cell.landAreaHa);
            countriesFellingsNAIYear.inc(
                    plot.country, year,
                    resU.getTotalHarvestedBiomass() * plot.fTimber * cell.U.forestShare.back() * cell.landAreaHa /
                    NAI_old_m3ha);

            if (appThinningForest(plot.x, plot.y) > 0 || appThinningForest10(plot.x, plot.y) > 0 ||
                appThinningForest30(plot.x, plot.y) > 0) {
                if (appThinningForest(plot.x, plot.y) > 0) {
                    countriesManagedCount.inc(plot.country, year, 1);
                    countriesManagedForHa.inc(
                            plot.country, year,
                            (cell.N.forestShare.back() + cell.U.forestShare.back()) * cell.landAreaHa);
                    // Area of old used forest
                    countriesManagedForOldHa.inc(plot.country, year, cell.U.forestShare.back() * cell.landAreaHa);
                    // Area of old used forest
                    countriesManagedNewHa.inc(plot.country, year, cell.N.forestShare.back() * cell.landAreaHa);
                    // Gross Current Annual Increment, m3/ha
                    countriesGCAI_mng.inc(plot.country, year,
                                          grossCAI_m3ha * cell.U.forestShare.back() * cell.landAreaHa);
                    // Net Current Annual Increment, m3/ha
                    countriesNCAI_mng.inc(plot.country, year,
                                          netCAI_m3ha * cell.U.forestShare.back() * cell.landAreaHa);
                    // Gross Current Annual Increment, m3/ha
                    countriesGCAI_new_mng.inc(plot.country, year,
                                              grossCAI_new_m3ha * cell.U.forestShare.back() * cell.landAreaHa);
                    // Net Current Annual Increment, m3/ha
                    countriesNCAI_new_mng.inc(plot.country, year,
                                              netCAI_new_m3ha * cell.U.forestShare.back() * cell.landAreaHa);
                }
                if (appThinningForest10(plot.x, plot.y) > 0)
                    countriesManagedForHa.inc(plot.country, year, cell.O10.forestShare.back() * cell.landAreaHa);
                if (appThinningForest30(plot.x, plot.y) > 0)
                    countriesManagedForHa.inc(plot.country, year, cell.O30.forestShare.back() * cell.landAreaHa);
            } else
                countriesManagedForHa.setVal(plot.country, year, 0);

            countriesMAI.inc(plot.country, year, appMaiForest(plot.x, plot.y) * plot.fTimber);
            countriesCAI.inc(plot.country, year, cell.U.CAI * cell.U.forestShare.back() * cell.landAreaHa);

            double CAI_FAWS_O = cell.U.CAI * cell.U.forestShare.back() + cell.O10.CAI * cell.O10.forestShare.back() +
                                cell.O30.CAI * cell.O30.forestShare.back();
            double CAI_P = cell.P.CAI * cell.P.forestShare.back();
            double CAI_N = cell.N.CAI * cell.N.forestShare.end()[-2];

            if (plot.protect) {
                countriesCAI_FAWS_old.inc(plot.country, year, 0);
                countriesCAI_FAWS_all.inc(plot.country, year, 0);
            } else {
                countriesCAI_FAWS_old.inc(plot.country, year, CAI_FAWS_O * cell.landAreaHa);
                countriesCAI_FAWS_all.inc(plot.country, year, (CAI_FAWS_O + CAI_N) * cell.landAreaHa);
            }
            countriesCAI_ALL_old.inc(plot.country, year, (CAI_FAWS_O + CAI_P) * cell.landAreaHa);
            countriesCAI_ALL_all.inc(plot.country, year, (CAI_FAWS_O + CAI_P + CAI_N) * cell.landAreaHa);
            countriesCAI_new.inc(plot.country, year, CAI_N * cell.landAreaHa);

            countriesNAI_old.inc(plot.country, year, NAI_old_m3ha * cell.U.forestShare.back() * cell.landAreaHa);
            countriesGCAI.inc(plot.country, year, grossCAI_m3ha * cell.U.forestShare.back() *
                                                  cell.landAreaHa); // Gross Current Annual Increment, m3/ha
            countriesNCAI.inc(plot.country, year, netCAI_m3ha * cell.U.forestShare.back() *
                                                  cell.landAreaHa);// Net Current Annual Increment, m3/ha
            countriesGCAI_new.inc(plot.country, year, grossCAI_new_m3ha * cell.N.forestShare.end()[-2] *
                                                      cell.landAreaHa); // Gross Current Annual Increment, m3/ha
            countriesNCAI_new.inc(plot.country, year, netCAI_new_m3ha * cell.N.forestShare.end()[-2] *
                                                      cell.landAreaHa);// Net Current Annual Increment, m3/ha

            double biomassChangeAb_all = (cell.U.biomassChangeAb * cell.U.forestShare.back() +
                                          cell.O10.biomassChangeAb * cell.O10.forestShare.back() +
                                          cell.O30.biomassChangeAb * cell.O30.forestShare.back() +
                                          cell.P.biomassChangeAb * cell.P.forestShare.back());
            double biomassChangeTotal_all = (cell.U.biomassChangeTotal * cell.U.forestShare.back() +
                                             cell.O10.biomassChangeTotal * cell.O10.forestShare.back() +
                                             cell.O30.biomassChangeTotal * cell.O30.forestShare.back() +
                                             cell.P.biomassChangeTotal * cell.P.forestShare.back());

            double biomassChangeTotal_faws = 0;
            if (!plot.protect)
                biomassChangeTotal_faws = (cell.U.biomassChangeTotal * cell.U.forestShare.back() +
                                           cell.O10.biomassChangeTotal * cell.O10.forestShare.back() +
                                           cell.O30.biomassChangeTotal * cell.O30.forestShare.back());

            countriesFM.inc(plot.country, year, biomassChangeAb_all * cell.landAreaHa);
            countriesFMbm.inc(plot.country, year, biomassChangeTotal_all * cell.landAreaHa);

            countriesFMbmFAWS_old.inc(plot.country, year, biomassChangeTotal_faws * cell.landAreaHa);

            // profit only from harvesting old forest per ha
            double profit = !plot.protect ? (
                    (resU.getTotalWoodRemoval() + cleanedWoodUseCurrent * resULost.getTotalWoodRemoval()) *
                    plot.fTimber * timberPrice - plantingCosts * appCohortsU[plot.asID].getArea(size_t{0})) : 0;
            countriesProfit.inc(plot.country, year, profit);

            countryRotation.inc(plot.country, year, cell.rotation);
            if (cell.rotation > 0)
                countryRotationUsed.inc(plot.country, year, cell.rotation);
            else if (cell.rotation < 0)
                countryRotationUnused.inc(plot.country, year, cell.rotation);

            if (plot.protect) {
                countryRegWoodHarvestDfM3Year.inc(plot.country, year, 0);
                countryRegMaxHarvest.inc(plot.country, year, 0);
            } else {
                countryRegWoodHarvestDfM3Year.inc(plot.country, year, cell.deforestationWoodTotalM3);
                countryRegMaxHarvest.inc(
                        plot.country, year, cell.deforestationWoodTotalM3 +
                                            (appMaiForest(plot.x, plot.y) * plot.fTimber * (1 - coef.harvLoos) *
                                             (cell.U.forestShare.back() + cell.O10.forestShare.back() +
                                              cell.O30.forestShare.back()) +
                                             harvestWoodPlusN * cell.N.forestShare.back()) * cell.landAreaHa);
            }
            countryRegWoodProd.setVal(plot.country, year, dms.woodDemand.at(plot.country)(year));

            if (plot.managed_UNFCCC) {
                countriesOForestCoverUNFCCC.inc(plot.country, year, cell.forestShareOld(-1) * cell.landAreaHa);
                countriesFMbmUNFCCC.inc(plot.country, year, biomassChangeTotal_all * cell.landAreaHa);
            }

            // Estimation of harvest residues (branches and leaves) per grid in tC deprecated
            // Harvest details for Fulvio deprecated

            if (!plot.protect) {
                // harvest of broadleaf
                if (plot.isBroadleaf()) {
                    countriesWoodHarvBroadleafFc_oldM3Year.inc(plot.country, year, cell.U.harvestFc);
                    countriesWoodHarvBroadleafTh_oldM3Year.inc(plot.country, year, cell.U.harvestTh);
                    countriesWoodHarvBroadleafSc_oldM3Year.inc(plot.country, year, cell.U.harvestSc);

                    countriesWoodHarvBroadleafFc_newM3Year.inc(plot.country, year, cell.N.harvestFc);
                    countriesWoodHarvBroadleafTh_newM3Year.inc(plot.country, year, cell.N.harvestTh);
                    countriesWoodHarvBroadleafSc_newM3Year.inc(plot.country, year, cell.N.harvestSc);

                    if (appThinningForest(plot.x, plot.y) > 0) {
                        countriesAreaUsedBroadleaf_oldHa.inc(plot.country, year,
                                                             cell.U.forestShare.back() * cell.landAreaHa);
                        countriesAreaUsedBroadleaf_newdHa.inc(plot.country, year,
                                                              cell.N.forestShare.end()[-2] * cell.landAreaHa);
                        if (resU.positiveAreas())
                            countriesAreaHarvBroadleafFc_oldHaYear.inc(plot.country, year,
                                                                       resU.getAreaRatio() * cell.U.forestShare.back() *
                                                                       cell.landAreaHa);
                        if (resN.positiveAreas())
                            countriesAreaHarvBroadleafFc_newHaYear.inc(plot.country, year, resN.getAreaRatio() *
                                                                                           cell.N.forestShare.end()[-2] *
                                                                                           cell.landAreaHa);
                    } else {
                        countriesAreaScBroadleaf_oldHa.inc(plot.country, year,
                                                           cell.U.forestShare.back() * cell.landAreaHa);
                        countriesAreaScBroadleaf_newHa.inc(plot.country, year,
                                                           cell.N.forestShare.end()[-2] * cell.landAreaHa);
                    }
                } else { // harvest of conifers
                    countriesWoodHarvConiferFc_oldM3Year.inc(plot.country, year, cell.U.harvestFc);
                    countriesWoodHarvConiferTh_oldM3Year.inc(plot.country, year, cell.U.harvestTh);
                    countriesWoodHarvConiferSc_oldM3Year.inc(plot.country, year, cell.U.harvestSc);

                    countriesWoodHarvConiferFc_newM3Year.inc(plot.country, year, cell.N.harvestFc);
                    countriesWoodHarvConiferTh_newM3Year.inc(plot.country, year, cell.N.harvestTh);
                    countriesWoodHarvConiferSc_newM3Year.inc(plot.country, year, cell.N.harvestSc);

                    if (appThinningForest(plot.x, plot.y) > 0) {
                        countriesAreaUsedConifer_oldHa.inc(plot.country, year,
                                                           cell.U.forestShare.back() * cell.landAreaHa);
                        countriesAreaUsedConifer_newdHa.inc(plot.country, year,
                                                            cell.N.forestShare.end()[-2] * cell.landAreaHa);
                        if (resU.positiveAreas())
                            countriesAreaHarvConiferFc_oldHaYear.inc(plot.country, year,
                                                                     resU.getAreaRatio() * cell.U.forestShare.back() *
                                                                     cell.landAreaHa);
                        if (resN.positiveAreas())
                            countriesAreaHarvConiferFc_newHaYear.inc(plot.country, year, resN.getAreaRatio() *
                                                                                         cell.N.forestShare.end()[-2] *
                                                                                         cell.landAreaHa);
                    } else {
                        countriesAreaScConifer_oldHa.inc(plot.country, year,
                                                         cell.U.forestShare.back() * cell.landAreaHa);
                        countriesAreaScConifer_newHa.inc(plot.country, year,
                                                         cell.N.forestShare.end()[-2] * cell.landAreaHa);
                    }
                }
            }
            // deadwood_dynamics
            if (plot.protect) {
                if (cell.U.forestShare.back() > 0) {
                    rf.deadwoodTestBuffer += format("{},{},{},{},{},{},", plot.asID, plot.simuID,
                                                    idCountryGLOBIOM.at(plot.country), year, "U", false);
                    tie(cell.U.deadwoodInput, cell.U.litterInput) =
                            deadwoodPoolCalcFunc(plot, resULost, true, 0, 0, 0, 0, 0);
                }
                if (cell.O10.forestShare.back() > 0) {
                    rf.deadwoodTestBuffer += format("{},{},{},{},{},{},", plot.asID, plot.simuID,
                                                    idCountryGLOBIOM.at(plot.country), year, "O10", false);
                    tie(cell.O10.deadwoodInput, cell.O10.litterInput) =
                            deadwoodPoolCalcFunc(plot, res10Lost, true, 0, 0, 0, 0, 0);
                }
                if (cell.O30.forestShare.back() > 0) {
                    rf.deadwoodTestBuffer += format("{},{},{},{},{},{},", plot.asID, plot.simuID,
                                                    idCountryGLOBIOM.at(plot.country), year, "O30", false);
                    tie(cell.O30.deadwoodInput, cell.O30.litterInput) =
                            deadwoodPoolCalcFunc(plot, res30Lost, true, 0, 0, 0, 0, 0);
                }
                // No new forest in the protected cell (no afforestation)
            } else {
                if (cell.U.forestShare.back() > 0) {
                    bool used = appThinningForest(plot.x, plot.y) > 0;
                    rf.deadwoodTestBuffer += format("{},{},{},{},{},{},", plot.asID, plot.simuID,
                                                    idCountryGLOBIOM.at(plot.country), year, "U", used);
                    tie(cell.U.deadwoodInput, cell.U.litterInput) =
                            used ?
                            deadwoodPoolCalcFunc(plot, resU, false, cell.U.extractedResidues,
                                                 cell.U.extractedStump, cell.U.extractedCleaned,
                                                 harvestedWoodUse[plot.country], 1) :
                            deadwoodPoolCalcFunc(plot, resULost, true, cell.U.extractedResidues,
                                                 cell.U.extractedStump, cell.U.extractedCleaned,
                                                 harvestedWoodUse[plot.country], cleanedWoodUseCurrent);
                }
                // TODO decide if U or N extracted
                if (cell.N.forestShare.back() > 0) {
                    bool used = thinningForestNew(plot.x, plot.y) > 0;
                    rf.deadwoodTestBuffer += format("{},{},{},{},{},{},", plot.asID, plot.simuID,
                                                    idCountryGLOBIOM.at(plot.country), year, "N", used);
                    tie(cell.N.deadwoodInput, cell.N.litterInput) =
                            used ?
                            deadwoodPoolCalcFunc(plot, resN, false, cell.U.extractedResidues,
                                                 cell.U.extractedStump, cell.U.extractedCleaned,
                                                 harvestedWoodUse[plot.country], 1) :
                            deadwoodPoolCalcFunc(plot, resNLost, true, cell.U.extractedResidues,
                                                 cell.U.extractedStump, cell.U.extractedCleaned,
                                                 harvestedWoodUse[plot.country], cleanedWoodUseCurrent);
                }
                // To be set according to BIOCLIMA scenario for the 10% forest
                if (cell.O10.forestShare.back() > 0) {
                    bool used = appThinningForest10(plot.x, plot.y) > 0;
                    rf.deadwoodTestBuffer += format("{},{},{},{},{},{},", plot.asID, plot.simuID,
                                                    idCountryGLOBIOM.at(plot.country), year, "O10", used);
                    tie(cell.O10.deadwoodInput, cell.O10.litterInput) =
                            used ?
                            deadwoodPoolCalcFunc(plot, res10, false, cell.O10.extractedResidues,
                                                 cell.O10.extractedStump, cell.O10.extractedCleaned,
                                                 harvestedWoodUse[plot.country], 1) :
                            deadwoodPoolCalcFunc(plot, res10Lost, true, cell.O10.extractedResidues,
                                                 cell.O10.extractedStump, cell.O10.extractedCleaned,
                                                 harvestedWoodUse[plot.country], cleanedWoodUseCurrent10);
                }
                if (cell.O30.forestShare.back() > 0) {
                    bool used = appThinningForest30(plot.x, plot.y) > 0;
                    rf.deadwoodTestBuffer += format("{},{},{},{},{},{},", plot.asID, plot.simuID,
                                                    idCountryGLOBIOM.at(plot.country), year, "O30", used);
                    tie(cell.O30.deadwoodInput, cell.O30.litterInput) =
                            used ?
                            deadwoodPoolCalcFunc(plot, res30, false, cell.O30.extractedResidues,
                                                 cell.O30.extractedStump, cell.O30.extractedCleaned,
                                                 harvestedWoodUse[plot.country], 1) :
                            deadwoodPoolCalcFunc(plot, res30Lost, true, cell.O30.extractedResidues,
                                                 cell.O30.extractedStump, cell.O30.extractedCleaned,
                                                 harvestedWoodUse[plot.country], cleanedWoodUseCurrent30);
                }
            }
            if (cell.P.forestShare.back() > 0) {
                rf.deadwoodTestBuffer += format("{},{},{},{},{},{},", plot.asID, plot.simuID,
                                                idCountryGLOBIOM.at(plot.country), year, "P", false);
                tie(cell.P.deadwoodInput, cell.P.litterInput) =
                        deadwoodPoolCalcFunc(plot, resPLost, true, 0, 0, 0, 0, 0);
            }

            cell.resetExtracted();

            cell.U.deadwood += cell.U.deadwoodInput;
            cell.N.deadwood += cell.N.deadwoodInput;
            cell.O10.deadwood += cell.O10.deadwoodInput;
            cell.O30.deadwood += cell.O30.deadwoodInput;
            cell.P.deadwood += cell.P.deadwoodInput;

            cell.U.litter += cell.U.litterInput;
            cell.N.litter += cell.N.litterInput;
            cell.O10.litter += cell.O10.litterInput;
            cell.O30.litter += cell.O30.litterInput;
            cell.P.litter += cell.P.litterInput;

            double decStem_dec = max(0., 1 - decStem * modTimeStep);

            // 20.01.2022: Change to decStem as the Osnabrück decomposition rate is for woody litter
            cell.U.deadwood *= decStem_dec;
            cell.N.deadwood *= decStem_dec;
            cell.O10.deadwood *= decStem_dec;
            cell.O30.deadwood *= decStem_dec;
            cell.P.deadwood *= decStem_dec;

            cell.U.deadwood *= decHerb_decWood_dec_avg;
            cell.N.deadwood *= decHerb_decWood_dec_avg;
            cell.O10.deadwood *= decHerb_decWood_dec_avg;
            cell.O30.deadwood *= decHerb_decWood_dec_avg;
            cell.P.deadwood *= decHerb_decWood_dec_avg;

            // 20.01.2022: Change to decStem as the Osnabrück decomposition rate is for woody litter
            double deadwoodPoolOut_U = (cell.U.deadwood * decStem) + cell.U.burntDeadwood;
            double deadwoodPoolOut_N = (cell.N.deadwood * decStem) + cell.N.burntDeadwood;
            double deadwoodPoolOut_O10 = (cell.O10.deadwood * decStem) + cell.O10.burntDeadwood;
            double deadwoodPoolOut_O30 = (cell.O30.deadwood * decStem) + cell.O30.burntDeadwood;
            double deadwoodPoolOut_P = (cell.P.deadwood * decStem) + cell.P.burntDeadwood;

            double decWood_decHerb_avg = midpoint(plot.decWood, plot.decHerb);

            double litterPoolOut_U = (cell.U.litter * decWood_decHerb_avg) + cell.U.burntLitter;
            double litterPoolOut_N = (cell.N.litter * decWood_decHerb_avg) + cell.N.burntLitter;
            double litterPoolOut_O10 = (cell.O10.litter * decWood_decHerb_avg) + cell.O10.burntLitter;
            double litterPoolOut_O30 = (cell.O30.litter * decWood_decHerb_avg) + cell.O30.burntLitter;
            double litterPoolOut_P = (cell.P.litter * decWood_decHerb_avg) + cell.P.burntLitter;

            cell.U.deadwoodEmissions = deadwoodPoolOut_U * modTimeStep - cell.U.deadwoodInput;
            cell.N.deadwoodEmissions = deadwoodPoolOut_N * modTimeStep - cell.N.deadwoodInput;
            cell.O10.deadwoodEmissions = deadwoodPoolOut_O10 * modTimeStep - cell.O10.deadwoodInput;
            cell.O30.deadwoodEmissions = deadwoodPoolOut_O30 * modTimeStep - cell.O30.deadwoodInput;
            cell.P.deadwoodEmissions = deadwoodPoolOut_P * modTimeStep - cell.P.deadwoodInput;

            cell.U.litterEmissions = litterPoolOut_U * modTimeStep - cell.U.litterInput;
            cell.N.litterEmissions = litterPoolOut_N * modTimeStep - cell.N.litterInput;
            cell.O10.litterEmissions = litterPoolOut_O10 * modTimeStep - cell.O10.litterInput;
            cell.O30.litterEmissions = litterPoolOut_O30 * modTimeStep - cell.O30.litterInput;
            cell.P.litterEmissions = litterPoolOut_P * modTimeStep - cell.P.litterInput;

            countriesDeadwood_old_tCha.inc(plot.country, year,
                                           (cell.U.deadwood * cell.U.forestShare.back() +
                                            cell.O10.deadwood * cell.O10.forestShare.back() +
                                            cell.O30.deadwood * cell.O30.forestShare.back() +
                                            cell.P.deadwood * cell.P.forestShare.back()) * cell.landAreaHa);
            countriesDeadwood_new_tCha.inc(plot.country, year,
                                           cell.N.deadwood * cell.N.forestShare.back() * cell.landAreaHa);
            countriesLitter_old_tCha.inc(plot.country, year,
                                         (cell.U.litter * cell.U.forestShare.back() +
                                          cell.O10.litter * cell.O10.forestShare.back() +
                                          cell.O30.litter * cell.O30.forestShare.back() +
                                          cell.P.litter * cell.P.forestShare.back()) * cell.landAreaHa);
            countriesLitter_new_tCha.inc(plot.country, year,
                                         cell.N.litter * cell.N.forestShare.back() * cell.landAreaHa);

            countriesDeadwoodEm_old_mtco2year.inc(plot.country, year,
                                                  (cell.U.deadwoodEmissions * cell.U.forestShare.back() +
                                                   cell.O10.deadwoodEmissions * cell.O10.forestShare.back() +
                                                   cell.O30.deadwoodEmissions * cell.O30.forestShare.back() +
                                                   cell.P.deadwoodEmissions * cell.P.forestShare.back()) *
                                                  cell.landAreaHa * unitConvCoef);
            countriesDeadwoodEm_new_mtco2year.inc(plot.country, year,
                                                  cell.N.deadwoodEmissions * cell.N.forestShare.end()[-2] *
                                                  cell.landAreaHa * unitConvCoef);
            countriesLitterEm_old_mtco2year.inc(plot.country, year,
                                                (cell.U.litterEmissions * cell.U.forestShare.back() +
                                                 cell.O10.litterEmissions * cell.O10.forestShare.back() +
                                                 cell.O30.litterEmissions * cell.O30.forestShare.back() +
                                                 cell.P.litterEmissions * cell.P.forestShare.back()) * cell.landAreaHa *
                                                unitConvCoef);
            countriesLitterEm_new_mtco2year.inc(plot.country, year,
                                                cell.N.litterEmissions * cell.N.forestShare.end()[-2] *
                                                cell.landAreaHa *
                                                unitConvCoef);

            appOForestShGrid(plot.x, plot.y) = cell.forestShareOld(-1);
            cell.SD = appThinningForest(plot.x, plot.y);

            cell.U.OAC = appCohortsU[plot.asID].getActiveAge();
            cell.N.OAC = appCohortsN[plot.asID].getActiveAge();
            cell.O10.OAC = appCohorts10[plot.asID].getActiveAge();
            cell.O30.OAC = appCohorts30[plot.asID].getActiveAge();
            cell.P.OAC = appCohortsP[plot.asID].getActiveAge();

            if (year > 2000) {
                cell.deforestationShare = cell.deforestationShareTimeA[age];
                cell.deforestationPrev = cell.deforestationShareTimeA[age];
            }
        }

        // Deadwood input (d > 10cm) in the cell, tC/ha, in the old forest
        // Litter input (d <= 10cm) in the cell, tC/ha, in the old forest
        // returns pair<deadwoodPoolIn, litterPoolIn>
        pair<double, double>
        deadwoodPoolCalcFunc(const DataStruct &plot, const CohortRes &cr, const bool lost,
                             const double extractedResidues, // share of extracted logging residues
                             const double extractedStump, // share of extracted stump
                             const double extractedCleaned, // share of extracted dead trees in the multifunctional forests
                             const double harvestedWoodUse, // share of harvested wood use in the production forests
                             const double cleanedWoodUse // share of cleaned wood use in the multifunctional forests
        ) {
            const double shareLargeBranches = 0.3; // share of branches greater than 10 cm in diameter
            // deadwood (stem) of trees dying in each age cohort and at the end of lifespan in the old forests not used for intensive wood production
            // Total current harvested wood in the cell which is not used for intensive wood production, m^3/ha
            double deadwood_fc_unmanaged = 0;
            // deadwood (stem) of trees dying in each age cohort and at the end of lifespan in the old forests used for intensive wood production
            // Total current harvested wood in the cell which is used for intensive wood production, m^3/ha
            double deadwood_fc_managed = 0;
            // Litter from harvest losses in intensively managed old forest
            double litter_hRes_managed = 0;
            // Litter from harvest losses in non-intensively managed old forest
            double litter_hRes_unmanaged = 0;

            if (lost) {
                deadwood_fc_unmanaged = (1 - cleanedWoodUse) * cr.getTotalWoodRemoval();
                litter_hRes_unmanaged = cr.getHarvestLosses();
            } else {
                deadwood_fc_managed = (1 - harvestedWoodUse) * cr.getTotalWoodRemoval();
                litter_hRes_managed = cr.getHarvestLosses();
            }

            // deadwood (stem) due to mortality in old forest that is not classified as potentially merchantable
            double deadwood_mort = cr.realArea > 0 ? cr.thinning.deadwood / cr.realArea : 0;
            // litter (stem) due to mortality in old forest that is not classified as potentially merchantable
            double litter_mort = cr.realArea > 0 ? cr.thinning.litter / cr.realArea : 0;
            // thinned wood area-weighted, tC/ha
            double thinned_weight = cr.thinning.thinnedWeight;
            // area-weighted deadwood (stem) due to mortality in old forest that is not classified as potentially merchantable
            double deadwood_mort_weight = cr.thinning.deadwoodWeight;
            // area-weighted litter (stem) due to mortality in old forest that is not classified as potentially merchantable
            double litter_mort_weight = cr.thinning.litterWeight;

            double fcDBH = cr.finalCut.DBH;
            double fcH = cr.finalCut.H;
            double fcGS = cr.getHarvestGrowingStock();

            double thDBH_old = cr.thinning.DBH;
            double thH_old = cr.thinning.H;

            double deadwoodStump_fc = 0;
            double deadwoodStump_th = plot.DBHHToStump(thDBH_old, thH_old, cr.biomassThinning);
            // tC/ha in the cell
            double deadwoodBranches_fc = 0;

            if (cr.positiveAreas()) {
                deadwoodStump_fc = plot.DBHHToStump(fcDBH, fcH, fcGS) * cr.getAreaRatio();
                deadwoodBranches_fc = shareLargeBranches * cr.biomassHarvest * (plot.BEF(fcGS) - 1);
            }

            double litterBranches_fc = deadwoodBranches_fc * (1 / shareLargeBranches - 1);

            double deadwoodBranches_th = shareLargeBranches * cr.biomassThinning * (plot.BEF(thinned_weight) - 1);
            double litterBranches_th = deadwoodBranches_th * (1 / shareLargeBranches - 1);

            double deadwoodBranches_mort = shareLargeBranches * deadwood_mort * (plot.BEF(deadwood_mort_weight) - 1);
            double litterBranches_mort = litter_mort * (plot.BEF(litter_mort_weight) - 1);

            litterBranches_mort += deadwoodBranches_mort * (1 / shareLargeBranches - 1);

            double deadwoodStump_mort = plot.DBHHToStump(cr.thinning.mortDeadwoodDBH, cr.thinning.mortDeadwoodH,
                                                         deadwood_mort);
            double litterStump_mort = plot.DBHHToStump(cr.thinning.mortLitterDBH, cr.thinning.mortLitterH, litter_mort);

            double residuesLeftShare = 1 - extractedResidues * plot.residuesUseShare;
            double stumpLeftShare = 1 - extractedStump * plot.residuesUseShare;
            double cleanedLeftShare = 1 - extractedCleaned * plot.residuesUseShare;

            double deadwoodPoolIn =
                    // branches from harvested trees
                    cleanedWoodUse * (deadwoodBranches_fc + deadwoodBranches_th) * residuesLeftShare
                    // branches from dead trees in multifunction forest collected for bioenergy
                    + (1 - cleanedWoodUse) * (deadwoodBranches_fc + deadwoodBranches_th) * cleanedLeftShare
                    // + (deadwoodStump_fc + deadwoodStump_th) * stumpLeftShare
                    + deadwood_fc_managed
                    // stem of dead trees in multifunction forest collected for bioenergy
                    + deadwood_fc_unmanaged * cleanedLeftShare
                    + deadwood_mort
                    + deadwoodBranches_mort
            // + deadwoodStump_mort
            ;
            double litterPoolIn =
                    // branches from harvested trees
                    cleanedWoodUse * (litterBranches_fc + litterBranches_th) * residuesLeftShare
                    // branches from dead trees in multifunction forest collected for bioenergy
                    + (1 - cleanedWoodUse) * (litterBranches_fc + litterBranches_th) * cleanedLeftShare
                    + litter_mort
                    + litterBranches_mort
                    // + litterStump_mort
                    + litter_hRes_managed * residuesLeftShare
                    + litter_hRes_unmanaged * residuesLeftShare * cleanedWoodUse
                    + litter_hRes_unmanaged * cleanedLeftShare * (1 - cleanedWoodUse);

            rf.deadwoodTestBuffer +=
                    format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}\n",
                           extractedResidues, extractedCleaned, plot.residuesUseShare, deadwoodBranches_fc,
                           deadwoodBranches_th,
                           cleanedWoodUse * (deadwoodBranches_fc + deadwoodBranches_th) * residuesLeftShare,
                           (1 - cleanedWoodUse) * (deadwoodBranches_fc + deadwoodBranches_th) * cleanedLeftShare,
                           deadwood_fc_managed, deadwood_fc_unmanaged, deadwood_fc_unmanaged * cleanedLeftShare,
                           deadwood_mort, deadwoodBranches_mort, deadwoodStump_mort, deadwoodPoolIn, litterBranches_fc,
                           litterBranches_th,
                           cleanedWoodUse * (litterBranches_fc + litterBranches_th) * residuesLeftShare,
                           (1 - cleanedWoodUse) * (litterBranches_fc + litterBranches_th) * cleanedLeftShare,
                           litter_mort, litterBranches_mort, litterStump_mort, litter_hRes_managed,
                           litter_hRes_unmanaged, litter_hRes_managed * residuesLeftShare * cleanedWoodUse,
                           litter_hRes_unmanaged * cleanedLeftShare * (1 - cleanedWoodUse), litterPoolIn);

            return {deadwoodPoolIn, litterPoolIn};
        }
    };
}

#endif
