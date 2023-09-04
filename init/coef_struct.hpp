#ifndef G4M_EUROPE_DG_COEF_STRUCT_HPP
#define G4M_EUROPE_DG_COEF_STRUCT_HPP

#include <fstream>
#include <filesystem>
#include <ranges>

#include "../misc/concrete/ipol.hpp"
#include "../log.hpp"

using namespace std;
using namespace g4m::misc::concrete;

namespace g4m::init {

    struct CoefStruct {
        // Starting Year of simulation
        uint16_t bYear = 2000;
        // Ending Year of simulation
        uint16_t eYear = 2005;
        // Interaction between cells
        int16_t cellsInteract = 0;
        // Consider afforestation
        int16_t inclAffor = 0;
        // No pay
        int16_t noPay = 0;
        // Below-ground biomass
        int16_t uBiomass = 0;
        // Litter
        int16_t litter = 0;
        // Soil organic carbon
        int SOC = 0;
        //******************************************************************************
        //** parameters
        //******************************************************************************
        // Price-index of reference country
        double priceIndexE = 0;
        // Minimum Land-price [cash/ha]
        double priceLandMinR = 0;
        // Maximum Land-price [cash/ha]
        double priceLandMaxR = 0;
        // Factor Carbon uptake (DIMA-Model)
        double fCUptake = 0;
//        // Commercial timber-volume per ton of carbon [m3/tC]
//        double fTimber = 0;  // not used, but okay
        // HarvestingLosses (Share of losses during harvest)
        double harvLoos = 0;
        // Carbon price [Cash/tC] (9/25)
        double priceC = 0;
        // Share of Long-living products [0-1]
        double fracLongProd = 0;
        // Decrease rate of long living products
        double decRateL = 0;
        // Decrease rate of short living products
        double decRateS = 0;
        // Share of SlashBurn deforestation [0-1]
        double slashBurn = 0;
        // Frequency of aid payments (PRICECAID) [Years]
        double freqAid = 0;
        // Aid Carbon Price [Cash/tC/FREQAID] (6)
        double priceCAid = 0;
        // Maximum rotation time im Years
        double maxRotInter = 0;
        // Minimum rotation time im Years
        double minRotInter = 0;
        // Baseline
        double baseline = 0;
        // Maximum Timber-price [cash/m3]
        double priceTimberMaxR = 0;
        // Minimum Timber-price [cash/m3]
        double priceTimberMinR = 0;
        // Planting costs in reference country [Cash/ha]
        double plantingCostsR = 0;
        // Standardised Population-density [1-10]
        double sPopDens = 0;

        // reading coefficients
        void readCoef(const string_view fileName) {
            INFO("> Reading coefficients...");
            // Opening a file
            ifstream fp{filesystem::path{fileName}};

            if (!fp.is_open()) {
                FATAL("Cannot read {}", fileName);
                throw runtime_error{"Cannot read setting file"};
            }

            vector<string> s_args;

            for (string line, buf; fp;) {
                getline(fp, line);

                // Jump over lines starting with #
                if (line.empty() || line[0] == '#')
                    continue;

                s_args = line | rv::transform(::toupper) | rv::split(' ') | ranges::to<vector<string> >();

                if (s_args.size() != 2) {  // no ipol is currently used
                    WARN("Unused data: {}", line);
                    continue;
                }

                double value = stod(s_args[1]);

                if (s_args[0] == "BYEAR")
                    bYear = static_cast<uint16_t>(value);
                else if (s_args[0] == "EYEAR")
                    eYear = static_cast<uint16_t>(value);
                else if (s_args[0] == "CELLSINTERACT")
                    cellsInteract = static_cast<int16_t>(value);
                else if (s_args[0] == "INCLAFFOR")
                    inclAffor = static_cast<int16_t>(value);
                else if (s_args[0] == "NOPAY")
                    noPay = static_cast<int16_t>(value);
                else if (s_args[0] == "UBIOMASS")
                    uBiomass = static_cast<int16_t>(value);
                else if (s_args[0] == "LITTER")
                    litter = static_cast<int16_t>(value);
                else if (s_args[0] == "SOC")
                    SOC = static_cast<uint16_t>(value);
                else if (s_args[0] == "PRICELANDMINR")
                    priceLandMinR = value;
                else if (s_args[0] == "PRICELANDMAXR")
                    priceLandMaxR = value;
                else if (s_args[0] == "FCUPTAKE")
                    fCUptake = value;
//                else if (s_args[0] == "FTIMBER")
//                    fTimber = value;
                else if (s_args[0] == "HARVLOOS")
                    harvLoos = value;
                else if (s_args[0] == "PRICEC")
                    priceC = value;
                else if (s_args[0] == "FRACLONGPROD")
                    fracLongProd = value;
                else if (s_args[0] == "DECRATEL")
                    decRateL = value;
                else if (s_args[0] == "DECRATES")
                    decRateS = value;
                else if (s_args[0] == "SLASHBURN")
                    slashBurn = value;
                else if (s_args[0] == "FREQAID")
                    freqAid = value;
                else if (s_args[0] == "PRICECAID")
                    priceCAid = value;
                else if (s_args[0] == "MAXROTINTER")
                    maxRotInter = value;
                else if (s_args[0] == "MINROTINTER")
                    minRotInter = value;
                else if (s_args[0] == "BASELINE")
                    baseline = value;
                else if (s_args[0] == "PRICETIMBERMAXR")
                    priceTimberMaxR = value;
                else if (s_args[0] == "PRICETIMBERMINR")
                    priceTimberMinR = value;
                else if (s_args[0] == "PRICEINDEXE")
                    priceIndexE = value;
                else if (s_args[0] == "PLANTINGCOSTSR")
                    plantingCostsR = value;
                else if (s_args[0] == "SPOPDENS")
                    sPopDens = value;
                else
                    WARN("Unused data: {}", line);
            }
        }
    };
}

#endif
