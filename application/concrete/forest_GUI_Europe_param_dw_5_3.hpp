#ifndef G4M_EUROPE_DG_FOREST_GUI_EUROPE_PARAM_DW_5_3_DG_HPP
#define G4M_EUROPE_DG_FOREST_GUI_EUROPE_PARAM_DW_5_3_DG_HPP

#include <iostream>
#include <array>
#include <string>
#include <set>
#include <unordered_set>
#include <numbers>

#include "../../log.hpp"
#include "../../diagnostics/debugging.hpp"
#include "../../diagnostics/timer.hpp"
#include "../abstract/application.hpp"

#include "../../start_data/start_data.hpp"
#include "../../settings/dicts/dicts.hpp"

#include "../../structs/lw_price.hpp"
#include "../../structs/harvest_residues.hpp"

#include "../../misc/concrete/ffipolm.hpp"

#include "../../init/species.hpp"

#include "../../GLOBIOM_scenarios_data/datamaps/datamaps.hpp"
#include "../../GLOBIOM_scenarios_data/simu_ids/simu_ids.hpp"

using namespace std;

using namespace numbers;
using namespace g4m::structs;
using namespace g4m::misc::concrete;
using namespace g4m::increment;
using namespace g4m::init;
using namespace g4m::diagnostics;
using namespace g4m::application::abstract;
using namespace g4m::StartData;
using namespace g4m::Dicts;
using namespace g4m::GLOBIOM_scenarios_data;

namespace g4m::application::concrete {

    class Forest_GUI_Europe_param_dw_5_3 : public Application {
    public:
        explicit Forest_GUI_Europe_param_dw_5_3(const span<const string> args_)
                : Application{args_}, dms{datamapScenarios, full_scenario, inputPriceC},
                  sis{simuIdScenarios, full_scenario, c_scenario[1]} {
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
            if (fmPol && !binFilesOnDisk && inputPriceC == 0)
                signalZeroCToMainScenarios.emplace(suffix0, 0);
        }

        // start calculations
        void Run() override {
            INFO("Application {} is running", appName);
            // loop by years
            for (uint16_t year = coef.bYear; year <= coef.eYear; ++year) {
                INFO("Processing year {}", year);
                uint16_t age = year - coef.bYear;

                if (year == forPolicyYearBioClima) {
                    appForest10Policy = commonForest10PolicyKey;
                    appForest30Policy = commonForest30PolicyKey;
                    appMultifunction10 = commonMultifunction10Key;
                    appMultifunction30 = commonMultifunction30Key;
                }

                if (MAIClimateShift && year > 2020)
                    for (const auto &plot: appPlots) {
                        // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
                        // Max mean annual increment of New forest (with uniform age structure and managed with rotation length maximizing MAI)
                        double MAI = max(0.,
                                         appDats[plot.asID].forestShare0 > 0 ?
                                                 MAI = plot.MAIE(year) : MAI = plot.MAIN(year));

                        appMaiForest(plot.x, plot.y) = MAI;

                        appCohortsU[plot.asID].setMaiAndAvgMai(MAI);
                        appCohorts10[plot.asID].setMaiAndAvgMai(MAI);
                        appCohorts30[plot.asID].setMaiAndAvgMai(MAI);
                        appCohortsP[plot.asID].setMaiAndAvgMai(MAI);
                        appCohortsN[plot.asID].setMaiAndAvgMai(MAI);
                    }

                INFO("Adjusting FM...");
                adjustManagedForest(year);
            }

            if (fmPol && !binFilesOnDisk && inputPriceC == 0)
                signalZeroCToMainScenarios.at(suffix0).release();
        }

    protected:
        string appName = format("{}_{}_{}_{}", args[1], args[2], args[3], args[4]);
        Timer timer{appName};

        array<string, 3> c_scenario = {args[1], args[2], args[3]};
        int inputPriceC = stoi(args[4]);
        string full_scenario = c_scenario[0] + '_' + c_scenario[1] + '_' + c_scenario[2];
        string local_suffix = string{suffix} + full_scenario + (inputPriceC == 0 ? "_Pco2_0" : "");
        string suffix0 = c_scenario[0] + '_' + c_scenario[1];

        // Apply the MAI climate shifters starting from MAIClimateShiftYear
        bool MAIClimateShift = c_scenario[1].contains("RCP");

        vector<DataStruct> appPlots = commonPlots;

        Datamaps dms;
        SimuIds sis;

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

        CountryData countriesResiduesExtract_m3;        // sustainably extracted harvest residuals, m3
        CountryData countriesResiduesDemand_m3;         // harvest residues demand recalculated to m3
        CountryData countriesResExtSoilEm_MtCO2Year;    // soil loss emissions due to sustainably extracted harvest residuals, MtCO2/year

        CountryData countryFMCPolMaxNPV;
        CountryData countryFMCPolCurrNPV;

        CountryData countriesWoodHarvestM3Year;
        CountryData countriesWoodHarvestUNM3Year;   // Harvest from usual old and new forests plus deforested wood
        CountryData countriesWoodHarvest10M3Year;   // Harvest from 10% forest
        CountryData countriesWoodHarvest30M3Year;   // Harvest from 30% forest

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
                double maie = plot.MAIE.data.at(2000);
                double main = plot.MAIN.data.at(2000);
                double npp = plot.NPP.data.at(2000);

                plot.MAIE.data[2020] = maie;
                plot.MAIN.data[2020] = main;
                plot.NPP.data[2020] = npp;

                plot.decHerb.data[2020] = plot.decHerb.data.at(2000);
                plot.decWood.data[2020] = plot.decWood.data.at(2000);
                plot.decSOC.data[2020] = plot.decSOC.data.at(2000);

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

            for (auto &plot: appPlots)
                if (!plot.protect) {

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

                        if (protectedNatLnd && plot.grLnd_protect > 0) {
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

            if (protectedNatLnd && sumGrLnd_protect <= 0)
                ERROR("GRLND_PROTECT is 0. Check spatial input data (plots)!!!");
        }

        // apply corruption and populate country CO2Price if inputPriceC > 0
        void applyCorruptionModifyCO2Price() noexcept {
            // CO2 price different for regions: set priceC to negative value then the price is defined in the function carbonPriceRegion
            if (inputPriceC > 0) {
                double priceC = inputPriceC * deflator * 44. / 12.;  // * 44. / 12. if input price is $/tCO2

                for (auto &[country, ipol]: dms.CO2Price)
                    ipol.data[refYear] = priceC;
            }

            array<double, numberOfCountries> countryCorruption{};
            for (const auto &plot: appPlots)
                countryCorruption[plot.country] = plot.corruption;

            for (const auto &[country, corruption]: countryCorruption | rv::enumerate)
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
                for (const auto &plot: appPlots)
                    if (!plot.protect) {
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
                for (const auto &plot: appPlots)
                    if (!plot.protect && appThinningForest(plot.x, plot.y) < 0)
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

                // TODO continue logic
                doneList.clear();
                for (uint8_t maxDiffCountry = 0;
                     doneList.size() < countriesList.size(); doneList.insert(maxDiffCountry)) {
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
                            if (fm_hurdle < 2 && fm_hurdle > 0) fm_hurdle += 0.2;
                            else if (fm_hurdle < 10 && fm_hurdle >= 2) fm_hurdle += 1;
                            else if (fm_hurdle < 100 && fm_hurdle >= 10) fm_hurdle += 10;
                            else if (fm_hurdle < 1'000 && fm_hurdle >= 100) fm_hurdle += 100;
                            else if (fm_hurdle < 100'000 && fm_hurdle >= 1'000) fm_hurdle += 50'000;
                            else if (fm_hurdle < 1'000'000 && fm_hurdle >= 100'000) fm_hurdle += 500'000;
                            else if (fm_hurdle < 10'000'000 && fm_hurdle >= 1'000'000) fm_hurdle += 5'000'000;
                            else if (fm_hurdle < 100'000'000 && fm_hurdle >= 10'000'000) fm_hurdle += 50'000'000;
                            else if (fm_hurdle < 1'000'000'000 && fm_hurdle >= 100'000'000) fm_hurdle += 500'000'000;
                            else if (fm_hurdle >= 1'000'000'000) fm_hurdle = -1;    // Try for Ireland
                            else if (fm_hurdle >= -1 && fm_hurdle < 0) fm_hurdle = -2;
                            else if (fm_hurdle >= -2 && fm_hurdle < 0) fm_hurdle = -10;
                            else if (fm_hurdle >= -100'000 && fm_hurdle < 0) fm_hurdle = -1'000'000;
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
                    for (const auto i: countriesList)
                        if (dms.woodDemand.at(i)(year) > 0) {
                            double residuesDemand = dms.residuesDemand.at(i)(year);
                            double residuesDiff = residuesDemand > 0 ? abs(residueHarvest[i] / residuesDemand - 1) : 0;
                            TRACE("{}: residuesDiff = {}\tresiduesDemand = {}\tresiduesHarvest = {}\tharvestResiduesSoilEmissions = {}",
                                  i, residuesDiff, residuesDemand, residueHarvest[i], harvestResiduesSoilEmissions[i]);
                        }
                }

                INFO("-----NPV post control-----");
                if (NPV_postControl)
                    for (const auto &plot: appPlots)
                        if (!plot.protect) {
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
                if (fmPol && year > refYear && NPV_postControl_0)
                    for (const auto &plot: appPlots)
                        if (!plot.protect) {
                            bool used = appThinningForest(plot.x, plot.y) > 0;
                            double NPVTmp = npvCalc(plot, appCohortsU[plot.asID], year,
                                                    appRotationForest(plot.x, plot.y), used).first;
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

            for (const auto &plot: appPlots)
                if (!plot.protect) {
                    double forestShareConcerned = forestConcerned == 10 ?
                                                  appDats[plot.asID].O10.forestShare
                                                                        : appDats[plot.asID].O30.forestShare;

                    if (forestShareConcerned > 0) {
                        double SD = cohortVec[plot.asID].getStockingDegree();
                        double biomassRot = 0;
                        double rotMaxBm = 0;

                        double biomass = forestConcerned == 10 ?
                                         appDats[plot.asID].O10.prevStemBiomass
                                                               : appDats[plot.asID].O30.prevStemBiomass;

                        if (biomass > 0 && plot.CAboveHa > 0 && appMaiForest(plot.x, plot.y) > 0) {
                            double biomassTmp = max(biomass, appDats[plot.asID].OBiomass0);
                            // rotation time to get current biomass (without thinning)
                            biomassRot = species[plot.speciesType].getU(biomassTmp, appMaiForest(plot.x, plot.y));
                            rotMaxBm = species[plot.speciesType].getTOpt(appMaiForest(plot.x, plot.y), ORT::MaxBm);
                        }

                        //  TO BE CHANGED to specific cleanedWoodUse10[...] / cleanedWoodUse30[...]
                        double cleanedWoodUseCurrent = cleanedWoodUse[plot.country];

                        if (biomass > 0 && appMaiForest(plot.x, plot.y) > 0 && SD != 0) {
                            double harvestTmp = appHarvestGrid(plot.x, plot.y);
                            double harvestO = cohortVec[plot.asID].cohortRes().getTotalWoodRemoval();
                            double lostHarvestTmp = cleanedWoodUseCurrent * harvestO * forestShareConcerned *
                                                    appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear);
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
                                double cleanedWoodUseNew = cleanedWoodUse[plot.country];

                                if (forestConcerned == 10 && appForest10Policy)
                                    cleanedWoodUseNew *= cleanWoodUseShare10;
                                else if (forestConcerned == 30 && appForest30Policy)
                                    cleanedWoodUseNew *= cleanWoodUseShare30;

                                harvestTmp = appHarvestGrid(plot.x, plot.y);
                                harvestO = cohortVec[plot.asID].cohortRes().getTotalWoodRemoval();
                                double harvestNewTmp = cleanedWoodUseNew * harvestO * forestShareConcerned *
                                                       appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear);
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
                double reciprocalFTimber = 1 / plot.fTimber.data.at(2000);

                double damagedWind = 0;
                double damagedFire = 0;
                double damagedBiotic = 0;

                if (disturbanceTrend) {
                    damagedWind = sis.disturbWind[plot.simuID](year) * reciprocalFTimber;
                    damagedFire = sis.disturbFire[plot.simuID](year) * reciprocalFTimber;
                    damagedBiotic = sis.disturbBiotic[plot.simuID](year) * reciprocalFTimber;
                } else if (disturbanceExtreme && year == disturbanceExtremeYear) {
                    damagedWind = sis.disturbWindExtreme[plot.simuID](year) * reciprocalFTimber;
                    damagedFire = sis.disturbFireExtreme[plot.simuID](year) * reciprocalFTimber;
                    damagedBiotic = sis.disturbBioticExtreme[plot.simuID](year) * reciprocalFTimber;
                }

                double cleanedWoodUseCurrent10 = 0;
                if (!appForest10Policy)
                    cleanedWoodUseCurrent10 =
                            cleanedWoodUse[plot.country] + appDats[plot.asID].harvestEfficiencyMultifunction;
                else if (appMultifunction10)
                    cleanedWoodUseCurrent10 = cleanWoodUseShare10 * cleanedWoodUse[plot.country];

                double cleanedWoodUseCurrent30 = 1;
                if (appForest30Policy)
                    cleanedWoodUseCurrent30 = appMultifunction30 ? cleanWoodUseShare30 * cleanedWoodUse[plot.country]
                                                                 : 0;
                else if (appThinningForest30(plot.x, plot.y) < 0)
                    cleanedWoodUseCurrent30 =
                            cleanedWoodUse[plot.country] + appDats[plot.asID].harvestEfficiencyMultifunction;

                double harvestableFire = 0;
                double harvestableWind = 0;
                double harvestableBiotic = 0;

                if (!plot.protect) {
                    harvestableFire = 0.25;  // after consulting CBM database, email by Viorel Blujdea 26.07.2023 // Uncertain
                    harvestableWind = 0.7;
                    harvestableBiotic = 0.95;
                }

                const double shareU = appDats[plot.asID].U.forestShare;
                const double share10 = appDats[plot.asID].O10.forestShare;
                const double share30 = appDats[plot.asID].O30.forestShare;
                const double shareP = plot.strictProtected;  // The share of strictly protected forest. Usually, the area of strict protected forest does not change
                const double shareNew = appDats[plot.asID].N.forestShare;

                const auto [harvestO, bmH, damagedFireU, harvAreaO] =
                        shareU > 0 ? appCohortsU[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                    damagedFire,
                                                                                    damagedBiotic,
                                                                                    harvestableWind,
                                                                                    harvestableFire,
                                                                                    harvestableBiotic)
                                   : array{0., 0., 0., 0.};

                const auto [harvestO30, bmH30, damagedFire30, harvArea30] =
                        share30 > 0 ? appCohorts30[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                      damagedFire,
                                                                                      damagedBiotic,
                                                                                      harvestableWind,
                                                                                      harvestableFire,
                                                                                      harvestableBiotic)
                                    : array{0., 0., 0., 0.};

                const auto [harvestO10, bmH10, damagedFire10, harvArea10] =
                        share10 > 0 ? appCohorts10[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                      damagedFire,
                                                                                      damagedBiotic,
                                                                                      harvestableWind,
                                                                                      harvestableFire,
                                                                                      harvestableBiotic)
                                    : array{0., 0., 0., 0.};

                // ---- we don't account for the deadwood accumulation at the moment (to be improved) ---
                const auto [harvestNew, bmH_new, damagedFireNew, harvAreaNew] =
                        shareNew > 0 ? appCohortsN[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                      damagedFire,
                                                                                      damagedBiotic,
                                                                                      harvestableWind,
                                                                                      harvestableFire,
                                                                                      harvestableBiotic)
                                     : array{0., 0., 0., 0.};

                // ---- we don't clean salvage in the primary forest and don't account for the deadwood accumulation at the moment (to be improved) ---
                const auto [harvestP, bmHP, damagedFireP, harvAreaP] =
                        shareNew > 0 ? appCohortsP[plot.asID].salvageLoggingAllAgents(damagedWind,
                                                                                      damagedFire,
                                                                                      damagedBiotic,
                                                                                      harvestableWind,
                                                                                      harvestableFire,
                                                                                      harvestableBiotic)
                                     : array{0., 0., 0., 0.};

                salvageLogging(plot.x, plot.y) =
                        (harvestO * (shareU - appDats[plot.asID].deforestShare) + shareNew * harvestNew +
                         harvestO30 * cleanedWoodUseCurrent30 * share30 +
                         harvestO10 * cleanedWoodUseCurrent10 * share10) * plot.fTimber.data.at(2000) *
                        appDats[plot.asID].landAreaHa;

                if (damagedFireU + damagedFire30 + damagedFire10 + damagedFireNew + damagedFireP <= 0)
                    residueHarvest[plot.country] += adjustResiduesDisturbed(plot, year,
                                                                            harvestO, harvestNew, harvestO30,
                                                                            bmH, bmH_new, bmH30,
                                                                            harvAreaO, harvAreaNew, harvArea30);

                const auto [deadWoodPoolIn, litterPoolIn] =
                        shareU > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohortsU[plot.asID],
                                                                     harvestO, shareU, harvAreaO, bmH,
                                                                     appDats[plot.asID].U.extractedResidues,
                                                                     appDats[plot.asID].U.extractedStump)
                                   : pair{0., 0.};

                const auto [deadWoodPoolIn_new, litterPoolIn_new] =
                        shareNew > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohortsN[plot.asID],
                                                                       harvestNew,
                                                                       appDats[plot.asID].N.prevForestShare,
                                                                       harvAreaNew, bmH_new,
                                                                       appDats[plot.asID].U.extractedResidues,
                                                                       appDats[plot.asID].U.extractedStump)
                                     : pair{0., 0.};

                const auto [deadWoodPoolIn10, litterPoolIn10] =
                        share10 > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohorts10[plot.asID],
                                                                      harvestO10 * (1 - cleanedWoodUseCurrent10),
                                                                      share10, harvArea10, bmH10,
                                                                      appDats[plot.asID].O10.extractedResidues,
                                                                      appDats[plot.asID].O10.extractedStump)
                                    : pair{0., 0.};

                const auto [deadWoodPoolIn30, litterPoolIn30] =
                        share30 > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohorts30[plot.asID],
                                                                      harvestO30, share30, harvArea30, bmH30,
                                                                      appDats[plot.asID].O30.extractedResidues,
                                                                      appDats[plot.asID].O30.extractedStump)
                                    : pair{0., 0.};

                const auto [deadWoodPoolInP, litterPoolInP] =
                        shareP > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohortsP[plot.asID], 0, shareP,
                                                                     harvAreaP, bmHP, 0, 0)
                                   : pair{0., 0.};

                appDats[plot.asID].U.burntDeadwood = min(damagedFireU, 0.9 * appDats[plot.asID].U.deadwood);
                appDats[plot.asID].O30.burntDeadwood = min(damagedFire30, 0.9 * appDats[plot.asID].O30.deadwood);
                appDats[plot.asID].O10.burntDeadwood = min(damagedFire10, 0.9 * appDats[plot.asID].O10.deadwood);
                appDats[plot.asID].N.burntDeadwood = min(damagedFireNew, 0.9 * appDats[plot.asID].N.deadwood);
                appDats[plot.asID].P.burntDeadwood = min(damagedFireP, 0.9 * appDats[plot.asID].P.deadwood);

                appDats[plot.asID].U.burntLitter = min(damagedFireU, 0.9 * appDats[plot.asID].U.litter);
                appDats[plot.asID].O30.burntLitter = min(damagedFire30, 0.9 * appDats[plot.asID].O30.litter);
                appDats[plot.asID].O10.burntLitter = min(damagedFire10, 0.9 * appDats[plot.asID].O10.litter);
                appDats[plot.asID].N.burntLitter = min(damagedFireNew, 0.9 * appDats[plot.asID].N.litter);
                appDats[plot.asID].P.burntLitter = min(damagedFireP, 0.9 * appDats[plot.asID].P.litter);

                appDats[plot.asID].U.deadwood += deadWoodPoolIn;
                appDats[plot.asID].N.deadwood += deadWoodPoolIn_new;
                appDats[plot.asID].O10.deadwood += deadWoodPoolIn10;
                appDats[plot.asID].O30.deadwood += deadWoodPoolIn30;
                appDats[plot.asID].P.deadwood += deadWoodPoolInP;

                appDats[plot.asID].U.litter += litterPoolIn;
                appDats[plot.asID].N.litter += litterPoolIn_new;
                appDats[plot.asID].O10.litter += litterPoolIn10;
                appDats[plot.asID].O30.litter += litterPoolIn30;
                appDats[plot.asID].P.litter += litterPoolInP;
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
                                                     const double harvestO,
                                                     const double harvestNew,
                                                     const double harvest30,
                                                     const double bmH,       // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
                                                     const double bmHNew,       // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
                                                     const double bmH30,       // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
                                                     const double harvAreaO, const double harvAreaN,
                                                     const double harvArea30) {
            if (year <= residueHarvYear || plot.protect || plot.residuesUseShare <= 0)
                return 0;

            double realAreaO = appCohortsU[plot.asID].getArea();
            double realAreaN = appCohortsN[plot.asID].getArea();
            double realArea30 = appCohorts30[plot.asID].getArea();

            const double OForestShareU = appDats[plot.asID].U.forestShare - appDats[plot.asID].deforestShare;
            const double OForestShare30 = appDats[plot.asID].O30.forestShare;
            const double AForestShare = appDats[plot.asID].N.forestShare;
            const double residueUse30 = !appForest30Policy;

            //---- Estimation of harvest residues (branches and leaves) per grid in tC ------------------------------------
            double harvRes_fcO = 0;
            double harvRes_fcN = 0;
            double harvRes_fc30 = 0;
//            double harvRes_thO = 0;

//            double harvRes_thN = 0;
//            double harvRes_scO = 0;  // residues extracted from cleaned (old) forest that is taken for wood production
//            double harvRes_scN = 0;  // residues extracted from cleaned (new) forest that is taken for wood production
//            double harvRes_scN_notTaken = 0;  // all wood extracted from (new) cleaned forest that is not taken for wood production
//            double harvRes_scO_notTaken = 0;  // all wood extracted from (old) cleaned forest that is not taken for wood production

            if (harvAreaO > 0 && realAreaO > 0 && harvestO > 0 && bmH > 0)  // tC/ha
                harvRes_fcO = harvestO * (plot.BEF(bmH * realAreaO / harvAreaO) - 2) + bmH;

            if (harvAreaN > 0 && realAreaN > 0 && harvestNew > 0 && bmHNew > 0)  // tC/ha
                harvRes_fcN = harvestNew * (plot.BEF(bmHNew * realAreaN / harvAreaN) - 2) + bmHNew;

            if (harvArea30 > 0 && realArea30 > 0 && harvest30 > 0 && bmH30 > 0 && residueUse30 > 0)  // tC/ha
                harvRes_fc30 = harvest30 * (plot.BEF(bmH30 * realArea30 / harvArea30) - 2) + bmH30;

            double stump = 0;       // stumps of old forest // tC/ha
            double stump_new = 0;   // stumps of new forest// tC/ha
            double stump30 = 0;     // stumps of old forest // tC/ha
//            double stump_df = 0;
//            double stump10 = 0;     // stumps of old forest // tC/ha
//            double stump_df10 = 0;

//            double stump_df30 = 0;

            // calculate amount of stumps + big roots for final felling, tC/ha
            if (stumpHarvCountrySpecies.contains({plot.country, plot.speciesType})) {
                if (appThinningForest(plot.x, plot.y) > 0) {
                    double hDBHOld = appCohortsU[plot.asID].getDBHSlashCut();
                    double hHOld = appCohortsU[plot.asID].getHSlashCut();
                    double hDBHNew = appCohortsN[plot.asID].getDBHSlashCut();
                    double hHNew = appCohortsN[plot.asID].getHSlashCut();

                    if (harvAreaO > 0 && realAreaO > 0) {
                        double harvestGSOld = bmH / harvAreaO * realAreaO;
                        stump = plot.DBHHToStump(hDBHOld, hHOld, harvestGSOld) * harvAreaO / realAreaO;
                    }

                    if (harvAreaN > 0 && realAreaN > 0) {
                        double harvestGSNew = bmHNew / harvAreaN * realAreaN;
                        stump_new = plot.DBHHToStump(hDBHNew, hHNew, harvestGSNew) * harvAreaN / realAreaN;
                    }
                }

                if (appThinningForest30(plot.x, plot.y) > 0 && !appForest30Policy) {
                    double hDBHOld30 = appCohorts30[plot.asID].getDBHSlashCut();
                    double hHOld30 = appCohorts30[plot.asID].getHSlashCut();

                    if (harvArea30 > 0 && realArea30 > 0 && plot.fTimber.data.at(2000) > 0 && hDBHOld30 > 0 &&
                        hHOld30 > 0) {
                        double harvestGSOld30 = bmH30 / harvArea30 * realArea30;
                        stump30 = plot.DBHHToStump(hDBHOld30, hHOld30, harvestGSOld30) * harvArea30 / realArea30;
                    }
                }
            }

            double residuesSuit1_perHa = plot.residuesUseShare * (harvRes_fcO + harvRes_fcN);   // tC/ha
            double residuesSuit2_perHa = plot.residuesUseShare * (stump + stump_new);           // tC/ha

            double residuesSuit1_perHa30 = plot.residuesUseShare * harvRes_fc30;               // tC/ha
            double residuesSuit2_perHa30 = plot.residuesUseShare * stump30 * residueUse30;      // tC/ha

            double residuesSuit1 = plot.residuesUseShare * (harvRes_fcO * OForestShareU + harvRes_fcN * AForestShare) *
                                   appDats[plot.asID].landAreaHa;
            double residuesSuit2 = plot.residuesUseShare * (stump * OForestShareU + stump_new * AForestShare) *
                                   appDats[plot.asID].landAreaHa;

            double residuesSuit1_30 = residuesSuit1_perHa30 * OForestShare30 * appDats[plot.asID].landAreaHa;
            double residuesSuit2_30 = residuesSuit2_perHa30 * OForestShare30 * appDats[plot.asID].landAreaHa;

            double residueHarvestDist =
                    (residuesSuit1 + residuesSuit2 + residuesSuit1_30 + residuesSuit2_30) * plot.fTimber.data.at(2000);

            countriesResiduesExtract_m3.inc(plot.country, year, residueHarvestDist);
            return residueHarvestDist;
        }

        // Deadwood input (d > 10cm) in the cell, tC/ha, in the old forest
        // Litter input (d <= 10cm) in the cell, tC/ha, in the old forest
        // return pair<deadWoodPoolIn, litterPoolIn>
        [[nodiscard]] static pair<double, double>
        deadWoodPoolDisturbanceCalcFunc(const DataStruct &plot, const AgeStruct &cohort,
                                        const double harvestedWood, // wood harvested in the production forest, m3/ha
                                        const double forestShare, // considered forest share in the cell
                                        const double harvArea, // pure harvested area
                                        const double bmH, // total harvested stem biomass at final felling in the production forest and multifunction forest
                                        const double extractedResidues, // share of extracted logging residues
                                        const double extractedStump) // share of extracted stump
        {
            // Dead wood
//            double deadWood_fc_unmng = 0;       // deadwood (stem) of trees dying in each age cohort and at the end of lifespan in the old forests not used for intensive wood production
            double deadWood_fc_mng = 0;         // deadwood (stem) of trees dying in each age cohort and at the end of lifespan in the old forests used for intensive wood production
//
//            double deadWood_mort = 0;           // deadwood (stem) due to mortality in old forest that is not classified as potentially merchantable
//            double litter_mort = 0;             // litter (stem) due to mortality in old forest that is not classified as potentially merchantable

//            double deadWoodBranches_mort = 0;
//            double litterBranches_mort = 0;
//            double deadWoodStump_mort = 0;
//            double litterStump_mort = 0;

            double areaRatio = harvArea > 0 ? cohort.getArea() / harvArea : 0;

            double fcDBH = cohort.getDBHSlashCut();
            double fcH = cohort.getHSlashCut();
            double fcGS = bmH * areaRatio;

            double deadWoodStump_fc = areaRatio > 0 ? plot.DBHHToStump(fcDBH, fcH, fcGS) / areaRatio : 0;
//            double deadWoodStump_th = 0;

            // tC/ha in the cell
            double deadWoodBranches_fc = 0.3 * bmH * (plot.BEF(fcGS) - 1);
            double litterBranches_fc = deadWoodBranches_fc / 0.3 * 0.7;
//            double litterBranches_th = 0;

//            double deadWoodBranches_th = 0;

            const double harvLossShare = 0.25;

            double harvL = max(0., bmH - harvestedWood);
            // deadwood from harvest losses in intensively managed old forest
            double deadwood_hRes_mng = (1 - harvLossShare) * harvL;
            // litter from harvest losses in intensively managed old forest
            double litter_hRes_mng = harvLossShare * harvL;

//            double thDBHold = 0;
//            double thHold = 0;

            // branches from harvested trees
            double tmp_share = 1 - extractedResidues * plot.residuesUseShare;

            double deadWoodPoolIn = (deadWoodBranches_fc + deadwood_hRes_mng) * tmp_share +
                                    deadWoodStump_fc * (1 - extractedStump * plot.residuesUseShare) + deadWood_fc_mng;

            double litterPoolIn = (litterBranches_fc + litter_hRes_mng) * tmp_share;

            return {deadWoodPoolIn, litterPoolIn};
        }

        void findMaxWoodAndCNPV(const uint16_t year) {
            for (const auto &plot: appPlots)
                // Don't do initial disturbance for countries which cannot produce demanded amount of wood due to lack of forest resources or very high deforestation
                if (countriesNoFmCPol.contains(plot.country) && toAdjust.contains(plot.country) && !plot.protect)
                    if (appMaiForest(plot.x, plot.y) > 0 && appDats[plot.asID].OForestShare > 0) {

                        if (appManagedForest(plot.x, plot.y) > 0) {
                            double rotMAI = species[plot.speciesType].getTOptT(
                                    appMaiForest(plot.x, plot.y), ORT::MAI);
                            double rotMaxBmTh = species[plot.speciesType].getTOptT(
                                    appMaiForest(plot.x, plot.y), ORT::MaxBm);
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
                            double rotMaxBm = species[plot.speciesType].getTOpt(appMaiForest(plot.x, plot.y),
                                                                                ORT::MaxBm);
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
            double harvestO = appCohortsU[plot.asID].cohortRes().getTotalWoodRemoval();
            double harvestO10 = appCohorts10[plot.asID].cohortRes().getTotalWoodRemoval();
            double harvestO30 = appCohorts30[plot.asID].cohortRes().getTotalWoodRemoval();
            double harvestN = appCohortsN[plot.asID].cohortRes().getTotalWoodRemoval();

            double cleanedWoodUseCurrent =
                    cleanedWoodUse[plot.country] + appDats[plot.asID].harvestEfficiencyMultifunction;

            double cleanedWoodUseCurrent10 = appForest10Policy ? 0 : cleanedWoodUseCurrent;
            if (appForest10Policy && appMultifunction10)
                cleanedWoodUseCurrent10 = cleanWoodUseShare10 * cleanedWoodUse[plot.country];

            double cleanedWoodUseCurrent30 = appForest30Policy ? 0 : cleanedWoodUseCurrent;
            if (appForest30Policy && appMultifunction30)
                cleanedWoodUseCurrent30 = cleanWoodUseShare30 * cleanedWoodUse[plot.country];

            double forestShareApplied =
                    appDats[plot.asID].U.forestShare - appDats[plot.asID].deforestShare;

            // Total current harvested wood in the cell, m3
            double newHarvestTmp =
                    (harvestO * forestShareApplied + harvestN * appDats[plot.asID].N.forestShare) *
                    appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                    appDats[plot.asID].deforWoodTotM3;

            if (appThinningForest(plot.x, plot.y) <= 0)
                newHarvestTmp *= cleanedWoodUseCurrent;

            double newHarvestTmp10 =
                    harvestO10 * appDats[plot.asID].O10.forestShare * appDats[plot.asID].landAreaHa *
                    plot.fTimber(coef.bYear);

            if (appThinningForest10(plot.x, plot.y) <= 0)
                newHarvestTmp10 *= cleanedWoodUseCurrent10;

            double newHarvestTmp30 =
                    harvestO30 * appDats[plot.asID].O30.forestShare * appDats[plot.asID].landAreaHa *
                    plot.fTimber(coef.bYear);

            if (appThinningForest30(plot.x, plot.y) <= 0)
                newHarvestTmp30 *= cleanedWoodUseCurrent30;

            return {newHarvestTmp, newHarvestTmp10, newHarvestTmp30};
        }

        // Control (calculate) the detailed amount of harvested wood after adjusting forest management.
        // Used together with printHarvestDiagnostics to check how do the adjustment functions work.
        // Not necessary for release.
        // woodHarvestDetailed[country]: 0 - woodHarvestTotal, 1 - UN, 2 - O10, 3 - O30, 4 - salvageLogging
        // returns woodHarvestDetailed, harvestGridLocal
        [[nodiscard]] pair<array<array<double, numberOfCountries>, 5>, DataGrid<double> >
        calculateHarvestDetailed(const bool toAdjustOnly) const {
            // 0 - woodHarvestTotal, 1 - UN, 2 - O10, 3 - O30, 4 - salvageLogging
            array<array<double, numberOfCountries>, 5> woodHarvestDetailed{};
            DataGrid<double> harvestGridLocal{resLatitude};

            for (const auto &plot: appPlots)
                if ((!toAdjustOnly || toAdjust.contains(plot.country)) && !plot.protect) {  // using Karnaugh map
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
            for (const auto i: countriesList) {
                TRACE("i = {}\t\tharvDiff = {}\t\twoodDemand = {}\t\twoodHarvestTotal = {}\t\tUN = {}\t\tO10 = {}\t\tO30 = {}\t\tsalvageLogging = {}",
                      i, harvDiff[i], dms.woodDemand.at(i)(year), woodHarvestDetailed[0][i],
                      woodHarvestDetailed[1][i], woodHarvestDetailed[2][i], woodHarvestDetailed[3][i],
                      woodHarvestDetailed[4][i]);
//                TRACE("harvestCalcPrev = {}\t\tprevUN = {}\t\tprevO10 = {}\t\tprevO30 = {}",
//                      countriesWoodHarvestM3Year.getVal(i, year - 1), countriesWoodHarvestUNM3Year.getVal(i, year - 1),
//                      countriesWoodHarvest10M3Year.getVal(i, year - 1),
//                      countriesWoodHarvest30M3Year.getVal(i, year - 1));
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

            forest30GeneralFM();
            return woodHarvest;
        }

        // TEMPORAL SHORTCUT FOR forest10 and forest30 when there are no the forest10 and forest30 policies
        void forest30GeneralFM() {
            if (!appForest30Policy)  // write separate loop for no forest10 policy
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
            double OForestShare = appDats[plot.asID].OForestShare;

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
            double maiV = appMaiForest(plot.x, plot.y) * plot.fTimber(coef.bYear);
            double rotMAI = species[plot.speciesType].getTOptT(maiV / plot.fTimber(year), ORT::MAI);
            double costsScaling = plot.priceIndex(year) / priceIndexAvgEU27;
            if (plot.country == 69)
                costsScaling *= scaleCostsFactorEs;
            double damageRiscCosts = max(0., (rotation - rotMAI) * damageRiscCostsYear * costsScaling);
            double harvestTmp = 0;
            double harvestedArea = 0;

            double npvSum = 0;
            size_t n = biomassBauScenarios.at(suffix0).size();
            auto maxYear = min(static_cast<uint16_t>(refYear + n * modTimeStep), coef.eYear);
            for (int j = 0; year + j + modTimeStep <= maxYear; j += modTimeStep) {
                auto resTmp = cohortTmp.aging();
                double realAreaO = clamp(cohortTmp.getArea(), 0., 1.);
                double bm = realAreaO > 0 && !zeroC ? cohortTmp.getBm() / realAreaO : 0;

                if (used) {
                    harvestTmp = AgeStruct::cohortRes(realAreaO, resTmp)[0] * plot.fTimber(year);
                    harvestedArea = cohortTmp.getArea(0ull);
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
            double maiV = appMaiForest(plot.x, plot.y) * plot.fTimber(coef.bYear);
            if (harvestedArea > 0) {
                double plantRate = clamp((maiV - 3) / 6., 0., 1.);
                plantingCosts =
                        plantRate * coef.plantingCostsR * harvestedArea * plot.priceIndex(year) / coef.priceIndexE;
            }

            double priceWExt = 0;

            if (harvestedW > 0) {
                double sFor = (1 - appDats[plot.asID].OForestShare) * 9 + 1;
                // MG: use internal G4M wood price
                // MG: Changed to external SawnLogsPrice
                double c4 = (coef.priceTimberMaxR - coef.priceTimberMinR) / 99;
                double c3 = coef.priceTimberMinR - c4;
                priceWExt = (c3 + c4 * plot.sPopDens(year) * sFor) * plot.priceIndex(year) / coef.priceIndexE *
                            dms.woodPrice.at(plot.country)(year) / dms.woodPrice.at(plot.country).data.at(2000) *
                            wpMult;
            }

            // extension to total biomass
            double CBenefit = fdcFlag ? 1.2 * dms.CO2Price.at(plot.country)(year) *
                                        (biomassCur - biomassBauScenarios.at(suffix0)[
                                                (year - refYear) / modTimeStep - 1][plot.asID]) : 0;

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

            for (const auto &plot: appPlots)
                if (toAdjust.contains(plot.country) && !plot.protect)
                    if (appDats[plot.asID].U.forestShare > 0 && appMaiForest(plot.x, plot.y) > 0) {
                        double countryWoodDemand = dms.woodDemand.at(plot.country)(year);

                        double rotationForestTmp = appRotationForest(plot.x, plot.y);
                        int8_t managedForestTmp = appManagedForest(plot.x, plot.y);
                        double thinningForestTmp = appThinningForest(plot.x, plot.y);

                        double rotationForestTmpNew = appCohortsU[plot.asID].getU();
                        double thinningForestTmpNew = appCohortsN[plot.asID].getStockingDegree();

                        if (woodHarvest[plot.country] < (1 - woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) <= 0) {
                                double rotMAI = 0;
                                double rotMaxBmTh = 0;
                                double biomassRotTh2 = 0;

                                if (appDats[plot.asID].U.prevStemBiomass > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0) {

                                    biomassRotTh2 = species[plot.speciesType].getUSdTab(
                                            appDats[plot.asID].OBiomass0, appMaiForest(plot.x, plot.y),
                                            stockingDegree);     // rotation time to get current biomass (with thinning)
                                    rotMAI = species[plot.speciesType].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                ORT::MAI);
                                    rotMaxBmTh = species[plot.speciesType].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                    ORT::MaxBm);
                                } else if (appDats[plot.asID].prevPlantPhytHaBmGr > 0) {

                                    rotMAI = species[plot.speciesType].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                ORT::MAI);
                                    rotMaxBmTh = species[plot.speciesType].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                    ORT::MaxBm);
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

                                double forestShareApplied =
                                        appDats[plot.asID].U.forestShare - appDats[plot.asID].deforestShare;
                                if (!appForest30Policy)
                                    forestShareApplied += appDats[plot.asID].O30.forestShare;

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp = (harvestO * forestShareApplied +
                                                        harvestN * appDats[plot.asID].N.forestShare) *
                                                       appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                       appDats[plot.asID].deforWoodTotM3;
                                double countryHarvestTmp =
                                        woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                double costsScaling = plot.priceIndex.data.at(2000) / priceIndexAvgEU27;
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

                                        if (appDats[plot.asID].constructedRoadsMultifunction < 0 &&
                                            appManageChForest(plot.x, plot.y) == 0)
                                            NPVTmp -= appDats[plot.asID].currentYearRoadInvestment +
                                                      (meanRoadDensityProduction - roadMultifunctional -
                                                       appDats[plot.asID].constructedRoadsMultifunction) *
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
                                        appDats[plot.asID].constructedRoadsMultifunction =
                                                meanRoadDensityProduction - roadMultifunctional;
                                        appDats[plot.asID].harvestEfficiencyMultifunction = 0;

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
                                    if (appDats[plot.asID].constructedRoadsMultifunction < 0 &&
                                        appManageChForest(plot.x, plot.y) == 0)
                                        NPVTmp -= appDats[plot.asID].currentYearRoadInvestment +
                                                  (meanRoadDensityProduction - roadMultifunctional -
                                                   appDats[plot.asID].constructedRoadsMultifunction) *
                                                  forestRoadConstructionCostsEuroM * costsScaling;

                                    if (abs(countryHarvestTmp - countryWoodDemand) < (1 + tolerance * adjustTolerance) *
                                                                                     abs(woodHarvest[plot.country] -
                                                                                         countryWoodDemand) &&
                                        NPVTmp > 0) {
                                        woodHarvest[plot.country] = countryHarvestTmp;
                                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;
                                        appManageChForest(plot.x, plot.y) = 1;

                                        appDats[plot.asID].constructedRoadsMultifunction =
                                                meanRoadDensityProduction - roadMultifunctional;
                                        appDats[plot.asID].harvestEfficiencyMultifunction = 0;

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
                                if (appDats[plot.asID].U.prevStemBiomass > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDats[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMaxBm = species[plot.speciesType].getTOpt(appMaiForest(plot.x, plot.y),
                                                                                 ORT::MaxBm);

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

                                double forestShareApplied =
                                        appDats[plot.asID].U.forestShare - appDats[plot.asID].deforestShare;

                                if (!appForest30Policy)
                                    forestShareApplied += appDats[plot.asID].O30.forestShare;

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp = ((cleanedWoodUse[plot.country] +
                                                         appDats[plot.asID].harvestEfficiencyMultifunction) *
                                                        harvestO * forestShareApplied + (cleanedWoodUse[plot.country] +
                                                                                         appDats[plot.asID].harvestEfficiencyMultifunction) *
                                                                                        harvestN *
                                                                                        appDats[plot.asID].N.forestShare) *
                                                       appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                       appDats[plot.asID].deforWoodTotM3;

                                double countryHarvestTmp =
                                        woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                         rotationForestTmp, thinningForestTmp, rotationForestTmpNew,
                                         thinningForestTmpNew, managedForestTmp, fm_hurdle);
                            }
                        }
                    }

            for (const auto &plot: appPlots)
                if (toAdjust.contains(plot.country) && !plot.protect)
                    if (appDats[plot.asID].U.forestShare > 0) {
                        double countryWoodDemand = dms.woodDemand.at(plot.country)(year);

                        double rotationForestTmp = appRotationForest(plot.x, plot.y);
                        int8_t managedForestTmp = appManagedForest(plot.x, plot.y);
                        double thinningForestTmp = appThinningForest(plot.x, plot.y);

                        double rotationForestTmpNew = appCohortsU[plot.asID].getU();
                        double thinningForestTmpNew = appCohortsN[plot.asID].getStockingDegree();

                        if (woodHarvest[plot.country] > (1 - woodProdTolerance) * countryWoodDemand)
                            if (appManagedForest(plot.x, plot.y) > 0) {
                                double rotMaxBm = 0;
                                if (appDats[plot.asID].U.prevStemBiomass > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDats[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMaxBm = species[plot.speciesType].getTOpt(appMaiForest(plot.x, plot.y),
                                                                                 ORT::MaxBm);

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

                                double forestShareApplied =
                                        appDats[plot.asID].U.forestShare - appDats[plot.asID].deforestShare;

                                if (!appForest30Policy)
                                    forestShareApplied += appDats[plot.asID].O30.forestShare;

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp = ((cleanedWoodUse[plot.country] +
                                                         appDats[plot.asID].harvestEfficiencyMultifunction) *
                                                        harvestO * forestShareApplied + (cleanedWoodUse[plot.country] +
                                                                                         appDats[plot.asID].harvestEfficiencyMultifunction) *
                                                                                        harvestN *
                                                                                        appDats[plot.asID].N.forestShare) *
                                                       appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                       appDats[plot.asID].deforWoodTotM3;

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

            for (const auto &plot: appPlots)
                if (toAdjust.contains(plot.country) && !plot.protect)
                    if (appDats[plot.asID].U.forestShare > 0) {
                        double countryWoodDemand = dms.woodDemand.at(plot.country)(year);

                        double rotationForestTmp = appRotationForest(plot.x, plot.y);

                        if (woodHarvest[plot.country] < (1 - woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) >= 2 && !allMng ||
                                appManagedForest(plot.x, plot.y) > 0 && allMng) {

                                double rotMAI = 0;
                                if (appDats[plot.asID].U.prevStemBiomass > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDats[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMAI = species[plot.speciesType].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                ORT::MAI);

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

                                    double forestShareApplied =
                                            appDats[plot.asID].U.forestShare - appDats[plot.asID].deforestShare;

                                    if (!appForest30Policy)
                                        forestShareApplied += appDats[plot.asID].O30.forestShare;

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp = (harvestO * forestShareApplied +
                                                            harvestN * appDats[plot.asID].N.forestShare) *
                                                           appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                           appDats[plot.asID].deforWoodTotM3;

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
                                if (appDats[plot.asID].U.prevStemBiomass > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDats[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMaxBmTh = species[plot.speciesType].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                    ORT::MaxBm);

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

                                    double forestShareApplied =
                                            appDats[plot.asID].U.forestShare - appDats[plot.asID].deforestShare;

                                    if (!appForest30Policy)
                                        forestShareApplied += appDats[plot.asID].O30.forestShare;

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp = (harvestO * forestShareApplied +
                                                            harvestN * appDats[plot.asID].N.forestShare) *
                                                           appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                           appDats[plot.asID].deforWoodTotM3;

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
                for (const auto &plot: appPlots)  // additionally filtered by countriesList (see filter plots)
                    if (toAdjust.contains(plot.country) && !plot.protect &&
                        plot.speciesType == Species::Spruce)
                        if (salvageHarvest[yearSalvage] <
                            (1 - woodProdTolerance) * salvageLoggingTotal[yearSalvage] * 1e6) {
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
                            if (appDats[plot.asID].OForestShare > 0)
                                resO = appCohortsU[plot.asID].cohortRes();

                            CohortRes resN;
                            if (appDats[plot.asID].N.forestShare > 0)
                                resN = appCohortsN[plot.asID].cohortRes();

                            double forestShareApplied =
                                    appDats[plot.asID].OForestShare - appDats[plot.asID].deforestShare;

                            // Total current harvested wood in the cell, m3
                            double newHarvestTmp = (resO.getTotalWoodRemoval() * forestShareApplied +
                                                    resN.getTotalWoodRemoval() * appDats[plot.asID].N.forestShare) *
                                                   appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear);

                            woodHarvest[plot.country] += newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                            appHarvestGrid(plot.x, plot.y) = newHarvestTmp;

                            // total salvage felling in the cell m3
                            double salvageFelled = (resO.getTotalHarvestedBiomass() * forestShareApplied +
                                                    resN.getTotalHarvestedBiomass() *
                                                    appDats[plot.asID].N.forestShare) *
                                                   appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear);
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

            for (const auto i: countriesList)
                if ((!toAdjustOnly || toAdjust.contains(i) && !countriesNoFmCPol.contains(i)) &&
                    dms.woodDemand.at(i)(year) > 0)
                    harvDiff[i] = abs(woodHarvest[i] / dms.woodDemand.at(i)(year) - 1);

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

            for (const auto &plot: appPlots)
                if (toAdjust.contains(plot.country) && !plot.protect)
                    if (plot.country == 57 && !infested.contains(plot.asID)) {
                        double countryWoodDemand = dms.woodDemand.at(plot.country)(year);

                        double rotationForestTmp = appRotationForest(plot.x, plot.y);

                        if (woodHarvest[plot.country] < (1 - woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) > 0) {

                                double rotMAI = 0;
                                if (appDats[plot.asID].U.prevStemBiomass > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDats[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMAI = species[plot.speciesType].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                ORT::MAI);

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

                                    double forestShareApplied =
                                            appDats[plot.asID].OForestShare - appDats[plot.asID].deforestShare;

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp = (harvestO * forestShareApplied +
                                                            harvestN * appDats[plot.asID].N.forestShare) *
                                                           appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                           appDats[plot.asID].deforWoodTotM3;

                                    double countryHarvestTmp =
                                            woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                    CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                             rotationForestTmp, NPVTmp0, true);
                                }
                            }
                        } else if (woodHarvest[plot.country] > (1 + woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) > 0) {

                                double rotMaxBmTh = 0;
                                if (appDats[plot.asID].U.prevStemBiomass > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDats[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMaxBmTh = species[plot.speciesType].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                    ORT::MaxBm);

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

                                    double forestShareApplied =
                                            appDats[plot.asID].OForestShare - appDats[plot.asID].deforestShare;

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp = (harvestO * forestShareApplied +
                                                            harvestN * appDats[plot.asID].N.forestShare) *
                                                           appDats[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                           appDats[plot.asID].deforWoodTotM3;

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

            for (const auto &plot: appPlots)
                if (!plot.protect && plot.residuesUseShare > 0) {
                    double cleanedWoodUseCurrent = cleanedWoodUse[plot.country];
                    double cleanedWoodUseCurrent10 =
                            cleanedWoodUse[plot.country] + appDats[plot.asID].harvestEfficiencyMultifunction;
                    double cleanedWoodUseCurrent30 = cleanedWoodUseCurrent10;
                    if (appForest10Policy)
                        cleanedWoodUseCurrent10 *= cleanWoodUseShare10;
                    if (appForest30Policy)
                        cleanedWoodUseCurrent30 *= cleanWoodUseShare30;

                    auto cohortResAll = appDats[plot.asID].U.forestShare > 0 ? appCohortsU[plot.asID].cohortRes()
                                                                             : CohortRes{};
                    auto cohortResNew =
                            appDats[plot.asID].N.forestShare > 0 ? appCohortsN[plot.asID].cohortRes()
                                                                 : CohortRes{};
                    auto cohortRes30 =
                            appDats[plot.asID].O30.forestShare > 0 ? appCohorts30[plot.asID].cohortRes()
                                                                   : CohortRes{};

                    // harvRes_fcO = ((sawnW + restW) * (BEF(int(iter->SPECIESTYPE[byear]) - 1, (bmH / harvAreaO * realAreaO), iter->FTIMBER[byear]) - 1) + (bmH - (sawnW + restW)));
                    double harvRes_fcO = cohortResAll.if_fc() ? cohortResAll.getFinalCutWood() *
                                                                (plot.BEF(cohortResAll.getHarvestGS()) - 2) +
                                                                cohortResAll.bmH : 0;
                    double harvRes_thO = cohortResAll.if_th() ? cohortResAll.getThinnedWood() *
                                                                (plot.BEF(appDats[plot.asID].U.prevStemBiomass) -
                                                                 2) +
                                                                cohortResAll.bmTh : 0;
                    double harvRes_fcN = cohortResNew.if_fc() ? cohortResNew.getFinalCutWood() *
                                                                (plot.BEF(cohortResNew.getHarvestGS()) - 2) +
                                                                cohortResNew.bmH : 0;
                    // harvRes_thN = ((sawnThWnew + restThWnew) * (((PlantPhytHaBmGrBef - PlantPhytHaBmGr) / PlantPhytHaBmGr) - 1) + (bmThnew - (sawnThWnew + restThWnew)));
                    // For the forest planted after 2000 we calculate above-ground biomass, PlantPhytHaBmGrBef,
                    // for each age cohort in calc
                    double harvRes_thN = cohortResNew.if_th() ? cohortResNew.getThinnedWood() *
                                                                (appDats[plot.asID].prevPlantPhytHaBmGrBef /
                                                                 appDats[plot.asID].prevPlantPhytHaBmGr - 3) +
                                                                cohortResNew.bmTh : 0;
                    double harvRes_fc30 = cohortRes30.if_fc() ? cohortRes30.getFinalCutWood() *
                                                                (plot.BEF(cohortRes30.getHarvestGS()) - 2) +
                                                                cohortRes30.bmH : 0;
                    double harvRes_th30 = cohortRes30.if_th() ? cohortRes30.getThinnedWood() *
                                                                (plot.BEF(appDats[plot.asID].U.prevStemBiomass) -
                                                                 2) +
                                                                cohortRes30.bmTh : 0;

                    double harvRes_scO = 0, harvRes_scO_notTaken = 0;
                    double harvRes_scN = 0, harvRes_scN_notTaken = 0;
                    double harvRes_sc30 = 0, harvRes_sc30_notTaken = 0;

                    if (appThinningForest(plot.x, plot.y) < 0) {
                        tie(harvRes_scO, harvRes_scO_notTaken)
                                = cohortResAll.harvResiduesSanitaryFellings(plot, cleanedWoodUseCurrent,
                                                                            plot.BEF(
                                                                                    appDats[plot.asID].U.prevStemBiomass));
                        tie(harvRes_scN, harvRes_scN_notTaken)
                                = cohortResNew.harvResiduesSanitaryFellings(plot, cleanedWoodUseCurrent,
                                                                            appDats[plot.asID].prevPlantPhytHaBmGrBef /
                                                                            appDats[plot.asID].prevPlantPhytHaBmGr -
                                                                            1);
                    }
                    if (appThinningForest30(plot.x, plot.y) < 0 && residueUse30 > 0)
                        tie(harvRes_sc30, harvRes_sc30_notTaken)
                                = cohortResAll.harvResiduesSanitaryFellings(plot, cleanedWoodUseCurrent30,
                                                                            plot.BEF(
                                                                                    appDats[plot.asID].O30.prevStemBiomass));

                    double stump = 0;       // stumps of old forest, tC/ha
                    double stump_new = 0;   // stumps of new forest, tC/ha
                    double stump30 = 0;     // stumps of old forest, tC/ha

                    if (stumpHarvCountrySpecies.contains({plot.country, plot.speciesType})) {
                        if (appThinningForest(plot.x, plot.y) > 0) {
                            // calculate amount of stumps + big roots for final felling, tC/ha
                            if (cohortResAll.positiveAreas())
                                stump = plot.DBHHToStump(cohortResAll.DBH, cohortResAll.H, cohortResAll.getHarvestGS());
                            if (cohortResNew.positiveAreas())
                                stump_new = plot.DBHHToStump(cohortResNew.DBH, cohortResNew.H,
                                                             cohortResNew.getHarvestGS());
                        }
                        if (appThinningForest30(plot.x, plot.y) > 0 && residueUse30 > 0) {
                            // calculate amount of stumps + big roots for final felling, tC/ha
                            if (cohortRes30.positiveAreas())
                                stump30 = plot.DBHHToStump(cohortRes30.DBH, cohortRes30.H, cohortRes30.getHarvestGS());
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

                    hr.U.residuesSuit1_perHa = harvRes_fcO + harvRes_thO + harvRes_fcN + harvRes_thN;
                    hr.U.residuesSuit2_perHa = stump + stump_new;
                    hr.U.residuesSuit3_perHa = harvRes_scO + harvRes_scN;
                    hr.U.residuesSuit4_notTaken_perHa = harvRes_scO_notTaken + harvRes_scN_notTaken;

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

                    hr.U.residuesSuit1 = (harvRes_fcO + harvRes_thO) * appDats[plot.asID].U.forestShare +
                                         (harvRes_fcN + harvRes_thN) * appDats[plot.asID].N.forestShare;
                    hr.U.residuesSuit2 = stump * appDats[plot.asID].U.forestShare +
                                         stump_new * appDats[plot.asID].N.forestShare;
                    hr.U.residuesSuit3 = harvRes_scO * appDats[plot.asID].U.forestShare +
                                         harvRes_scN * appDats[plot.asID].N.forestShare;
                    hr.U.residuesSuit4_notTaken = harvRes_scO_notTaken * appDats[plot.asID].U.forestShare +
                                                  harvRes_scN_notTaken * appDats[plot.asID].N.forestShare;

                    double useShareArea = plot.residuesUseShare * appDats[plot.asID].landAreaHa;
                    hr.U.residuesSuit1 *= useShareArea;
                    hr.U.residuesSuit2 *= useShareArea;
                    hr.U.residuesSuit3 *= useShareArea;
                    hr.U.residuesSuit4_notTaken *= useShareArea;

                    double shareArea30 = appDats[plot.asID].O30.forestShare * appDats[plot.asID].landAreaHa;
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

                    hr.em_harvRes_fcO = harvRes_fcO > 0;
                    hr.em_harvRes_thO = harvRes_thO > 0;
                    hr.em_harvRes_fcN = harvRes_fcN > 0;
                    hr.em_harvRes_thN = harvRes_thN > 0;
                    hr.em_harvRes_scO = harvRes_scO > 0;
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

                            double em_fo = 0, em_fn = 0;
                            if (hr.em_harvRes_fcO || hr.em_harvRes_thO)
                                em_fo = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                        appDats[hr.asID].U.forestShare * appDats[hr.asID].landAreaHa;
                            if (hr.em_harvRes_fcN || hr.em_harvRes_thN)
                                em_fn = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                        appDats[hr.asID].N.forestShare * appDats[hr.asID].landAreaHa;

                            hr.emissionsSuit1 = lastShare * (em_fo + em_fn);
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

                                double em_fo = 0;
                                if (hr.em_harvRes_fc30 || hr.em_harvRes_th30)
                                    em_fo = hr.lerpERUS(emissionsResUseSust1, hr.O30.timeUseSust1) *
                                            appDats[hr.asID].O30.forestShare * appDats[hr.asID].landAreaHa;

                                hr.emissionsSuit1 = lastShare * em_fo;
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

                            double em_fo = 0, em_fn = 0, em_fo_sust1 = 0, em_fn_sust1 = 0;
                            if (hr.em_harvRes_fcO || hr.em_harvRes_thO) {
                                em_fo = hr.lerpERUS(emissionsResUseSust2, hr.U.timeUseSust2) *
                                        appDats[hr.asID].U.forestShare * appDats[hr.asID].landAreaHa;
                                em_fo_sust1 = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust2) *
                                              appDats[hr.asID].U.forestShare * appDats[hr.asID].landAreaHa;
                            }
                            if (hr.em_harvRes_fcN || hr.em_harvRes_thN) {
                                em_fn = hr.lerpERUS(emissionsResUseSust2, hr.U.timeUseSust2) *
                                        appDats[hr.asID].N.forestShare * appDats[hr.asID].landAreaHa;
                                em_fn_sust1 = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust2) *
                                              appDats[hr.asID].N.forestShare * appDats[hr.asID].landAreaHa;
                            }

                            hr.emissionsSuit2 = lastShare * (em_fo + em_fn - em_fo_sust1 - em_fn_sust1);
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

                                double em_fo30 = 0, em_fo_sust1_30 = 0;
                                if (hr.em_harvRes_fc30 || hr.em_harvRes_th30) {
                                    em_fo30 = hr.lerpERUS(emissionsResUseSust2, hr.O30.timeUseSust2) *
                                              appDats[hr.asID].O30.forestShare * appDats[hr.asID].landAreaHa;
                                    em_fo_sust1_30 = hr.lerpERUS(emissionsResUseSust1, hr.O30.timeUseSust2) *
                                                     appDats[hr.asID].O30.forestShare *
                                                     appDats[hr.asID].landAreaHa;
                                }

                                hr.emissionsSuit2 = lastShare * (em_fo30 - em_fo_sust1_30);
                                harvestResiduesSoilEmissions[country] += hr.emissionsSuit2;
                                hr.useSuit2 = lastShare;
                            }
                        }

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

                            double em_sco = 0, em_scn = 0;
                            if (hr.em_harvRes_scO)
                                em_sco = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                         appDats[hr.asID].U.forestShare * appDats[hr.asID].landAreaHa;
                            if (hr.em_harvRes_scN)
                                em_scn = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                         appDats[hr.asID].N.forestShare * appDats[hr.asID].landAreaHa;

                            double cleanedWoodUseCurrent = hr.protect ? 0 : cleanedWoodUse[country];
                            em_sco *= cleanedWoodUseCurrent;
                            em_scn *= cleanedWoodUseCurrent;

                            hr.emissionsSuit3 = lastShare * (em_sco + em_scn);
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

                                double em_sco = 0;
                                if (hr.em_harvRes_scO)
                                    em_sco = hr.lerpERUS(emissionsResUseSust1, hr.O30.timeUseSust1) *
                                             appDats[hr.asID].O30.forestShare * appDats[hr.asID].landAreaHa;

                                double cleanedWoodUseCurrent30 = hr.protect ? 0 : cleanedWoodUse[country] +
                                                                                  appDats[hr.asID].harvestEfficiencyMultifunction;
                                em_sco *= cleanedWoodUseCurrent30;

                                hr.emissionsSuit3 = lastShare * em_sco;
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

                            double em_sco_notTaken = 0, em_scn_notTaken = 0;
                            if (hr.em_harvRes_scO)
                                em_sco_notTaken = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                                  appDats[hr.asID].U.forestShare * appDats[hr.asID].landAreaHa;
                            if (hr.em_harvRes_scN)
                                em_scn_notTaken = hr.lerpERUS(emissionsResUseSust1, hr.U.timeUseSust1) *
                                                  appDats[hr.asID].N.forestShare * appDats[hr.asID].landAreaHa;

                            double cleanedWoodUseCurrent = hr.protect ? 0 : cleanedWoodUse[country];
                            em_sco_notTaken *= 1 - cleanedWoodUseCurrent;
                            em_scn_notTaken *= 1 - cleanedWoodUseCurrent;

                            hr.emissionsSuit4_notTaken = lastShare * (em_sco_notTaken + em_scn_notTaken);
                            harvestResiduesSoilEmissions[country] += hr.emissionsSuit4_notTaken;
                            hr.useSuit4 = lastShare;
                        }
                    }

                    // whole (above-ground) trees from multifunctional forests that die but are not harvested
                    // forest 30
                    for (auto &hr: commonHarvestResiduesCountry.at(country)) {
                        if (residueHarvest[country] >= dms.residuesDemand.at(country)(year))
                            break;

                        if (hr.O30.usedForest) {
                            double resDeficit = dms.residuesDemand.at(country)(year) - residueHarvest[country];
                            double lastShare = min(resDeficit / hr.O30.residuesSuit4_notTaken, 1.);
                            residueHarvest[country] += lastShare * hr.O30.residuesSuit4_notTaken;
                            appDats[hr.asID].O30.extractedCleaned = lastShare;
                            hr.O30.timeUseSust1++;

                            double em_sco_notTaken = 0, em_scn_notTaken = 0;
                            if (hr.em_harvRes_scO)
                                em_sco_notTaken = hr.lerpERUS(emissionsResUseSust1, hr.O30.timeUseSust1) *
                                                  appDats[hr.asID].O30.forestShare * appDats[hr.asID].landAreaHa;

                            double cleanedWoodUseCurrent30 = hr.protect ? 0 : cleanedWoodUse[country] +
                                                                              appDats[hr.asID].harvestEfficiencyMultifunction;
                            em_sco_notTaken *= 1 - cleanedWoodUseCurrent30;

                            hr.emissionsSuit4_notTaken = lastShare * em_sco_notTaken;
                            harvestResiduesSoilEmissions[country] += hr.emissionsSuit4_notTaken;
                            hr.useSuit4 = lastShare;
                        }
                    }
                }
                // TODO fTimber by cells!
                double fTimber = commonHarvestResiduesCountry.at(country)[0].fTimber;
                countriesResiduesDemand_m3.setVal(country, year, dms.residuesDemand.at(country)(year) * fTimber);
                countriesResiduesExtract_m3.inc(country, year, residueHarvest[country] * fTimber);
                countriesResExtSoilEm_MtCO2Year.setVal(country, year, harvestResiduesSoilEmissions[country] * 1e-6);
            }
            return harvestResiduesSoilEmissions;
        }
    };
}

#endif
