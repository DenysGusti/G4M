#ifndef G4M_EUROPE_DG_FOREST_GUI_EUROPE_PARAM_DW_5_3_DG_HPP
#define G4M_EUROPE_DG_FOREST_GUI_EUROPE_PARAM_DW_5_3_DG_HPP

#include <iostream>
#include <array>
#include <string>
#include <set>
#include <unordered_set>

#include "../../log.hpp"
#include "../../diagnostics/debugging.hpp"
#include "../../diagnostics/timer.hpp"
#include "../abstract/application.hpp"

#include "../../start_data/start_data.hpp"

#include "../../structs/lw_price.hpp"
#include "../../structs/harvest_residues.hpp"

#include "../../misc/concrete/ffipolm.hpp"

using namespace std;

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
            initMaiClimateShifters();
            applyMAIClimateShifters();
            modifyDisturbances();
            initGlobiomLandLocal();
            initManagedForestLocal();
        }

        // start calculations
        void Run() override {
            INFO("Application {} is running", appName);

            // loop by years
            for (uint16_t year = coef.bYear; year <= coef.eYear; ++year) {
                INFO("Processing year {}", year);
                uint16_t age = year - coef.bYear;

                if (year == forPolicyYearBioclima) {
                    appForest10_policy = forest10_policyKey;
                    appForest30_policy = forest30_policyKey;
                    appMultifunction10 = multifunction10Key;
                    appMultifunction30 = multifunction30Key;
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
                adjustManagedForest(year);
            }
        }

    protected:
        string appName = format("{}_{}_{}_{}", args[1], args[2], args[3], args[4]);
        Timer timer{appName};

        array<string, 3> c_scenario = {args[1], args[2], args[3]};
        int inputPriceC = stoi(args[4]);
        string full_scenario = c_scenario[0] + '_' + c_scenario[1] + '_' + c_scenario[2];
        string local_suffix = string{suffix} + full_scenario + (inputPriceC == 0 ? "_Pco2_0" : "");
        string suffix0 = string{suffix} + c_scenario[1] + '_' + c_scenario[2];

        // Apply the MAI climate shifters starting from MAIClimateShiftYear
        bool MAIClimateShift = c_scenario[2].contains("RCP");


        vector<DataStruct> appPlots = plots;

        datamapType appLandPrice;
        datamapType appWoodPrice;
        datamapType appWoodDemand;
        datamapType appResiduesDemand;

        datamapType appGlobiomAfforMaxCountry;
        datamapType appGlobiomLandCountry;

        // wood from outside forests in Belgium to cover the inconsistency between FAOSTAT removals and Forest Europe increment and felling
        Ipol<double> woodSupplement;

        simuIdType appDisturbWind = disturbWind;
        simuIdType appDisturbFire = disturbFire;
        simuIdType appDisturbBiotic = disturbBiotic;

        simuIdType appMaiClimateShifters;

        simuIdType appGlobiomAfforMax;
        simuIdType appGlobiomLand;

        // Initializing forest cover array by gridcells
        DataGrid<double> thinningForestNew{resLatitude};
        DataGrid<double> rotationForestNew{resLatitude};

        DataGrid<double> appHarvestGrid = harvestGrid;
        DataGrid<double> appMaiForest = maiForest;
        DataGrid<double> appRotationForest = rotationForest;
        DataGrid<double> appThinningForest = thinningForest;
        DataGrid<double> appThinningForest10 = thinningForest10;
        DataGrid<double> appThinningForest30 = thinningForest30;
        DataGrid<double> appOForestShGrid = OForestShGrid;

        DataGrid<int8_t> appDecisionGrid = decisionGrid;
        DataGrid<int8_t> appManagedForest = managedForest;
        DataGrid<int8_t> appManageChForest = manageChForest;
        DataGrid<int8_t> appRotationType = rotationType;
        DataGrid<int8_t> appUnmanaged = unmanaged;

        vector<AgeStruct> appCohort_all = cohort_all;
        vector<AgeStruct> appNewCohort_all = newCohort_all;
        vector<AgeStruct> appCohort10_all = cohort10_all;
        vector<AgeStruct> appCohort30_all = cohort30_all;
        vector<AgeStruct> appCohort_primary_all = cohort_primary_all;

        vector<Dat> appDat_all = dat_all;

        bool appForest10_policy = forest10_policy;
        bool appForest30_policy = forest30_policy;
        bool appMultifunction10 = multifunction10;
        bool appMultifunction30 = multifunction30;

        array<double, numberOfCountries> harvestResiduesSoilEmissions{};    // Soil loss emissions resulting from extraction of harvest residues, MtCO2/year
        array<double, numberOfCountries> residueHarvest{};                  // Extraction of harvest residues, tC

        datamapType mergeDatamap(const heterDatamapScenariosType &datamapScenarios, const string_view message) {
            // Swiss project 21.04.2022, Nicklas Forsell
            datamapType datamapDest = datamapScenarios.at(full_scenario);
            datamapType histDatamap = datamapScenarios.at(s_bauScenario);

            for (auto &[id, ipol]: datamapDest)
                ipol.data.merge(histDatamap.at(id).data);

            TRACE("Merged {}:", message);
            for (const auto &[id, ipol]: datamapDest)
                TRACE("{}\n{}", idCountryGLOBIOM[id], ipol.str());

            TRACE("Obsolete {}:", message);
            for (const auto &[id, ipol]: histDatamap)
                TRACE("{}\n{}", idCountryGLOBIOM[id], ipol.str());

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

        void initMaiClimateShifters() {
            if (!maiClimateShiftersScenarios.contains(c_scenario[2])) {
                WARN("maiClimateShifters doesn't contain c_scenario[2]: {}", c_scenario[2]);
                return;
            }

            appMaiClimateShifters = maiClimateShiftersScenarios[c_scenario[2]];
        }

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

        void modifyDisturbances() noexcept {
            if (!disturbanceClimateSensitive) {
                INFO("disturbanceClimateSensitive is turned off");
                return;
            }

            if (c_scenario[2].contains("7p0")) {
                for (auto &[id, ipol]: appDisturbFire)
                    ipol *= 2;
                for (auto &[id, ipol]: appDisturbBiotic)
                    ipol *= 2;
            } else if (c_scenario[2].contains("8p5")) {
                for (auto &[id, ipol]: appDisturbFire)
                    ipol *= 2.5;
                for (auto &[id, ipol]: appDisturbBiotic)
                    ipol *= 2.5;
            } else
                INFO("c_scenario[2] ({}) doesn't contain \"7p0\" or \"8p5\"", c_scenario[2]);
        }

        void initGlobiomLandLocal() noexcept {
            // file_globiom_LC_correction is not implemented
            double sumGrLnd_protect = 0;

            for (auto &plot: appPlots)
                if (plot.protect.data[2000] == 0) {

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
                            plot.afforMax.data[2000] = max(0., plot.afforMax.data[2000] + plot.grLnd_protect);
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

        void adjustManagedForest(uint16_t year) {
            double stockingDegree = 1.3;    // test for Belgium
            bool harvControl = true;        // Additional information to control output of the fm_cpol module
            bool NPV_postControl_0 = false; // Control of old forest management NPV at 0 C price: Use only for testing!!!!
            bool NPV_postControl = true;    // Control of old forest management NPV at non-zero C price
            uint8_t country_to_check = 0;       // Country code for deep analysis of G4M FM algorithm; 0 - no output of the deep analysis
            auto toAdjust = countriesList;
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
                    switch2Conserved(10, appCohort10_all, multifunction10);
                if (appForest30_policy)
                    switch2Conserved(30, appCohort30_all, multifunction30);
            }

            // Calculate salvage logging additional to historical
            if (disturbanceTrend && year > 2020 || disturbanceExtreme && year == disturbanceExtremeYear)
                disturbanceDamageHarvest(year);
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
                            rotMaxBm = species[plot.speciesType - 1].getTOpt(appMaiForest(plot.x, plot.y), optimMaxBm);
                        }

                        //  TO BE CHANGED to specific cleanedWoodUse10[...] / cleanedWoodUse30[...]
                        double cleanedWoodUseCurrent = cleanedWoodUse[plot.country];

                        if (biomass > 0 && appMaiForest(plot.x, plot.y) > 0 && SD != 0) {  // TODO SD != 0? & branches?
                            double harvestTmp = appHarvestGrid(plot.x, plot.y);
                            auto cohortTmp = cohortVec[plot.asID];
                            auto resTmp = cohortTmp.aging();
                            double harvestO = AgeStruct::cohortRes(cohortTmp.getArea(), resTmp);
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
                                        thinningForest10(plot.x, plot.y) = -1 : thinningForest30(plot.x, plot.y) = -1;
                            }

                            if (multifunction) {
                                double cleanedWoodUseNew = cleanedWoodUse[plot.country];

                                if (forestConcerned == 10 && appForest10_policy)
                                    cleanedWoodUseNew *= cleanWoodUseShare10;
                                else if (forestConcerned == 30 && appForest30_policy)
                                    cleanedWoodUseNew *= cleanWoodUseShare30;

                                harvestTmp = appHarvestGrid(plot.x, plot.y);
                                cohortTmp = cohortVec[plot.asID];
                                resTmp = cohortTmp.aging();
                                harvestO = AgeStruct::cohortRes(cohortTmp.getArea(), resTmp);
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
                    damagedWind = disturbWind[plot.simuID](year) * reciprocalFTimber;
                    damagedFire = disturbFire[plot.simuID](year) * reciprocalFTimber;
                    damagedBiotic = disturbBiotic[plot.simuID](year) * reciprocalFTimber;
                } else if (disturbanceExtreme && year == disturbanceExtremeYear) {
                    damagedWind = disturbWindExtreme[plot.simuID](year) * reciprocalFTimber;
                    damagedFire = disturbFireExtreme[plot.simuID](year) * reciprocalFTimber;
                    damagedBiotic = disturbBioticExtreme[plot.simuID](year) * reciprocalFTimber;
                }

                double cleanedWoodUseCurrent10 = 0;
                if (!appForest10_policy)
                    cleanedWoodUseCurrent10 =
                            cleanedWoodUse[plot.country] + appDat_all[plot.simuID].harvestEfficiencyMultifunction;
                else if (appMultifunction10)
                    cleanedWoodUseCurrent10 = cleanWoodUseShare10 * cleanedWoodUse[plot.country];

                double cleanedWoodUseCurrent30 = 1;
                if (forest30_policy) {
                    cleanedWoodUseCurrent30 = multifunction30 ? cleanWoodUseShare30 * cleanedWoodUse[plot.country] : 0;
                } else if (thinningForest30(plot.x, plot.y) < 0) {
                    cleanedWoodUseCurrent30 =
                            cleanedWoodUse[plot.country] + dat_all[plot.asID].harvestEfficiencyMultifunction;
                }

                double harvestableFire = 0;
                double harvestableWind = 0;
                double harvestableBiotic = 0;

                if (plot.protect.data.at(2000) == 0) {
                    harvestableFire = 0.25;  // after consulting CBM database, email by Viorel Blujdea 26.07.2023 // Uncertain
                    harvestableWind = 0.7;
                    harvestableBiotic = 0.95;
                }
            }
        }
    };
}

#endif
