#ifndef G4M_EUROPE_DG_PRINTING_HPP
#define G4M_EUROPE_DG_PRINTING_HPP

#include <string>

#include "../start_data/start_data.hpp"
#include "../settings/constants.hpp"
#include "../settings/arrays/arrays.hpp"
#include "../log.hpp"

using namespace std;
using namespace g4m::StartData;

namespace g4m::DataIO::printing {
    void printPlots() noexcept {
        TRACE("Plots:");
        for (size_t i = 0; const auto &plot: commonPlots)
            TRACE("plots[{}]:\n{}", i++, plot.str());
    }

    void printCountryLandArea() noexcept {
        for (const auto &[i, area]: countryLandArea | rv::enumerate)
            TRACE("{} area = {}", idCountryGLOBIOM.at(i), area);
    }

    void printNuts2Id() noexcept {
        for (const auto &[cords, NUTS2]: nuts2id)
            TRACE("x = {}, y = {}, NUTS2 = {}", cords.first, cords.second, NUTS2);
    }

    void printData() noexcept {
        printPlots();
        datamapScenarios.printData();
        printCountryLandArea();
        printNuts2Id();
        simuIdScenarios.printData();
    }
}

#endif
