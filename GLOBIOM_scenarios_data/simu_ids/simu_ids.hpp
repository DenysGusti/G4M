#ifndef G4M_EUROPE_DG_SIMU_IDS_HPP
#define G4M_EUROPE_DG_SIMU_IDS_HPP

#include "simu_id_scenarios.hpp"

using namespace std;
using namespace g4m::misc::concrete;

namespace g4m::GLOBIOM_scenarios_data {
    class SimuIds {
    public:
        [[nodiscard]] static simuIdType
        mergeOptionalSimuId(const simuIdScenariosType &simuIdScenarios, const string_view scenario,
                            const string_view message) {
            if (!simuIdScenarios.contains(scenario)) {
                WARN("{} is not filled in, check scenarios!!!, full_scenario = {}", message, scenario);
                return {};
            }

            simuIdType simuIdDest = simuIdScenarios.at(string{scenario});
            simuIdType histSimuId = simuIdScenarios.at(settings.bauScenario);

            bool obsolete = false;

            for (auto &[id, ipol]: simuIdDest) {
                if (histSimuId.contains(id)) {
                    ipol.data.merge(histSimuId.at(id).data);
                    obsolete |= !histSimuId.at(id).data.empty();
                } else
                    WARN("Country {} ({}) is missing in {} historical simuId ({}) albeit it exists in scenario simuId ({})!",
                         id, idCountryGLOBIOM.at(id), message, settings.bauScenario, message);
            }

            TRACE("Merged {}:", message);
            for (const auto &[id, ipol]: simuIdDest)
                TRACE("{}\n{}", idCountryGLOBIOM.at(id), ipol.str());

            if (obsolete) {
                TRACE("Obsolete {}:", message);
                for (const auto &[id, ipol]: histSimuId)
                    if (!ipol.data.empty())
                        TRACE("{}\n{}", idCountryGLOBIOM.at(id), ipol.str());
            }

            return simuIdDest;
        }

        simuIdType maiClimateShifters;

        simuIdType GLOBIOM_AfforMax;
        simuIdType GLOBIOM_Land;

        simuIdType disturbWind;
        simuIdType disturbFire;
        simuIdType disturbBiotic;

        simuIdType disturbWindExtreme;
        simuIdType disturbFireExtreme;
        simuIdType disturbBioticExtreme;

        SimuIds(const SimuIdScenarios &simuIdScenarios, const string_view scenario, const string_view c_scenario_1)
                : disturbWind{simuIdScenarios.disturbWind},
                  disturbFire{simuIdScenarios.disturbFire},
                  disturbBiotic{simuIdScenarios.disturbBiotic},
                  disturbWindExtreme{simuIdScenarios.disturbWindExtreme},
                  disturbFireExtreme{simuIdScenarios.disturbFireExtreme},
                  disturbBioticExtreme{simuIdScenarios.disturbBioticExtreme} {
            mergeOptionalSimuIds(simuIdScenarios, scenario);
            initMaiClimateShifters(simuIdScenarios, c_scenario_1);
            modifyDisturbances(c_scenario_1);
        }

    private:
        void mergeOptionalSimuIds(const SimuIdScenarios &simuIdScenarios, const string_view scenario) {
            GLOBIOM_AfforMax = mergeOptionalSimuId(simuIdScenarios.GLOBIOM_AfforMaxScenarios, scenario,
                                                   "GLOBIOM Affor Max Country");
            GLOBIOM_Land = mergeOptionalSimuId(simuIdScenarios.GLOBIOM_LandScenarios, scenario, "GLOBIOM Land Country");
        }

        void initMaiClimateShifters(const SimuIdScenarios &simuIdScenarios, const string_view c_scenario_1) {
            if (!simuIdScenarios.maiClimateShiftersScenarios.contains(c_scenario_1)) {
                WARN("maiClimateShifters doesn't contain c_scenario[1]: {}", c_scenario_1);
                return;
            }

            maiClimateShifters = simuIdScenarios.maiClimateShiftersScenarios.at(string{c_scenario_1});
        }

        // Make projected forest damage sensitive to the climate change scenario
        void modifyDisturbances(const string_view c_scenario_1) noexcept {
            if (!disturbanceClimateSensitive) {
                INFO("disturbanceClimateSensitive is turned off");
                return;
            }

            if (c_scenario_1.contains("7p0")) {
                for (auto &[id, ipol]: disturbFire)
                    ipol *= 2;
                for (auto &[id, ipol]: disturbBiotic)
                    ipol *= 2;
            } else if (c_scenario_1.contains("8p5")) {
                for (auto &[id, ipol]: disturbFire)
                    ipol *= 2.5;
                for (auto &[id, ipol]: disturbBiotic)
                    ipol *= 2.5;
            } else
                INFO("c_scenario[1] ({}) doesn't contain \"7p0\" or \"8p5\"", c_scenario_1);
        }
    };
}

#endif
