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
            mergeObligatoryDatamaps();
            mergeOptionalDatamaps();
            mergeOptionalSimuIds();
            correctBelgium();
            initMaiClimateShifters();
            modifyDisturbances();
            initGlobiomLandLocal();
            initManagedForestLocal();
        }

        // start calculations
        void Run() override {
            INFO("Application {} is running", appName);
            INFO("Scenario to read in & GL: {}", full_scenario);
        }

    protected:
        string appName = format("{}_{}_{}_{}", args[1], args[2], args[3], args[4]);
        Timer timer{appName};

        array<string, 3> c_scenario = {args[1], args[2], args[3]};
        int inputPriceC = stoi(args[4]);
        string full_scenario = c_scenario[0] + '_' + c_scenario[1] + '_' + c_scenario[2];
        string local_suffix = string{suffix} + full_scenario + (inputPriceC == 0 ? "_Pco2_0" : "");
        string suffix0 = string{suffix} + c_scenario[1] + '_' + c_scenario[2];

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

        array<double, numberOfCountries> harvestResiduesSoilEmissions{};    // Soil loss emissions resulting from extraction of harvest residues, MtCO2/year
        array<double, numberOfCountries> residueHarvest{};                  // Extraction of harvest residues, tC

        datamapType mergeDatamap(const heterDatamapScenariosType &datamapScenarios, const string_view message) {
            // Swiss project 21.04.2022, Nicklas Forsell
            datamapType datamapDest = datamapScenarios.at(full_scenario);
            datamapType histDatamap = datamapScenarios.at(s_bauScenario);

            for (auto&[id, ipol]: datamapDest)
                ipol.data.merge(histDatamap.at(id).data);

            TRACE("Merged {}:", message);
            for (const auto &[id, ipol]: datamapDest)
                TRACE("{}\n{}", idCountryGLOBIOM[id], ipol.str());

            TRACE("Obsolete {}:", message);
            for (const auto &[id, ipol]: histDatamap)
                TRACE("{}\n{}", idCountryGLOBIOM[id], ipol.str());

            return datamapDest;
        }

        datamapType mergeObligatoryDatamap(const heterDatamapScenariosType &datamapScenarios, const string_view message) {
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

            for (auto&[id, ipol]: simuIdDest)
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
    };
}

#endif
