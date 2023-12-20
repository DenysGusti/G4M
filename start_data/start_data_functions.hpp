#ifndef G4M_EUROPE_DG_START_DATA_FUNCTIONS_HPP
#define G4M_EUROPE_DG_START_DATA_FUNCTIONS_HPP

#include <future>

#include "../data_io/reading.hpp"
#include "../data_io/printing.hpp"
#include "../increment/dima.hpp"
#include "../dicts/dicts.hpp"

using namespace g4m;
using namespace g4m::DataIO::reading;
using namespace g4m::DataIO::printing;
using namespace g4m::Dicts;

namespace g4m::StartData {
    void setCountryData(const unordered_set<uint8_t> &countries_list) noexcept {
        countriesNforCover.setListOfCountries(countries_list);
        countriesAfforHaYear.setListOfCountries(countries_list);

        countriesNforTotC.setListOfCountries(countries_list);
        countriesAfforCYear.setListOfCountries(countries_list);
        countriesAfforCYear_ab.setListOfCountries(countries_list);
        countriesAfforCYear_bl.setListOfCountries(countries_list);
        countriesAfforCYear_biom.setListOfCountries(countries_list);
        countriesAfforCYear_dom.setListOfCountries(countries_list);
        countriesAfforCYear_soil.setListOfCountries(countries_list);
//---------
        countriesOforCover.setListOfCountries(countries_list);
        countriesDeforHaYear.setListOfCountries(countries_list);

        countriesOfor_ab_C.setListOfCountries(countries_list);
        countriesOforC_biom.setListOfCountries(countries_list);
        countriesDeforCYear.setListOfCountries(countries_list);
        countriesDeforCYear_bl.setListOfCountries(countries_list);
        countriesDeforCYear_ab.setListOfCountries(countries_list);
        countriesDeforCYear_biom.setListOfCountries(countries_list);
        countriesDeforCYear_dom.setListOfCountries(countries_list);
        countriesDeforCYear_soil.setListOfCountries(countries_list);

//---------

        countriesWoodHarvestM3Year.setListOfCountries(countries_list);
        countriesWoodHarvestPlusM3Year.setListOfCountries(countries_list);
        countriesWoodHarvestFmM3Year.setListOfCountries(countries_list);
        countriesWoodHarvestDfM3Year.setListOfCountries(countries_list);
        countriesWoodLoosCYear.setListOfCountries(countries_list);
        countriesHarvLossesYear.setListOfCountries(countries_list);
//---------
        countriesManagedForHa.setListOfCountries(countries_list);
        countriesManagedCount.setListOfCountries(countries_list);

        countriesMAI.setListOfCountries(countries_list);
        countriesCAI.setListOfCountries(countries_list);
        countriesCAI_new.setListOfCountries(countries_list);

        countriesFM.setListOfCountries(countries_list);
        countriesFMbm.setListOfCountries(countries_list);
//---------
        //countriesWprod.setListOfCountries(countries_list);
//---------
        countriesProfit.setListOfCountries(countries_list);
    }

    [[nodiscard]] CountryData setCountriesWoodProdStat() noexcept {
        CountryData fun_countriesWoodProdStat;

        for (size_t i = 0; i < woodProdEUStats.size(); ++i)
            for (size_t j = 0; j < woodProdEUStats[0].size(); ++j)
                // Malta is in countryNwp[18], there are no wood production data for Malta
                fun_countriesWoodProdStat.setVal(countryNwp[i + (i >= 18)], 1990 + j, woodProdEUStats[i][j]);

        return fun_countriesWoodProdStat;
    }

    [[nodiscard]] CountryData setCountriesFmEmission_unfccc() noexcept {
        CountryData fun_countriesFmEmission_unfccc;

        for (size_t i = 0; i < fmEmission_unfccc_CRF.size(); ++i)
            for (size_t j = 0; j < fmEmission_unfccc_CRF[0].size(); ++j)
                fun_countriesFmEmission_unfccc.setVal(eu28OrderCode[i], 1990 + j, fmEmission_unfccc_CRF[i][j]);

        return fun_countriesFmEmission_unfccc;
    }

    void calcAvgFM_sink_stat() noexcept {
        for (size_t i = 0; i < fmEmission_unfccc_CRF.size(); ++i) {
            int count = 0;
            double fmSinkSumTmp = 0;
            for (size_t j = 0; j < min(adjustLength, fmEmission_unfccc_CRF[0].size()) &&
                               coef.bYear - 1990 + j < fmEmission_unfccc_CRF[0].size(); ++j) {
                fmSinkSumTmp += fmEmission_unfccc_CRF[i][coef.bYear - 1990 + j] * -1;
                ++count;
            }
            FM_sink_stat[eu28OrderCode[i] - 1] = count > 0 ? fmSinkSumTmp * 1'000 / count : 0; // GgCO2/year
        }
    }

    [[nodiscard]] vector<DataStruct> filterPlots(const span<const DataStruct> plots) noexcept {
        vector<DataStruct> filteredPlots;
        filteredPlots.reserve(plots.size());

        for (const auto &plot: plots)
            if (regions.contains(plot.polesReg) && countriesList.contains(plot.country))
                filteredPlots.push_back(plot);

        return filteredPlots;
    }

    void correctNUTS2Data(const span<const DataStruct> plots) noexcept {
        for (const auto &plot: plots)
            // Test only some regions and some countries
            // plot.protect.data.at(2000) == 0)
        {
            // locate the struct with asID == asID within the country
            string_view countryISO = countryOrderISO2[countryCodeOrder[plot.country - 1]];
            if (countryISO == "GB")
                countryISO = "UK";

            auto findNeighbour = [&](const uint32_t radius) -> optional<string> {
                nuts2grid.setNeighNum(radius, radius);
                auto neighbours = nuts2grid.getNeighValues(plot.x, plot.y);
                auto it_nearbyCountry = ranges::find_if(neighbours, [countryISO](const string_view el) {
                    return el.substr(0, 2) == countryISO;
                });
                if (it_nearbyCountry == neighbours.end())
                    return {};
                return *it_nearbyCountry;
            };

            const uint32_t MAX_RADIUS = 3;  // 3 for remote islands

            if (auto it_nuts2 = nuts2id.find({plot.x, plot.y}); it_nuts2 != nuts2id.end()) {
                auto &[coords, NUTS2] = *it_nuts2;

                if (NUTS2.substr(0, 2) != countryISO)
                    for (uint32_t radius = 1; radius <= MAX_RADIUS; ++radius) {
                        optional<string> opt_neighbour = findNeighbour(radius);
                        if (opt_neighbour) {
                            NUTS2 = *opt_neighbour;
                            DEBUG("x = {}, y = {}, NUTS2 = {}, countryISO = {}, *opt_neighbour = {}, radius = {}",
                                  plot.x, plot.y, NUTS2, countryISO, *opt_neighbour, radius);
                            break;
                        }

                        if (radius == MAX_RADIUS)
                            ERROR("!No x = {}, y = {}, NUTS2 = {}, countryISO = {}",
                                  plot.x, plot.y, NUTS2, countryISO);
                    }

            } else
                for (uint32_t radius = 1; radius <= MAX_RADIUS; ++radius) {
                    optional<string> opt_neighbour = findNeighbour(radius);
                    if (opt_neighbour) {
                        nuts2id[{plot.x, plot.y}] = *opt_neighbour;
                        DEBUG("x = {}, y = {}, countryISO = {}, *opt_neighbour = {}, radius = {}",
                              plot.x, plot.y, countryISO, *opt_neighbour, radius);
                        break;
                    }

                    if (radius == MAX_RADIUS)
                        ERROR("!No x = {}, y = {}, countryISO = {}", plot.x, plot.y, countryISO);
                }
        }
    }

    // Setup forest management parameters similar for all countries (cells)
    void setupFMP() noexcept {
        sws.data[10] = 0;
        sws.data[30] = 0.6;

        // Georg's recommendation
        // 7: 0
        // 25: 1 - HarvestingLosesCountry
        // 50: 1 - (0.7 * HarvestingLosesCountry)
        // This is for hle, for the thinning (hlv) multiply the values of
        // harvestable biomass with 0.8.
        hlv.data[0] = 0;
        hlv.data[27] = 0;  // Testing!

        hle.data[0] = 0;
        hle.data[27] = 0;  // Testing!

        sdMaxH.data[0] = 1;
        sdMinH.data[0] = 1;

        // diameter, stocking volume [tC stem-wood/ha], share of harvest (0 - 1)
        cov.data[{0, 2, 0.3}] = 4;
        cov.data[{40, 30, 0.2}] = 2;

        // We allow thinning from Dbh 3 cm to allow more energy wood (discussion with Fulvio 14 May 2020)
        dov.data[{0, 0, 0}] = 0;
        dov.data[{3, 5, 0.001}] = 0;  // Testing!
        dov.data[{10, 20, 0.01}] = 1;  // Testing!

        coe.data[{0, 2}] = 3;
        coe.data[{40, 30}] = 1;

        doe.data[{15, 10}] = 0;
        doe.data[{16, 11}] = 1;

        ffsws.overwrite(sws);
        ffhlv.overwrite(hlv);
        ffhle.overwrite(hle);
        ffsdMinH.overwrite(sdMinH);
        ffsdMaxH.overwrite(sdMaxH);

        ffcov = FFIpolM<double>{cov};
        ffcoe = FFIpolM<double>{coe};
        ffdov = FFIpolM<double>{dov};
        ffdoe = FFIpolM<double>{doe};
    }

    void correctMAI(const span<DataStruct> plots) {
        for (auto &plot: plots)
            // Test only some regions and some countries
            if (plot.protect.data.at(2000) == 0) {  // if there is no lerp, why not simple map then?
                // forest with specified age structure
                plot.MAIE.data[2000] *= maiCoefficients[plot.country];
                plot.MAIN.data[2000] *= maiCoefficients[plot.country];
            }
    }

    [[nodiscard]] array<double, numberOfCountries> calculateAverageMAI(const span<const DataStruct> plots) {
        INFO("calculating average MAI");
        array<double, numberOfCountries> fun_MAI_CountryUProtect{};
        array<double, numberOfCountries> forestAreaCountry{};
        for (const auto &plot: plots)
            if (plot.protect.data.at(2000) == 0) {
                double forestArea0 = plot.landArea * 100 * clamp(plot.forest, 0., 1.);
                if (forestArea0 > 0) {
                    // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
                    fun_MAI_CountryUProtect[plot.country - 1] += plot.MAIE.data.at(2000) * forestArea0;
                    forestAreaCountry[plot.country - 1] += forestArea0;
                }
            }

        for (auto &&[MAI, area]: rv::zip(fun_MAI_CountryUProtect, forestAreaCountry))
            if (area > 0)
                MAI /= area;

        for (size_t i = 0; i < fun_MAI_CountryUProtect.size(); ++i)
            if (fun_MAI_CountryUProtect[i] > 0)
                DEBUG("fun_MAI_CountryUProtect[{}] = {}", i, fun_MAI_CountryUProtect[i]);

        INFO("calculated average MAI");
        return fun_MAI_CountryUProtect;
    }

    [[nodiscard]] unordered_set<uint32_t> initPlotsSimuID(const span<const DataStruct> plots) noexcept {
        unordered_set<uint32_t> fun_plotsSimuID;
        fun_plotsSimuID.reserve(plots.size());

        for (const auto &plot: plots)
            fun_plotsSimuID.insert(plot.simuID);

        return fun_plotsSimuID;
    }

    [[nodiscard]] map<pair<uint32_t, uint32_t>, uint32_t>
    initPlotsXY_SimuID(const span<const DataStruct> plots) noexcept {
        map<pair<uint32_t, uint32_t>, uint32_t> fun_plotsXYSimuID;

        for (const auto &plot: plots)
            fun_plotsXYSimuID[{plot.x, plot.y}] = plot.simuID;

        return fun_plotsXYSimuID;
    }

    // Scaling the MAI climate shifters to the 2020 value (i.e., MAIShifter_year = MAIShifter_year/MAIShifter_2000, so the 2000 value = 1);
    void scaleMAIClimate2020(heterSimuIdScenariosType &simuIdScenarios) {
        if (!scaleMAIClimate) {
            INFO("scaleMAIClimate is turned off");
            return;
        }

        INFO("Scaling MAI climate shifters to the 2020 value!");
        for (auto &[scenario, MAI]: simuIdScenarios)
            for (auto &[simu_id, ipol]: MAI) {
                double reciprocal_value_2020 = 1 / ipol.data.at(2020);
                ipol *= reciprocal_value_2020;
            }
    }

    void add2020Disturbances() {
        for (auto &[id, ipol]: commonDisturbWind)
            ipol.data[2020] = ipol.data.at(2030) / 1.025;
        for (auto &[id, ipol]: commonDisturbFire)
            ipol.data[2020] = ipol.data.at(2030) / 1.05;
        for (auto &[id, ipol]: commonDisturbBiotic)
            ipol.data[2020] = ipol.data.at(2030) / 1.05;
    }

    void scaleDisturbance(simuIdType &disturbance, const uint16_t scaleYear) {
        for (auto &[id, ipol]: disturbance)
            ipol += -ipol.data.at(scaleYear);
    }

    void scaleDisturbances2020() {
        if (!scaleDisturbance2020) {
            INFO("scaleDisturbance2020 is turned off");
            return;
        }

        const uint16_t scaleYear = 2020;
        scaleDisturbance(commonDisturbWind, scaleYear);
        scaleDisturbance(commonDisturbFire, scaleYear);
        scaleDisturbance(commonDisturbBiotic, scaleYear);

        INFO("Disturbances are scaled to the {} value!", scaleYear);
    }

    void initGlobiomLandAndManagedForest() {
        array<double, numberOfCountries> woodHarvest{};
        array<double, numberOfCountries> woodLost{};

        double sawnW = 0;      // MG: get harvestable sawn-wood for the set (old) forest tC/ha for final cut.
        double restW = 0;      // MG: get harvestable rest-wood for the set (old) forest tC/ha for final cut.
        double sawnThW = 0;    // MG: get harvestable sawn-wood for the set (old) forest tC/ha for thinning.
        double restThW = 0;    // MG: get harvestable rest-wood for the set (old) forest tC/ha for thinning.
        double bmH = 0;        // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
        double bmTh = 0;       // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for thinning
        double harvRes = 0;    // MG: usable harvest residues for the set (old) forest tC/ha

        for (auto &plot: commonPlots) {
            plot.initForestArrange();  // initGlobiomLandGlobal included here
            double forestShare0 = max(0., plot.forest);
            plot.forestsArrangement();
            commonOForestShGrid.country(plot.x, plot.y) = plot.country;
            double maxAffor = plot.getMaxAffor();

            if (forestShare0 > maxAffor) {
                optional<double> opt_dfor = plot.initForestArea(forestShare0 - maxAffor);
                if (opt_dfor) {
                    // take years from globiomAfforMaxScenarios (initForestArea corrects in)
                    for (const auto year: globiomAfforMaxScenarios.at(bauScenario).at(plot.simuID).data | rv::keys)
                        if (year > 2000)
                            // before subtraction was later in initManagedForestLocal (values are negative!!!)
                            plot.GLOBIOM_reserved.data[year] = -*opt_dfor;
                }
                forestShare0 = maxAffor;
            }

            commonOForestShGrid(plot.x, plot.y) = forestShare0;
            commonOForestShGrid.update1YearForward();  // populate the OForestShGridPrev with forestShare0 data
            double forestArea0 = plot.landArea * 100 * forestShare0; // all forest area in the cell, ha

            double biomassRot = 0;     // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
            double biomassRotTh = 0;   // MG: rotation time fitted to get certain biomass under certain MAI (with thinning)
            double harvWood = 0;    // MG: harvestable wood, m3
            double abBiomassO = 0;
            // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
            // Max mean annual increment of New forest (with uniform age structure and managed with rotation length maximizing MAI)
            double MAI = max(0., forestShare0 > 0 ? plot.MAIE(coef.bYear) : plot.MAIN(coef.bYear));
            double defIncome = 0;

            double rotUnmanaged = 0, rotMAI = 0, rotMaxBm = 0, rotMaxBmTh = 0, rotHarvFin = 0, rotHarvAve = 0;

            if (plot.protect.data.at(2000) == 1)
                plot.managedFlag = false;

            commonMaiForest(plot.x, plot.y) = MAI;
            double harvMAI = MAI * plot.fTimber(coef.bYear) * (1 - coef.harvLoos);

            if (plot.CAboveHa > 0 && commonMaiForest(plot.x, plot.y) > 0) {
                if (plot.speciesType == Species::NoTree) {
                    ERROR("plot.speciesType = {}", static_cast<int>(plot.speciesType));
                    return;
                }
                // rotation time to get current biomass (without thinning)
                biomassRot = species[plot.speciesType].getU(plot.CAboveHa, MAI);
                // rotation time to get current biomass (with thinning)
                biomassRotTh = species[plot.speciesType].getUT(plot.CAboveHa, MAI);
            }

            if (commonMaiForest(plot.x, plot.y) > 0) {
                rotMAI = species[plot.speciesType].getTOptT(MAI, ORT::MAI);
                rotMaxBm = species[plot.speciesType].getTOpt(MAI, ORT::MaxBm);
                rotMaxBmTh = species[plot.speciesType].getTOptT(MAI, ORT::MaxBm);
            }

            DIMA decision{1990, plot.NPP, plot.sPopDens, plot.sAgrSuit, plot.priceIndex, coef.priceIndexE, plot.R,
                          coef.priceC, coef.plantingCostsR, coef.priceLandMinR, coef.priceLandMaxR, coef.maxRotInter,
                          coef.minRotInter, coef.decRateL, coef.decRateS, plot.fracLongProd, coef.baseline,
                          plot.fTimber, coef.priceTimberMaxR, coef.priceTimberMinR, coef.fCUptake, plot.GDP,
                          coef.harvLoos, forestShare0,
                          datamapScenarios.woodPriceScenarios.at(bauScenario).at(plot.country), rotMAI, harvMAI};

            double rotation = 0;
            if (plot.protect.data.at(2000) < 1) {
                rotation = max(biomassRotTh + 1, rotMAI);

                double pDefIncome =
                        plot.CAboveHa * (decision.priceTimber() * plot.fTimber(coef.bYear) * (1 - coef.harvLoos));
                // Immediate Pay if deforested (Slash and Burn)
                double sDefIncome = pDefIncome;
                defIncome =
                        pDefIncome * (1 - plot.slashBurn(coef.bYear)) + sDefIncome * plot.slashBurn(coef.bYear);

                if (plot.managedFlag) {
                    commonThinningForest(plot.x, plot.y) = 1;
                    commonRotationType(plot.x, plot.y) = 11;

                    if (MAI > MAI_CountryUprotect[plot.country - 1])
                        commonManagedForest(plot.x, plot.y) = 3;
                    else {
                        if (decision.forValNC() * hurdle_opt[plot.country - 1] > decision.agrVal() + defIncome)
                            commonManagedForest(plot.x, plot.y) = 2;
                        else
                            commonManagedForest(plot.x, plot.y) = 1;
                    }

                } else {
                    commonThinningForest(plot.x, plot.y) = -1;
                    commonRotationType(plot.x, plot.y) = 10;

                    if (MAI > MAI_CountryUprotect[plot.country - 1])
                        commonManagedForest(plot.x, plot.y) = 0;
                    else {
                        if (decision.forValNC() * hurdle_opt[plot.country - 1] > decision.agrVal() + defIncome)
                            commonManagedForest(plot.x, plot.y) = -1;
                        else
                            commonManagedForest(plot.x, plot.y) = -2;
                    }
                }

            } else {
                commonThinningForest(plot.x, plot.y) = -1;
                rotation = biomassRot + 1;
            }

            commonRotationForest(plot.x, plot.y) = rotation;
            commonThinningForest10(plot.x, plot.y) = -1;
            commonThinningForest30(plot.x, plot.y) = commonThinningForest(plot.x, plot.y);
        }
    }

    void setAsIds(const span<DataStruct> plots) noexcept {
        for (auto &&[i, plot]: plots | rv::enumerate)
            plot.asID = i;
    }

    [[nodiscard]] unordered_map<uint8_t, FFIpolsCountry>
    initCountriesFFIpols(const span<const DataStruct> plots) noexcept {
        unordered_map<uint8_t, FFIpolsCountry> fun_countriesFFIpols;
        fun_countriesFFIpols.reserve(256);

        for (const auto &plot: plots)
            fun_countriesFFIpols.emplace(plot.country, plot.country);

        return fun_countriesFFIpols;
    }

    pair<string, vector<vector<double> > > findAndReadBau(const string_view prefix) {
        string bauName = string{prefix} + string{suffix};
        for (const auto &dir_entry: fs::directory_iterator{settings.inputPath})
            if (dir_entry.path().string().contains(bauName))
                return {dir_entry.path().stem().string().substr(bauName.size())
                        | rv::transform(::toupper) | ranges::to<string>(),
                        readBau(dir_entry.path().filename().string(), prefix)};

        FATAL("file with bauName = {} is not found in {}", bauName, settings.inputPath);
        throw runtime_error{"Missing bau file"};
    }

    // Initialise forest objects with observed parameters in each grid cell
    void initLoop() {
        INFO("Start initialising cohorts");
        commonCohort_all.reserve(commonPlots.size());
        commonCohort30_all.reserve(commonPlots.size());
        commonCohort10_all.reserve(commonPlots.size());
        commonCohort_primary_all.reserve(commonPlots.size());
        commonNewCohort_all.reserve(commonPlots.size());
        commonDat_all.reserve(commonPlots.size());
        harvestResiduesCountry.reserve(256);

        const unordered_map<uint8_t, vector<double> > ageStructData = readAgeStructData();

        // type and size will be deduced
        constexpr array priceCiS = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 70, 100, 150};
        constexpr array ageBreaks = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 999};
        constexpr array ageSize = {11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

        for (size_t asId = 0; const auto &plot: commonPlots) {
            coef.priceC = priceCiS[0] * plot.corruption;

            double forestShare = clamp(plot.getForestShare(), 0., plot.getMaxAffor());

            // MG: 15 December 2020: make soft condition for Ireland (allowing harvesting of stands younger than MaiRot
            // according to the explanation of the national experts in December 2020 the actual rotation time is 30-40%
            // lover than the MAI rotation
            double minRotVal = (plot.country == 103) ? 0.6 : 1;

            double MAIRot = 1;         // MG: optimal rotation time (see above)
            double rotation = 1;
            double rotMaxBm = 1;
            double rotMaxBmTh = 1;
            double biomassRot = 1;     // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
            double biomassRotTh = 1;   // MG: rotation time fitted to get certain biomass under certain MAI (with thinning)

            bool forFlag = false;   // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlag10 = false; // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlag30 = false; // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlagP = false;  // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest

            double mai_tmp = commonMaiForest(plot.x, plot.y);
            double thinning_tmp = commonThinningForest(plot.x, plot.y);

            if (mai_tmp > 0) {
                MAIRot = max(1., species[plot.speciesType].getTOptT(mai_tmp, ORT::MAI));
                rotMaxBm = max(1., species[plot.speciesType].getTOpt(mai_tmp, ORT::MaxBm));
                rotMaxBmTh = max(1., species[plot.speciesType].getTOptT(mai_tmp, ORT::MaxBm));

                if (plot.CAboveHa > 0) {
                    biomassRot = max(1., species[plot.speciesType].getU(plot.CAboveHa, mai_tmp));
                    biomassRotTh = max(1., species[plot.speciesType].getUSdTab(plot.CAboveHa, mai_tmp,
                                                                               abs(thinning_tmp))); // with thinning

                    forFlag = plot.forest > 0;
                    forFlag10 = plot.oldGrowthForest_ten > 0;
                    forFlag30 = plot.oldGrowthForest_thirty > 0;
                    forFlagP = plot.strictProtected > 0;
                }
            }

            rotation = thinning_tmp > 0 ? biomassRotTh : biomassRot;

            double abBiomass0 = 0;  // Modelled biomass at time 0, tC/ha

            // Stocking degree depending on tree height is not implemented
            // saving results to initial vectors
            commonCohort_all.emplace_back(&species[plot.speciesType], &ffsws, &countriesFFIpols.at(plot.country),
                                          &ffcov,
                                          &ffcoe,
                                          &ffdov, &ffdoe, mai_tmp, 0, 1, 0, 0, 0, 0, thinning_tmp * sdMaxCoef,
                                          thinning_tmp * sdMinCoef, 30, minRotVal, 1, 0, 1);
            commonCohort30_all.push_back(commonCohort_all.back());
            commonCohort10_all.emplace_back(&species[plot.speciesType], &ffsws, &countriesFFIpols.at(plot.country),
                                            &ffcov,
                                            &ffcoe,
                                            &ffdov, &ffdoe, mai_tmp, 0, 1, 0, 0, 0, 0, -sdMaxCoef, -sdMinCoef, 30,
                                            minRotVal,
                                            1, 0, 1);
            commonCohort_primary_all.push_back(commonCohort10_all.back());

            AgeStruct &cohort = commonCohort_all.back();
            AgeStruct &cohort30 = commonCohort30_all.back();
            AgeStruct &cohort10 = commonCohort10_all.back();
            AgeStruct &cohort_primary = commonCohort_primary_all.back();

            size_t oldestAgeGroup = 0;
            double oldestAge = 0;

            if (ageStructData.contains(plot.country)) {
                oldestAgeGroup = distance(ranges::find_if(ageStructData.at(plot.country) | rv::reverse,
                                                          [](const auto x) { return x > 0; }),
                                          ageStructData.at(plot.country).rend()) - 1;  // last positive

                oldestAge = ageBreaks.at(oldestAgeGroup);
                if (oldestAge > 150)
                    oldestAge = rotMaxBm * 0.7;

            } else
                WARN("ageStructData doesn't contain plot.country = {} ({})",
                     plot.country, idCountryGLOBIOM.at(plot.country));

            if (plot.forest + plot.oldGrowthForest_thirty > 0 && mai_tmp > 0) {
                if (ageStructData.contains(plot.country) && thinning_tmp > 0 && (forFlag || forFlag30) &&
                    plot.potVeg < 10) {
                    cohort.createNormalForest(321, 0, 1);

                    for (size_t i = 1; i < 161; ++i) {
                        size_t ageGroup = distance(ageBreaks.begin(),
                                                   ranges::lower_bound(ageBreaks, 161));  // first x <= i
                        cohort.setArea(i, ageStructData.at(plot.country)[ageGroup] /
                                          static_cast<double>(ageSize[ageGroup]));
                    }

                    double biomass = cohort.getBm() * plot.BEF(cohort.getBm());
                    //Tune age structure for current cell
                    if (cohort.getArea() > 0 && biomass > 0) {

                        if (biomass < 0.95 * plot.CAboveHa) {

                            for (size_t young = 0, oag = oldestAgeGroup;
                                 biomass < 0.95 * plot.CAboveHa && oag < 30; ++young, ++oag)
                                if (ageSize[young] > 0 && oag > young)
                                    for (int i = 0; i < 10; ++i) {
                                        double halfAreaTmp = cohort.getArea(i + young * 10 + 1) * 0.5;
                                        cohort.setArea(i + young * 10 + 1, halfAreaTmp);
                                        cohort.setArea(i + (oag + 1) * 10 + 1, halfAreaTmp);
                                        biomass = cohort.getBm() * plot.BEF(cohort.getBm());
                                    }

                        } else if (biomass > 2 * plot.CAboveHa) {  // v_24_11

                            for (size_t young = 0, oag = oldestAgeGroup;
                                 biomass > 2 * plot.CAboveHa && oag > 2; ++young, --oag) {
                                if (oag > young) {
                                    if (ageSize[oag] > 0 && ageSize[young] > 0) {
                                        for (int i = 0; i < ageSize[oag]; ++i) {
                                            double areaTmp_oag = cohort.getArea(i + oag * 10 + 1);
                                            double areaTmp_young = cohort.getArea(i + young * 10 + 1);
                                            cohort.setArea(i + oag * 10 + 1, 0);
                                            cohort.setArea(i + young * 10 + 1, areaTmp_oag + areaTmp_young);
                                            biomass = cohort.getBm() * plot.BEF(cohort.getBm());
                                        }
                                    }
                                } else if (ageSize[oag] > 0 && ageSize[oag - 1] > 0) {
                                    for (int i = 0; i < ageSize[oag]; ++i) {
                                        double areaTmp_oag = cohort.getArea(i + oag * 10 + 1);
                                        double areaTmp_young = cohort.getArea(i + (oag - 1) * 10 + 1);
                                        cohort.setArea(i + oag * 10 + 1, 0);
                                        cohort.setArea(i + (oag - 1) * 10 + 1, areaTmp_oag + areaTmp_young);
                                        biomass = cohort.getBm() * plot.BEF(cohort.getBm());
                                    }
                                }
                            }

                        }

                        double stockingDegree =
                                max(0., plot.CAboveHa * cohort.getArea() / (cohort.getBm() * plot.BEF(cohort.getBm())));
                        cohort.setStockingDegreeMin(stockingDegree * sdMinCoef);
                        cohort.setStockingDegreeMax(stockingDegree * sdMaxCoef);
                        commonThinningForest(plot.x, plot.y) = stockingDegree;
                        for (int i = 0; i < 321; ++i)
                            cohort.setBm(i, stockingDegree * cohort.getBm(i));
                        cohort.setU(321);
                        auto ignored = cohort.aging();
                    }

                    rotation = max(MAIRot,
                                   species[plot.speciesType].getUSdTab(cohort.getBm() / cohort.getArea(), mai_tmp,
                                                                       thinning_tmp) + 1);
                    cohort.setU(rotation);

                    if (plot.oldGrowthForest_thirty > 0)
                        cohort30 = cohort;
                    else
                        cohort30.createNormalForest(1, 0, 1);

                } else if (forFlag || forFlag30) {
                    rotation = max(MAIRot, rotation);

                    cohort.createNormalForest(rotation, forFlag, thinning_tmp);
                    cohort.setStockingDegreeMin(thinning_tmp * sdMinCoef);
                    cohort.setStockingDegreeMax(thinning_tmp * sdMaxCoef);
                    cohort.setU(rotation);

                    if (plot.oldGrowthForest_thirty > 0) {
                        cohort30.createNormalForest(rotation, forFlag30, thinning_tmp);
                        cohort30.setStockingDegreeMin(thinning_tmp * sdMinCoef);
                        cohort30.setStockingDegreeMax(thinning_tmp * sdMaxCoef);
                        cohort30.setU(rotation);
                    } else
                        cohort30.createNormalForest(1, 0, thinning_tmp);
                } else {  // MG: create an existing forest with 0 area for consistency of the singleCell structure
                    cohort.createNormalForest(1, 0, thinning_tmp);
                    cohort30.createNormalForest(1, 0, thinning_tmp);
                }

                commonRotationForest(plot.x, plot.y) = rotation;
                abBiomass0 = cohort.getBm();  // modelled biomass at time 0, tC/ha

            } else {
                cohort.createNormalForest(1, forFlag, 1);
                cohort30.createNormalForest(1, forFlag30, 1);
            }

            if (plot.oldGrowthForest_ten > 0) {
                double biomassRot10 = species[plot.speciesType].getU(1.3 * plot.CAboveHa, mai_tmp);
                double MAIRot10 = species[plot.speciesType].getTOpt(mai_tmp, ORT::MAI);

                biomassRot10 = max(biomassRot10, 0.5 * oldestAge);
                biomassRot10 = max(biomassRot10, 1.5 * MAIRot10);

                cohort10.createNormalForest(biomassRot10, 1., -1.);
                cohort10.setU(biomassRot10);

                double oldest = cohort10.getActiveAge();
                double stockingDegree = 1;
                // 16.09.2021: In natural forest stocking 1 is already maximum
                if (plot.CAboveHa > 0 && cohort10.getArea() > 0)
                    stockingDegree = clamp(
                            1.3 * plot.CAboveHa * cohort10.getArea() / (cohort10.getBm() * plot.BEF(cohort10.getBm())),
                            0.6, 1.);

                cohort10.setStockingDegreeMin(-stockingDegree * sdMinCoef);
                cohort10.setStockingDegreeMax(-stockingDegree * sdMaxCoef);
                commonThinningForest10(plot.x, plot.y) = -stockingDegree;

                int tmp_max = static_cast<int>(ceil(oldest / modTimeStep)) + 1;
                for (int i = 0; i <= tmp_max; ++i)
                    cohort10.setBm(i, stockingDegree * cohort10.getBm(i * modTimeStep));

            } else
                cohort10.createNormalForest(1, 0, -1);

            if (plot.strictProtected > 0) {
                double biomassRotP = species[plot.speciesType].getU(1.3 * plot.CAboveHa, mai_tmp);
                double MAIRotP = species[plot.speciesType].getTOpt(mai_tmp, ORT::MAI);

                biomassRotP = max(biomassRotP, oldestAge);
                biomassRotP = max(biomassRotP, MAIRotP);

                cohort_primary.createNormalForest(biomassRotP, 1, -1);
                cohort_primary.setU(biomassRotP);

                double oldest = cohort_primary.getActiveAge();
                double stockingDegree = 1;
                //16.09.2021: In natural forest stocking 1 is already maximum
                if (plot.CAboveHa > 0 && cohort_primary.getArea() > 0)
                    stockingDegree = clamp(1.3 * plot.CAboveHa * cohort_primary.getArea() /
                                           (cohort_primary.getBm() * plot.BEF(cohort_primary.getBm())), 0.6, 1.);

                cohort_primary.setStockingDegreeMin(-stockingDegree * sdMinCoef);
                cohort_primary.setStockingDegreeMax(-stockingDegree * sdMaxCoef);

                int tmp_max = static_cast<int>(ceil(oldest / modTimeStep)) + 1;
                for (int i = 0; i <= tmp_max; ++i)
                    cohort_primary.setBm(i, stockingDegree * cohort_primary.getBm(i * modTimeStep));

            } else
                cohort_primary.createNormalForest(1, 0, -1);

            // rotation changes
            commonNewCohort_all.emplace_back(&species[plot.speciesType], &ffsws, &countriesFFIpols.at(plot.country),
                                             &ffcov,
                                             &ffcoe, &ffdov, &ffdoe, mai_tmp, 0, rotation, 0, 0, 0, 0,
                                             thinning_tmp * sdMaxCoef, thinning_tmp * sdMinCoef, 30, minRotVal, 1, 0,
                                             1);
            AgeStruct &newCohort = commonNewCohort_all.back();
            newCohort.createNormalForest(rotation, 0, thinning_tmp);

            commonDat_all.emplace_back();
            Dat &singleCell = commonDat_all.back();
            singleCell.rotation = commonRotationForest(plot.x, plot.y);
            singleCell.landAreaHa = plot.landArea * 100;
            singleCell.forestShare = plot.getForestShare();
            singleCell.forestShare0 = singleCell.forestShare;
            singleCell.OForestShare = singleCell.forestShare;
            singleCell.OForestShareU = plot.forest;
            singleCell.OForestShare10 = plot.oldGrowthForest_ten;
            singleCell.OForestShare30 = plot.oldGrowthForest_thirty;
            singleCell.prevOForShare = forestShare;  // MG: Old forest share in the previous reporting year
            singleCell.prevOForShare = singleCell.OForestShare;     // forest share of all old forest one modelling step back in each cell
            singleCell.prevOForShareU = singleCell.OForestShareU;   // forest share of "usual" old forest one modelling step back in each cell
            singleCell.prevOForShare10 = singleCell.OForestShare10; // forest share of 10% policy forest one modelling step back in each cell
            singleCell.prevOForShare30 = singleCell.OForestShare30; // forest share of 30% policy forest one modelling step back in each cell
            singleCell.prevOForShareRP = forestShare;  // MG: Old forest share in the previous reporting year
            singleCell.OBiomassPrev = abBiomass0;
            singleCell.OBiomass0 = abBiomass0;                   // Modelled biomass at time 0, tC/ha
            singleCell.OBiomassPrev10 = cohort10.getBm();   // Biomass of 10% policy forest on a previous step, tC/ha
            singleCell.OBiomassPrev30 = cohort30.getBm();   // Biomass of 30% policy forest on a previous step, tC/ha
            singleCell.OBiomassPrevP = cohort_primary.getBm();      // Biomass of primary forest on a previous step, tC/ha
            singleCell.oForestBm = abBiomass0;
            singleCell.oForestBm10 = singleCell.OBiomassPrev10;
            singleCell.oForestBm30 = singleCell.OBiomassPrev30;
            singleCell.oForestBmP = singleCell.OBiomassPrevP;
            singleCell.rotBiomass = rotation;
            singleCell.SD = thinning_tmp;
            singleCell.species = plot.speciesType;
            singleCell.deforPrev = plot.forLoss;
            singleCell.road = plot.road.data.at(2000);
            singleCell.slashBurn = plot.slashBurn.data.at(2000);
            singleCell.deadwood = plot.forest > 0 ? plot.deadWood : 0;
            singleCell.deadwood10 = plot.oldGrowthForest_ten > 0 ? plot.deadWood : 0;
            singleCell.deadwood30 = plot.oldGrowthForest_thirty > 0 ? plot.deadWood : 0;
            singleCell.deadwoodP = plot.strictProtected > 0 ? plot.deadWood : 0;
            singleCell.forest10 = plot.oldGrowthForest_ten;
            singleCell.forest30 = plot.oldGrowthForest_thirty;

            harvestResiduesCountry[plot.country].emplace_back();
            HarvestResidues &residuesCellTmp = harvestResiduesCountry[plot.country].back();
            residuesCellTmp.simuId = plot.simuID;
            residuesCellTmp.asID = plot.asID;
            residuesCellTmp.country = plot.country;
            residuesCellTmp.fTimber = plot.fTimber.data.at(2000);
            residuesCellTmp.costsSuit1 = plot.residuesUseCosts;
            residuesCellTmp.costsSuit2 = plot.residuesUseCosts + 10;
            residuesCellTmp.costsSuit3 = plot.residuesUseCosts;
            residuesCellTmp.costsSuit4_notTaken = plot.residuesUseCosts * 10;
        }
    }

    // MG: 22 July 2022
    // Converts all forests to unused, then converts back to used starting from most productive forest in each country
    // while potential harvest (MAI - harvest losses) satisfies wood demand initial year.
    // Wood and land prices by countries!
    // Estimate area of wood production forests and initialize respective forest objects in each cell.
    void initZeroProdArea() {
        if (!zeroProdAreaInit) {
            INFO("initZeroProdArea is turned off");
            return;
        }

        auto thinningForestInit = commonThinningForest;
        array<double, numberOfCountries> woodPotHarvest{};

        INFO("Putting data for current cell into container...");
        for (const auto &plot: commonPlots)
            if (plot.protect.data.at(2000) == 0) {
                double MAI = commonMaiForest(plot.x, plot.y);  // MG: mean annual increment in tC/ha/2000

                double biomassRot = 1;  // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
                double biomassRotTh = 1;
                double rotMAI = 1;
                double rotMaxBm = 1;

                double Bm = commonCohort_all[plot.asID].getBm();

                if (plot.CAboveHa > 0 && MAI > 0) {
                    // rotation time to get current biomass (without thinning)
//                        biomassRot = species[plot.speciesType].getU(Bm, MAI);  overwritten
                    rotMAI = species[plot.speciesType].getTOpt(MAI, ORT::MAI);
                    rotMaxBm = species[plot.speciesType].getTOpt(MAI, ORT::MaxBm);
                    // rotation time to get current biomass (with thinning)
                    biomassRotTh = species[plot.speciesType].getUSdTab(Bm, MAI, commonThinningForest(plot.x, plot.y));
                }

                biomassRot = max(rotMaxBm, commonRotationForest(plot.x, plot.y));
                DIMA decision{1990, plot.NPP, plot.sPopDens, plot.sAgrSuit, plot.priceIndex, coef.priceIndexE,
                              plot.R, coef.priceC, coef.plantingCostsR, coef.priceLandMinR, coef.priceLandMaxR,
                              coef.maxRotInter, coef.minRotInter, coef.decRateL, coef.decRateS, plot.fracLongProd,
                              coef.baseline, plot.fTimber, coef.priceTimberMaxR, coef.priceTimberMinR,
                              coef.fCUptake, plot.GDP, coef.harvLoos,
                              commonOForestShGrid(plot.x, plot.y) -
                              plot.strictProtected,  // forestShare0 - forest available for wood supply initially
                              datamapScenarios.woodPriceScenarios.at(bauScenario).at(plot.country), rotMAI,
                              MAI * plot.fTimber.data.at(2000) * (1 - coef.harvLoos)};  // harvMAI

                double thinning = -1;
                double rotation = 1;

                if (commonThinningForest(plot.x, plot.y) > 0) {
                    commonThinningForest(plot.x, plot.y) = thinning;
                    commonThinningForest30(plot.x, plot.y) = thinning;
                    commonRotationType(plot.x, plot.y) = 10;

                    commonCohort_all[plot.asID].setStockingDegree(thinning);
                    commonNewCohort_all[plot.asID].setStockingDegree(thinning);
                    commonCohort30_all[plot.asID].setStockingDegree(thinning);

                    // defIncome = 0 => decision.agrVal() + defIncome = decision.agrVal()
                    rotation = max(biomassRot, rotMAI) + 1;

                    if (MAI > MAI_CountryUprotect[plot.country - 1]) {
                        if (decision.forValNC() * hurdle_opt[plot.country - 1] > decision.agrVal()) {
                            commonManagedForest(plot.x, plot.y) = 0;
                            commonRotationType(plot.x, plot.y) = 1;
                        } else {
                            commonManagedForest(plot.x, plot.y) = -1;
                            commonRotationType(plot.x, plot.y) = 10;
                        }
                    } else {
                        if (decision.forValNC() * hurdle_opt[plot.country - 1] > decision.agrVal()) {
                            commonManagedForest(plot.x, plot.y) = -1;
                            commonRotationType(plot.x, plot.y) = 10;
                        } else {
                            commonManagedForest(plot.x, plot.y) = -2;
                            commonRotationType(plot.x, plot.y) = 10;
                        }
                    }

                    commonRotationForest(plot.x, plot.y) = rotation;
                    commonCohort_all[plot.asID].setU(rotation);
                    commonNewCohort_all[plot.asID].setU(rotation);
                    commonCohort30_all[plot.asID].setU(rotation);
                }
            }

        for (const auto &plot: commonPlots)
            if (plot.protect.data.at(2000) == 0 && thinningForestInit(plot.x, plot.y) > 0) {
                double MAI = commonMaiForest(plot.x, plot.y);  // MG: mean annual increment in tC/ha/2000

                double rotMAI = 0;
                double rotMaxBm = 0;
                double rotMaxBmTh = 0;
                double biomassRotTh2 = 0;  // MG: rotation time fitted to get certain biomass under certain MAI (with thinning = 2)

                double stockingDegree = thinningForestInit(plot.x, plot.y);
                double Bm = commonCohort_all[plot.asID].getBm();
                double rotation = 0;

                if (plot.CAboveHa > 0 && MAI > 0) {
                    // rotation time to get current biomass (with thinning)
                    biomassRotTh2 = species[plot.speciesType].getUSdTab(Bm, MAI, stockingDegree);
                    rotMAI = species[plot.speciesType].getTOptSdTab(MAI, stockingDegree, ORT::MAI);
                    rotMaxBmTh = species[plot.speciesType].getTOptSdTab(MAI, stockingDegree, ORT::MaxBm);
                } else if (MAI > 0) {
                    rotMAI = species[plot.speciesType].getTOpt(MAI, ORT::MAI);
                    rotMaxBm = species[plot.speciesType].getTOpt(MAI, ORT::MaxBm);
                }

                if (datamapScenarios.woodPriceScenarios.at(bauScenario).at(plot.country)(coef.bYear) >
                    woodPotHarvest[plot.country - 1]) {
                    if (commonManagedForest(plot.x, plot.y) == 0) {
                        rotation = rotMAI + 1;
                        commonManagedForest(plot.x, plot.y) = 3;
                        commonRotationType(plot.x, plot.y) = 1;
                    } else if (commonManagedForest(plot.x, plot.y) == -1) {
                        rotation = min(rotMAI + 1, rotMaxBmTh);
                        commonManagedForest(plot.x, plot.y) = 2;
                        commonRotationType(plot.x, plot.y) = 2;
                    } else if (commonManagedForest(plot.x, plot.y) == -2) {
                        commonManagedForest(plot.x, plot.y) = 1;
                        commonRotationType(plot.x, plot.y) = 3;
                        rotation = clamp(biomassRotTh2 + 1, rotMAI, rotMaxBmTh);
                    }

                    double harvMAI = MAI * plot.fTimber(coef.bYear) * (1 - coef.harvLoos);
                    // area of forest available for wood supply
                    double forestArea0 = plot.landArea * 100 * (plot.forest + plot.oldGrowthForest_thirty);
                    woodPotHarvest[plot.country - 1] += harvMAI * forestArea0;

                    commonRotationForest(plot.x, plot.y) = rotation;
                    commonCohort_all[plot.asID].setU(rotation);

                    commonThinningForest(plot.x, plot.y) = stockingDegree;
                    commonCohort_all[plot.asID].setStockingDegree(stockingDegree);

                    commonNewCohort_all[plot.asID].setU(rotation);
                    commonNewCohort_all[plot.asID].setStockingDegree(stockingDegree);

                    commonCohort30_all[plot.asID].setU(rotation);
                    commonThinningForest30(plot.x, plot.y) = stockingDegree;
                    commonCohort30_all[plot.asID].setStockingDegree(stockingDegree);
                }
            }
    }
}

#endif
