#ifndef G4M_EUROPE_DG_DAT_HPP
#define G4M_EUROPE_DG_DAT_HPP

#include <array>

#include "cell_forest.hpp"
#include "../init/species.hpp"
#include "../init/data_struct.hpp"

using namespace std;

using namespace g4m::init;

namespace g4m::structs {

    struct Dat {
        double landAreaHa = 0;

        CellForest U;                       // usual old forest
        CellForest O10;                     // old forest 10% policy
        CellForest O30;                     // old forest 30% policy
        CellForest P;                       // primary old-growth forest
        CellForest N;                       // new forest

        double harvestEfficiencyMultifunction = 0;  // Gained (additional) efficiency of collecting wood in multifunctional forests
        double constructedRoadsMultifunction = 0;   // Additional constructed road in multifunctional forest
        double currentYearRoadInvestment = 0;       // Investment in forest road construction in current year

        double savedCarbonPrev = 0;
        double gainedCarbonPrev = 0;
        double emissionsTotPrev = 0;
        double emissionsAfforPrev = 0;

        vector<double> N_belowgroundBiomass;     // new forest below-ground biomass, N.forestShare * tC/ha

        vector<vector<double> > N_abovegroundBiomassBelow20yoAge;
        vector<vector<double> > N_abovegroundBiomassOver20yoAge;

        // afforestation share occurred at time A, formerly forestAgeShare
        vector<double> afforestationShareTimeA;
        // deforestation share occurred at time A, formerly deforestA
        vector<double> deforestationShareTimeA;
        // litter pool under deforestation occurred at time A, formerly litterA
        vector<vector<double> > deforestationLitterTimeA;
        // soil pool under deforestation occurred at time A, formerly SOCA
        vector<vector<double> > deforestationSoilTimeA;
        // soil pool under deforestation occurred at time A for alternative calculations, formerly SOCA1
        vector<vector<double> > deforestationSoilTimeA1;
        // long-lived woody products made of forest deforested at time A, formerly prodLongA
        vector<vector<double> > deforestationWoodyProductsLongLivedA;
        // short-lived woody products made of forest deforested at time A, formerly prodShortA
        vector<vector<double> > deforestationWoodyProductsShortLivedA;
        // fine root pool under deforestation occurred at time A
        vector<vector<double> > fineRootA;
        // litter pool under afforestation occurred at time A, formerly litterAffor
        vector<vector<double> > afforestationLitterTimeA;
        // soil pool under afforestation occurred at time A, formerly SOCAffor
        vector<vector<double> > afforestationSoilTimeA;

        vector<double> rotation;
        vector<double> rotationBiomass;
        vector<double> SD;

        double deforestHaTotal = 0;
        double afforestHaTotal = 0;
        double emissionsProduct = 0;
        double emissionsLitter = 0;
        double emissionsSOC = 0;
        double emissionsSlashBurn = 0;
        double emissionsDeadBurn = 0;
        double emissionsCRootBurn = 0;
        double emissionsTotal = 0;
        double emissionsLitterAffor = 0;
        double emissionsSOCAffor = 0;
        double emissionsAffor = 0;

        double abBiomassTotPrev = 0;
        int iReportYear = 0;
        //TODO wood obtained from deforestation in the cell, m^3
        double deforestationWoodTotalM3 = 0;    // stem wood obtained from deforestation averaged for last 5 years
        array<double, 5> deforWoodArray{};      // stem wood obtained from deforestation for last 5 years
        array<double, 5> deforAreaArray{};      // deforested area for the last 5 years
        double deforestShare = 0;
        double afforestHaYear = 0;
        double deforestHaYear = 0;
        double harvestTot_all = 0;
        double harvestFcM3Ha = 0;
        double harvestThM3Ha = 0;
        double harvestScM3Ha = 0;
        double emissionsD_Bm = 0;   // CO2 lost from living biomass of old forests due to deforestation, mtCO2/ha year
        double emissionsD_S = 0;
        double emissionsA_Bm = 0;   // CO2 sink in forest living biomass of planted forest, mtCO2/ha year
        double emissionsA_S = 0;
        double road = 0;

        double deforestPrev = 0;
        double deforestationRateCoefCell = 0;

        FMResult resultDeforestation;
        double slashBurn = 0;               // share of burnt wood at deforestation

        Dat() = default;

        Dat(const DataStruct &plot, const uint16_t bYear_, const double rotation_, const double abBiomass0,
            const double cohort10Bm, const double cohort30Bm, const double cohortPrimaryBm, const double thinning) :
                bYear{bYear_}, landAreaHa{plot.landArea * 100}, deforestPrev{plot.forLoss},
                road{plot.road.data.at(2000)}, slashBurn{plot.slashBurn} {
            U.forestShare = {plot.forest};
            O10.forestShare = {plot.oldGrowthForest_ten};
            O30.forestShare = {plot.oldGrowthForest_thirty};
            P.forestShare = {plot.strictProtected};
            N.forestShare = {0}; // for consistency

            rotation = {rotation_};
            rotationBiomass = {rotation_};
            SD = {thinning};
            rotation.reserve(128);
            rotationBiomass.reserve(128);
            SD.reserve(128);

            U.stemBiomass = {abBiomass0};
            O10.stemBiomass = {cohort10Bm};        // biomass of 10% policy forest on a previous step, tC/ha
            O30.stemBiomass = {cohort30Bm};        // biomass of 30% policy forest on a previous step, tC/ha
            P.stemBiomass = {cohortPrimaryBm};    // biomass of primary forest on a previous step, tC/ha
            N.stemBiomass = {0}; // for consistency

            U.fellings = {0}; // for consistency
            O10.fellings = {0}; // for consistency
            O30.fellings = {0}; // for consistency
            P.fellings = {0}; // for consistency
            N.fellings = {0}; // for consistency

            U.CAI = {0}; // for consistency
            O10.CAI = {0}; // for consistency
            O30.CAI = {0}; // for consistency
            P.CAI = {0}; // for consistency
            N.CAI = {0}; // for consistency

            U.OAC = {0}; // for consistency
            O10.OAC = {0}; // for consistency
            O30.OAC = {0}; // for consistency
            P.OAC = {0}; // for consistency
            N.OAC = {0}; // for consistency

            U.deadwoodInput = {0}; // for consistency
            O10.deadwoodInput = {0}; // for consistency
            O30.deadwoodInput = {0}; // for consistency
            P.deadwoodInput = {0}; // for consistency
            N.deadwoodInput = {0}; // for consistency

            U.litterInput = {0}; // for consistency
            O10.litterInput = {0}; // for consistency
            O30.litterInput = {0}; // for consistency
            P.litterInput = {0}; // for consistency
            N.litterInput = {0}; // for consistency

            U.biomassChange_ab = {0}; // for consistency
            O10.biomassChange_ab = {0}; // for consistency
            O30.biomassChange_ab = {0}; // for consistency
            P.biomassChange_ab = {0}; // for consistency
            N.biomassChange_ab = {0}; // for consistency

            U.biomassChange_total = {0}; // for consistency
            O10.biomassChange_total = {0}; // for consistency
            O30.biomassChange_total = {0}; // for consistency
            P.biomassChange_total = {0}; // for consistency
            N.biomassChange_total = {0}; // for consistency

            // TODO check if right
            U.totalBiomass = U.stemBiomass.back();
            O10.totalBiomass = O10.stemBiomass.back();
            O30.totalBiomass = O30.stemBiomass.back();
            P.totalBiomass = P.stemBiomass.back();
            N.totalBiomass = N.stemBiomass.back(); // for consistency

            N_belowgroundBiomass = {0}; // for consistency
            N_belowgroundBiomass.reserve(128);

            afforestationShareTimeA = {0}; // for consistency
            deforestationShareTimeA = {0}; // for consistency
            afforestationShareTimeA.reserve(128);
            deforestationShareTimeA.reserve(128);

            N_abovegroundBiomassBelow20yoAge.reserve(128);
            N_abovegroundBiomassOver20yoAge.reserve(128);

            deforestationLitterTimeA.reserve(128);
            deforestationSoilTimeA.reserve(128);
            deforestationSoilTimeA1.reserve(128);
            deforestationWoodyProductsLongLivedA.reserve(128);
            deforestationWoodyProductsShortLivedA.reserve(128);
            fineRootA.reserve(128);
            afforestationLitterTimeA.reserve(128);
            afforestationSoilTimeA.reserve(128);

            N_abovegroundBiomassBelow20yoAge.emplace_back(1); // for consistency
            N_abovegroundBiomassOver20yoAge.emplace_back(1); // for consistency

            deforestationLitterTimeA.emplace_back(1); // for consistency
            deforestationSoilTimeA.emplace_back(1); // for consistency
            deforestationSoilTimeA1.emplace_back(1); // for consistency
            deforestationWoodyProductsLongLivedA.emplace_back(1); // for consistency
            deforestationWoodyProductsShortLivedA.emplace_back(1); // for consistency
            fineRootA.emplace_back(1); // for consistency
            afforestationLitterTimeA.emplace_back(1); // for consistency
            afforestationSoilTimeA.emplace_back(1); // for consistency

            U.deadwood = plot.forest > 0 ? plot.deadWood : 0;
            O10.deadwood = plot.oldGrowthForest_ten > 0 ? plot.deadWood : 0;
            O30.deadwood = plot.oldGrowthForest_thirty > 0 ? plot.deadWood : 0;
            P.deadwood = plot.strictProtected > 0 ? plot.deadWood : 0;
        }

        // share of all forests in the cell (U + 10 + 30 + P + N)
        // -1 - current, -2 - previous, ...; 0 - first, 1 - second
        [[nodiscard]] inline double forestShareAll(const ptrdiff_t idx) const noexcept {
            return forestShareOld(idx) + (idx >= 0 ? N.forestShare[idx] : N.forestShare.end()[idx]);
        }

        // forest share of old forest in each cell
        // -1 - current, -2 - previous, ...; 0 - first, 1 - second
        [[nodiscard]] inline double forestShareOld(const ptrdiff_t idx) const noexcept {
            return idx >= 0 ?
                   U.forestShare[idx] + O10.forestShare[idx] + O30.forestShare[idx] + P.forestShare[idx] :
                   U.forestShare.end()[idx] + O10.forestShare.end()[idx] + O30.forestShare.end()[idx] +
                   P.forestShare.end()[idx];
        }

        [[nodiscard]] inline bool checkLastForestShares() const noexcept {
            return U.forestShare.back() >= 0 && O10.forestShare.back() >= 0 && O30.forestShare.back() >= 0 &&
                   P.forestShare.back() >= 0 && N.forestShare.back() >= 0;
        }

        void addModelingStep(const uint16_t year) noexcept {
            const size_t age = (year - bYear) / modTimeStep + 1;

            rotation.push_back(0);
            rotationBiomass.push_back(0);
            SD.push_back(0);

            U.forestShare.push_back(U.forestShare.back());
            O10.forestShare.push_back(O10.forestShare.back());
            O30.forestShare.push_back(O30.forestShare.back());
            P.forestShare.push_back(P.forestShare.back());
            N.forestShare.push_back(N.forestShare.back());

            U.stemBiomass.push_back(0);
            O10.stemBiomass.push_back(0);
            O30.stemBiomass.push_back(0);
            P.stemBiomass.push_back(0);
            N.stemBiomass.push_back(0);

            U.fellings.push_back(0);
            O10.fellings.push_back(0);
            O30.fellings.push_back(0);
            P.fellings.push_back(0);
            N.fellings.push_back(0);

            U.CAI.push_back(0);
            O10.CAI.push_back(0);
            O30.CAI.push_back(0);
            P.CAI.push_back(0);
            N.CAI.push_back(0);

            U.OAC.push_back(0);
            O10.OAC.push_back(0);
            O30.OAC.push_back(0);
            P.OAC.push_back(0);
            N.OAC.push_back(0);

            U.deadwoodInput.push_back(0);
            O10.deadwoodInput.push_back(0);
            O30.deadwoodInput.push_back(0);
            P.deadwoodInput.push_back(0);
            N.deadwoodInput.push_back(0);

            U.litterInput.push_back(0);
            O10.litterInput.push_back(0);
            O30.litterInput.push_back(0);
            P.litterInput.push_back(0);
            N.litterInput.push_back(0);

            U.biomassChange_ab.push_back(0);
            O10.biomassChange_ab.push_back(0);
            O30.biomassChange_ab.push_back(0);
            P.biomassChange_ab.push_back(0);
            N.biomassChange_ab.push_back(0);

            U.biomassChange_total.push_back(0);
            O10.biomassChange_total.push_back(0);
            O30.biomassChange_total.push_back(0);
            P.biomassChange_total.push_back(0);
            N.biomassChange_total.push_back(0);

            N_belowgroundBiomass.push_back(0);

            afforestationShareTimeA.push_back(0);
            deforestationShareTimeA.push_back(0);

            N_abovegroundBiomassBelow20yoAge.emplace_back(age + 1);
            N_abovegroundBiomassOver20yoAge.emplace_back(age + 1);

            deforestationLitterTimeA.push_back(deforestationLitterTimeA.back());
            deforestationSoilTimeA.push_back(deforestationSoilTimeA.back());
            deforestationSoilTimeA1.push_back(deforestationSoilTimeA1.back());
            deforestationWoodyProductsLongLivedA.push_back(deforestationWoodyProductsLongLivedA.back());
            deforestationWoodyProductsShortLivedA.push_back(deforestationWoodyProductsShortLivedA.back());
            fineRootA.push_back(fineRootA.back());
            afforestationLitterTimeA.push_back(afforestationLitterTimeA.back());
            afforestationSoilTimeA.push_back(afforestationSoilTimeA.back());

            deforestationLitterTimeA.back().push_back(0);
            deforestationSoilTimeA.back().push_back(0);
            deforestationSoilTimeA1.back().push_back(0);
            deforestationWoodyProductsLongLivedA.back().push_back(0);
            deforestationWoodyProductsShortLivedA.back().push_back(0);
            fineRootA.back().push_back(0);
            afforestationLitterTimeA.back().push_back(0);
            afforestationSoilTimeA.back().push_back(0);
        }

        void correctForestShareDynamics() noexcept {
            U.forestShare.back() = min(U.forestShare.end()[-2], U.forestShare.back());
            O10.forestShare.back() = min(O10.forestShare.end()[-2], O10.forestShare.back());
            O30.forestShare.back() = min(O30.forestShare.end()[-2], O30.forestShare.back());
            P.forestShare.back() = min(O30.forestShare.end()[-2], P.forestShare.back());    // for consistency
            N.forestShare.back() = max(N.forestShare.end()[-2], N.forestShare.back());
        }

        void resetExtracted() noexcept {
            U.extractedResidues = 0;
            U.extractedStump = 0;
            U.extractedCleaned = 0;
            O10.extractedResidues = 0;
            O10.extractedStump = 0;
            O10.extractedCleaned = 0;
            O30.extractedResidues = 0;
            O30.extractedStump = 0;
            O30.extractedCleaned = 0;
        }

        [[nodiscard]] string csv(const string_view line_prefix) const noexcept {
//            constexpr int width = 15;
//            constexpr int precision = 6;

//            output += format(
//                    "{1:^120}{2:^65}{3:^76}{4:^126}{5:^32}\n{6:{0}}{7:{0}}{8:{0}}{9:{0}}{10:{0}}{11:{0}}{12:{0}}{13:{0}}"
//                    "  {14:{0}}{15:{0}}{16:{0}}{17:{0}}{18:{0}}{19:{0}}{20:{0}}{21:{0}}{22:{0}}   {23:{0}}{24:{0}}   "
//                    "{25:{0}}{26:{0}}{27:{0}}{28:{0}}{29:{0}}   {30:{0}}{31:{0}}{32:{0}}{33:{0}}{34:{0}}\n", width,
//                    "Forest Share", "Stem Biomass", "New Forest", "Fellings", "Current Annual Increment", "", "U",
//                    "O10", "O30", "P", "N", "Old", "All", "U", "O10", "O30", "P", "N", "BG_B", "AG_B_B20", "AG_B_O20",
//                    "AG_B", "afforestA", "deforestA", "U", "O10", "O30", "P", "N", "U", "O10", "O30", "P", "N");

//                output += format(
//                        "{2}{3:{0}.{1}f}{4:{0}.{1}f}{5:{0}.{1}f}{6:{0}.{1}f}{7:{0}.{1}f}{8:{0}.{1}f}{9:{0}.{1}f}  |"
//                        "{10:{0}.{1}f}{11:{0}.{1}f}{12:{0}.{1}f}{13:{0}.{1}f}{14:{0}.{1}f}  |{15:{0}.{1}f}{16:{0}.{1}f}"
//                        "{17:{0}.{1}f}{18:{0}.{1}f}  |{19:{0}.{1}f}{20:{0}.{1}f}  |{21:{0}.{1}f}{22:{0}.{1}f}"
//                        "{23:{0}.{1}f}{24:{0}.{1}f}{25:{0}.{1}f}  |{26:{0}.{1}f}{27:{0}.{1}f}{28:{0}.{1}f}{29:{0}.{1}f}"
//                        "{30:{0}.{1}f}\n",
//                        width, precision, year, U.forestShare[i], O10.forestShare[i], O30.forestShare[i],
//                        P.forestShare[i], N.forestShare[i], forestShareOld, forestShareAll, U.stemBiomass[i],
//                        O10.stemBiomass[i], O30.stemBiomass[i], P.stemBiomass[i], N.stemBiomass[i],
//                        N_belowgroundBiomass[i], N_abovegroundBiomassBelow20yo, N_abovegroundBiomassOver20yo,
//                        N_abovegroundBiomass, afforestationShareTimeA[i], deforestationShareTimeA[i], U.fellings[i],
//                        O10.fellings[i], O30.fellings[i], P.fellings[i], N.fellings[i], U.CAI[i], O10.CAI[i],
//                        O30.CAI[i], P.CAI[i], N.CAI[i]);

            string output;
            output.reserve(1'000'000);

            for (size_t i = 0; i < U.forestShare.size(); ++i) {
                const uint16_t year = bYear + i - 1;
                double forestShareOld = U.forestShare[i] + O10.forestShare[i] + O30.forestShare[i] + P.forestShare[i];
                double forestShareAll = forestShareOld + N.forestShare[i];
                double N_abovegroundBiomassBelow20yo = ranges::fold_left(N_abovegroundBiomassBelow20yoAge[i], 0.,
                                                                         plus{});
                double N_abovegroundBiomassOver20yo = ranges::fold_left(N_abovegroundBiomassOver20yoAge[i], 0., plus{});
                double N_abovegroundBiomass = N_abovegroundBiomassBelow20yo + N_abovegroundBiomassOver20yo;

                output += format(
                        "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},"
                        "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}\n",
                        line_prefix, year, U.forestShare[i], O10.forestShare[i], O30.forestShare[i],
                        P.forestShare[i], N.forestShare[i], forestShareOld, forestShareAll, U.stemBiomass[i],
                        O10.stemBiomass[i], O30.stemBiomass[i], P.stemBiomass[i], N.stemBiomass[i],
                        N_belowgroundBiomass[i], N_abovegroundBiomassBelow20yo, N_abovegroundBiomassOver20yo,
                        N_abovegroundBiomass, afforestationShareTimeA[i], deforestationShareTimeA[i], U.fellings[i],
                        O10.fellings[i], O30.fellings[i], P.fellings[i], N.fellings[i], U.CAI[i], O10.CAI[i],
                        O30.CAI[i], P.CAI[i], N.CAI[i], U.OAC[i], O10.OAC[i], O30.OAC[i], P.OAC[i], N.OAC[i],
                        U.deadwoodInput[i], O10.deadwoodInput[i], O30.deadwoodInput[i], P.deadwoodInput[i],
                        N.deadwoodInput[i], U.litterInput[i], O10.litterInput[i], O30.litterInput[i], P.litterInput[i],
                        N.litterInput[i], rotation[i], rotationBiomass[i], SD[i], U.biomassChange_ab[i],
                        O10.biomassChange_ab[i], O30.biomassChange_ab[i], P.biomassChange_ab[i], N.biomassChange_ab[i],
                        U.biomassChange_total[i], O10.biomassChange_total[i], O30.biomassChange_total[i],
                        P.biomassChange_total[i], N.biomassChange_total[i]);
            }
            return output;
        }

        void takeForestShareSnapshot() noexcept {
            U_forestShare_backup = U.forestShare.back();
            O10_forestShare_backup = O10.forestShare.back();
            O30_forestShare_backup = O30.forestShare.back();
            P_forestShare_backup = P.forestShare.back();
            N_forestShare_backup = N.forestShare.back();
        }

        void restoreForestShareSnapshot() noexcept {
            U.forestShare.back() = U_forestShare_backup;
            O10.forestShare.back() = O10_forestShare_backup;
            O30.forestShare.back() = O30_forestShare_backup;
            P.forestShare.back() = P_forestShare_backup;
            N.forestShare.back() = N_forestShare_backup;
        }

        [[nodiscard]] inline double afforestationSoilInput(const size_t age) const noexcept {
            return pow(1 - exp(-1.2 * afforestationLitterTimeA.back()[age] /
                               (afforestationShareTimeA[age] * landAreaHa)), 3) *
                   afforestationShareTimeA[age] * landAreaHa * modTimeStep;
        }

        [[nodiscard]] double afforestationLitterInput(const size_t age) const noexcept {
            const size_t idx = N_abovegroundBiomassBelow20yoAge.back().size() - 1 - age;  // age - i
            const double abovePhCurBef = N_abovegroundBiomassBelow20yoAge.back()[idx] +
                                         N_abovegroundBiomassOver20yoAge.back()[idx];
            return 0.95 * pow(1 - exp(-0.1 * abovePhCurBef / afforestationShareTimeA[age]), 3)
                   * afforestationShareTimeA[age] * landAreaHa * modTimeStep;
        }

        // new forest below 20 y.o. above-ground biomass, N.forestShare * tC/ha
        // -1 - current, -2 - previous, ...; 0 - first, 1 - second
        [[nodiscard]] inline double N_abovegroundBiomassBelow20yo(const ptrdiff_t idx) const noexcept {
            return idx >= 0 ?
                   ranges::fold_left(N_abovegroundBiomassBelow20yoAge[idx], 0., plus{}) :
                   ranges::fold_left(N_abovegroundBiomassBelow20yoAge.end()[idx], 0., plus{});
        }

        // new forest over 20 y.o. above-ground biomass, N.forestShare * tC/ha
        // -1 - current, -2 - previous, ...; 0 - first, 1 - second
        [[nodiscard]] inline double N_abovegroundBiomassOver20yo(const ptrdiff_t idx) const noexcept {
            return idx >= 0 ?
                   ranges::fold_left(N_abovegroundBiomassOver20yoAge[idx], 0., plus{}) :
                   ranges::fold_left(N_abovegroundBiomassOver20yoAge.end()[idx], 0., plus{});
        }

        // new forest above-ground biomass, N.forestShare * tC/ha
        // -1 - current, -2 - previous, ...; 0 - first, 1 - second
        [[nodiscard]] inline double N_abovegroundBiomass(const ptrdiff_t idx) const noexcept {
            return N_abovegroundBiomassBelow20yo(idx) + N_abovegroundBiomassOver20yo(idx);
        }

    private:
        const uint16_t bYear = 0;

        double U_forestShare_backup = nan("");
        double O10_forestShare_backup = nan("");
        double O30_forestShare_backup = nan("");
        double P_forestShare_backup = nan("");
        double N_forestShare_backup = nan("");
    };
}

#endif
