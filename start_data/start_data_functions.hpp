#ifndef G4M_EUROPE_DG_START_DATA_FUNCTIONS_HPP
#define G4M_EUROPE_DG_START_DATA_FUNCTIONS_HPP

#include <future>

#include "../start_data/start_data.hpp"
#include "../increment/dima.hpp"
#include "../settings/dicts/dicts.hpp"

using namespace g4m;
using namespace g4m::Dicts;
using namespace g4m::init;

namespace g4m::StartData {
    [[nodiscard]] CountryData setCountriesWoodProdStat() {
        CountryData fun_countriesWoodProdStat;

        for (size_t i = 0; i < woodProdEUStats.size(); ++i)
            for (size_t j = 0; j < woodProdEUStats[0].size(); ++j)
                // Malta is in countryNwp[18], there are no wood production data for Malta
                fun_countriesWoodProdStat.setVal(countryNwp[i + (i >= 18)], 1990 + j, woodProdEUStats[i][j]);

        return fun_countriesWoodProdStat;
    }

    [[nodiscard]] CountryData setCountriesFmEmission_unfccc() {
        CountryData fun_countriesFmEmission_unfccc;

        for (size_t i = 0; i < fmEmission_unfccc_CRF.size(); ++i)
            for (size_t j = 0; j < fmEmission_unfccc_CRF[0].size(); ++j)
                fun_countriesFmEmission_unfccc.setVal(eu28OrderCode[i], 1990 + j, fmEmission_unfccc_CRF[i][j]);

        return fun_countriesFmEmission_unfccc;
    }

    void calcAvgFM_sink_stat() {
        for (size_t i = 0; i < fmEmission_unfccc_CRF.size(); ++i) {
            double fmSinkSumTmp = 0;
            int count = 0;
            for (; count < min(adjustLength, fmEmission_unfccc_CRF[0].size()) &&
                   coef.bYear - 1990 + count < fmEmission_unfccc_CRF[0].size(); ++count)
                fmSinkSumTmp -= fmEmission_unfccc_CRF[i][coef.bYear - 1990 + count];
            FM_sink_stat[eu28OrderCode[i]] = count > 0 ? fmSinkSumTmp * 1'000 / count : 0; // GgCO2/year
        }
    }

    void initGlobiomLandAndManagedForest() {
        for (auto &plot: plots.filteredPlots) {
            plot.initForestArrange();  // initGlobiomLandGlobal included here
            double forestShare0 = max(0., plot.forest);
            plot.forestsArrangement();
            commonOForestShGrid.country(plot.x, plot.y) = plot.country;
            double maxAffor = plot.getMaxForestShare(2000);

            if (forestShare0 > maxAffor) {
                optional<double> opt_dfor = plot.initForestArea(forestShare0 - maxAffor);
                if (opt_dfor) {
                    // take years from GLOBIOM_AfforMaxScenarios (initForestArea corrects in)
                    for (const auto year:
                            simuIdScenarios.GLOBIOM_AfforMaxScenarios.at(settings.bauScenario).at(plot.simuID).data |
                            rv::keys)
                        if (year > 2000)
                            // before subtraction was later in initManagedForestLocal (values are negative!!!)
                            plot.GLOBIOM_reserved.data[year] = -*opt_dfor;
                }
                forestShare0 = maxAffor;
            }

            commonOForestShGrid(plot.x, plot.y) = forestShare0;
            commonOForestShGrid.update1YearForward();  // populate the OForestShGridPrev with forestShare0 data
            // double forestArea0 = plot.landArea * 100 * forestShare0; // all forest area in the cell, ha

            double biomassRot = 0;     // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
            double biomassRotTh = 0;   // MG: rotation time fitted to get certain biomass under certain MAI (with thinning)
//            double harvWood = 0;    // MG: harvestable wood, m3
//            double abBiomassO = 0;
            // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
            // Max mean annual increment of New forest (with uniform age structure and managed with rotation length maximizing MAI)
            double MAI = max(0., forestShare0 > 0 ? plot.MAIE(coef.bYear) : plot.MAIN(coef.bYear));

            if (plot.protect)
                plot.managedFlag = false;

            commonMaiForest(plot.x, plot.y) = MAI;
            double harvMAI = MAI * plot.fTimber * (1 - coef.harvLoos);

            if (plot.CAboveHa > 0 && commonMaiForest(plot.x, plot.y) > 0) {
                if (plot.speciesType == Species::NoTree) {
                    ERROR("plot.speciesType = {}", static_cast<int>(plot.speciesType));
                    return;
                }
                // rotation time to get current biomass (without thinning)
                biomassRot = species.at(plot.speciesType).getU(plot.CAboveHa, MAI);
                // rotation time to get current biomass (with thinning)
                biomassRotTh = species.at(plot.speciesType).getUT(plot.CAboveHa, MAI);
            }

            double rotMAI = commonMaiForest(plot.x, plot.y) > 0 ? species.at(plot.speciesType).getTOptT(MAI,
                                                                                                        OptRotTimes::Mode::MAI)
                                                                : 0;

            DIMA decision{1990, plot.NPP, plot.sPopDens, plot.sAgrSuit, plot.priceIndex, coef.priceIndexE, plot.R,
                          coef.priceC, coef.plantingCostsR, coef.priceLandMinR, coef.priceLandMaxR, coef.maxRotInter,
                          coef.minRotInter, coef.decRateL, coef.decRateS, plot.fracLongProd, coef.baseline,
                          plot.fTimber, coef.priceTimberMaxR, coef.priceTimberMinR, coef.fCUptake, plot.GDP,
                          coef.harvLoos, forestShare0,
                          datamapScenarios.woodPriceScenarios.at(settings.bauScenario).at(plot.country), rotMAI,
                          harvMAI};

            double rotation = 0;
            if (!plot.protect) {
                rotation = max(biomassRotTh + 1, rotMAI);

                double pDefIncome =
                        plot.CAboveHa * (decision.priceTimber() * plot.fTimber * (1 - coef.harvLoos));
                // Immediate Pay if deforested (Slash and Burn)
                double sDefIncome = pDefIncome;
                double defIncome = lerp(pDefIncome, sDefIncome, plot.slashBurn);

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

    // Initialise forest objects with observed parameters in each grid cell
    void initLoop() {
        INFO("Start initialising cohorts");
        commonCohortsU.reserve(plots.filteredPlots.size());
        commonCohorts30.reserve(plots.filteredPlots.size());
        commonCohorts10.reserve(plots.filteredPlots.size());
        commonCohortsP.reserve(plots.filteredPlots.size());
        commonCohortsN.reserve(plots.filteredPlots.size());
        commonDats.reserve(plots.filteredPlots.size());
        commonHarvestResiduesCountry.reserve(256);

        // type and size will be deduced
        constexpr array priceCiS = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 70, 100, 150};
        constexpr array ageBreaks = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 999};
        constexpr array ageSize = {11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

        for (const auto &plot: plots.filteredPlots) {
            coef.priceC = priceCiS[0] * plot.corruption;

//            double forestShare = clamp(plot.getForestShare(), 0., plot.getMaxAffor());

            // MG: 15 December 2020: make soft condition for Ireland (allowing harvesting of stands younger than MaiRot
            // according to the explanation of the national experts in December 2020 the actual rotation time is 30-40%
            // lover than the MAI rotation
            double minRotVal = (plot.country == 103) ? 0.6 : 1;

            double MAIRot = 1;         // MG: optimal rotation time (see above)
            double rotation = 1;
            double rotMaxBm = 1;
//            double rotMaxBmTh = 1;
            double biomassRot = 1;     // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
            double biomassRotTh = 1;   // MG: rotation time fitted to get certain biomass under certain MAI (with thinning)

            bool forFlag = false;   // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlag10 = false; // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlag30 = false; // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlagP = false;  // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest

            double mai_tmp = commonMaiForest(plot.x, plot.y);
            double thinning_tmp = commonThinningForest(plot.x, plot.y);

            if (mai_tmp > 0) {
                MAIRot = max(1., species.at(plot.speciesType).getTOptT(mai_tmp, OptRotTimes::Mode::MAI));
                rotMaxBm = max(1., species.at(plot.speciesType).getTOpt(mai_tmp, OptRotTimes::Mode::MaxBm));
//                rotMaxBmTh = max(1., species.at(plot.speciesType).getTOptT(mai_tmp, OptRotTimes::Mode::MaxBm));

                if (plot.CAboveHa > 0) {
                    biomassRot = max(1., species.at(plot.speciesType).getU(plot.CAboveHa, mai_tmp));
                    biomassRotTh = max(1., species.at(plot.speciesType).getUSdTab(plot.CAboveHa, mai_tmp,
                                                                                  abs(thinning_tmp))); // with thinning

                    forFlag = plot.forest > 0;
                    forFlag10 = plot.oldGrowthForest_ten > 0;
                    forFlag30 = plot.oldGrowthForest_thirty > 0;
                    forFlagP = plot.strictProtected > 0;
                }
            }

            rotation = thinning_tmp > 0 ? biomassRotTh : biomassRot;
            double abBiomass0 = 0;  // Modelled biomass at time 0, tC/ha

            auto &[hlv, hle] = fmp.hlveCountries.at(plot.country);

            // Stocking degree depending on tree height is not implemented
            // saving results to initial vectors
            commonCohortsU.emplace_back(&species.at(plot.speciesType), &fmp.sws, &hlv, &hle, &fmp.cov, &fmp.coe,
                                        &fmp.decisions, mai_tmp, 0, 1, 0, 0, 0, 0, thinning_tmp * sdMaxCoef,
                                        thinning_tmp * sdMinCoef, 30, minRotVal, 1, 0, 1);
            commonCohorts30.push_back(commonCohortsU.back());
            commonCohorts10.emplace_back(&species.at(plot.speciesType), &fmp.sws, &hlv, &hle, &fmp.cov, &fmp.coe,
                                         &fmp.decisions, mai_tmp, 0, 1, 0, 0, 0, 0, -sdMaxCoef, -sdMinCoef, 30,
                                         minRotVal, 1, 0, 1);
            commonCohortsP.push_back(commonCohorts10.back());

            AgeStruct &cohort = commonCohortsU.back();
            AgeStruct &cohort30 = commonCohorts30.back();
            AgeStruct &cohort10 = commonCohorts10.back();
            AgeStruct &cohort_primary = commonCohortsP.back();

            size_t oldestAgeGroup = 0;
            double oldestAge = 0;

            if (asd.ageStructData.contains(plot.country)) {
                oldestAgeGroup = distance(ranges::find_if(asd.ageStructData.at(plot.country) | rv::reverse,
                                                          [](const auto x) { return x > 0; }),
                                          asd.ageStructData.at(plot.country).rend()) - 1;  // last positive

                oldestAge = ageBreaks.at(oldestAgeGroup);
                if (oldestAge > 150)
                    oldestAge = rotMaxBm * 0.7;

            } else
                WARN("ageStructData doesn't contain plot.country = {} ({})",
                     plot.country, idCountryGLOBIOM.at(plot.country));

            if (plot.forest + plot.oldGrowthForest_thirty > 0 && mai_tmp > 0) {
                if (asd.ageStructData.contains(plot.country) && thinning_tmp > 0 && (forFlag || forFlag30) &&
                    static_cast<uint8_t>(plot.potVeg) < 10) {

//                    DEBUG("before createNormalForest");
//                    for (const auto &[i, el]: cohort.getDat() | rv::enumerate)
//                        DEBUG("dat[{}] = [{}]", i, el.str());

                    cohort.createNormalForest(321, 0, 1);

//                    DEBUG("after createNormalForest");
//                    for (const auto &[i, el]: cohort.getDat() | rv::enumerate)
//                        DEBUG("dat[{}] = [{}]", i, el.str());

                    for (size_t i = 1; i < 161; ++i) {
                        size_t ageGroup = distance(ageBreaks.begin(),
                                                   ranges::lower_bound(ageBreaks, i));  // first x <= i
                        cohort.setArea(i, asd.ageStructData.at(plot.country)[ageGroup] /
                                          static_cast<double>(ageSize[ageGroup]));
                    }

                    double cohort_bm = cohort.getBm();
                    double biomass = cohort_bm * plot.BEF(cohort_bm);
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
                                        cohort_bm = cohort.getBm();
                                        biomass = cohort_bm * plot.BEF(cohort_bm);
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
                                            cohort_bm = cohort.getBm();
                                            biomass = cohort_bm * plot.BEF(cohort_bm);
                                        }
                                    }
                                } else if (ageSize[oag] > 0 && ageSize[oag - 1] > 0) {
                                    for (int i = 0; i < ageSize[oag]; ++i) {
                                        double areaTmp_oag = cohort.getArea(i + oag * 10 + 1);
                                        double areaTmp_young = cohort.getArea(i + (oag - 1) * 10 + 1);
                                        cohort.setArea(i + oag * 10 + 1, 0);
                                        cohort.setArea(i + (oag - 1) * 10 + 1, areaTmp_oag + areaTmp_young);
                                        cohort_bm = cohort.getBm();
                                        biomass = cohort_bm * plot.BEF(cohort_bm);
                                    }
                                }
                            }

                        }

                        double stockingDegree = max(0., plot.CAboveHa * cohort.getArea() / biomass);
                        cohort.setStockingDegreeMin(stockingDegree * sdMinCoef);
                        cohort.setStockingDegreeMax(stockingDegree * sdMaxCoef);
                        commonThinningForest(plot.x, plot.y) = stockingDegree;

                        cohort.correctBmBySD(stockingDegree);
                        cohort.setU(321);

//                        DEBUG("after setU(321), before aging");
//                        for (const auto &[i, el]: cohort.getDat() | rv::enumerate)
//                            DEBUG("dat[{}] = [{}]", i, el.str());

                        auto _ = cohort.aging();

//                        DEBUG("after aging");
//                        for (const auto &[i, el]: cohort.getDat() | rv::enumerate)
//                            DEBUG("dat[{}] = [{}]", i, el.str());
                    }

                    rotation = max(MAIRot,
                                   species.at(plot.speciesType).getUSdTab(cohort.getBm() / cohort.getArea(), mai_tmp,
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
                double biomassRot10 = species.at(plot.speciesType).getU(1.3 * plot.CAboveHa, mai_tmp);
                double MAIRot10 = species.at(plot.speciesType).getTOpt(mai_tmp, OptRotTimes::Mode::MAI);

                biomassRot10 = max(biomassRot10, 0.5 * oldestAge);
                biomassRot10 = max(biomassRot10, 1.5 * MAIRot10);

                cohort10.createNormalForest(biomassRot10, 1., -1.);
                cohort10.setU(biomassRot10);

                double stockingDegree = 1;
                // 16.09.2021: In natural forest stocking 1 is already maximum
                if (plot.CAboveHa > 0 && cohort10.getArea() > 0)
                    stockingDegree = clamp(
                            1.3 * plot.CAboveHa * cohort10.getArea() / (cohort10.getBm() * plot.BEF(cohort10.getBm())),
                            0.6, 1.);

                cohort10.setStockingDegreeMin(-stockingDegree * sdMinCoef);
                cohort10.setStockingDegreeMax(-stockingDegree * sdMaxCoef);
                commonThinningForest10(plot.x, plot.y) = -stockingDegree;

                cohort10.correctBmBySD(stockingDegree);

            } else
                cohort10.createNormalForest(1, 0, -1);

            if (plot.strictProtected > 0) {
                double biomassRotP = species.at(plot.speciesType).getU(1.3 * plot.CAboveHa, mai_tmp);
                double MAIRotP = species.at(plot.speciesType).getTOpt(mai_tmp, OptRotTimes::Mode::MAI);

                biomassRotP = max(biomassRotP, oldestAge);
                biomassRotP = max(biomassRotP, MAIRotP);

                cohort_primary.createNormalForest(biomassRotP, 1, -1);
                cohort_primary.setU(biomassRotP);

                double stockingDegree = 1;
                //16.09.2021: In natural forest stocking 1 is already maximum
                if (plot.CAboveHa > 0 && cohort_primary.getArea() > 0)
                    stockingDegree = clamp(1.3 * plot.CAboveHa * cohort_primary.getArea() /
                                           (cohort_primary.getBm() * plot.BEF(cohort_primary.getBm())), 0.6, 1.);

                cohort_primary.setStockingDegreeMin(-stockingDegree * sdMinCoef);
                cohort_primary.setStockingDegreeMax(-stockingDegree * sdMaxCoef);

                cohort_primary.correctBmBySD(stockingDegree);

            } else
                cohort_primary.createNormalForest(1, 0, -1);

            // rotation changes
            commonCohortsN.emplace_back(&species.at(plot.speciesType), &fmp.sws, &hlv, &hle, &fmp.cov, &fmp.coe,
                                        &fmp.decisions, mai_tmp, 0, rotation, 0, 0, 0, 0,
                                        thinning_tmp * sdMaxCoef, thinning_tmp * sdMinCoef, 30, minRotVal, 1, 0,
                                        1);
            AgeStruct &newCohort = commonCohortsN.back();
            newCohort.createNormalForest(rotation, 0, thinning_tmp);

            commonDats.emplace_back(plot, commonRotationForest(plot.x, plot.y), abBiomass0, cohort10.getBm(),
                                    cohort30.getBm(), cohort_primary.getBm(), thinning_tmp);

            commonHarvestResiduesCountry[plot.country].emplace_back(plot);
        }
    }

    // MG: 22 July 2022
    // Converts all forests to unused, then converts back to used starting from most productive forest in each country
    // while potential harvest (MAI - harvest losses) satisfies wood demand initial year.
    // Wood and land prices by countries!
    // Estimate area of wood production forests and initialize respective forest objects in each cell.
    void initZeroProdArea() {
        auto thinningForestInit = commonThinningForest;
        array<double, numberOfCountries> woodPotHarvest{};

        INFO("Putting data for current cell into container...");
        for (const auto &plot: plots.filteredPlots)
            if (!plot.protect) {
                double MAI = commonMaiForest(plot.x, plot.y);  // MG: mean annual increment in tC/ha/2000

                double biomassRot = 1;  // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
//                double biomassRotTh = 1;
                double rotMAI = 1;
                double rotMaxBm = 1;

//                double bm = commonCohortsU[plot.asID].getBm();

                if (plot.CAboveHa > 0 && MAI > 0) {
                    // rotation time to get current biomass (without thinning)
//                        biomassRot = species.at(plot.speciesType).getU(Bm, MAI);  overwritten
                    rotMAI = species.at(plot.speciesType).getTOpt(MAI, OptRotTimes::Mode::MAI);
                    rotMaxBm = species.at(plot.speciesType).getTOpt(MAI, OptRotTimes::Mode::MaxBm);
                    // rotation time to get current biomass (with thinning)
//                    biomassRotTh = species.at(plot.speciesType).getUSdTab(bm, MAI, commonThinningForest(plot.x, plot.y));
                }

                biomassRot = max(rotMaxBm, commonRotationForest(plot.x, plot.y));
                DIMA decision{1990, plot.NPP, plot.sPopDens, plot.sAgrSuit, plot.priceIndex, coef.priceIndexE,
                              plot.R, coef.priceC, coef.plantingCostsR, coef.priceLandMinR, coef.priceLandMaxR,
                              coef.maxRotInter, coef.minRotInter, coef.decRateL, coef.decRateS, plot.fracLongProd,
                              coef.baseline, plot.fTimber, coef.priceTimberMaxR, coef.priceTimberMinR,
                              coef.fCUptake, plot.GDP, coef.harvLoos,
                              commonOForestShGrid(plot.x, plot.y) -
                              plot.strictProtected,  // forestShare0 - forest available for wood supply initially
                              datamapScenarios.woodPriceScenarios.at(settings.bauScenario).at(plot.country), rotMAI,
                              MAI * plot.fTimber * (1 - coef.harvLoos)};  // harvMAI

                double thinning = -1;
                double rotation = 1;

                if (commonThinningForest(plot.x, plot.y) > 0) {
                    commonThinningForest(plot.x, plot.y) = thinning;
                    commonThinningForest30(plot.x, plot.y) = thinning;
                    commonRotationType(plot.x, plot.y) = 10;

                    commonCohortsU[plot.asID].setStockingDegree(thinning);
                    commonCohortsN[plot.asID].setStockingDegree(thinning);
                    commonCohorts30[plot.asID].setStockingDegree(thinning);

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
                    commonCohortsU[plot.asID].setU(rotation);
                    commonCohortsN[plot.asID].setU(rotation);
                    commonCohorts30[plot.asID].setU(rotation);
                }
            }

        for (const auto &plot: plots.filteredPlots)
            if (!plot.protect && thinningForestInit(plot.x, plot.y) > 0) {
                double MAI = commonMaiForest(plot.x, plot.y);  // MG: mean annual increment in tC/ha/2000

                double rotMAI = 0;
//                double rotMaxBm = 0;
                double rotMaxBmTh = 0;
                double biomassRotTh2 = 0;  // MG: rotation time fitted to get certain biomass under certain MAI (with thinning = 2)

                double stockingDegree = thinningForestInit(plot.x, plot.y);
                double bm = commonCohortsU[plot.asID].getBm();
                double rotation = 0;

                if (plot.CAboveHa > 0 && MAI > 0) {
                    // rotation time to get current biomass (with thinning)
                    biomassRotTh2 = species.at(plot.speciesType).getUSdTab(bm, MAI, stockingDegree);
                    rotMAI = species.at(plot.speciesType).getTOptSdTab(MAI, stockingDegree, OptRotTimes::Mode::MAI);
                    rotMaxBmTh = species.at(plot.speciesType).getTOptSdTab(MAI, stockingDegree,
                                                                           OptRotTimes::Mode::MaxBm);
                } else if (MAI > 0) {
                    rotMAI = species.at(plot.speciesType).getTOpt(MAI, OptRotTimes::Mode::MAI);
//                    rotMaxBm = species.at(plot.speciesType).getTOpt(MAI, OptRotTimes::Mode::MaxBm);
                }

                if (datamapScenarios.woodDemandScenarios.at(settings.bauScenario).at(plot.country)(coef.bYear) >
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

                    double harvMAI = MAI * plot.fTimber * (1 - coef.harvLoos);
                    // area of forest available for wood supply
                    double forestArea0 = plot.landArea * 100 * (plot.forest + plot.oldGrowthForest_thirty);
                    woodPotHarvest[plot.country - 1] += harvMAI * forestArea0;

                    commonRotationForest(plot.x, plot.y) = rotation;
                    commonCohortsU[plot.asID].setU(rotation);

                    commonThinningForest(plot.x, plot.y) = stockingDegree;
                    commonCohortsU[plot.asID].setStockingDegree(stockingDegree);

                    commonCohortsN[plot.asID].setU(rotation);
                    commonCohortsN[plot.asID].setStockingDegree(stockingDegree);

                    commonCohorts30[plot.asID].setU(rotation);
                    commonThinningForest30(plot.x, plot.y) = stockingDegree;
                    commonCohorts30[plot.asID].setStockingDegree(stockingDegree);
                }
            }
    }
}

#endif
