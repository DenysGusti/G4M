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
        Ipol<float> PriceIndexE;
        // Minimum Land-price [cash/ha]
        Ipol<float> PriceLandMinR;
        // Maximum Land-price [cash/ha]
        Ipol<float> PriceLandMaxR;
        // Factor Carbon uptake (DIMA-Model)
        Ipol<float> FCuptake;
        // Commercial timber-volume per ton of carbon [m3/tC]
        //  Ipol<float,float> FTimber;
        // HarvestingLosses (Share of losses during harvest)
        Ipol<float> HarvLoos;
        // Carbon price [Cash/tC] (9/25)
        Ipol<float> PriceC;
        // Share of Long-living products [0-1]
        Ipol<float> FracLongProd;
        // Decrease rate of long living products
        Ipol<float> decRateL;
        // Decrease rate of short living products
        Ipol<float> decRateS;
        // Share of SlashBurn deforestation [0-1]
        Ipol<float> SlashBurn;
        // Frequency of aid payments (PRICECAID) [Years]
        Ipol<float> FreqAid;
        // Aid Carbon Price [Cash/tC/FREQAID] (6)
        Ipol<float> PriceCAid;
        // Maximum rotation time im Years
        Ipol<float> MaxRotInter;
        // Minimum rotation time im Years
        Ipol<float> MinRotInter;
        // Baseline
        Ipol<float> baseline;
        // Maximum Timber-price [cash/m3]
        Ipol<float> PriceTimberMaxR;
        // Minimum Timber-price [cash/m3]
        Ipol<float> PriceTimberMinR;
        // Planting costs in reference country [Cash/ha]
        Ipol<float> PlantingCostsR;
        // Standardised Population-density [1-10]
        Ipol<float> sPopDens;

        // reading coefficients
        void readCoef(const string_view fileName) {
            // Opening a file
            ifstream fp{filesystem::path{fileName}};
            if (!fp.is_open()) {
                FATAL("Cannot read {}", fileName);
                throw runtime_error{"Cannot read setting file"};
            }

            INFO("> Reading coefficients...");
            for (string line, buf; fp;) {
                getline(fp, line);
                //Jump over lines starting with #
                if (line.empty() || line[0] == '#')
                    continue;

                ranges::transform(line, line.begin(), ::toupper);
                stringstream ss{line};
                double val = 0;
                ss >> buf;
                const auto [tmp_s, tmp_d] = getNumber(buf);
                ss >> val;

                if (tmp_s == "BYEAR")
                    bYear = static_cast<uint16_t>(val);
                else if (tmp_s == "EYEAR")
                    eYear = static_cast<uint16_t>(val);
                else if (tmp_s == "CELLSINTERACT")
                    cellsInteract = static_cast<int16_t>(val);
                else if (tmp_s == "INCLAFFOR")
                    inclAffor = static_cast<int16_t>(val);
                else if (tmp_s == "NOPAY")
                    noPay = static_cast<int16_t>(val);
                else if (tmp_s == "UBIOMASS")
                    uBiomass = static_cast<int16_t>(val);
                else if (tmp_s == "LITTER")
                    litter = static_cast<int16_t>(val);
                else if (tmp_s == "SOC")
                    SOC = static_cast<uint16_t>(val);
                else if (tmp_s == "PRICELANDMINR")
                    PriceLandMinR.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "PRICELANDMAXR")
                    PriceLandMaxR.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "FCUPTAKE")
                    FCuptake.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
//                else if (tmp_s == "FTIMBER")
//                    FTimber.data[static_cast<float>(tmp_d)] = val;
                else if (tmp_s == "HARVLOOS")
                    HarvLoos.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "PRICEC")
                    PriceC.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "FRACLONGPROD")
                    FracLongProd.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "DECRATEL")
                    decRateL.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "DECRATES")
                    decRateS.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "SLASHBURN")
                    SlashBurn.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "FREQAID")
                    FreqAid.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "PRICECAID")
                    PriceCAid.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "MAXROTINTER")
                    MaxRotInter.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "MINROTINTER")
                    MinRotInter.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "BASELINE")
                    baseline.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "PRICETIMBERMAXR")
                    PriceTimberMaxR.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "PRICETIMBERMINR")
                    PriceTimberMinR.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "PRICEINDEXE")
                    PriceIndexE.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "PLANTINGCOSTSR")
                    PlantingCostsR.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else if (tmp_s == "SPOPDENS")
                    sPopDens.data[static_cast<float>(tmp_d)] = static_cast<float>(val);
                else
                    WARN("Unused data: {}\t{}", tmp_s, tmp_d);
            }
        }

    private:
        // Returns a pair of string, double;
        [[nodiscard]] static pair<string, double> getNumber(const string_view str) noexcept {
            string text;
            string number;
            for (const auto c: str) {
                if (c >= '0' && c <= '9' || c == '.')
                    number += c;
                else if (c != '[' && c != ']')
                    text += c;
            }
            double x = 0;
            stringstream ss{number};
            ss >> x;
            return {text, x};
        }
    };
}

#endif
