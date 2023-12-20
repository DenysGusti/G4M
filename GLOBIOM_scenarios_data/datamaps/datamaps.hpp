#ifndef G4M_EUROPE_DG_DATAMAPS_HPP
#define G4M_EUROPE_DG_DATAMAPS_HPP

#include "unordered_map"

#include "datamap_scenarios.hpp"

using namespace std;
using namespace g4m::misc::concrete;

namespace g4m::GLOBIOM_scenarios_data {
    class Datamaps {
    public:
        static datamapType mergeDatamap(const datamapScenariosType &datamapScenarios, const string_view full_scenario,
                                        const string_view message) {
            // Swiss project 21.04.2022, Nicklas Forsell
            datamapType datamapDest = datamapScenarios.at(string{full_scenario});
            datamapType histDatamap = datamapScenarios.at(bauScenario);

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

        static datamapType
        mergeObligatoryDatamap(const datamapScenariosType &datamapScenarios, const string_view full_scenario,
                               const string_view message) {
            if (!datamapScenarios.contains(full_scenario)) {
                FATAL("{} is not filled in, check scenarios!!!, full_scenario = {}", message, full_scenario);
                throw runtime_error{"no scenario in datamapScenarios"};
            }

            return mergeDatamap(datamapScenarios, full_scenario, message);
        }

        static datamapType
        mergeOptionalDatamap(const datamapScenariosType &datamapScenarios, const string_view full_scenario,
                             const string_view message) {
            if (!datamapScenarios.contains(full_scenario)) {
                WARN("{} is not filled in, check scenarios!!!, full_scenario = {}", message, full_scenario);
                return {};
            }

            return mergeDatamap(datamapScenarios, full_scenario, message);
        }

        datamapType landPrice;
        datamapType woodPrice;
        datamapType woodDemand;
        datamapType residuesDemand;

        datamapType CO2Price;

        datamapType GLOBIOM_AfforMaxCountry;
        datamapType GLOBIOM_LandCountry;

        Datamaps(const DatamapScenarios &datamapScenarios, const string_view scenario, const int inputPriceC) {
            mergeObligatoryDatamaps(datamapScenarios, scenario);
            mergeOptionalDatamaps(datamapScenarios, scenario);
            initCO2Price(datamapScenarios, scenario, inputPriceC);
        }

    private:
        void mergeObligatoryDatamaps(const DatamapScenarios &datamapScenarios, const string_view scenario) {
            landPrice = mergeObligatoryDatamap(datamapScenarios.landPriceScenarios, scenario, "Land Price");
            woodPrice = mergeObligatoryDatamap(datamapScenarios.woodPriceScenarios, scenario, "Wood Price");
            woodDemand = mergeObligatoryDatamap(datamapScenarios.woodDemandScenarios, scenario, "Wood Demand");
            residuesDemand = mergeObligatoryDatamap(datamapScenarios.residuesDemandScenarios, scenario,
                                                    "Residues Demand");
        }

        void mergeOptionalDatamaps(const DatamapScenarios &datamapScenarios, const string_view scenario) {
            GLOBIOM_AfforMaxCountry = mergeOptionalDatamap(datamapScenarios.GLOBIOM_AfforMaxCountryScenarios, scenario,
                                                           "GLOBIOM Affor Max Country");
            GLOBIOM_LandCountry = mergeOptionalDatamap(datamapScenarios.GLOBIOM_LandCountryScenarios, scenario,
                                                       "GLOBIOM Land Country");
        }

        void initCO2Price(const DatamapScenarios &datamapScenarios, const string_view scenario, const int inputPriceC) {
            if (inputPriceC == 0) {
                INFO("CO2PriceScenarios is not used, inputPriceC = 0");
                return;
            }

            if (!datamapScenarios.CO2PriceScenarios.contains(scenario)) {
                FATAL("CO2PriceScenarios is not filled in, check scenarios!!!, full_scenario = {}", scenario);
                throw runtime_error{"no scenario in CO2PriceScenarios"};
            }

            CO2Price = datamapScenarios.CO2PriceScenarios.at(string{scenario});
        }
    };
}

#endif
