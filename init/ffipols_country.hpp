#ifndef G4M_EUROPE_DG_FFIPOLS_COUNTRY_HPP
#define G4M_EUROPE_DG_FFIPOLS_COUNTRY_HPP

#include "../misc/concrete/ipol.hpp"
#include "../misc/concrete/ffipol.hpp"
#include "../log.hpp"
#include "../arrays/arrays.hpp"

using namespace std;
using namespace g4m::misc::concrete;
using namespace g4m::Arrays;

namespace g4m::init {
    // 1-harvesting losses (depending on d) in relation to standing timber (Vorratsfestmeter) for a country
    class FFIpolsCountry {
    public:
        explicit FFIpolsCountry(const uint8_t countryId_) {
            setupHarvestLosses(countryId_);
        }

        // set up country specific harvest losses as a share of felling
        // Nicklas' suggestion for the EUCLIMIT 2015 project, 05.01.2016
        void setupHarvestLosses(const uint8_t countryId_) noexcept {
            countryId = countryId_;

            Ipol<double> hlv;       // 1-Ernteverluste Vornutzung
            Ipol<double> hle;       // 1-Ernteverluste Endnutzung

            hlv.data[0] = 0;
            hlv.data[countryLossesD[countryId - 1]] = 1 - countryLosses[countryId - 1];
            hlv.data[25] = 1 - countryLosses[countryId - 1];
            hlv.data[50] = 1 - 0.7 * countryLosses[countryId - 1];

            hle = hlv;

            ffhlv.overwrite(hlv);
            ffhle.overwrite(hle);
        }

        [[nodiscard]] uint8_t getCountryId() const noexcept {
            return countryId;
        }

        // 1-harvesting losses thinning (Vornutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
        [[nodiscard]] const IIpol<double> &getHlv() const noexcept {
            return ffhlv;
        }

        // 1-harvesting losses final felling (Endnutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
        [[nodiscard]] const IIpol<double> &getHle() const noexcept {
            return ffhle;
        }

    private:
        uint8_t countryId = 0;
        FFIpol<double> ffhlv; // 1-harvesting losses thinning (Vornutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
        FFIpol<double> ffhle; // 1-harvesting losses final felling (Endnutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
    };

}

#endif
