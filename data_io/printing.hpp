#ifndef G4M_EUROPE_DG_PRINTING_HPP
#define G4M_EUROPE_DG_PRINTING_HPP

#include <string>

#include "../start_data/start_data.hpp"
#include "../constants.hpp"
#include "../arrays/arrays.hpp"
#include "../log.hpp"

using namespace std;
using namespace g4m::StartData;

namespace g4m::DataIO::printing {
    void printPlots() noexcept {
        TRACE("Plots:");
        for (size_t i = 0; const auto &plot: plots)
            TRACE("plots[{}]:\n{}", i++, plot.str());
    }

    void printDatamapScenarios(const heterDatamapScenariosType &datamapScenarios, const string_view message) noexcept {
        TRACE("{}", message);
        for (const auto &[scenario, datamap]: datamapScenarios) {
            TRACE("{}", scenario);
            for (const auto &[id, ipol]: datamap)
                TRACE("{}\n{}", idCountryGLOBIOM.at(id), ipol.str());
        }
    }

    void printCountryLandArea() noexcept {
        for (uint16_t i = 0; const auto area: countryLandArea)
            TRACE("{} area = {}", idCountryGLOBIOM.at(i++), area);
    }

    void printNuts2Id() noexcept {
        for (const auto &[cords, NUTS2]: nuts2id)
            TRACE("x = {}, y = {}, NUTS2 = {}", cords.first, cords.second, NUTS2);
    }

    void printSimuId(const simuIdType &simuIdDatamap, const string_view message) noexcept {
        TRACE("{}", message);
        for (const auto &[id, ipol]: simuIdDatamap)
            TRACE("{}\n{}", id, ipol.str());
    }

    void printSimuIdScenarios(const heterSimuIdScenariosType &simuIdScenarios, const string_view message) noexcept {
        TRACE("{}", message);
        for (const auto &[scenario, simuIdDatamap]: simuIdScenarios) {
            TRACE("{}", scenario);
            for (const auto &[simu_id, ipol]: simuIdDatamap)
                TRACE("{}\n{}", simu_id, ipol.str());
        }
    }

    void printData() noexcept {
        printPlots();

        printDatamapScenarios(landPriceScenarios, "Globiom scenarios Land Price");
        printDatamapScenarios(woodPriceScenarios, "Globiom scenarios Wood Price");
        printDatamapScenarios(woodDemandScenarios, "Globiom scenarios Wood Demand");
        printDatamapScenarios(residuesDemandScenarios, "Globiom scenarios Residues Demand");
        printDatamapScenarios(globiomAfforMaxCountryScenarios, "globiomAfforMaxCountryScenarios");
        printDatamapScenarios(globiomLandCountryScenarios, "globiomLandCountryScenarios");
        printDatamapScenarios(CO2PriceScenarios, "CO2PriceScenarios");

        printCountryLandArea();
        printNuts2Id();

        printSimuIdScenarios(globiomLandScenarios, "globiomLandScenarios");
        printSimuIdScenarios(globiomAfforMaxScenarios, "globiomAfforMaxScenarios");
        printSimuIdScenarios(maiClimateShiftersScenarios, "maiClimateShiftersScenarios");

        printSimuId(disturbWind, "disturbWind");
        printSimuId(disturbFire, "disturbFire");
        printSimuId(disturbBiotic, "disturbBiotic");
        printSimuId(disturbWindExtreme, "disturbWindExtreme");
        printSimuId(disturbFireExtreme, "disturbFireExtreme");
        printSimuId(disturbBioticExtreme, "disturbBioticExtreme");
    }
}

#endif
