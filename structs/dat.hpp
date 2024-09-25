#ifndef G4M_EUROPE_DG_DAT_HPP
#define G4M_EUROPE_DG_DAT_HPP

#include <array>
#include <vector>
#include <ranges>

#include "cell_forest.hpp"
#include "../init/species.hpp"
#include "../init/data_struct.hpp"

using namespace std;

using namespace g4m::init;

namespace g4m::structs {
    struct Dat {
        [[nodiscard]] static string csvHeader() noexcept {
            return format("{},{},{},{},{},landAreaHa,U_stemBiomass0,N_BGB,N_AGB_B20,N_AGB_O20,afforestA,deforestA,"
                          "deforestLitterA,deforestSoilA,deforestSoilA1,longLivedA,shortLivedA,fineRootA,"
                          "afforestLitterA,afforestSoilA,deforestWoodTotal,salvageLogging,rotation,rotationBiomass,SD,"
                          "harvestEfficiencyMultifunction,constructedRoadsMultifunction,currentYearRoadInvestment,"
                          "deforestHaTotal,afforestHaTotal,afforestHaYear,deforestHaYear,emissionsDBm,emissionsDS,"
                          "emissionsAS,deforestShare,deforestPrev,deforestRateCC", CellForest::csvHeader("U"),
                          CellForest::csvHeader("O10"), CellForest::csvHeader("O30"), CellForest::csvHeader("P"),
                          CellForest::csvHeader("N"));
        }

        CellForest U;                       // usual old forest
        CellForest O10;                     // old forest 10% policy
        CellForest O30;                     // old forest 30% policy
        CellForest P;                       // primary old-growth forest
        CellForest N;                       // new forest

        double landAreaHa = 0;

        double U_stemBiomass0 = 0;

        array<double, 2> N_belowgroundBiomass{};     // new forest below-ground biomass, tC / ha

        array<vector<double>, 2> N_abovegroundBiomassBelow20yoAge = {{{}, {0}}};   // tC / ha
        array<vector<double>, 2> N_abovegroundBiomassOver20yoAge = {{{}, {0}}};    // tC / ha

        // historical variables
        // afforestation share occurred at time A, formerly forestAgeShare
        vector<double> afforestationShareTimeA = {0};
        // deforestation share occurred at time A, formerly deforestA
        vector<double> deforestationShareTimeA = {0};

        // litter pool under deforestation occurred at time A, formerly litterA
        vector<double> deforestationLitterTimeA = {0};
        // soil pool under deforestation occurred at time A, formerly SOCA
        vector<double> deforestationSoilTimeA = {0};
        // soil pool under deforestation occurred at time A for alternative calculations, formerly SOCA1
        vector<double> deforestationSoilTimeA1 = {0};
        // long-lived woody products made of forest deforested at time A, formerly prodLongA
        vector<double> deforestationWoodyProductsLongLivedA = {0};
        // short-lived woody products made of forest deforested at time A, formerly prodShortA
        vector<double> deforestationWoodyProductsShortLivedA = {0};
        // fine root pool under deforestation occurred at time A
        vector<double> fineRootA = {0};
        // litter pool under afforestation occurred at time A, formerly litterAffor
        vector<double> afforestationLitterTimeA = {0};
        // soil pool under afforestation occurred at time A, formerly SOCAffor
        vector<double> afforestationSoilTimeA = {0};

        double deforestationWoodTotalM3 = 0;    // wood obtained from deforestation in the cell, m^3
        double salvageLogging = 0;
        double rotation = 0; // current rotation, years
        double rotationBiomass = 0; // rotation for getting observed biomass, years
        double SD = 0; // stocking density

        double harvestEfficiencyMultifunction = 0;  // Gained (additional) efficiency of collecting wood in multifunctional forests
        double constructedRoadsMultifunction = 0;   // Additional constructed road in multifunctional forest
        double currentYearRoadInvestment = 0;       // Investment in forest road construction in current year

        double deforestHaTotal = 0;
        double afforestHaTotal = 0;

        double afforestHaYear = 0;
        double deforestHaYear = 0;

        double emissionsDeforestationBiomass = 0;   // CO2 lost from living biomass of old forests due to deforestation, mtCO2 / (ha * year)
        double emissionsDeforestationSoil = 0;    // CO2 lost from soil in old forests due to deforestation, mtCO2 / (ha * year)
        double emissionsAfforestationSoil = 0;   // CO2 emissions from soil in new forests due to afforestation, mtCO2 / (ha * year)

        double deforestationShare = 0;
        double deforestationPrev = 0; // deforestation share in old forest at a previous time step
        double deforestationRateCoefCell = 0; // calibration coefficient to match observed deforestation rate in the tropics

        Dat() = default;

        Dat(const DataStruct &plot, const double rotation_, const double abBiomass0, const double cohort10Bm,
            const double cohort30Bm, const double cohortPrimaryBm, const double thinning) :
                landAreaHa{plot.landArea * 100}, deforestationPrev{plot.forLoss}, rotation{rotation_},
                rotationBiomass{rotation_}, SD{thinning}, U_stemBiomass0{abBiomass0} {
            U.forestShare.back() = plot.forest;
            O10.forestShare.back() = plot.oldGrowthForest_ten;
            O30.forestShare.back() = plot.oldGrowthForest_thirty;
            P.forestShare.back() = plot.strictProtected;

            U.stemBiomass.back() = abBiomass0;
            O10.stemBiomass.back() = cohort10Bm;        // biomass of 10% policy forest on a previous step, tC/ha
            O30.stemBiomass.back() = cohort30Bm;        // biomass of 30% policy forest on a previous step, tC/ha
            P.stemBiomass.back() = cohortPrimaryBm;    // biomass of primary forest on a previous step, tC/ha

            if (U.stemBiomass.back() > 0)
                U.totalBiomass = {U.stemBiomass.back() * plot.BEF(U.stemBiomass.back()) + plot.CBelowHa};

            if (O10.stemBiomass.back() > 0)
                O10.totalBiomass = O10.stemBiomass.back() * plot.BEF(O10.stemBiomass.back()) + plot.CBelowHa;

            if (O30.stemBiomass.back() > 0)
                O30.totalBiomass = O30.stemBiomass.back() * plot.BEF(O30.stemBiomass.back()) + plot.CBelowHa;

            if (P.stemBiomass.back() > 0)
                P.totalBiomass = P.stemBiomass.back() * plot.BEF(P.stemBiomass.back()) + plot.CBelowHa;

            U.deadwood = plot.forest > 0 ? plot.deadwood : 0;
            O10.deadwood = plot.oldGrowthForest_ten > 0 ? plot.deadwood : 0;
            O30.deadwood = plot.oldGrowthForest_thirty > 0 ? plot.deadwood : 0;
            P.deadwood = plot.strictProtected > 0 ? plot.deadwood : 0;

            const size_t expectedSimulationLength = 128;

            for (auto &arr: N_abovegroundBiomassBelow20yoAge)
                arr.reserve(expectedSimulationLength);
            for (auto &arr: N_abovegroundBiomassOver20yoAge)
                arr.reserve(expectedSimulationLength);

            afforestationShareTimeA.reserve(expectedSimulationLength);
            deforestationShareTimeA.reserve(expectedSimulationLength);
            deforestationLitterTimeA.reserve(expectedSimulationLength);
            deforestationSoilTimeA.reserve(expectedSimulationLength);
            deforestationSoilTimeA1.reserve(expectedSimulationLength);
            deforestationWoodyProductsLongLivedA.reserve(expectedSimulationLength);
            deforestationWoodyProductsShortLivedA.reserve(expectedSimulationLength);
            fineRootA.reserve(expectedSimulationLength);
            afforestationLitterTimeA.reserve(expectedSimulationLength);
            afforestationSoilTimeA.reserve(expectedSimulationLength);
        }

        void update() noexcept {
            U.update();
            O10.update();
            O30.update();
            P.update();
            N.update();

            ranges::shift_left(N_belowgroundBiomass, 1);
            N_belowgroundBiomass.back() = 0;

            ranges::shift_left(N_abovegroundBiomassBelow20yoAge, 1);
            N_abovegroundBiomassBelow20yoAge.back().assign(N_abovegroundBiomassBelow20yoAge.end()[-2].size() + 1, 0);

            ranges::shift_left(N_abovegroundBiomassOver20yoAge, 1);
            N_abovegroundBiomassOver20yoAge.back().assign(N_abovegroundBiomassOver20yoAge.end()[-2].size() + 1, 0);

            afforestationShareTimeA.push_back(0);
            deforestationShareTimeA.push_back(0);
            deforestationLitterTimeA.push_back(0);
            deforestationSoilTimeA.push_back(0);
            deforestationSoilTimeA1.push_back(0);
            deforestationWoodyProductsLongLivedA.push_back(0);
            deforestationWoodyProductsShortLivedA.push_back(0);
            fineRootA.push_back(0);
            afforestationLitterTimeA.push_back(0);
            afforestationSoilTimeA.push_back(0);

            deforestationWoodTotalM3 = 0;
            salvageLogging = 0;
            rotation = 0;
            rotationBiomass = 0;
            SD = 0;

            afforestHaYear = 0;
            deforestHaYear = 0;

            emissionsDeforestationBiomass = 0;
            emissionsDeforestationSoil = 0;
            emissionsAfforestationSoil = 0;

            deforestationShare = 0;
            deforestationPrev = 0;
        }

        void resetExtracted() noexcept {
            U.resetExtracted();
            O10.resetExtracted();
            O30.resetExtracted();
            P.resetExtracted();
            N.resetExtracted();
        }

        // share of all forests in the cell (U + 10 + 30 + P + N)
        // -1 - current, -2 - previous
        [[nodiscard]] inline double forestShareAll(const ptrdiff_t idx) const noexcept {
            return forestShareOld(idx) + N.forestShare.end()[idx];
        }

        // forest share of old forest in each cell
        // -1 - current, -2 - previous
        [[nodiscard]] inline double forestShareOld(const ptrdiff_t idx) const noexcept {
            return U.forestShare.end()[idx] + O10.forestShare.end()[idx] + O30.forestShare.end()[idx] +
                   P.forestShare.end()[idx];
        }

        [[nodiscard]] inline bool checkLastForestShares() const noexcept {
            return U.forestShare.back() >= 0 && O10.forestShare.back() >= 0 && O30.forestShare.back() >= 0 &&
                   P.forestShare.back() >= 0 && N.forestShare.back() >= 0;
        }

        void correctForestShareDynamics() noexcept {
            U.forestShare.back() = min(U.forestShare.end()[-2], U.forestShare.back());
            O10.forestShare.back() = min(O10.forestShare.end()[-2], O10.forestShare.back());
            O30.forestShare.back() = min(O30.forestShare.end()[-2], O30.forestShare.back());
            P.forestShare.back() = min(O30.forestShare.end()[-2], P.forestShare.back());    // for consistency
            N.forestShare.back() = max(N.forestShare.end()[-2], N.forestShare.back());
        }

        void restoreForestShares() noexcept {
            U.forestShare.back() = U.forestShare.end()[-2];
            O10.forestShare.back() = O10.forestShare.end()[-2];
            O30.forestShare.back() = O30.forestShare.end()[-2];
            P.forestShare.back() = P.forestShare.end()[-2];
            N.forestShare.back() = N.forestShare.end()[-2];
        }

        // new forest below 20 y.o. above-ground biomass, tC / ha
        // -1 - current, -2 - previous
        [[nodiscard]] inline double N_abovegroundBiomassBelow20yo(const ptrdiff_t idx) const noexcept {
            return ranges::fold_left(N_abovegroundBiomassBelow20yoAge.end()[idx], 0., plus{});
        }

        // new forest over 20 y.o. above-ground biomass, tC / ha
        // -1 - current, -2 - previous
        [[nodiscard]] inline double N_abovegroundBiomassOver20yo(const ptrdiff_t idx) const noexcept {
            return ranges::fold_left(N_abovegroundBiomassOver20yoAge.end()[idx], 0., plus{});
        }

        // new forest above-ground biomass, tC / ha
        // -1 - current, -2 - previous
        [[nodiscard]] inline double N_abovegroundBiomass(const ptrdiff_t idx) const noexcept {
            return N_abovegroundBiomassBelow20yo(idx) + N_abovegroundBiomassOver20yo(idx);
        }

        [[nodiscard]] inline double afforestationSoilInput(const size_t age) const noexcept {
            return pow(1 - exp(-1.2 * afforestationLitterTimeA[age] / (afforestationShareTimeA[age] * landAreaHa)), 3) *
                   afforestationShareTimeA[age] * landAreaHa * modTimeStep;
        }

        [[nodiscard]] double afforestationLitterInput(const size_t age) const noexcept {
            const size_t idx = N_abovegroundBiomassBelow20yoAge.back().size() - 1 - age;  // age - i
            const double abovePhCurBef = (N_abovegroundBiomassBelow20yoAge.back()[idx] +
                                          N_abovegroundBiomassOver20yoAge.back()[idx]);
            return 0.95 * pow(1 - exp(-0.1 * abovePhCurBef * N.forestShare.back() / afforestationShareTimeA[age]), 3)
                   * afforestationShareTimeA[age] * landAreaHa * modTimeStep;
        }

        [[nodiscard]] string csv() const noexcept {
            return format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},"
                          "{},{},{},{},{},{},{},{}", U.csv(), O10.csv(), O30.csv(), P.csv(), N.csv(), landAreaHa,
                          U_stemBiomass0, N_belowgroundBiomass.back(), N_abovegroundBiomassBelow20yo(-1),
                          N_abovegroundBiomassOver20yo(-1), afforestationShareTimeA.back(),
                          deforestationShareTimeA.back(), deforestationLitterTimeA.back(),
                          deforestationSoilTimeA.back(), deforestationSoilTimeA1.back(),
                          deforestationWoodyProductsLongLivedA.back(), deforestationWoodyProductsShortLivedA.back(),
                          fineRootA.back(), afforestationLitterTimeA.back(), afforestationSoilTimeA.back(),
                          deforestationWoodTotalM3, salvageLogging, rotation, rotationBiomass, SD,
                          harvestEfficiencyMultifunction, constructedRoadsMultifunction, currentYearRoadInvestment,
                          deforestHaTotal, afforestHaTotal, afforestHaYear, deforestHaYear,
                          emissionsDeforestationBiomass, emissionsDeforestationSoil, emissionsAfforestationSoil,
                          deforestationShare, deforestationPrev, deforestationRateCoefCell);
        }
    };
}

#endif
