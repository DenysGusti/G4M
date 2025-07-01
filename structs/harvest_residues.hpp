#ifndef G4M_EUROPE_DG_HARVEST_RESIDUES_HPP
#define G4M_EUROPE_DG_HARVEST_RESIDUES_HPP

#include "residues_forest.hpp"
#include "../init/data_struct.hpp"
#include "../init/species.hpp"
#include "../settings/arrays/arrays.hpp"

namespace g4m::structs {
    struct HarvestResidues {
        uint32_t simuId = 0;                        // ID of simulation unit
        size_t asID = 0;                            // ID of G4M cell
        uint8_t country = 0;                        // ID of G4M country
        Species species = Species::NoTree;
        double fTimber = 0;                         // tC -> m3 conversion
        ResiduesForest U, O10, O30;
        double costsSuit1 = 0;                      // costs of harvesting the residuesSuit1 emissions including the transportation costs
        double costsSuit2 = 0;                      // costs of harvesting the residuesSuit2 emissions including the transportation costs
        double costsSuit3 = 0;
        double costsSuit4_notTaken = 0;             // costs of harvesting the residuesSuit1 emissions including the transportation costs from planes which are not used for wood production
        double costsTotal = 0;                      // Total costs of harvesting the residues (costsSuit1+costsSuit2+costsSuit1_notTaken)
        double emissionsSuit1 = 0;                  // soil loss CO2 emissions in the cell associated with residuesSuit1
        double emissionsSuit2 = 0;                  // soil loss CO2 emissions in the cell associated with residuesSuit2
        double emissionsSuit3 = 0;                  // soil loss CO2 emissions in the cell associated with residuesSuit3
        double emissionsSuit4_notTaken = 0;         // soil loss CO2 emissions in the cell associated with residuesSuit1_notTaken
        double useSuit1 = 0;                        // switch to use the Suit1 residues
        double useSuit2 = 0;                        // switch to use the Suit2 residues
        double useSuit3 = 0;                        // switch to use the Suit2 residues
        double useSuit4 = 0;                        // switch to use the Suit2 residues
        int emissionsTimeFrame = 0;                 // period of residue extraction for estimation of emissions
        bool protect = false;                       // protected cell

        bool em_harvRes_fcU = false;
        bool em_harvRes_thU = false;
        bool em_harvRes_fcN = false;
        bool em_harvRes_thN = false;
        bool em_harvRes_scU = false;
        bool em_harvRes_scN = false;

        bool em_harvRes_fc10 = false;
        bool em_harvRes_th10 = false;
        bool em_harvRes_sc10 = false;
        bool em_harvRes_fc30 = false;
        bool em_harvRes_th30 = false;
        bool em_harvRes_sc30 = false;

        HarvestResidues() = default;

        explicit HarvestResidues(const DataStruct &plot)
                : simuId{plot.simuID}, asID{plot.asID}, country{plot.country}, fTimber{plot.fTimber},
                  costsSuit1{plot.residuesUseCosts}, costsSuit2{plot.residuesUseCosts + 10},
                  costsSuit3{plot.residuesUseCosts}, costsSuit4_notTaken{plot.residuesUseCosts * 10} {
            if (country < 0 || country >= numberOfCountries) {
                FATAL("HarvestResidues: provide correct country code!!!\ncountry = {}", country);
                throw runtime_error{"HarvestResidues: provide correct country code!!!"};
            }
        }

        void initTotalCost() {
            costsTotal = 0;
            if (U.residuesSuit1_perHa > 0 || U.residuesSuit2_perHa > 0 || U.residuesSuit3_perHa > 0 ||
                U.residuesSuit4_notTaken_perHa > 0
                //|| residuesSuit1_perHa10 > 0 || residuesSuit2_perHa10 > 0 || residuesSuit3_perHa10 > 0 ||
                // residuesSuit4_notTaken_perHa10 > 0
                || O30.residuesSuit1_perHa > 0 || O30.residuesSuit2_perHa > 0 || O30.residuesSuit3_perHa > 0 ||
                O30.residuesSuit4_notTaken_perHa > 0) {

                double numerator =
                        // costsSuit1 * (residuesSuit1 + residuesSuit1_10 + residuesSuit1_30)
                        // + costsSuit2 * (residuesSuit2 + residuesSuit2_10 + residuesSuit2_30)
                        // + costsSuit3 * (residuesSuit3 + residuesSuit3_10 + residuesSuit3_30)
                        costsSuit1 * (U.residuesSuit1 + O30.residuesSuit1)
                        + costsSuit2 * (U.residuesSuit2 + O30.residuesSuit2)
                        + costsSuit3 * (U.residuesSuit3 + O30.residuesSuit3)
                        //+ costsSuit4_notTaken * (residuesSuit4_notTaken + residuesSuit4_notTaken10 + residuesSuit4_notTaken30)
                        + costsSuit4_notTaken * (U.residuesSuit4_notTaken + O30.residuesSuit4_notTaken);

                double denominator =
                        U.residuesSuit1 + U.residuesSuit2 + U.residuesSuit3 + U.residuesSuit4_notTaken
                        // + residuesSuit1_10 + residuesSuit2_10 + residuesSuit3_10 + residuesSuit4_notTaken10
                        + O30.residuesSuit1 + O30.residuesSuit2 + O30.residuesSuit3 + O30.residuesSuit4_notTaken;

                costsTotal = numerator / denominator;
            }
        }

        // change to mdspan in the future
        [[nodiscard]] double
        lerpERUS(const array<array<double, 4>, numberOfCountries> &emissionsResUseSust, uint16_t year) const {
            /*
            if (country < 0 || country >= numberOfCountries) {
                FATAL("emissionsResUseSust: provide correct country code!!!\ncountry = {}, year = {}", country,
                      year);
                throw runtime_error{"emissionsResUseSust: provide correct country code!!!"};
            }
            */
            year = max(uint16_t{0}, year);
            constexpr array<uint16_t, 4> midYears = {2, 12, 35, 65};

            double em_factor = 0;
            if (year <= midYears[1])
                em_factor = lerp(emissionsResUseSust[country][0], emissionsResUseSust[country][1],
                                 (year - midYears[0]) / (midYears[1] - midYears[0]));
            else if (year <= midYears[2])
                em_factor = lerp(emissionsResUseSust[country][1], emissionsResUseSust[country][2],
                                 (year - midYears[1]) / (midYears[2] - midYears[1]));
            else
                em_factor = lerp(emissionsResUseSust[country][2], emissionsResUseSust[country][3],
                                 (year - midYears[2]) / (midYears[3] - midYears[2]));

            em_factor = max(0., em_factor);
            return em_factor;
        }

        void setTimeUseSust() {
            if (useSuit1 <= 0 && useSuit3 <= 0 && useSuit4 <= 0)
                U.timeUseSust1 = 0;
            if (useSuit2 <= 0)
                U.timeUseSust2 = 0;
        }
    };

}

#endif
