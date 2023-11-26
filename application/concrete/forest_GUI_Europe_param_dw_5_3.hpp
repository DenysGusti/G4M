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

#include "../../structs/lw_price.hpp"
#include "../../structs/harvest_residues.hpp"

#include "../../misc/concrete/ffipolm.hpp"

using namespace std;

using namespace numbers;
using namespace g4m::structs;
using namespace g4m::misc::concrete;
using namespace g4m::increment;
using namespace g4m::diagnostics;
using namespace g4m::application::abstract;
using namespace g4m::StartData;

namespace g4m::application::concrete {

    class Forest_GUI_Europe_param_dw_5_3 : public Application {
    public:
        explicit Forest_GUI_Europe_param_dw_5_3(const span<const string> args_) : Application{args_} {
            Log::Init(appName);
            INFO("Scenario to read in & GL: {}", full_scenario);
            mergeObligatoryDatamaps();
            mergeOptionalDatamaps();
            mergeOptionalSimuIds();
            correctBelgium();
            initCO2Price();
            initMaiClimateShifters();
            applyMAIClimateShifters();
            modifyDisturbances();
            initGlobiomLandLocal();
            initManagedForestLocal();
            toAdjust.reserve(256);
            countriesNoFmCPol.reserve(256);
            doneList.reserve(256);
            if (fmPol && !binFilesOnDisk && inputPriceC == 0)
                signalZeroCtoMainScenarios.emplace(suffix0, 0);
        }

        // start calculations
        void Run() override {
            INFO("Application {} is running", appName);
            // loop by years
            for (uint16_t year = coef.bYear; year <= coef.eYear; ++year) {
                INFO("Processing year {}", year);
                uint16_t age = year - coef.bYear;

                if (year == forPolicyYearBioClima) {
                    appForest10_policy = commonForest10_policyKey;
                    appForest30_policy = commonForest30_policyKey;
                    appMultifunction10 = commonMultifunction10Key;
                    appMultifunction30 = commonMultifunction30Key;
                }

                double priceC = 0;
                // CO2 price different for regions: set priceC to negative value then the price is defined in the function carbonPriceRegion
                if (year > refYear && inputPriceC < 0)
                    priceC = -1;
                else if (year > refYear && inputPriceC > 0)
                    priceC = inputPriceC * deflator * 44. / 12.;  // * 44. / 12. if input price is $/tCO2

                if (MAIClimateShift && year > 2020)
                    for (const auto &plot: appPlots) {
                        // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
                        // Max mean annual increment of New forest (with uniform age structure and managed with rotation length maximizing MAI)
                        double MAI = max(0.,
                                         appDat_all[plot.asID].forestShare0 > 0 ?
                                                 MAI = plot.MAIE(year) : MAI = plot.MAIN(year));

                        appMaiForest(plot.x, plot.y) = MAI;

                        appCohort_all[plot.asID].setMaiAndAvgMai(MAI);
                        appCohort10_all[plot.asID].setMaiAndAvgMai(MAI);
                        appCohort30_all[plot.asID].setMaiAndAvgMai(MAI);
                        appCohort_primary_all[plot.asID].setMaiAndAvgMai(MAI);
                        appNewCohort_all[plot.asID].setMaiAndAvgMai(MAI);
                    }

                INFO("Adjusting FM...");
                adjustManagedForest(year, priceC);
            }

            if (fmPol && !binFilesOnDisk && inputPriceC == 0)
                signalZeroCtoMainScenarios.at(suffix0).release();
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

        datamapType appLandPrice;
        datamapType appWoodPrice;
        datamapType appWoodDemand;
        datamapType appResiduesDemand;

        datamapType appCO2Price;

        datamapType appGlobiomAfforMaxCountry;
        datamapType appGlobiomLandCountry;

        // wood from outside forests in Belgium to cover the inconsistency between FAOSTAT removals and Forest Europe increment and felling
        Ipol<double> woodSupplement;

        simuIdType appDisturbWind = commonDisturbWind;
        simuIdType appDisturbFire = commonDisturbFire;
        simuIdType appDisturbBiotic = commonDisturbBiotic;

        simuIdType appDisturbWindExtreme = commonDisturbWindExtreme;
        simuIdType appDisturbFireExtreme = commonDisturbFireExtreme;
        simuIdType appDisturbBioticExtreme = commonDisturbBioticExtreme;

        simuIdType appMaiClimateShifters;

        simuIdType appGlobiomAfforMax;
        simuIdType appGlobiomLand;

        // Initializing forest cover array by gridcells
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

        vector<AgeStruct> appCohort_all = commonCohort_all;
        vector<AgeStruct> appNewCohort_all = commonNewCohort_all;
        vector<AgeStruct> appCohort10_all = commonCohort10_all;
        vector<AgeStruct> appCohort30_all = commonCohort30_all;
        vector<AgeStruct> appCohort_primary_all = commonCohort_primary_all;

        vector<Dat> appDat_all = commonDat_all;

        bool appForest10_policy = commonForest10_policy;
        bool appForest30_policy = commonForest30_policy;
        bool appMultifunction10 = commonMultifunction10;
        bool appMultifunction30 = commonMultifunction30;

        array<double, numberOfCountries> harvestResiduesSoilEmissions{};    // Soil loss emissions resulting from extraction of harvest residues, MtCO2/year
        array<double, numberOfCountries> residueHarvest{};                  // Extraction of harvest residues, tC
        array<double, numberOfCountries> harvDiff{};

        CountryData countriesResiduesExtract_m3;  // sustainably extracted harvest residuals, m3

        unordered_set<uint8_t> toAdjust;            // country where FM to be adjusted
        unordered_set<uint8_t> countriesNoFmCPol;   // List of countries where it's impossible to match demanded wood production in current year
        unordered_set<uint8_t> doneList;            // countries already adjusted

        CountryData CountryRegMaxHarvest;
        CountryData CountryRegWoodProd;
        CountryData countryRegWoodHarvestDfM3Year;

        double appCoefPriceC = coef.priceC;

        datamapType mergeDatamap(const heterDatamapScenariosType &datamapScenarios, const string_view message) {
            // Swiss project 21.04.2022, Nicklas Forsell
            datamapType datamapDest = datamapScenarios.at(full_scenario);
            datamapType histDatamap = datamapScenarios.at(s_bauScenario);

            for (auto &[id, ipol]: datamapDest)
                ipol.data.merge(histDatamap.at(id).data);

            TRACE("Merged {}:", message);
            for (const auto &[id, ipol]: datamapDest)
                TRACE("{}\n{}", idCountryGLOBIOM.at(id), ipol.str());

            TRACE("Obsolete {}:", message);
            for (const auto &[id, ipol]: histDatamap)
                TRACE("{}\n{}", idCountryGLOBIOM.at(id), ipol.str());

            return datamapDest;
        }

        datamapType
        mergeObligatoryDatamap(const heterDatamapScenariosType &datamapScenarios, const string_view message) {
            if (!datamapScenarios.contains(full_scenario)) {
                FATAL("{} is not filled in, check scenarios!!!, full_scenario = {}", message, full_scenario);
                throw runtime_error{"no scenario in datamapScenarios"};
            }

            return mergeDatamap(datamapScenarios, message);
        }

        datamapType mergeOptionalDatamap(const heterDatamapScenariosType &datamapScenarios, const string_view message) {
            if (!datamapScenarios.contains(full_scenario)) {
                WARN("{} is not filled in, check scenarios!!!, full_scenario = {}", message, full_scenario);
                return {};
            }

            return mergeDatamap(datamapScenarios, message);
        }

        simuIdType mergeOptionalSimuId(const heterSimuIdScenariosType &simuIdScenarios, const string_view message) {
            if (!simuIdScenarios.contains(full_scenario)) {
                WARN("{} is not filled in, check scenarios!!!, full_scenario = {}", message, full_scenario);
                return {};
            }

            simuIdType simuIdDest = simuIdScenarios.at(full_scenario);
            simuIdType histSimuId = simuIdScenarios.at(s_bauScenario);

            for (auto &[id, ipol]: simuIdDest)
                ipol.data.merge(histSimuId.at(id).data);

            TRACE("Merged {}:", message);
            for (const auto &[id, ipol]: simuIdDest)
                TRACE("{}\n{}", idCountryGLOBIOM[id], ipol.str());

            TRACE("Obsolete {}:", message);
            for (const auto &[id, ipol]: histSimuId)
                TRACE("{}\n{}", idCountryGLOBIOM[id], ipol.str());

            return simuIdDest;
        }

        void mergeObligatoryDatamaps() {
            appLandPrice = mergeObligatoryDatamap(landPriceScenarios, "Land Price");
            appWoodPrice = mergeObligatoryDatamap(woodPriceScenarios, "Wood Price");
            appWoodDemand = mergeObligatoryDatamap(woodDemandScenarios, "Wood Demand");
            appResiduesDemand = mergeObligatoryDatamap(residuesDemandScenarios, "Residues Demand");
        }

        void mergeOptionalDatamaps() {
            appGlobiomAfforMaxCountry = mergeOptionalDatamap(globiomAfforMaxCountryScenarios,
                                                             "GLOBIOM Affor Max Country");
            appGlobiomLandCountry = mergeOptionalDatamap(globiomLandCountryScenarios, "GLOBIOM Land Country");
        }

        void mergeOptionalSimuIds() {
            appGlobiomAfforMax = mergeOptionalSimuId(globiomAfforMaxScenarios, "GLOBIOM Affor Max Country");
            appGlobiomLand = mergeOptionalSimuId(globiomLandScenarios, "GLOBIOM Land Country");
        }

        void correctBelgium() noexcept {
            woodSupplement = appWoodDemand.at(20);
            // 05.04.2023: we assume that 14% of round-wood comes from outside forest
            // the Forest Europe net increment and felling values are less than FAOSTAT round-wood
            const double forestWood = 0.86;
            appWoodDemand[20] *= forestWood;  // Belgium
            woodSupplement *= 1 - forestWood;
        }

        void initCO2Price() {
            if (inputPriceC == 0) {
                INFO("CO2PriceScenarios is not used, inputPriceC != 0");
                return;
            }

            if (!CO2PriceScenarios.contains(full_scenario)) {
                FATAL("CO2PriceScenarios is not filled in, check scenarios!!!, full_scenario = {}", full_scenario);
                throw runtime_error{"no scenario in CO2PriceScenarios"};
            }

            appCO2Price = CO2PriceScenarios.at(full_scenario);
        }

        void initMaiClimateShifters() {
            if (!maiClimateShiftersScenarios.contains(c_scenario[1])) {
                WARN("maiClimateShifters doesn't contain c_scenario[1]: {}", c_scenario[1]);
                return;
            }

            appMaiClimateShifters = maiClimateShiftersScenarios.at(c_scenario[1]);
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

                if (appMaiClimateShifters.contains(plot.simuID))
                    for (const auto [year, value]: appMaiClimateShifters.at(plot.simuID).data)
                        if (year >= 2030) {
                            plot.MAIE.data[year] = maie * value;
                            plot.MAIN.data[year] = main * value;
                            plot.NPP.data[year] = npp * value;
                        }
            }
        }

        // Make projected forest damage sensitive to the climate change scenario
        void modifyDisturbances() noexcept {
            if (!disturbanceClimateSensitive) {
                INFO("disturbanceClimateSensitive is turned off");
                return;
            }

            if (c_scenario[1].contains("7p0")) {
                for (auto &[id, ipol]: appDisturbFire)
                    ipol *= 2;
                for (auto &[id, ipol]: appDisturbBiotic)
                    ipol *= 2;
            } else if (c_scenario[1].contains("8p5")) {
                for (auto &[id, ipol]: appDisturbFire)
                    ipol *= 2.5;
                for (auto &[id, ipol]: appDisturbBiotic)
                    ipol *= 2.5;
            } else
                INFO("c_scenario[1] ({}) doesn't contain \"7p0\" or \"8p5\"", c_scenario[1]);
        }

        // Synchronize land use with the GLOBIOM model
        void initGlobiomLandLocal() noexcept {
            // file_globiom_LC_correction is not implemented
            double sumGrLnd_protect = 0;

            for (auto &plot: appPlots)
                if (plot.protect.data.at(2000) == 0) {

                    // we leave the previous values if in current dataset this cell is absent
                    if (!appGlobiomAfforMax[plot.simuID].data.empty())
                        for (const auto &[year, value]: appGlobiomAfforMax[plot.simuID].data)
                            if (year > 2000)
                                plot.afforMax.data[year] = max(0., value + plot.natLnd_correction);

                    // we leave the previous values if in current dataset this cell is absent
                    if (!appGlobiomLand[plot.simuID].data.empty())
                        for (const auto &[year, value]: appGlobiomLand[plot.simuID].data)
                            plot.GLOBIOM_reserved.data[year] = max(0., value + plot.GL_correction);

                    else if (!appGlobiomLandCountry[plot.country].data.empty() &&
                             !appGlobiomAfforMaxCountry[plot.country].data.empty()) {

                        if (protectedNatLnd && plot.grLnd_protect > 0) {
                            plot.afforMax.data[2000] = max(0., plot.afforMax.data.at(2000) + plot.grLnd_protect);
                            sumGrLnd_protect += plot.grLnd_protect;
                        }

                        for (auto &[year, GL]: plot.GLOBIOM_reserved.data)
                            if (year > 2000 && GL > 0) {
                                double &natLnd = plot.afforMax.data[year];  // no interpolation, I suppose data is already there
                                double dGL = (appGlobiomLandCountry[plot.country].data.at(year) -
                                              appGlobiomLandCountry[plot.country](year - modTimeStep)) /
                                             countryLandArea[plot.country];
                                dGL = clamp(dGL, -GL, natLnd);
                                GL = max(0., GL + dGL);
                                natLnd = max(0., natLnd - dGL);
                            }
                    }
                }

            if (protectedNatLnd && sumGrLnd_protect <= 0)
                ERROR("GRLND_PROTECT is 0. Check spatial input data (plots)!!!");
        }

        void initManagedForestLocal() noexcept {
            for (auto &plot: appPlots)
                if (simuIdDfor.contains(plot.simuID))
                    for (auto &[year, value]: plot.GLOBIOM_reserved.data)
                        if (year > 2000)
                            value -= simuIdDfor[plot.simuID];
        }

        // Adjust forest management in each grid cell to harvest the demanded amount of wood on a country scale every
        // time step. Disturbance damage and respective salvage logging is implemented here as well.
        void adjustManagedForest(const uint16_t year, const double priceC) {
            double stockingDegree = 1.3;    // test for Belgium
            bool harvControl = true;        // Additional information to control output of the fmCPol module
            bool NPV_postControl_0 = false; // Control of old forest management NPV at 0 C price: Use only for testing!!!!
            bool NPV_postControl = true;    // Control of old forest management NPV at non-zero C price
            uint8_t country_to_check = 0;       // Country code for deep analysis of G4M FM algorithm; 0 - no output of the deep analysis
            toAdjust = countriesList;
            array<double, numberOfCountries> woodHarvest{};

            if (year == coef.bYear)
                for (const auto &plot: appPlots)
                    if (plot.protect.data.at(2000) == 0) {
                        thinningForestNew(plot.x, plot.y) = appNewCohort_all[plot.asID].getStockingDegree();
                        rotationForestNew(plot.x, plot.y) = appNewCohort_all[plot.asID].getU();

                        double rotation = appRotationForest(plot.x, plot.y);
                        double SD = appThinningForest(plot.x, plot.y);

                        appThinningForest30(plot.x, plot.y) = SD;
                        appCohort30_all[plot.asID].setU(rotation);
                        appCohort30_all[plot.asID].setStockingDegree(SD * sdMinCoef);
                        appCohort30_all[plot.asID].setStockingDegreeMax(SD * sdMaxCoef);
                    }

            if (year == coef.bYear + 1)
                for (const auto &plot: appPlots)
                    if (plot.protect.data.at(2000) == 0 && appThinningForest(plot.x, plot.y) < 0)
                        appUnmanaged(plot.x, plot.y) = 1;

            // Apply the forest 10% and forest 30% policies
            if (year > 2020) {
                if (appForest10_policy)
                    switch2Conserved(10, appCohort10_all, appMultifunction10);
                if (appForest30_policy)
                    switch2Conserved(30, appCohort30_all, appMultifunction30);
            }

            // Calculate salvage logging additional to historical
            if (disturbanceTrend && year > 2020 || disturbanceExtreme && year == disturbanceExtremeYear)
                disturbanceDamageHarvest(year);

            // Gradually adjust thinning
            for (const auto &plot: appPlots) {
                // for the most productive forest (MAI = 6) the forest cleaning time is about 20 years,
                // and for the least productive (MAI close to 0) the cleaning time is 50 years
                double SD_conv = (stockingDegree - 1) / 20 - (6 - appMaiForest(plot.x, plot.y)) * 0.0015;

                double thinningTmp = appThinningForest(plot.x, plot.y);
                if (thinningTmp > 1 && appManageChForest(plot.x, plot.y) > 0) {
                    thinningTmp = max(1., thinningTmp - SD_conv * modTimeStep);
                    appThinningForest(plot.x, plot.y) = thinningTmp;
                    appCohort_all[plot.asID].setStockingDegreeMin(thinningTmp * sdMinCoef);
                    appCohort_all[plot.asID].setStockingDegreeMax(thinningTmp * sdMaxCoef);
                }

                double thinningTmpNew = thinningForestNew(plot.x, plot.y);
                if (thinningTmpNew > 1 && appManageChForest(plot.x, plot.y) > 0) {
                    thinningTmpNew = max(1., thinningTmpNew - SD_conv * modTimeStep);
                    thinningForestNew(plot.x, plot.y) = thinningTmpNew;
                    appNewCohort_all[plot.asID].setStockingDegreeMin(thinningTmpNew * sdMinCoef);
                    appNewCohort_all[plot.asID].setStockingDegreeMax(thinningTmpNew * sdMaxCoef);
                }
            }

            // TEMPORAL SHORTCUT FOR forest10 and forest30 when there are no the forest10 and forest30 policies
            if (!appForest30_policy)  // write separate loop for no forest10 policy
                for (const auto &plot: appPlots) {
                    appCohort30_all[plot.asID].setU(appRotationForest(plot.x, plot.y));
                    double SD = appThinningForest(plot.x, plot.y);
                    appThinningForest30(plot.x, plot.y) = SD;
                    appCohort30_all[plot.asID].setStockingDegreeMin(SD * sdMinCoef);
                    appCohort30_all[plot.asID].setStockingDegreeMax(SD * sdMaxCoef);
                }

            if (fmPol && priceC != 0 && year > refYear) {
                double fm_hurdle = 1;
                double diffMaxDiff = 1;
                array<double, numberOfCountries> maxDiff0{};

                // populate list of countries where it's impossible to match demanded wood production in current year
                countriesNoFmCPol.clear();
                for (double tmpTimeStep = modTimeStep / 100.; const auto country: countriesList)
                    if (CountryRegMaxHarvest.getVal(country, year - 1) <
                        (0.8 - tmpTimeStep) * CountryRegWoodProd.getVal(country, year - 1) ||
                        countryRegWoodHarvestDfM3Year.getVal(country, year - 1) >
                        (1.1 + tmpTimeStep) * CountryRegWoodProd.getVal(country, year - 1) ||
                        (appCO2Price.at(country)(year) <= 0 && priceC <= 0))
                        countriesNoFmCPol.insert(country);

                DEBUG("countriesNoFmCPol:");
                for (const auto country: countriesNoFmCPol)
                    DEBUG("{}", int{country});

                fmCPol(fm_hurdle, priceC, year); // TODO return double

                TRACE("maxDiff = {}\tfm_hurdle = {}\tdiffMaxDiff = {}", "maxDiff", fm_hurdle, diffMaxDiff);
                maxDiff0 = harvDiff;
                doneList.clear();

                for (size_t maxDiffCountry = 0; doneList.size() < countriesList.size();) {
                    while (doneList.contains(maxDiffCountry)) {
                        harvDiff[maxDiffCountry] = 0;
                        maxDiffCountry = distance(harvDiff.begin(), ranges::max_element(harvDiff));
                    }
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
                if (plot.protect.data.at(2000) == 0) {
                    double forestShareConcerned = forestConcerned == 10 ?
                                                  appDat_all[plot.asID].OForestShare10
                                                                        : appDat_all[plot.asID].OForestShare30;

                    if (forestShareConcerned > 0) {
                        double SD = cohortVec[plot.asID].getStockingDegree();
                        double biomassRot = 0;
                        double rotMaxBm = 0;

                        double biomass = forestConcerned == 10 ?
                                         appDat_all[plot.asID].OBiomassPrev10 : appDat_all[plot.asID].OBiomassPrev30;

                        if (biomass > 0 && plot.CAboveHa > 0 && appMaiForest(plot.x, plot.y) > 0) {
                            double biomassTmp = max(biomass, appDat_all[plot.asID].OBiomass0);
                            // rotation time to get current biomass (without thinning)
                            biomassRot = species[plot.speciesType - 1].getU(biomassTmp, appMaiForest(plot.x, plot.y));
                            rotMaxBm = species[plot.speciesType - 1].getTOpt(appMaiForest(plot.x, plot.y), ORT::MaxBm);
                        }

                        //  TO BE CHANGED to specific cleanedWoodUse10[...] / cleanedWoodUse30[...]
                        double cleanedWoodUseCurrent = cleanedWoodUse[plot.country];

                        if (biomass > 0 && appMaiForest(plot.x, plot.y) > 0 && SD != 0) {
                            double harvestTmp = appHarvestGrid(plot.x, plot.y);
                            double harvestO = cohortVec[plot.asID].cohortRes();
                            double lostHarvestTmp = cleanedWoodUseCurrent * harvestO * forestShareConcerned *
                                                    appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear);
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

                                if (forestConcerned == 10 && appForest10_policy)
                                    cleanedWoodUseNew *= cleanWoodUseShare10;
                                else if (forestConcerned == 30 && appForest30_policy)
                                    cleanedWoodUseNew *= cleanWoodUseShare30;

                                harvestTmp = appHarvestGrid(plot.x, plot.y);
                                harvestO = cohortVec[plot.asID].cohortRes();
                                double harvestNewTmp = cleanedWoodUseNew * harvestO * forestShareConcerned *
                                                       appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear);
                                appHarvestGrid(plot.x, plot.y) = max(0., harvestTmp + harvestNewTmp);
                            }
                        }
                    }
                }
        }

        void disturbanceDamageHarvest(const uint16_t year) {
            for (const auto &plot: appPlots) {
                double reciprocalFTimber = 1 / plot.fTimber.data.at(2000);

                double damagedWind = 0;
                double damagedFire = 0;
                double damagedBiotic = 0;

                if (disturbanceTrend) {
                    damagedWind = appDisturbWind[plot.simuID](year) * reciprocalFTimber;
                    damagedFire = appDisturbFire[plot.simuID](year) * reciprocalFTimber;
                    damagedBiotic = appDisturbBiotic[plot.simuID](year) * reciprocalFTimber;
                } else if (disturbanceExtreme && year == disturbanceExtremeYear) {
                    damagedWind = appDisturbWindExtreme[plot.simuID](year) * reciprocalFTimber;
                    damagedFire = appDisturbFireExtreme[plot.simuID](year) * reciprocalFTimber;
                    damagedBiotic = appDisturbBioticExtreme[plot.simuID](year) * reciprocalFTimber;
                }

                double cleanedWoodUseCurrent10 = 0;
                if (!appForest10_policy)
                    cleanedWoodUseCurrent10 =
                            cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction;
                else if (appMultifunction10)
                    cleanedWoodUseCurrent10 = cleanWoodUseShare10 * cleanedWoodUse[plot.country];

                double cleanedWoodUseCurrent30 = 1;
                if (appForest30_policy) {
                    cleanedWoodUseCurrent30 = appMultifunction30 ? cleanWoodUseShare30 * cleanedWoodUse[plot.country]
                                                                 : 0;
                } else if (appThinningForest30(plot.x, plot.y) < 0) {
                    cleanedWoodUseCurrent30 =
                            cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction;
                }

                double harvestO = 0;
                double harvestO30 = 0;
                double harvestO10 = 0;
                double harvestNew = 0;
                double harvestP = 0;

                double bmH = 0;
                double bmH10 = 0;
                double bmH30 = 0;
                double bmHP = 0;
                double bmH_new = 0;

                double damagedFireU = 0;
                double damagedFire10 = 0;
                double damagedFire30 = 0;
                double damagedFireNew = 0;
                double damagedFireP = 0;

                double harvAreaO = 0;
                double harvAreaO10 = 0;
                double harvAreaO30 = 0;
                double harvAreaNew = 0;
                double harvAreaP = 0;

                double harvestableFire = 0;
                double harvestableWind = 0;
                double harvestableBiotic = 0;

                const double burntReduction = 0.85;  // reduction of biomass due to burning / expert assumption

                if (plot.protect.data.at(2000) == 0) {
                    harvestableFire = 0.25;  // after consulting CBM database, email by Viorel Blujdea 26.07.2023 // Uncertain
                    harvestableWind = 0.7;
                    harvestableBiotic = 0.95;
                }

                const double shareU = appDat_all[plot.asID].OForestShareU;
                const double share10 = appDat_all[plot.asID].OForestShare10;
                const double share30 = appDat_all[plot.asID].OForestShare30;
                const double shareP = plot.strictProtected;
                const double shareNew = appDat_all[plot.asID].AForestShare;

                const double realAreaO = appCohort_all[plot.asID].getArea();
                const double realAreaO30 = appCohort30_all[plot.asID].getArea();
                const double realAreaO10 = appCohort10_all[plot.asID].getArea();
                const double realAreaNew = appNewCohort_all[plot.asID].getArea();
                const double realAreaP = appCohort_primary_all[plot.asID].getArea();

                if (shareU > 0 && realAreaO > 0) {
                    double reciprocalRealAreaO = 1 / realAreaO;
                    V resWind, resFire, resBiotic;

                    if (damagedWind > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resWind = appCohort_all[plot.asID].disturbanceDamage(damagedWind, 30, 15, 0, harvestableWind);
                    if (damagedFire > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resFire = appCohort_all[plot.asID].disturbanceDamage(damagedFire, 30, 0, 0, harvestableFire);
                    if (damagedBiotic > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resBiotic = appCohort_all[plot.asID].disturbanceDamage(damagedBiotic, 0, 0, 15,
                                                                               harvestableBiotic);

                    harvestO = ((resWind.sw + resWind.rw) * resWind.area + (resFire.sw + resFire.rw) * resFire.area +
                                (resBiotic.sw + resBiotic.rw) * resBiotic.area) * reciprocalRealAreaO;
                    bmH = (resWind.bm * resWind.area + resFire.bm * resFire.area * burntReduction +
                           resBiotic.bm * resBiotic.area) * reciprocalRealAreaO;
                    damagedFireU = resFire.bm * resFire.area * reciprocalRealAreaO;
                    harvAreaO = resWind.area + resFire.area + resBiotic.area;
                }

                if (share30 > 0 && realAreaO30 > 0) {
                    double reciprocalRealAreaO30 = 1 / realAreaO30;
                    V resWind30, resFire30, resBiotic30;

                    if (damagedWind > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resWind30 = appCohort30_all[plot.asID].disturbanceDamage(damagedWind, 30, 15, 0,
                                                                                 harvestableWind);
                    if (damagedFire > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resFire30 = appCohort30_all[plot.asID].disturbanceDamage(damagedFire, 30, 0, 0,
                                                                                 harvestableFire);
                    if (damagedBiotic > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resBiotic30 = appCohort30_all[plot.asID].disturbanceDamage(damagedBiotic, 0, 0, 15,
                                                                                   harvestableBiotic);

                    harvestO30 = ((resWind30.sw + resWind30.rw) * resWind30.area +
                                  (resWind30.sw + resFire30.rw) * resFire30.area +
                                  (resBiotic30.sw + resBiotic30.rw) * resBiotic30.area) * reciprocalRealAreaO30;
                    bmH30 = (resWind30.bm * resWind30.area + resFire30.bm * resFire30.area * burntReduction +
                             resBiotic30.bm * resBiotic30.area) * reciprocalRealAreaO30;
                    damagedFire30 = resFire30.bm * resFire30.area * reciprocalRealAreaO30;
                    harvAreaO30 = resWind30.area + resFire30.area + resBiotic30.area;
                }

                if (share10 > 0 && realAreaO10 > 0) {
                    double reciprocalRealAreaO10 = 1 / realAreaO10;
                    V resWind10, resFire10, resBiotic10;

                    if (damagedWind > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resWind10 = appCohort10_all[plot.asID].disturbanceDamage(damagedWind, 30, 15, 0,
                                                                                 harvestableWind);
                    if (damagedFire > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resFire10 = appCohort10_all[plot.asID].disturbanceDamage(damagedFire, 30, 0, 0,
                                                                                 harvestableFire);
                    if (damagedBiotic > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resBiotic10 = appCohort10_all[plot.asID].disturbanceDamage(damagedBiotic, 0, 0, 15,
                                                                                   harvestableBiotic);

                    harvestO10 = ((resWind10.sw + resWind10.rw) * resWind10.area +
                                  (resFire10.sw + resFire10.rw) * resFire10.area +
                                  (resBiotic10.sw + resBiotic10.rw) * resBiotic10.area) * reciprocalRealAreaO10;
                    bmH10 = (resWind10.bm * resWind10.area + resFire10.bm * resFire10.area * burntReduction +
                             resBiotic10.bm * resBiotic10.area) * reciprocalRealAreaO10;
                    damagedFire10 = resFire10.bm * resFire10.area * reciprocalRealAreaO10;
                    harvAreaO10 = resWind10.area + resFire10.area + resBiotic10.area;
                }

                // ---- we don't account for the deadwood accumulation at the moment (to be improved) ---
                if (shareNew > 0 && realAreaNew > 0) {
                    double reciprocalRealAreaNew = 1 / realAreaNew;
                    V resWindNew, resFireNew, resBioticNew;

                    if (damagedWind > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resWindNew = appNewCohort_all[plot.asID].disturbanceDamage(damagedWind, 30, 15, 0,
                                                                                   harvestableWind);
                    if (damagedFire > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resFireNew = appNewCohort_all[plot.asID].disturbanceDamage(damagedFire, 30, 0, 0,
                                                                                   harvestableFire);
                    if (damagedBiotic > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resBioticNew = appNewCohort_all[plot.asID].disturbanceDamage(damagedBiotic, 0, 0, 15,
                                                                                     harvestableBiotic);

                    harvestNew = ((resWindNew.sw + resWindNew.rw) * resWindNew.area +
                                  (resFireNew.sw + resFireNew.rw) * resFireNew.area +
                                  (resBioticNew.sw + resBioticNew.rw) * resBioticNew.area) * reciprocalRealAreaNew;
                    bmH_new = (resWindNew.bm * resWindNew.area + resFireNew.bm * resFireNew.area * burntReduction +
                               resBioticNew.bm * resBioticNew.area) * reciprocalRealAreaNew;
                    damagedFireNew = resFireNew.bm * resFireNew.area * reciprocalRealAreaNew;
                    harvAreaNew = resWindNew.area + resFireNew.area + resBioticNew.area;
                }

                // ---- we don't clean salvage in the primary forest and don't account for the deadwood accumulation at the moment (to be improved) ---
                if (shareP > 0 && realAreaP > 0) {
                    double reciprocalRealAreaP = 1 / realAreaP;
                    V resWindP, resFireP, resBioticP;

                    if (damagedWind > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resWindP = appCohort_primary_all[plot.asID].disturbanceDamage(damagedWind, 30, 15, 0, 0);
                    if (damagedFire > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resFireP = appCohort_primary_all[plot.asID].disturbanceDamage(damagedFire, 30, 0, 0, 0);
                    if (damagedBiotic > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                        resBioticP = appCohort_primary_all[plot.asID].disturbanceDamage(damagedBiotic, 0, 0, 15, 0);

                    harvestP =
                            ((resWindP.sw + resWindP.rw) * resWindP.area + (resFireP.sw + resFireP.rw) * resFireP.area +
                             (resBioticP.sw + resBioticP.rw) * resBioticP.area) * reciprocalRealAreaP;
                    bmHP = (resWindP.bm * resWindP.area + resFireP.bm * resFireP.area * burntReduction +
                            resBioticP.bm * resBioticP.area) * reciprocalRealAreaP;
                    damagedFireP = resFireP.bm * resFireP.area * reciprocalRealAreaP;
                    harvAreaP = resWindP.area + resFireP.area + resBioticP.area;
                }

                salvageLogging(plot.x, plot.y) =
                        (harvestO * (shareU - appDat_all[plot.asID].deforestShare) + shareNew * harvestNew +
                         harvestO30 * cleanedWoodUseCurrent30 * share30 +
                         harvestO10 * cleanedWoodUseCurrent10 * share10) * plot.fTimber.data.at(2000) *
                        appDat_all[plot.asID].landAreaHa;

                if (damagedFireU + damagedFire30 + damagedFire10 + damagedFireNew + damagedFireP <= 0)
                    adjustResiduesDisturbed(plot, year, harvestO, bmH, harvestNew, bmH_new, harvestO30, bmH30,
                                            harvAreaO, harvAreaNew, realAreaO, realAreaNew, harvAreaO30, realAreaO30);

                const auto [deadWoodPoolIn, litterPoolIn] =
                        shareU > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohort_all[plot.asID],
                                                                     harvestO, shareU, realAreaO, harvAreaO, bmH,
                                                                     appDat_all[plot.asID].extractedResidues,
                                                                     appDat_all[plot.asID].extractedStump)
                                   : pair{0., 0.};

                const auto [deadWoodPoolIn_new, litterPoolIn_new] =
                        shareNew > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appNewCohort_all[plot.asID],
                                                                       harvestNew,
                                                                       appDat_all[plot.asID].AForestSharePrev,
                                                                       realAreaNew, harvAreaNew, bmH_new,
                                                                       appDat_all[plot.asID].extractedResidues,
                                                                       appDat_all[plot.asID].extractedStump)
                                     : pair{0., 0.};

                const auto [deadWoodPoolIn10, litterPoolIn10] =
                        share10 > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohort10_all[plot.asID],
                                                                      harvestO10 * (1 - cleanedWoodUseCurrent10),
                                                                      share10, realAreaO10, harvAreaO10, bmH10,
                                                                      appDat_all[plot.asID].extractedResidues10,
                                                                      appDat_all[plot.asID].extractedStump10)
                                    : pair{0., 0.};

                const auto [deadWoodPoolIn30, litterPoolIn30] =
                        share30 > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohort30_all[plot.asID], harvestO30,
                                                                      share30, realAreaO30,
                                                                      harvAreaO30, bmH30,
                                                                      appDat_all[plot.asID].extractedResidues30,
                                                                      appDat_all[plot.asID].extractedStump30)
                                    : pair{0., 0.};

                const auto [deadWoodPoolInP, litterPoolInP] =
                        shareP > 0 ? deadWoodPoolDisturbanceCalcFunc(plot, appCohort_primary_all[plot.asID], 0, shareP,
                                                                     realAreaP, harvAreaP, bmHP, 0, 0)
                                   : pair{0., 0.};

                appDat_all[plot.asID].burntDeadwoodU = min(damagedFireU, 0.9 * appDat_all[plot.asID].deadwood);
                appDat_all[plot.asID].burntDeadwood30 = min(damagedFire30, 0.9 * appDat_all[plot.asID].deadwood30);
                appDat_all[plot.asID].burntDeadwood10 = min(damagedFire10, 0.9 * appDat_all[plot.asID].deadwood10);
                appDat_all[plot.asID].burntDeadwoodNew = min(damagedFireNew, 0.9 * appDat_all[plot.asID].deadwood_new);
                appDat_all[plot.asID].burntDeadwoodP = min(damagedFireP, 0.9 * appDat_all[plot.asID].deadwoodP);

                appDat_all[plot.asID].burntLitterU = min(damagedFireU, 0.9 * appDat_all[plot.asID].litter);
                appDat_all[plot.asID].burntLitter30 = min(damagedFire30, 0.9 * appDat_all[plot.asID].litter30);
                appDat_all[plot.asID].burntLitter10 = min(damagedFire10, 0.9 * appDat_all[plot.asID].litter10);
                appDat_all[plot.asID].burntLitterNew = min(damagedFireNew, 0.9 * appDat_all[plot.asID].litter_new);
                appDat_all[plot.asID].burntLitterP = min(damagedFireP, 0.9 * appDat_all[plot.asID].litterP);

                appDat_all[plot.asID].deadwood += deadWoodPoolIn;
                appDat_all[plot.asID].deadwood_new += deadWoodPoolIn_new;
                appDat_all[plot.asID].deadwood10 += deadWoodPoolIn10;
                appDat_all[plot.asID].deadwood30 += deadWoodPoolIn30;
                appDat_all[plot.asID].deadwoodP += deadWoodPoolInP;

                appDat_all[plot.asID].litter += litterPoolIn;
                appDat_all[plot.asID].litter_new += litterPoolIn_new;
                appDat_all[plot.asID].litter10 += litterPoolIn10;
                appDat_all[plot.asID].litter30 += litterPoolIn30;
                appDat_all[plot.asID].litterP += litterPoolInP;
            }
        }

        /*
        Modified 12 May 2020 according to suggestion by Fulvio Di Fulvio:
        First collect branches and harvest losses at the clearcut areas, then,
        if necessary, collect stumps at the clearcut areas,
        then, if necessary, collect branches and harvest losses at the low-intensity harvest areas
        then, if necessary, collect dead trees in the low-intensity management forests
         */
        void adjustResiduesDisturbed(const DataStruct &plot, const uint16_t year,
                                     const double harvestO,
                                     const double bmH,       // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
                                     const double harvestNew,
                                     const double bmHNew,       // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
                                     const double harvestO30,
                                     const double bmH30,       // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
                                     const double harvAreaO, const double harvAreaN,
                                     const double realAreaO, const double realAreaN,
                                     const double harvAreaO30, const double realAreaO30) {
            if (year <= residueHarvYear || plot.protect.data.at(2000) == 1 || plot.residuesUseShare <= 0)
                return;

            const double OForestShareU = appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare;
            const double OForestShare30 = appDat_all[plot.asID].OForestShare30;
            const double AForestShare = appDat_all[plot.asID].AForestShare;
            double residueUse30 = appForest30_policy ? 0 : 1;

            //---- Estimation of harvest residues (branches and leaves) per grid in tC ------------------------------------
            double harvRes_fcO = 0;
            double harvRes_thO = 0;
            double harvRes_fcN = 0;
            double harvRes_thN = 0;
            double harvRes_scO = 0;  // residues extracted from cleaned (old) forest that is taken for wood production
            double harvRes_scN = 0;  // residues extracted from cleaned (new) forest that is taken for wood production
            double harvRes_scN_notTaken = 0;  // all wood extracted from (new) cleaned forest that is not taken for wood production
            double harvRes_scO_notTaken = 0;  // all wood extracted from (old) cleaned forest that is not taken for wood production
            double harvRes_fcO30 = 0;

            bool em_harvRes_fcO = false;
            bool em_harvRes_thO = false;
            bool em_harvRes_fcN = false;
            bool em_harvRes_thN = false;
            bool em_harvRes_scO = false;
            bool em_harvRes_scN = false;
            bool em_harvRes_fcO30 = false;

            if (harvAreaO > 0 && realAreaO > 0 && harvestO > 0 && bmH > 0)  // tC/ha
                harvRes_fcO = harvestO * (plot.BEF(bmH * realAreaO / harvAreaO) - 2) + bmH;
            em_harvRes_fcO = harvRes_fcO > 0;

            if (harvAreaN > 0 && realAreaN > 0 && harvestNew > 0 && bmHNew > 0)  // tC/ha
                harvRes_fcN = harvestNew * (plot.BEF(bmHNew * realAreaN / harvAreaN) - 2) + bmHNew;
            em_harvRes_fcN = harvRes_fcN > 0;

            if (harvAreaO30 > 0 && realAreaO30 > 0 && harvestO30 > 0 && bmH30 > 0 && residueUse30 > 0)  // tC/ha
                harvRes_fcO30 = harvestO30 * (plot.BEF(bmH30 * realAreaO30 / harvAreaO30) - 2) + bmH30;
            em_harvRes_fcO30 = harvRes_fcO30 > 0;

            double stump = 0;       // stumps of old forest // tC/ha
            double stump_new = 0;   // stumps of new forest// tC/ha
            double stump_df = 0;
            double stump10 = 0;     // stumps of old forest // tC/ha
            double stump_df10 = 0;
            double stump30 = 0;     // stumps of old forest // tC/ha
            double stump_df30 = 0;

            // calculate amount of stumps + big roots for final felling, tC/ha
            if (stumpHarvCountrySpecies.contains({plot.country, plot.speciesType})) {

                if (appThinningForest(plot.x, plot.y) > 0) {
                    double hDBHOld = appCohort_all[plot.asID].getDBHFinalCut();
                    double hHOld = appCohort_all[plot.asID].getHFinalCut();
                    double hDBHNew = appNewCohort_all[plot.asID].getDBHFinalCut();
                    double hHNew = appNewCohort_all[plot.asID].getHFinalCut();

                    if (harvAreaO > 0 && realAreaO > 0 && plot.fTimber.data.at(2000) > 0 && hDBHOld > 0 && hHOld > 0) {
                        double harvestGSOld = bmH / harvAreaO * realAreaO;
                        stump = plot.DBHHToStump(hDBHOld, hHOld, harvestGSOld) * harvAreaO / realAreaO;
                    }

                    if (harvAreaN > 0 && realAreaN > 0 && plot.fTimber.data.at(2000) > 0 && hDBHNew > 0 && hHNew > 0) {
                        double harvestGSNew = bmHNew / harvAreaN * realAreaN;
                        stump_new = plot.DBHHToStump(hDBHNew, hHNew, harvestGSNew) * harvAreaN / realAreaN;
                    }
                }

                if (appThinningForest30(plot.x, plot.y) > 0 && residueUse30 > 0) {
                    double hDBHOld30 = appCohort30_all[plot.asID].getDBHFinalCut();
                    double hHOld30 = appCohort30_all[plot.asID].getHFinalCut();

                    if (harvAreaO30 > 0 && realAreaO30 > 0 && plot.fTimber.data.at(2000) > 0 && hDBHOld30 > 0 &&
                        hHOld30 > 0) {
                        double harvestGSOld30 = bmH30 / harvAreaO30 * realAreaO30;
                        stump30 = plot.DBHHToStump(hDBHOld30, hHOld30, harvestGSOld30) * harvAreaO30 / realAreaO30;
                    }
                }
            }

            double residuesSuit1_perHa = plot.residuesUseShare * (harvRes_fcO + harvRes_fcN);   // tC/ha
            double residuesSuit2_perHa = plot.residuesUseShare * (stump + stump_new);           // tC/ha

            double residuesSuit1_perHa30 = plot.residuesUseShare * harvRes_fcO30;               // tC/ha
            double residuesSuit2_perHa30 = plot.residuesUseShare * stump30 * residueUse30;      // tC/ha

            double residuesSuit1 = plot.residuesUseShare * (harvRes_fcO * OForestShareU + harvRes_fcN * AForestShare) *
                                   appDat_all[plot.asID].landAreaHa;
            double residuesSuit2 = plot.residuesUseShare * (stump * OForestShareU + stump_new * AForestShare) *
                                   appDat_all[plot.asID].landAreaHa;

            double residuesSuit1_30 = residuesSuit1_perHa30 * OForestShare30 * appDat_all[plot.asID].landAreaHa;
            double residuesSuit2_30 = residuesSuit2_perHa30 * OForestShare30 * appDat_all[plot.asID].landAreaHa;

            double residueHarvestDist =
                    (residuesSuit1 + residuesSuit2 + residuesSuit1_30 + residuesSuit2_30) * plot.fTimber.data.at(2000);
            residueHarvest[plot.country] += residueHarvestDist;
            countriesResiduesExtract_m3.inc(plot.country, year, residueHarvestDist);
        }

        // Deadwood input (d > 10cm) in the cell, tC/ha, in the old forest
        // Litter input (d <= 10cm) in the cell, tC/ha, in the old forest
        // return pair<deadWoodPoolIn, litterPoolIn>
        [[nodiscard]] static pair<double, double>
        deadWoodPoolDisturbanceCalcFunc(const DataStruct &plot, const AgeStruct &cohort,
                                        const double harvestedWood, // wood harvested in the production forest, m3/ha
                                        const double forestShare, // considered forest share in the cell
                                        const double realAreaO, // relative forest area in the forest simulator
                                        const double harvAreaO, // pure harvested area
                                        const double bmH, // total harvested stem biomass at final felling in the production forest and multifunction forest
                                        const double extractedResidues, // share of extracted logging residues
                                        const double extractedStump) // share of extracted stump
        {
            // Dead wood
            double deadWood_fc_unmng = 0;       // deadwood (stem) of trees dying in each age cohort and at the end of lifespan in the old forests not used for intensive wood production
            double deadWood_fc_mng = 0;         // deadwood (stem) of trees dying in each age cohort and at the end of lifespan in the old forests used for intensive wood production

            double deadWood_mort = 0;           // deadwood (stem) due to mortality in old forest that is not classified as potentially merchantable
            double litter_mort = 0;             // litter (stem) due to mortality in old forest that is not classified as potentially merchantable

            double deadWoodBranches_mort = 0;
            double litterBranches_mort = 0;
            double deadWoodStump_mort = 0;
            double litterStump_mort = 0;

            // tC/ha in the cell
            double deadWoodBranches_fc = 0.3 * bmH * (plot.BEF(harvAreaO > 0 ? (bmH / harvAreaO * realAreaO) : 0) - 1);
            double litterBranches_fc = deadWoodBranches_fc / 0.3 * 0.7;
            double litterBranches_th = 0;

            double deadWoodBranches_th = 0;

            const double harvLossShare = 0.25;

            double harvL = max(0., bmH - harvestedWood);
            // deadwood from harvest losses in intensively managed old forest
            double deadwood_hRes_mng = (1 - harvLossShare) * harvL;
            // litter from harvest losses in intensively managed old forest
            double litter_hRes_mng = harvLossShare * harvL;

            double fcDBH = cohort.getDBHFinalCut();
            double fcH = cohort.getHFinalCut();
            double fcGS = bmH / harvAreaO * realAreaO;

            double deadWoodStump_fc = realAreaO > 0 ? plot.DBHHToStump(fcDBH, fcH, fcGS) * harvAreaO / realAreaO : 0;
            double deadWoodStump_th = 0;

            double thDBHold = 0;
            double thHold = 0;

            // branches from harvested trees
            double tmp_share = 1 - extractedResidues * plot.residuesUseShare;

            double deadWoodPoolIn = deadWoodBranches_fc * tmp_share +
                                    deadWoodStump_fc * (1 - extractedStump * plot.residuesUseShare) +
                                    deadWood_fc_mng +
                                    deadwood_hRes_mng * tmp_share;

            double litterPoolIn = litterBranches_fc * tmp_share +
                                  litter_hRes_mng * tmp_share;

            return {deadWoodPoolIn, litterPoolIn};
        }

        // Adjust forest management in the case of a non-zero carbon price of the carbon in tree biomass.
        // Wood and land prices by countries
        void fmCPol(const double fm_hurdle, const double priceC, const uint16_t year) {
            array<double, numberOfCountries> woodHarvest{};
            array<double, numberOfCountries> woodHarvestPostControl{};
            array<double, numberOfCountries> woodHarvestPostControlU{};
            array<double, numberOfCountries> woodHarvestPostControl10{};
            array<double, numberOfCountries> woodHarvestPostControl30{};

            if (fm_hurdle == 1 && toAdjust.size() > 1)
                for (const auto &plot: appPlots)
                    // Don't do initial disturbance for countries which cannot produce demanded amount of wood due to lack of forest resources or very high deforestation
                    if (countriesNoFmCPol.contains(plot.country) && toAdjust.contains(plot.country) &&
                        plot.protect.data.at(2000) == 0)
                        if (appMaiForest(plot.x, plot.y) > 0 && appDat_all[plot.asID].OForestShare > 0) {
                            appCoefPriceC = priceC < 0 ? appCO2Price.at(plot.country)(year) : priceC;
                            appCoefPriceC *= plot.corruption.data.at(2000);

                            if (appManagedForest(plot.x, plot.y) > 0) {
                                double rotMAI = species[plot.speciesType - 1].getTOptT(
                                        appMaiForest(plot.x, plot.y), ORT::MAI);
                                double rotMaxBmTh = species[plot.speciesType - 1].getTOptT(
                                        appMaiForest(plot.x, plot.y), ORT::MaxBm);
                                const auto [rotMaxNPV, maxNPV, harvestMaxNPV] =
                                        maxNPVRotation(plot, year, true, rotMAI, rotMaxBmTh);

                                maxNPVGrid(plot.x, plot.y) = maxNPV;
                                appRotationForest(plot.x, plot.y) = rotMaxNPV;
//                            appHarvestGrid(plot.x, plot.y) = harvestMaxNPV;
                                appCohort_all[plot.asID].setU(rotMaxNPV);
                                appNewCohort_all[plot.asID].setU(rotMaxNPV);

                                if (!appForest10_policy && appThinningForest10(plot.x, plot.y) > 0)
                                    appCohort10_all[plot.asID].setU(rotMaxNPV);

                                if (!appForest30_policy && appThinningForest30(plot.x, plot.y) > 0)
                                    appCohort30_all[plot.asID].setU(rotMaxNPV);

                            } else {  // if managed
                                double rotMaxBm = species[plot.speciesType - 1].getTOpt(appMaiForest(plot.x, plot.y),
                                                                                        ORT::MaxBm);
                                double maxNPV = npvCalc(plot, appCohort_all[plot.asID].createSetU(rotMaxBm), year,
                                                        rotMaxBm, false).first;
                                maxNPVGrid(plot.x, plot.y) = maxNPV;
                                appRotationForest(plot.x, plot.y) = rotMaxBm;
                                appCohort_all[plot.asID].setU(rotMaxBm);
                                appNewCohort_all[plot.asID].setU(rotMaxBm);

                                if (appThinningForest10(plot.x, plot.y) < 0)
                                    appCohort10_all[plot.asID].setU(rotMaxBm);

                                if (appThinningForest30(plot.x, plot.y) < 0)
                                    appCohort30_all[plot.asID].setU(rotMaxBm);
                            }
                        }

            for (const auto &plot: appPlots)
                if (plot.protect.data.at(2000) == 0) {
                    double harvestO = appCohort_all[plot.asID].cohortRes();
                    double harvestO10 = appCohort10_all[plot.asID].cohortRes();
                    double harvestO30 = appCohort30_all[plot.asID].cohortRes();
                    double harvestN = appNewCohort_all[plot.asID].cohortRes();

                    double cleanedWoodUseCurrent10 = 0;
                    if (!appForest10_policy)
                        cleanedWoodUseCurrent10 =
                                cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction;
                    else if (appForest10_policy && appMultifunction10)
                        cleanedWoodUseCurrent10 = cleanWoodUseShare10 * cleanedWoodUse[plot.country];

                    double cleanedWoodUseCurrent30 = 0;
                    if (!appForest30_policy)
                        cleanedWoodUseCurrent30 =
                                cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction;
                    else if (appForest30_policy && appMultifunction30)
                        cleanedWoodUseCurrent30 = cleanWoodUseShare30 * cleanedWoodUse[plot.country];

                    double newHarvestTmp = 0;               // Total current harvested wood in the cell, m3
                    double newHarvestChangeableTmp = 0;     // Total current harvested wood in the cell, m3

                    if (appThinningForest(plot.x, plot.y) > 0) {
                        newHarvestTmp += (harvestO *
                                          (appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare) +
                                          harvestN * appDat_all[plot.asID].AForestShare) *
                                         appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                         appDat_all[plot.asID].deforWoodTotM3;

                        newHarvestChangeableTmp += (harvestO * (appDat_all[plot.asID].OForestShareU -
                                                                appDat_all[plot.asID].deforestShare) +
                                                    harvestN * appDat_all[plot.asID].AForestShare) *
                                                   appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                   appDat_all[plot.asID].deforWoodTotM3;

                        woodHarvestPostControlU[plot.country] += (harvestO * (appDat_all[plot.asID].OForestShareU -
                                                                              appDat_all[plot.asID].deforestShare) +
                                                                  harvestN * appDat_all[plot.asID].AForestShare) *
                                                                 appDat_all[plot.asID].landAreaHa *
                                                                 plot.fTimber(coef.bYear) +
                                                                 appDat_all[plot.asID].deforWoodTotM3;
                    } else {
                        newHarvestTmp +=
                                ((cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                 harvestO *
                                 (appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare) +
                                 (cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                 harvestN * appDat_all[plot.asID].AForestShare) *
                                appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                appDat_all[plot.asID].deforWoodTotM3;

                        newHarvestChangeableTmp +=
                                ((cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                 harvestO *
                                 (appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare) +
                                 (cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                 harvestN * appDat_all[plot.asID].AForestShare) *
                                appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                appDat_all[plot.asID].deforWoodTotM3;

                        woodHarvestPostControlU[plot.country] +=
                                ((cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                 harvestO *
                                 (appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare) +
                                 (cleanedWoodUse[plot.country] + appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                 harvestN * appDat_all[plot.asID].AForestShare) *
                                appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                appDat_all[plot.asID].deforWoodTotM3;

                    }

                    if (appThinningForest10(plot.x, plot.y) > 0) {
                        newHarvestTmp +=
                                harvestO10 * (appDat_all[plot.asID].OForestShare10) * appDat_all[plot.asID].landAreaHa *
                                plot.fTimber(coef.bYear);
                        woodHarvestPostControl10[plot.country] +=
                                harvestO10 * (appDat_all[plot.asID].OForestShare10) * appDat_all[plot.asID].landAreaHa *
                                plot.fTimber(coef.bYear);

                    } else {
                        newHarvestTmp += cleanedWoodUseCurrent10 * harvestO10 * appDat_all[plot.asID].OForestShare10 *
                                         appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear);
                        woodHarvestPostControl10[plot.country] +=
                                cleanedWoodUseCurrent10 * harvestO10 * appDat_all[plot.asID].OForestShare10 *
                                appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear);
                    }

                    if (appThinningForest30(plot.x, plot.y) > 0) {
                        newHarvestTmp +=
                                harvestO30 * appDat_all[plot.asID].OForestShare30 * appDat_all[plot.asID].landAreaHa *
                                plot.fTimber(coef.bYear);

                        newHarvestChangeableTmp +=
                                harvestO30 * appDat_all[plot.asID].OForestShare30 * appDat_all[plot.asID].landAreaHa *
                                plot.fTimber(coef.bYear);

                        woodHarvestPostControl30[plot.country] +=
                                harvestO30 * appDat_all[plot.asID].OForestShare30 * appDat_all[plot.asID].landAreaHa *
                                plot.fTimber(coef.bYear);

                    } else {
                        newHarvestTmp += cleanedWoodUseCurrent30 * harvestO30 * appDat_all[plot.asID].OForestShare30 *
                                         appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear);

                        newHarvestChangeableTmp +=
                                cleanedWoodUseCurrent30 * harvestO30 * appDat_all[plot.asID].OForestShare30 *
                                appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear);

                        woodHarvestPostControl30[plot.country] +=
                                cleanedWoodUseCurrent30 * harvestO30 * appDat_all[plot.asID].OForestShare30 *
                                appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear);
                    }

                    woodHarvest[plot.country] += newHarvestTmp + salvageLogging(plot.x, plot.y);
                    appHarvestGrid(plot.x, plot.y) = newHarvestChangeableTmp;
                }

            if (toAdjust.size() > 1)
                for (size_t i = 0; i < numberOfCountries; ++i) {
                    if (appWoodDemand[i](year) > 0 && regions.contains(countryRegion[i]) && toAdjust.contains(i)) {
                        harvDiff[i] = abs(woodHarvest[i] / appWoodDemand[i](year) - 1);
                        TRACE("i = {}\tyear = \tharvDiff0 = {}\twoodHarvest = {}\twoodDemand = {}\tcountriesWoodHarvestM3Year = {}",
                              i, year, harvDiff[i], woodHarvest[i], appWoodDemand[i](year),
                              countriesWoodHarvestM3Year.getVal(i, year));

                        if (countriesNoFmCPol.contains(i)) {
                            harvDiff[i] = 0;
                            TRACE("No NoFmCPol for country");
                        }
                    } else
                        harvDiff[i] = 0;
                }

            // ------- Zero pass = Adjust thinning if population density changed --------------------
            const double stockingDegree = 1.3;
            if (year > 2000 && year % 10 == 0) {
                INFO("Start Zero pass = adjust thinning if population density changed");
                adjustSD(year, 0.12, woodHarvest, stockingDegree, priceC, true);
            }

            // ---- First pass = adjust rotation time -------
            INFO("Start First pass = adjust rotation time");
            adjustRT(year, 0.01, woodHarvest, priceC, true);

            //----Second pass = adjust rotation time -------
            INFO("Start Second pass = adjust rotation time");
            adjustRT(year, 0.02, woodHarvest, priceC, true, true);

            //----Third pass = adjust rotation time -------
            INFO("Start Third pass = adjust thinning");
            adjustSD(year, 0.01, woodHarvest, stockingDegree, priceC, true);

            //----Forth pass = adjust rotation time -------
            INFO("Start Forth pass = adjust rotation time");
            adjustRT(year, 0.01, woodHarvest, priceC, true);

            //----Fifth pass = adjust rotation time -------
            INFO("Start Fifth pass = adjust rotation time");
            adjustRT(year, 0.02, woodHarvest, priceC, true, true);
        }

        // maiV computes internally
        [[nodiscard]] tuple<double, double, double>
        maxNPVRotation(const DataStruct &plot, const uint16_t year, const bool used,
                       const double MAIRot, const double BmMaxRot) {
            double OForestShare = appDat_all[plot.asID].OForestShare;

            const auto [NPV_maiRot, harv_maiRot] = npvCalc(plot, appCohort_all[plot.asID].createSetU(MAIRot), year,
                                                           MAIRot, used);

            const auto [NPV_bmMaxRot, harv_bmMaxRot] = npvCalc(plot, appCohort_all[plot.asID].createSetU(BmMaxRot),
                                                               year, MAIRot, used);

            double rotTmp1 = lerp(MAIRot, BmMaxRot, 2 - phi);
            double rotTmp2 = lerp(MAIRot, BmMaxRot, phi - 1);
            double rotTmp3 = lerp(MAIRot, BmMaxRot, 0.05);

            auto [NPV_rotTmp1, harv_rotTmp1] = npvCalc(plot, appCohort_all[plot.asID].createSetU(rotTmp1), year, MAIRot,
                                                       used);

            auto [NPV_rotTmp2, harv_rotTmp2] = npvCalc(plot, appCohort_all[plot.asID].createSetU(rotTmp2), year, MAIRot,
                                                       used);

            const auto [NPV_rotTmp3, harv_rotTmp3] = npvCalc(plot, appCohort_all[plot.asID].createSetU(rotTmp3), year,
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
                        tie(NPV_rotTmp1, harvestMaxNPV) = npvCalc(plot, appCohort_all[plot.asID].createSetU(rotTmp1),
                                                                  year, MAIRot, used);
                        RotMaxNPV = rotTmp1;
                        MaxNPV = NPV_rotTmp1;
                    } else {
                        rotTmpL = rotTmp1;
                        rotTmp1 = rotTmp2;
                        rotTmp2 = lerp(rotTmpL, rotTmpR, phi - 1);
                        NPV_rotTmp1 = NPV_rotTmp2;
                        tie(NPV_rotTmp2, harvestMaxNPV) = npvCalc(plot, appCohort_all[plot.asID].createSetU(rotTmp2),
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
        [[nodiscard]] pair<double, double>
        npvCalc(const DataStruct &plot, AgeStruct cohortTmp, const uint16_t year, const double rotation,
                const bool used, const double wpMult = 1, const bool zeroC = false) const {
            double maiV = appMaiForest(plot.x, plot.y) * plot.fTimber(coef.bYear);
            double rotMAI = species[plot.speciesType - 1].getTOptT(maiV / plot.fTimber(year), ORT::MAI);
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
                    harvestTmp = AgeStruct::cohortRes(realAreaO, resTmp) * plot.fTimber(year);
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

        // fd if fdcFlag is false
        // maiV computes internally
        [[nodiscard]] double
        forNPV_fdc(const DataStruct &plot, const uint16_t year, const double harvestedArea, const double harvestedW,
                   double biomassCur, const double wpMult = 1, const bool fdcFlag = true) const {
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
                double sFor = (1 - appDat_all[plot.asID].OForestShare) * 9 + 1;
                // MG: use internal G4M wood price
                // MG: Changed to external SawnLogsPrice
                double c4 = (coef.priceTimberMaxR - coef.priceTimberMinR) / 99;
                double c3 = coef.priceTimberMinR - c4;
                priceWExt = (c3 + c4 * plot.sPopDens(year) * sFor) * plot.priceIndex(year) / coef.priceIndexE *
                            appWoodPrice.at(plot.country)(year) / appWoodPrice.at(plot.country).data.at(2000) * wpMult;
            }

            // extension to total biomass
            double CBenefit = fdcFlag ? 1.2 * appCoefPriceC * (biomassCur - biomassBauScenarios.at(suffix0)[
                    (year - refYear - modTimeStep) / modTimeStep][plot.asID]) : 0;

            //MG: Value of Forestry during one rotation External // Changed to 1 year!!!!
            return (priceWExt - priceHarvest) * harvestedW - plantingCosts + CBenefit;
        }

        // Adjust forest management type depending on wood demand and the state of the forests
        // wood and land prices are by countries!
        void adjustSD(const uint16_t year, const double woodProdTolerance, const span<double> woodHarvest,
                      const double stockingDegree, const double priceC, const bool CPol = false) {
            const double adjustTolerance = 2;

            auto CPolPart = [&](const DataStruct &plot, const double rotation, const double countryHarvestTmp,
                                const double countryWoodDemand, const double newHarvestTmp,
                                const double rotationForestTmp, const double thinningForestTmp,
                                const double rotationForestTmpNew, const double thinningForestTmpNew,
                                const int8_t managedForestTmp) -> void {
                if (CPol) {
                    double NPVTmp = 1e20;

                    if (countriesFmcpol.contains(plot.country) &&
                        !countriesNoFmCPol.contains(plot.country)) {
                        appCoefPriceC = priceC < 0 ? appCO2Price.at(plot.country)(year) : priceC;
                        appCoefPriceC *= plot.corruption.data.at(2000);

                        bool used = appThinningForest(plot.x, plot.y) > 0;
                        // fmHurdle as wpMult in the future
                        NPVTmp = npvCalc(plot, appCohort_all[plot.asID], year, rotation, used).first;
                    }

                    if (abs(countryHarvestTmp - countryWoodDemand) < (1 + tolerance * adjustTolerance) *
                                                                     abs(woodHarvest[plot.country] -
                                                                         countryWoodDemand) &&
                        NPVTmp >= maxNPVGrid(plot.x, plot.y)) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;
                        // usage of useChange was reconsidered
                        appManageChForest(plot.x, plot.y) = 1;

                        if (!appForest30_policy) {
                            appThinningForest30(plot.x, plot.y) = -1;
                            appCohort30_all[plot.asID].setU(rotation);
                            appCohort30_all[plot.asID].setStockingDegreeMin(-sdMinCoef);
                            appCohort30_all[plot.asID].setStockingDegreeMax(-sdMaxCoef);
                        }

                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appThinningForest(plot.x, plot.y) = thinningForestTmp;
                        appManagedForest(plot.x, plot.y) = managedForestTmp;
                        appCohort_all[plot.asID].setU(rotationForestTmp);
                        appCohort_all[plot.asID].setStockingDegreeMin(thinningForestTmp * sdMinCoef);
                        appCohort_all[plot.asID].setStockingDegreeMax(thinningForestTmp * sdMaxCoef);

                        rotationForestNew(plot.x, plot.y) = rotationForestTmpNew;
                        thinningForestNew(plot.x, plot.y) = thinningForestTmpNew;
                        appNewCohort_all[plot.asID].setU(rotationForestTmpNew);
                        appNewCohort_all[plot.asID].setStockingDegreeMin(
                                thinningForestTmpNew * sdMinCoef);
                        appNewCohort_all[plot.asID].setStockingDegreeMax(
                                thinningForestTmpNew * sdMaxCoef);
                    }
                } else {
                    if (abs(countryHarvestTmp - countryWoodDemand) < (1 + tolerance * adjustTolerance) *
                                                                     abs(woodHarvest[plot.country] -
                                                                         countryWoodDemand)) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;
                        appManageChForest(plot.x, plot.y) = 1;

                        if (!appForest30_policy) {
                            appThinningForest30(plot.x, plot.y) = -1;
                            appCohort30_all[plot.asID].setU(rotation);
                            appCohort30_all[plot.asID].setStockingDegreeMin(-sdMinCoef);
                            appCohort30_all[plot.asID].setStockingDegreeMax(-sdMaxCoef);
                        }
                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appThinningForest(plot.x, plot.y) = thinningForestTmp;
                        appManagedForest(plot.x, plot.y) = managedForestTmp;
                        appCohort_all[plot.asID].setU(rotationForestTmp);
                        appCohort_all[plot.asID].setStockingDegreeMin(thinningForestTmp * sdMinCoef);
                        appCohort_all[plot.asID].setStockingDegreeMax(thinningForestTmp * sdMaxCoef);

                        rotationForestNew(plot.x, plot.y) = rotationForestTmpNew;
                        thinningForestNew(plot.x, plot.y) = thinningForestTmpNew;
                        appNewCohort_all[plot.asID].setU(rotationForestTmpNew);
                        appNewCohort_all[plot.asID].setStockingDegreeMin(
                                thinningForestTmpNew * sdMinCoef);
                        appNewCohort_all[plot.asID].setStockingDegreeMax(
                                thinningForestTmpNew * sdMaxCoef);
                    }
                }
            };

            for (const auto &plot: appPlots)
                if (toAdjust.contains(plot.country) && plot.protect.data.at(2000) == 0)
                    if (appDat_all[plot.asID].OForestShareU > 0 && appMaiForest(plot.x, plot.y) > 0) {
                        double countryWoodDemand = appWoodDemand.at(plot.country)(year);

                        double rotationForestTmp = appRotationForest(plot.x, plot.y);
                        int8_t managedForestTmp = appManagedForest(plot.x, plot.y);
                        double thinningForestTmp = appThinningForest(plot.x, plot.y);

                        double rotationForestTmpNew = appCohort_all[plot.asID].getU();
                        double thinningForestTmpNew = appNewCohort_all[plot.asID].getStockingDegree();

                        if (woodHarvest[plot.country] < (1 - woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) <= 0) {
                                double rotMAI = 0;
                                double rotMaxBmTh = 0;
                                double biomassRotTh2 = 0;

                                if (appDat_all[plot.asID].OBiomassPrev > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0) {

                                    biomassRotTh2 = species[plot.speciesType - 1].getUSdTab(
                                            appDat_all[plot.asID].OBiomass0, appMaiForest(plot.x, plot.y),
                                            stockingDegree);     // rotation time to get current biomass (with thinning)
                                    rotMAI = species[plot.speciesType - 1].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                    ORT::MAI);
                                    rotMaxBmTh = species[plot.speciesType - 1].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                        ORT::MaxBm);
                                } else if (appDat_all[plot.asID].prevPlantPhytHaBmGr > 0) {

                                    rotMAI = species[plot.speciesType - 1].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                    ORT::MAI);
                                    rotMaxBmTh = species[plot.speciesType - 1].getTOptT(appMaiForest(plot.x, plot.y),
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

                                appCohort_all[plot.asID].setU(rotation);
                                appCohort_all[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                appCohort_all[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);

                                appNewCohort_all[plot.asID].setU(rotation);

                                if (appNewCohort_all[plot.asID].getActiveAge() >= rotation) {
                                    thinningForestNew(plot.x, plot.y) = stockingDegree;
                                    appNewCohort_all[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                    appNewCohort_all[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);
                                }

                                double harvestO = appCohort_all[plot.asID].cohortRes();
                                double harvestN = appNewCohort_all[plot.asID].cohortRes();

                                double forestShareApplied =
                                        appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare;
                                if (!appForest30_policy)
                                    forestShareApplied += appDat_all[plot.asID].OForestShare30;

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp = (harvestO * forestShareApplied +
                                                        harvestN * appDat_all[plot.asID].AForestShare) *
                                                       appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                       appDat_all[plot.asID].deforWoodTotM3;
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

                                    if (countriesFmcpol.contains(plot.country) &&
                                        !countriesNoFmCPol.contains(plot.country)) {
                                        appCoefPriceC = priceC < 0 ? appCO2Price.at(plot.country)(year) : priceC;
                                        appCoefPriceC *= plot.corruption.data.at(2000);

                                        bool used = appThinningForest(plot.x, plot.y) > 0;
                                        // fmHurdle as wpMult in the future
                                        NPVTmp = npvCalc(plot, appCohort_all[plot.asID], year, rotation, used).first;

                                        if (appDat_all[plot.asID].constructedRoadsMultifunction < 0 &&
                                            appManageChForest(plot.x, plot.y) == 0)
                                            NPVTmp -= appDat_all[plot.asID].currentYearRoadInvestment +
                                                      (meanRoadDensityProduction - roadMultifunctional -
                                                       appDat_all[plot.asID].constructedRoadsMultifunction) *
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
                                        appDat_all[plot.asID].constructedRoadsMultifunction =
                                                meanRoadDensityProduction - roadMultifunctional;
                                        appDat_all[plot.asID].harvestEfficiencyMultifunction = 0;

                                        if (!appForest30_policy) {
                                            appThinningForest30(plot.x, plot.y) = stockingDegree;
                                            appCohort30_all[plot.asID].setU(rotation);
                                            appCohort30_all[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                            appCohort30_all[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);
                                        }

                                    } else { // return old values
                                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                                        appThinningForest(plot.x, plot.y) = thinningForestTmp;
                                        appManagedForest(plot.x, plot.y) = managedForestTmp;
                                        appCohort_all[plot.asID].setU(rotationForestTmp);
                                        appCohort_all[plot.asID].setStockingDegreeMin(thinningForestTmp * sdMinCoef);
                                        appCohort_all[plot.asID].setStockingDegreeMax(thinningForestTmp * sdMaxCoef);

                                        rotationForestNew(plot.x, plot.y) = rotationForestTmpNew;
                                        thinningForestNew(plot.x, plot.y) = thinningForestTmpNew;
                                        appNewCohort_all[plot.asID].setU(rotationForestTmpNew);
                                        appNewCohort_all[plot.asID].setStockingDegreeMin(
                                                thinningForestTmpNew * sdMinCoef);
                                        appNewCohort_all[plot.asID].setStockingDegreeMax(
                                                thinningForestTmpNew * sdMaxCoef);
                                    }
                                } else {
                                    double NPVTmp = npvCalc(plot, appCohort_all[plot.asID], year, rotation, true, 1,
                                                            true).first;

                                    // to simplify? because meanRoadDensityProduction = roadMultifunctional
                                    // => 0 = meanRoadDensityProduction - roadMultifunctional
                                    if (appDat_all[plot.asID].constructedRoadsMultifunction < 0 &&
                                        appManageChForest(plot.x, plot.y) == 0)
                                        NPVTmp -= appDat_all[plot.asID].currentYearRoadInvestment +
                                                  (meanRoadDensityProduction - roadMultifunctional -
                                                   appDat_all[plot.asID].constructedRoadsMultifunction) *
                                                  forestRoadConstructionCostsEuroM * costsScaling;

                                    if (abs(countryHarvestTmp - countryWoodDemand) < (1 + tolerance * adjustTolerance) *
                                                                                     abs(woodHarvest[plot.country] -
                                                                                         countryWoodDemand) &&
                                        NPVTmp > 0) {
                                        woodHarvest[plot.country] = countryHarvestTmp;
                                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;
                                        appManageChForest(plot.x, plot.y) = 1;

                                        appDat_all[plot.asID].constructedRoadsMultifunction =
                                                meanRoadDensityProduction - roadMultifunctional;
                                        appDat_all[plot.asID].harvestEfficiencyMultifunction = 0;

                                        if (!appForest30_policy) {
                                            appThinningForest30(plot.x, plot.y) = stockingDegree;
                                            appCohort30_all[plot.asID].setU(rotation);
                                            appCohort30_all[plot.asID].setStockingDegreeMin(stockingDegree * sdMinCoef);
                                            appCohort30_all[plot.asID].setStockingDegreeMax(stockingDegree * sdMaxCoef);
                                        }
                                    } else { // return old values
                                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                                        appThinningForest(plot.x, plot.y) = thinningForestTmp;
                                        appManagedForest(plot.x, plot.y) = managedForestTmp;
                                        appCohort_all[plot.asID].setU(rotationForestTmp);
                                        appCohort_all[plot.asID].setStockingDegreeMin(thinningForestTmp * sdMinCoef);
                                        appCohort_all[plot.asID].setStockingDegreeMax(thinningForestTmp * sdMaxCoef);

                                        rotationForestNew(plot.x, plot.y) = rotationForestTmpNew;
                                        thinningForestNew(plot.x, plot.y) = thinningForestTmpNew;
                                        appNewCohort_all[plot.asID].setU(rotationForestTmpNew);
                                        appNewCohort_all[plot.asID].setStockingDegreeMin(
                                                thinningForestTmpNew * sdMinCoef);
                                        appNewCohort_all[plot.asID].setStockingDegreeMax(
                                                thinningForestTmpNew * sdMaxCoef);
                                    }
                                }
                            }

                        } else if (woodHarvest[plot.country] > (1 + woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) > 0 && appManagedForest(plot.x, plot.y) < 3) {
                                double rotMaxBm = 0;
                                if (appDat_all[plot.asID].OBiomassPrev > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDat_all[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMaxBm = species[plot.speciesType - 1].getTOpt(appMaiForest(plot.x, plot.y),
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
                                appCohort_all[plot.asID].setU(rotation);
                                appCohort_all[plot.asID].setStockingDegree(-1);
                                rotationForestNew(plot.x, plot.y) = rotation;
                                appNewCohort_all[plot.asID].setU(rotation);

                                // New forest age > rotation -> change FM for the new forest
                                if (appNewCohort_all[plot.asID].getActiveAge() > rotation && thinningForestTmpNew > 0) {
                                    thinningForestNew(plot.x, plot.y) = -1;
                                    appNewCohort_all[plot.asID].setStockingDegree(-1);
                                }

                                double harvestO = appCohort_all[plot.asID].cohortRes();
                                double harvestN = appNewCohort_all[plot.asID].cohortRes();

                                double forestShareApplied =
                                        appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare;

                                if (!appForest30_policy)
                                    forestShareApplied += appDat_all[plot.asID].OForestShare30;

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp = ((cleanedWoodUse[plot.country] +
                                                         appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                                        harvestO * forestShareApplied + (cleanedWoodUse[plot.country] +
                                                                                         appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                                                                        harvestN *
                                                                                        appDat_all[plot.asID].AForestShare) *
                                                       appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                       appDat_all[plot.asID].deforWoodTotM3;

                                double countryHarvestTmp =
                                        woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                         rotationForestTmp, thinningForestTmp, rotationForestTmpNew,
                                         thinningForestTmpNew, managedForestTmp);
                            }
                        }
                    }

            for (const auto &plot: appPlots)
                if (toAdjust.contains(plot.country) && plot.protect.data.at(2000) == 0)
                    if (appDat_all[plot.asID].OForestShareU > 0) {
                        double countryWoodDemand = appWoodDemand.at(plot.country)(year);

                        double rotationForestTmp = appRotationForest(plot.x, plot.y);
                        int8_t managedForestTmp = appManagedForest(plot.x, plot.y);
                        double thinningForestTmp = appThinningForest(plot.x, plot.y);

                        double rotationForestTmpNew = appCohort_all[plot.asID].getU();
                        double thinningForestTmpNew = appNewCohort_all[plot.asID].getStockingDegree();

                        if (woodHarvest[plot.country] > (1 - woodProdTolerance) * countryWoodDemand)
                            if (appManagedForest(plot.x, plot.y) > 0) {
                                double rotMaxBm = 0;
                                if (appDat_all[plot.asID].OBiomassPrev > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDat_all[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMaxBm = species[plot.speciesType - 1].getTOpt(appMaiForest(plot.x, plot.y),
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
                                appCohort_all[plot.asID].setU(rotation);
                                appCohort_all[plot.asID].setStockingDegree(-1);
                                rotationForestNew(plot.x, plot.y) = rotation;
                                appNewCohort_all[plot.asID].setU(rotation);

                                // New forest age > rotation -> change FM for the new forest
                                if (appNewCohort_all[plot.asID].getActiveAge() > rotation && thinningForestTmpNew > 0) {
                                    thinningForestNew(plot.x, plot.y) = -1;
                                    appNewCohort_all[plot.asID].setStockingDegree(-1);
                                }

                                double harvestO = appCohort_all[plot.asID].cohortRes();
                                double harvestN = appNewCohort_all[plot.asID].cohortRes();

                                double forestShareApplied =
                                        appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare;

                                if (!appForest30_policy)
                                    forestShareApplied += appDat_all[plot.asID].OForestShare30;

                                // Total current harvested wood in the cell, m3
                                double newHarvestTmp = ((cleanedWoodUse[plot.country] +
                                                         appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                                        harvestO * forestShareApplied + (cleanedWoodUse[plot.country] +
                                                                                         appDat_all[plot.asID].harvestEfficiencyMultifunction) *
                                                                                        harvestN *
                                                                                        appDat_all[plot.asID].AForestShare) *
                                                       appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                       appDat_all[plot.asID].deforWoodTotM3;

                                double countryHarvestTmp =
                                        woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                         rotationForestTmp, thinningForestTmp, rotationForestTmpNew,
                                         thinningForestTmpNew, managedForestTmp);
                            }
                    }
        }

        // Adjust rotation time depending on wood demand and the state of the forests
        // CPol = True - used for non-zero C price
        // allMng = False - shorten rotation in more productive forests if we need to increase wood harvest
        //                  and extend rotation in less productive forests if we need to decrease wood harvest
        // allMng = True - adjust rotation in forests
        void adjustRT(const uint16_t year, const double woodProdTolerance, const span<double> woodHarvest,
                      const double priceC, const bool CPol = false, const bool allMng = false) {

            auto CPolPart = [&](const DataStruct &plot, const double rotation, const double countryHarvestTmp,
                                const double countryWoodDemand, const double newHarvestTmp,
                                const double rotationForestTmp, const double NPVTmp0) -> void {
                if (CPol) {
                    double NPVTmp = 1e20;

                    if (countriesFmcpol.contains(plot.country) &&
                        !countriesNoFmCPol.contains(plot.country)) {
                        appCoefPriceC = priceC < 0 ? appCO2Price.at(plot.country)(year) : priceC;
                        appCoefPriceC *= plot.corruption.data.at(2000);

                        bool used = appThinningForest(plot.x, plot.y) > 0;
                        // fmHurdle as wpMult in the future
                        NPVTmp = npvCalc(plot, appCohort_all[plot.asID], year, rotation,
                                         used).first;
                    }

                    if (abs(countryHarvestTmp - countryWoodDemand) <
                        (1 + tolerance) * abs(woodHarvest[plot.country] - countryWoodDemand) &&
                        NPVTmp >= maxNPVGrid(plot.x, plot.y)) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;

                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appCohort_all[plot.asID].setU(rotationForestTmp);
                        appNewCohort_all[plot.asID].setU(rotationForestTmp);
                    }
                } else {
                    double NPVTmp1 = npvCalc(plot, appCohort_all[plot.asID], year, rotation, true,
                                             1, true).first;
                    if (abs(countryHarvestTmp - countryWoodDemand) <
                        (1 + tolerance) * abs(woodHarvest[plot.country] - countryWoodDemand) &&
                        NPVTmp1 > 0 && NPVTmp1 >= (1 - npvLoss) * NPVTmp0) {
                        woodHarvest[plot.country] = countryHarvestTmp;
                        appHarvestGrid(plot.x, plot.y) = newHarvestTmp;

                    } else { // return old values
                        appRotationForest(plot.x, plot.y) = rotationForestTmp;
                        appCohort_all[plot.asID].setU(rotationForestTmp);
                        appNewCohort_all[plot.asID].setU(rotationForestTmp);
                    }
                }
            };

            for (const auto &plot: appPlots)
                if (toAdjust.contains(plot.country) && plot.protect.data.at(2000) == 0)
                    if (appDat_all[plot.asID].OForestShareU > 0) {
                        double countryWoodDemand = appWoodDemand.at(plot.country)(year);

                        double rotationForestTmp = appRotationForest(plot.x, plot.y);

                        if (woodHarvest[plot.country] < (1 - woodProdTolerance) * countryWoodDemand) {
                            if (appManagedForest(plot.x, plot.y) >= 2 && !allMng ||
                                appManagedForest(plot.x, plot.y) > 0 && allMng) {

                                double rotMAI = 0;
                                if (appDat_all[plot.asID].OBiomassPrev > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDat_all[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMAI = species[plot.speciesType - 1].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                    ORT::MAI);

                                if (appRotationForest(plot.x, plot.y) != rotMAI) {
                                    double NPVTmp0 = CPol ? 0 : npvCalc(plot, appCohort_all[plot.asID], year,
                                                                        appRotationForest(plot.x, plot.y), true, 1,
                                                                        true).first;

                                    double rotation = 0;
                                    if (rotMAI < appRotationForest(plot.x, plot.y)) // TEST for AT
                                        rotation = max(appRotationForest(plot.x, plot.y) - 5, rotMAI);
                                    else if (rotMAI > appRotationForest(plot.x, plot.y))
                                        rotation = min(appRotationForest(plot.x, plot.y) + 5, rotMAI);

                                    appRotationForest(plot.x, plot.y) = rotation;
                                    appCohort_all[plot.asID].setU(rotation);
                                    appNewCohort_all[plot.asID].setU(rotation);

                                    double harvestO = appCohort_all[plot.asID].cohortRes();
                                    double harvestN = appNewCohort_all[plot.asID].cohortRes();

                                    double forestShareApplied =
                                            appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare;

                                    if (!appForest30_policy)
                                        forestShareApplied += appDat_all[plot.asID].OForestShare30;

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp = (harvestO * forestShareApplied +
                                                            harvestN * appDat_all[plot.asID].AForestShare) *
                                                           appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                           appDat_all[plot.asID].deforWoodTotM3;

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
                                if (appDat_all[plot.asID].OBiomassPrev > 0 && plot.CAboveHa > 0 &&
                                    appMaiForest(plot.x, plot.y) > 0 || appDat_all[plot.asID].prevPlantPhytHaBmGr > 0)
                                    rotMaxBmTh = species[plot.speciesType - 1].getTOptT(appMaiForest(plot.x, plot.y),
                                                                                        ORT::MaxBm);

                                if (appRotationForest(plot.x, plot.y) < rotMaxBmTh) {
                                    double NPVTmp0 = CPol ? 0 : npvCalc(plot, appCohort_all[plot.asID], year,
                                                                        appRotationForest(plot.x, plot.y), true, 1,
                                                                        true).first;

                                    double rotation = min(appRotationForest(plot.x, plot.y) + 5, rotMaxBmTh);

                                    appRotationForest(plot.x, plot.y) = rotation;
                                    appCohort_all[plot.asID].setU(rotation);
                                    appNewCohort_all[plot.asID].setU(rotation);

                                    double harvestO = appCohort_all[plot.asID].cohortRes();
                                    double harvestN = appNewCohort_all[plot.asID].cohortRes();

                                    double forestShareApplied =
                                            appDat_all[plot.asID].OForestShareU - appDat_all[plot.asID].deforestShare;

                                    if (!appForest30_policy)
                                        forestShareApplied += appDat_all[plot.asID].OForestShare30;

                                    // Total current harvested wood in the cell, m3
                                    double newHarvestTmp = (harvestO * forestShareApplied +
                                                            harvestN * appDat_all[plot.asID].AForestShare) *
                                                           appDat_all[plot.asID].landAreaHa * plot.fTimber(coef.bYear) +
                                                           appDat_all[plot.asID].deforWoodTotM3;

                                    double countryHarvestTmp =
                                            woodHarvest[plot.country] + newHarvestTmp - appHarvestGrid(plot.x, plot.y);

                                    CPolPart(plot, rotation, countryHarvestTmp, countryWoodDemand, newHarvestTmp,
                                             rotationForestTmp, NPVTmp0);
                                }
                            }
                        }
                    }
        }
    };
}

#endif
