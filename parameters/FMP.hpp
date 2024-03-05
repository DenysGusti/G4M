#ifndef G4M_EUROPE_DG_FMP_HPP
#define G4M_EUROPE_DG_FMP_HPP

#include <unordered_map>
#include <memory>

#include "decisions.hpp"

#include "../init/data_struct.hpp"
#include "../settings/arrays/arrays.hpp"
#include "../log.hpp"

#include "../misc/concrete/ipol.hpp"
#include "../misc/concrete/ipolm.hpp"
#include "../misc/concrete/ffipol.hpp"
#include "../misc/concrete/ffipolm.hpp"

using namespace std;
using namespace g4m::misc::concrete;
using namespace g4m::Arrays;
using namespace g4m::init;

namespace g4m::parameters {
    // forest management parameters
    class FMP {
    public:
        // set up country specific harvest losses as a share of felling
        // Nicklas' suggestion for the EUCLIMIT 2015 project, 05.01.2016
        // returns hlv, hle
        // 1-harvesting losses thinning (Vornutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
        // 1-harvesting losses final felling (Endnutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
        static pair<FFIpol < double>, FFIpol<double> >

        setupHarvestLosses(const uint8_t countryId) noexcept {
            Ipol<double> fun_hlv;       // 1-Ernteverluste Vornutzung
            Ipol<double> fun_hle;       // 1-Ernteverluste Endnutzung

            // Georg's recommendation
            // 7: 0
            // 25: 1 - HarvestingLosesCountry
            // 50: 1 - (0.7 * HarvestingLosesCountry)
            // This is for hle, for the thinning (hlv) multiply the values of harvestable biomass with 0.8.
            fun_hlv.data[0] = 0;
            fun_hlv.data[27] = 0;  // Testing!
            fun_hlv.data[countryLossesD[countryId]] = 1 - countryLosses[countryId];
            fun_hlv.data[25] = 1 - countryLosses[countryId];
            fun_hlv.data[50] = 1 - 0.7 * countryLosses[countryId];

            fun_hle = fun_hlv;

            // 1-harvesting losses thinning (Vornutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
            FFIpol<double> hlv;
            // 1-harvesting losses final felling (Endnutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
            FFIpol<double> hle;

            hlv.overwrite(fun_hlv);
            hle.overwrite(fun_hle);
            return {hlv, hle};
        }

        FFIpol<double> sws; // sawn-wood share of harvested wood depending on dbh
        FFIpol<double> sdMaxH; // stocking degree depending on max tree height
        FFIpol<double> sdMinH; // stocking degree depending on max (min?) tree height

        // Thinning costs (depending on d and removed volume per hectare) in relation to standing timber (Vorratsfestmeter)
        FFIpolM<double> cov;
        // Harvesting costs depending on d and vol
        FFIpolM<double> coe;
        // do thinning (Vornutzung) (depending on d and removed volume per hectare) and final fellings (Endnutzung)
        // (depending on d and stocking volume per hectare) in relation to standing timber (Vorratsfestmeter)

        // 1-harvesting losses (depending on d) in relation to standing timber (Vorratsfestmeter) for countries
        unordered_map<uint8_t, pair<FFIpol <double>, FFIpol <double> > > hlveCountries;  // <country, (hlv, hle)>

        Decisions decisions;

        void initCountriesHLVE(const span<const DataStruct> plots) noexcept {
            hlveCountries.reserve(256);

            for (const auto &plot: plots)
                hlveCountries[plot.country] = setupHarvestLosses(plot.country);
        }

        // Setup forest management parameters similar for all countries (cells)
        void setupFMP() noexcept {
            Ipol<double> fun_sws;       // Schnittholzanteil an Vfm
            IpolM<double> fun_cov;      // costs Vornutzung
            IpolM<double> fun_coe;      // costs Endnutzung
            Ipol<double> fun_sdMaxH;    // sdMaxH
            Ipol<double> fun_sdMinH;    // sdMinH

            fun_sws.data[10] = 0;
            fun_sws.data[30] = 0.6;

            fun_sdMaxH.data[0] = 1;
            fun_sdMinH.data[0] = 1;

            // diameter, stocking volume [tC stem-wood/ha], share of harvest (0 - 1)
            fun_cov.data[{0, 2, 0.3}] = 4;
            fun_cov.data[{40, 30, 0.2}] = 2;

            // We allow thinning from Dbh 3 cm to allow more energy wood (discussion with Fulvio 14 May 2020)
            decisions = Decisions{
                    .diameterDovMin = 3,
                    .stemBiomassDovMin = 5,
                    .thinShareDovMin = 0.0000000000000000000001, // 0.001, // TODO Find  correct parameter!
                    .diameterDoeMin = 15,
                    .stemBiomassDoeMin = 10
            };
//        fun_dov.data[{0, 0, 0}] = 0;
//        fun_dov.data[{3, 5, 0.001}] = 0;  // Testing!
//        fun_dov.data[{10, 20, 0.01}] = 1;  // Testing!

            fun_coe.data[{0, 2}] = 3;
            fun_coe.data[{40, 30}] = 1;

//        fun_doe.data[{15, 10}] = 0;
//        fun_doe.data[{16, 11}] = 1;

            sws.overwrite(fun_sws);
            sdMinH.overwrite(fun_sdMinH);
            sdMaxH.overwrite(fun_sdMaxH);

            cov = FFIpolM<double>{fun_cov};
            coe = FFIpolM<double>{fun_coe};
        }
    };
}

#endif
