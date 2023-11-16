#ifndef G4M_EUROPE_DG_AGE_STRUCT_HPP
#define G4M_EUROPE_DG_AGE_STRUCT_HPP

#include <cmath>
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>
#include <array>
#include <span>
#include <ranges>

#include "../misc/concrete/ffipolm.hpp"
#include "../init/ffipols_country.hpp"
#include "../log.hpp"
#include "v.hpp"
#include "cohort.hpp"
#include "shelter_wood_timer.hpp"
#include "increment_tab.hpp"
#include "constants.hpp"

using namespace std;
namespace rv = ranges::views;
using namespace g4m::misc::concrete;
using namespace g4m::init;

namespace g4m::increment {
    // simulator of forest growth, development, and forest management
    // this class doesn't own ffipols!
    class AgeStruct {
    public:
        // returns harvest
        [[nodiscard]] static double cohortRes(double realArea, const pair<V, V> &res) noexcept {
            if (realArea <= 0)
                return 0;

            double reciprocalRealAreaO = 1 / realArea;
            double areaRatio = res.second.area * reciprocalRealAreaO;  // harvArea / realArea

            // MG: get harvestable sawn-wood for the set (old) forest tC/ha for final cut.
            double sawnW = res.second.sw * areaRatio;
            // MG: get harvestable rest-wood for the set (old) forest tC/ha for final cut.
            double restW = res.second.rw * areaRatio;
            // MG: get harvestable sawn-wood for the set (old) forest tC/ha for thinning.
            double sawnThW = res.first.sw * reciprocalRealAreaO;
            // MG: get harvestable rest-wood for the set (old) forest tC/ha for thinning.
            double restThW = res.first.rw * reciprocalRealAreaO;
            // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
            double bmH = res.second.bm * areaRatio;
            // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for thinning
            double bmTh = res.first.bm * reciprocalRealAreaO;
            // MG: usable harvest residues for the set (old) forest tC/ha
            double harvRes = (bmH + bmTh - (sawnW + restW + sawnThW + restThW)) * resUse;
            return (sawnW + restW + sawnThW + restThW + harvRes) / modTimeStep;  // harvestW
        }

        AgeStruct(
                // Increment table which will be used, the time step width (simulation period length) of *it will also be used in ageStruct
                IncrementTab *aIt,
                // Sawn-wood share of harvested wood depending on dbh
                FFIpol<double> *aSws,
                FFIpolsCountry *aFc,
                // Thinning costs depending on d and removed volume per hectare in relation to standing timber (Vorratsfestmeter)
                FFIpolM<double> *aCov,
                // Harvesting costs depending on d and vol
                FFIpolM<double> *aCoe,
                // Do thinning (depending on d and removed volume per hectare) in relation to standing timber (Vorratsfestmeter)
                FFIpolM<double> *aDov,
                // was ffipolm<bool>
                // Do final felling (depending on d and stocking volume per hectare)
                FFIpolM<double> *aDoe,
                // was ffipolm<bool>
                // mean annual increment in tC stem-wood per hectare and year at increment optimal rotation time
                double aMai,
                // objective of production:
                // 0. Rotation time in years,
                // 1. amount of wood which need to be harvested every year,
                // 2. like 1 but the amount will not be fulfilled if rotation time will be shorter than minRot,
                // > 2 ignore Value of u and calculate it instead,
                // 3. Highest average increment,
                // 4. Maximum average Biomass,
                // 5. Highest possible age,
                // 6. Maximum harvest at final cut,
                // 7. Average Maximum harvest at final cut
                int aObjOfProd = 3,
                // Rotation time if objOfProd 0
                double aU = 0,
                // if objOfProd 1, 2 amount of sawn-wood to harvest
                double aMinSw = 0,
                // if objOfProd 1, 2 amount of rest-wood to harvest
                double aMinRw = 0,
                // if objOfProd 1, 2 amount of total harvest
                double aMinHarv = 0,
                // Usage of stocking degree:
                // 0. Keep all the time sdMax,
                // 1. alternate between sdMin and sdMax,
                // 3. alternate between sdMinH and sdMaxH
                int aSdDef = 0,
                // Stocking degree: if sd exceeds sdMax do thinning until sdMin.
                // d > 0 stockingDegree yield table,sd -1 to 0 natural stocking degree
                // Maybe sdMin and sdMax can be made dependent on h/hmax and MAI
                double aSdMax = 1,
                double aSdMin = 1,
                // Years to calculate average mai
                size_t aMaiYears = 30,
                // Minimal rotation time in years or as share given in minRotRef which needs to be exceeded until final harvests are done
                double aMinRotVal = 0.75,
                // meaning of minRotVal value
                // 0. use it as years,
                // 1. minRotVal * u (u > 0),
                // 2. * uMaxAvgIncrement,
                // 3. * uMaxAvgBiomass,
                // 4. * uMaxAge,
                // 5. * uMaxHarvest,
                // 6. * uAvgMaxHarvest
                int aMinRotRef = 2,
                // how fast should the stoking degree target be reached
                // 0. do only remove caused by stand density  to  1. do only typical removes
                double aFlexSd = 0,
                uint32_t aMaxAge = 300,
                // MG: A key to create multi-layer forest and provide selective logging // 07.01.2019
                bool aSelectiveLogging = false
                // , FFIpol<double> *sdMaxH  // Stocking degree depending on max tree height
                // , FFIpol<double> *sdMinH  // Stocking degree depending on min tree height
        ) : it{aIt},
            sws{aSws},
            fc{aFc},
            cov{aCov},
            coe{aCoe},
            dov{aDov},
            doe{aDoe},
            mai{aMai},
            uRef{aU},
            objOfProd{aObjOfProd},
            minSw{aMinSw},
            minRw{aMinRw},
            minHarv{aMinHarv},
            sdDef{aSdDef},
            sdMin{aSdMin},
            sdMax{aSdMax},
            minRotVal{aMinRotVal},
            minRotRef{aMinRotRef},
            flexSd{aFlexSd},
            selectiveLogging{aSelectiveLogging} {
            avgMai = mai;
            qMai.assign(aMaiYears, mai);
            setRotationTime();
            setMinRot();
            timeStep = it->getTimeframe();  // Check if this should be tStep or timeframe
            maxNumberOfAgeClasses = ceil(aMaxAge / timeStep);
        }

        void createNormalForest(const double rotationPeriod, const double aArea, const double sd = 1) {
            selectiveLogging ?
            createNormalForestSLTrue(rotationPeriod, aArea, sd) : createNormalForestSLFalse(rotationPeriod, aArea, sd);
        }

        // MG: Clear vector<Cohort> dat and create normal forest with new parameters
        void recreateNormalForest(const double rotationPeriod, const double aArea, const double sd = 1) {
            dat.assign(1, {});
            if (selectiveLogging)
                dat0.assign(1, {});
            createNormalForest(rotationPeriod, aArea, sd);
        }

        // get biomass per ha by age
        [[nodiscard]] double getBm(double age, const int canopyLayer = 1) const noexcept {
            age /= timeStep;
            size_t ageH = ceil(age);
            switch (canopyLayer) {
                case 0:
                    if (dat0.empty()) {
                        ERROR("dat0 is empty");
                        return 0;
                    }
                    if (ageH == 0)
                        return dat0.front().bm;
                    if (ageH >= dat.size())
                        return dat0.back().bm;
                    return lerp(dat0[ageH - 1].bm, dat0[ageH].bm, age - static_cast<double>(ageH));
                case 1:
                    if (dat.empty())
                        return 0;
                    if (ageH == 0)
                        return dat.front().bm;
                    if (ageH >= dat.size())
                        return dat.back().bm;
                    return lerp(dat[ageH - 1].bm, dat[ageH].bm, age - static_cast<double>(ageH));
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
                    return 0;
            }
        }

        // get average biomass per ha
        [[nodiscard]] double getBm() const noexcept {
            double aArea = 0;
            double bm = 0;
            if (selectiveLogging) {
                for (const auto &dat_i: dat)
                    bm += dat_i.area * dat_i.bm;
                for (const auto &dat0_i: dat0) {
                    aArea += dat0_i.area;
                    bm += dat0_i.area * dat0_i.bm;
                }
            } else {
                for (const auto &dat_i: dat) {
                    aArea += dat_i.area;
                    bm += dat_i.area * dat_i.bm;
                }
            }
            return aArea > 0 ? bm / aArea : 0;
        }

        // MG 09052022: get standing stem biomass with a diameter greater than dbh0 cm
        [[nodiscard]] double getBmGDbh(const double dbh0) const noexcept {
            double aArea = 0;
            double bm = 0;
            if (selectiveLogging) {
                for (const auto &dat_i: dat)
                    if (dat_i.d > dbh0)
                        bm += dat_i.area * dat_i.bm;
                for (const auto &dat0_i: dat0)
                    if (dat0_i.d > dbh0) {
                        aArea += dat0_i.area;
                        bm += dat0_i.area * dat0_i.bm;
                    }
            } else {
                for (const auto &dat_i: dat)
                    if (dat_i.d > dbh0) {
                        aArea += dat_i.area;
                        bm += dat_i.area * dat_i.bm;
                    }
            }
            return aArea > 0 ? bm / aArea : 0;
        }

        // get average biomass per ha
        [[nodiscard]] double getBmSL(const int canopyLayer) const noexcept {
            double aArea = 0;
            double bm = 0;
            switch (canopyLayer) {
                case 0:
                    for (const auto &dat0_i: dat0) {
                        aArea += dat0_i.area;
                        bm += dat0_i.area * dat0_i.bm;
                    }
                    break;
                case 1:
                    for (const auto &dat_i: dat) {
                        aArea += dat_i.area;
                        bm += dat_i.area * dat_i.bm;
                    }
                    break;
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
            }
            return aArea > 0 ? bm / aArea : 0;
        }

        // get biomass  in canopyLayer for age interval >= lAge and <= uAge
        [[nodiscard]] double getBmIntSL(const int canopyLayer, size_t lAge, size_t uAge) const noexcept {
            double aArea = 0;
            double bm = 0;
            if (lAge > uAge)  // size_t >= 0, possible expected lAge overflow
                lAge = 0;
            switch (canopyLayer) {
                case 0:
                    uAge = min(uAge, dat0.size());
                    for (size_t i = lAge; i <= uAge; ++i) {
                        aArea += dat0[i].area;
                        bm += dat0[i].area * dat0[i].bm;
                    }
                    break;
                case 1:
                    uAge = min(uAge, dat.size());
                    for (const auto &dat_i: dat) {
                        aArea += dat_i.area;
                        bm += dat_i.area * dat_i.bm;
                    }
                    break;
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
            }
            return aArea > 0 ? bm / aArea : 0;
        }

        // get average biomass per ha for age classes with Bm > 0 (including 0 age class)
        [[nodiscard]] double getBmNonZero() const noexcept {
            double aArea = 0;
            double bm = 0;
            for (size_t i = 0; i < dat.size(); ++i)
                if (i > 0 && dat[i].bm > 0 || i == 0) {
                    aArea += dat[i].area;
                    bm += dat[i].area * dat[i].bm;
                }
            return aArea > 0 ? bm / aArea : 0;
        }

        // get average biomass increment in canopyLayer for age interval >= lAge and <= uAge
        [[nodiscard]] double getAvgIncBmSL(const int canopyLayer, size_t lAge, size_t uAge) const noexcept {
            double aArea = 0;
            double bmInc = 0;
            if (lAge > uAge)  // size_t >= 0, possible expected lAge overflow
                lAge = 0;
            switch (canopyLayer) {
                case 0:
                    uAge = min(uAge, dat0.size());
                    for (size_t i = lAge; i < uAge; ++i) {
                        aArea += dat0[i].area;
                        bmInc += (dat0[i + 1].bm - dat0[i].bm) * dat0[i].area;
                    }
                    break;
                case 1:
                    uAge = min(uAge, dat.size());
                    for (size_t i = lAge; i < uAge; ++i) {
                        aArea += dat[i].area;
                        bmInc += (dat[i + 1].bm - dat[i].bm) * dat[i].area;
                    }
                    break;
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
            }
            return aArea > 0 ? bmInc / aArea : 0;
        }

        // Share of biomass of the canopy layer that can be removed without exceeding average maximum increment
        [[nodiscard]] double getMaxAvgIncBmShareSL() const {
            auto maxIncAge = static_cast<size_t>(it->getTOptSdNat(mai, slShare, 0));
            double bmInc_tmp = getAvgIncBmSL(1, slAge, maxIncAge);
            double bm_tmp = getBmIntSL(1, slAge, maxIncAge);
            return bm_tmp > 0 ? bmInc_tmp / bm_tmp : 0;
        }

        // get forest area by age (consider age class size)
        [[nodiscard]] double getArea(const size_t age, const int canopyLayer = 1) const noexcept {
            switch (canopyLayer) {
                case 0:
                    return age < dat0.size() ? dat0[age].area : 0;
                case 1:
                    return age < dat.size() ? dat[age].area : 0;
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
                    return 0;
            }
        }

        // get forest area by age (consider age class size)
        [[nodiscard]] double getArea(double age, const int canopyLayer = 1) const noexcept {
            //Here seems to be an error - Above just to allow some calculations
            age /= timeStep;
            size_t ageH = llround(age);
            switch (canopyLayer) {
                case 0:
                    return ageH < dat0.size() ? dat0[ageH].area / timeStep : 0;
                case 1:
                    return ageH < dat.size() ? dat[ageH].area / timeStep : 0;
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
                    return 0;
            }
        }

        // get forest area
        [[nodiscard]] double getArea() const noexcept {
            return area;
        }

        [[nodiscard]] double getAreaSL(const int canopyLayer) const noexcept {
            double areaSL = 0;
            switch (canopyLayer) {
                case 0:
                    for (const auto &dat0_i: dat0)
                        areaSL += dat0_i.area;
                    break;
                case 1:
                    for (const auto &dat_i: dat0)
                        areaSL += dat_i.area;
                    break;
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
            }
            return areaSL;
        }

        // get forest area where Bm > 0, including the first age class
        [[nodiscard]] double getAreaNonZero() const noexcept {
            double aArea = 0;
            for (size_t i = 0; i < dat.size(); ++i)
                if (i > 0 && dat[i].bm > 0 || i == 0)
                    aArea += dat[i].area;
            return aArea;
        }

        // get Diameter
        [[nodiscard]] double getD(double age, const int canopyLayer = 1) const noexcept {
            age /= timeStep;
            size_t ageH = ceil(age);
            switch (canopyLayer) {
                case 0:
                    if (dat0.empty()) {
                        ERROR("dat0 is empty");
                        return 0;
                    }
                    if (ageH == 0)
                        return dat0.front().d;
                    if (ageH >= dat0.size())
                        return dat0.back().d;
                    return lerp(dat0[ageH - 1].d, dat0[ageH].d, age - static_cast<double>(ageH));
                case 1:
                    if (dat.empty())
                        return 0;
                    if (ageH == 0)
                        return dat.front().d;
                    if (ageH >= dat.size())
                        return dat.back().d;
                    return lerp(dat[ageH - 1].d, dat[ageH].d, age - static_cast<double>(ageH));
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
                    return 0;
            }
        }

        // get Height / canopyLayer:
        // 1 - first canopy layer,
        // 0 - second (ground) canopy layer that is active if selective logging option is used
        [[nodiscard]] double getH(double age, const int canopyLayer = 1) const noexcept {
            age /= timeStep;
            size_t ageH = ceil(age);
            switch (canopyLayer) {
                case 0:
                    if (dat0.empty()) {
                        ERROR("dat0 is empty");
                        return 0;
                    }
                    if (ageH == 0)
                        return dat0.front().h;
                    if (ageH >= dat0.size())
                        return dat0.back().h;
                    return lerp(dat0[ageH - 1].h, dat0[ageH].h, age - static_cast<double>(ageH));
                case 1:
                    if (dat.empty())
                        return 0;
                    if (ageH == 0)
                        return dat.front().h;
                    if (ageH >= dat.size())
                        return dat.back().h;
                    return lerp(dat[ageH - 1].h, dat[ageH].h, age - static_cast<double>(ageH));
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
                    return 0;
            }
        }

        [[nodiscard]] size_t getAgeSL() const noexcept {
            return slAge;
        }

        [[nodiscard]] double getShareSL() const noexcept {
            return slShare;
        }

        // Set area for a specific ageCLASS for specified canopy layer
        void setArea(size_t ageClass, double aArea, const int canopyLayer = 1) noexcept {
            // Here seems to be an error - Above just to allow some calculations
            if (aArea < 0) {
                ERROR("aArea < 0: {}", aArea);
                return;
            }
            ageClass = min(ageClass, static_cast<size_t>(it->getTMax() / timeStep - 1));
            switch (canopyLayer) {
                case 0: {
                    if (ageClass < dat0.size()) {
                        area += aArea - dat0[ageClass].area;
                        dat0[ageClass].area = aArea;
                        return;
                    }
                    size_t oldSize = dat0.size();
                    dat0.resize(ageClass + 1);
                    initCohortSL0(oldSize, ageClass);
                    dat0[ageClass].area = aArea;
                    break;
                }
                case 1: {
                    if (ageClass < dat.size()) {
                        area += aArea - dat[ageClass].area;
                        dat[ageClass].area = aArea;
                        return;
                    }
                    size_t oldSize = dat.size();
                    dat.resize(ageClass + 1);
                    initCohortSL1(oldSize, ageClass);
                    dat[ageClass].area = aArea;
                    break;
                }
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
            }
            area += aArea; // MG: can be a problem here as area must be the same for L0 and L1
        }

        // Set biomass per hectare for a specific ageCLASS for specified canopy layer
        void setBm(size_t ageClass, double biomass, const int canopyLayer = 1) noexcept {
            if (biomass < 0) {
                ERROR("biomass < 0: {}", biomass);
                return;
            }
            ageClass = min(ageClass, static_cast<size_t>(it->getTMax() / timeStep - 1));
            switch (canopyLayer) {
                case 0: {
                    double maxBm = it->getBm(static_cast<double>(ageClass) * timeStep, (1 - slShare) * avgMai);
                    biomass = max(biomass, maxBm);
                    if (ageClass < dat0.size()) {
                        dat0[ageClass].bm = biomass;
                        return;
                    }
                    size_t oldSize = dat0.size();
                    dat0.resize(ageClass + 1);
                    initCohortSL0(oldSize, ageClass);
                    dat0[ageClass].bm = biomass;
                    break;
                }
                case 1: {
                    double sd = abs(midpoint(sdMin, sdMax));
                    double maxBm = it->getBmSdNat(static_cast<double>(ageClass) * timeStep, avgMai, slShare * sd);
                    biomass = max(biomass, maxBm);
                    if (ageClass < dat.size()) {
                        dat[ageClass].bm = biomass;
                        return;
                    }
                    size_t oldSize = dat.size();
                    dat.resize(ageClass + 1);
                    initCohortSL1(oldSize, ageClass);
                    dat[ageClass].bm = biomass;
                    break;
                }
                default:
                    ERROR("canopyLayer = {} is not implemented", canopyLayer);
            }
        }

        // Set dbh for a specific ageClass
        void setD(size_t ageClass, double dbh) noexcept {
            ageClass = min(ageClass, static_cast<size_t>(it->getTMax() / timeStep - 1));
            double age = static_cast<double>(ageClass) * timeStep;
            double minD = it->getDbh(age, avgMai);
            double maxD = it->getDbhSdNat(age, avgMai, 0);
            dbh = clamp(dbh, minD, maxD);
            if (ageClass < dat.size()) {
                dat[ageClass].d = dbh;
                return;
            }
            size_t oldSize = dat.size();
            dat.resize(ageClass + 1);
            initCohort(oldSize, ageClass);
            dat[ageClass].d = dbh;
        }

        // Just set mai but don't influence avgMai
        void setMai(const double aMai) noexcept {
            mai = aMai;
        }

        void setMaiYears(const size_t maiYears) {
            // front resize was default
//            if (maiYears >= qMai.size())
//                qMai.insert(qMai.begin(), maiYears - qMai.size(), avgMai);
//            else
//                qMai.erase(qMai.begin(),
//                           qMai.begin() + static_cast<deque<double>::difference_type>(qMai.size() - maiYears));
            qMai.resize(maiYears, avgMai);
            calcAvgMai();
            setRotationTime();
            setMinRot();
        }

        // Set avgMai and all values in qMai
        void setAvgMai(const double aAvgMai) {
            avgMai = max(0., aAvgMai);
            qMai.assign(qMai.size(), avgMai);
            setRotationTime();
            setMinRot();
        }

        void setMaiAndAvgMai(const double aMai) {
            setMai(aMai);
            setAvgMai(aMai);
        }

        [[nodiscard]] double getMai() const noexcept {
            return mai;
        }

        [[nodiscard]] double getAvgMai() const noexcept {
            return avgMai;
        }

        void setObjOfProd(const int aObjOfProd) noexcept {
            objOfProd = aObjOfProd;
        }

        void setU(const double aU) {
            uRef = aU;
            setRotationTime();
            setMinRot();
        }

        [[nodiscard]] double getURef() const noexcept {
            return uRef;
        }

        void setStockingDegreeMin(const double sd) noexcept {
            sdMin = sd;
        }

        void setStockingDegreeMax(const double sd) noexcept {
            sdMax = sd;
        }

        // min and max
        void setStockingDegree(const double sd) noexcept {
            sdMin = sd;
            sdMax = sd;
        }

        void setMinRotRef(int aMinRotRef) noexcept {
            minRotRef = aMinRotRef;
        }

        void setMinRotVal(double aMinRotVal) noexcept {
            minRotVal = aMinRotVal;
            setMinRot();
        }

        void setFlexSd(double aFlexSd) noexcept {
            flexSd = aFlexSd;
        }

        void setShareSL(double aslShare) noexcept {
            slShare = aslShare;
        }

        // Insert in the youngest age class
        void afforest(const double aArea) noexcept {
            if (aArea <= 0) {
                ERROR("aArea <= 0: {}", aArea);
                return;
            }
            dat[0].area += aArea;
            area += aArea;
        }

        // Make afforestation in age class 0 and 1
        // MG: correct problem with 0 biomass in 1st age class
        // Proposed by Georg 21 August 2015
        // Make fake afforestation in age class 0; new species are planted in the new forest instead
        void reforest(const double aArea, bool speciesChange = false) {
            if (aArea < 0) {
                ERROR("aArea < 0: {}", aArea);
                return;
            }
            if (dat.size() < 2) {
                size_t oldSize = dat.size();
                dat.resize(2);
                initCohort(oldSize, 2);
            }
            if (aArea == 0)
                return;
            if (speciesChange) {
                dat[0].area += aArea;
                dat[0].bm = 0;
                area += aArea;
                return;
            }
            double halfArea = aArea * 0.5;
            dat[0].area += halfArea;
            dat[1].bm = (dat[1].bm * dat[1].area +
                         it->getBmSdTab(0.5 * timeStep, mai, 1) * halfArea) / (dat[1].area + halfArea);
            dat[1].area += halfArea;
            area += aArea;
        }

        // MG: Make afforestation in age class 0 and 1 of the 2 canopy layer only
        void reforestSL(const double aArea) {
            if (aArea < 0) {
                ERROR("aArea < 0: {}", aArea);
                return;
            }
            if (dat0.size() < 2) {
                size_t oldSize = dat0.size();
                dat0.resize(2);
                initCohortSL0(oldSize, 2);
            }
            if (aArea == 0)
                return;
            double halfArea = aArea * 0.5;
            dat0[0].area += halfArea;
            dat0[1].bm = (dat0[1].bm * dat0[1].area + it->getBmSdNat(0.5 * timeStep, (1 - slShare) * mai, 1) * halfArea)
                         / (dat0[1].area + halfArea);
            dat0[1].area += halfArea;
            area += aArea;
        }

        // type: 0. Take from all age classes, 1. Take from the eldest age classes
        // area is a non-negative number, return the deforested biomass
        [[nodiscard]] V deforest(const double aArea, const int type = 0) {
            if (aArea < 0) {
                ERROR("aArea < 0: {}", aArea);
                return {};
            }
            return selectiveLogging ? deforestSLTrue(aArea) : deforestSLFalse(aArea, type);
        }

        // calculates damage to biomass of the 1 and 2 (understory) canopy layers when selective logging is done on area "area";
        // Share of damage can be set
        // aArea - area on which the trees of the 1 canopy layer were logged
        [[nodiscard]] V damage(double aArea) {
            if (area <= 0)
                return {};
            aArea = min(aArea, area);
            V ret;
            ret.area = aArea;
            double slDamage0 = 0.2; // damage to trees of the 2 canopy layer (understory) at selective logging
            double slDamage1 = 0.05; // damage to trees of 1 layer canopy layer
            double mul = 1 - aArea * slDamage1 / area;
            array<double, 2> dbhBm{};  // Key of dbh and biomass

            for (size_t i = 0; i < dat.size(); ++i)
                if (dat[i].area > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    dbhBm[0] = max(0., dat[i].d + id);
                    dbhBm[1] = max(0., dat[i].bm + dbm);
                    double totalWood = dat[i].area * (1 - mul) * dbhBm[1];
                    ret.bm += totalWood;
                    double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                    double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                    ret.sw += sawnWood;
                    ret.rw += harvestedWood - sawnWood;
                    ret.co += totalWood * coe->ip(dbhBm);
                    dat[i].bm *= mul;
                }

            mul = 1 - aArea * slDamage0 / area;
            double tmp_arg = (1 - slShare) * avgMai;

            for (size_t i = 0; i < dat0.size(); ++i)
                if (dat0[i].area > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    double sdNat = it->getSdNat(age, tmp_arg, dat0[i].bm);
                    double dbm = it->getIncBmSdNat(age, tmp_arg, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, tmp_arg, sdNat) * 0.5;
                    dbhBm[0] = max(0., dat[i].d + id);
                    dbhBm[1] = max(0., dat[i].bm + dbm);
                    double totalWood = dat0[i].area * (1 - mul) * dbhBm[1];
                    ret.bm += totalWood;
                    double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                    double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                    ret.sw += sawnWood;
                    ret.rw += harvestedWood - sawnWood;
                    ret.co += totalWood * coe->ip(dbhBm);

                    dat0[i].bm *= mul;
                }

            if (ret.area > 0) {  // Values per hectare
                ret.sw /= ret.area;
                ret.rw /= ret.area;
                ret.co /= ret.area;
                ret.bm /= ret.area;
            }  // MG: per ha of logged land
            return ret;
        }

        // .first = thinning, .second = harvest
        // MG: reforestation with harvested area but zero biomass; real reforestation occurs in the new forest in the same cell
        [[nodiscard]] pair<V, V> aging(const double aMai) {
            return aging(aMai, false, false);
        }

        // MG: reforestation with harvested area but zero biomass; real reforestation occurs in the new forest in the same cell
        // MG: reforestation after final cut is an option / 27.12.2018
        [[nodiscard]] pair<V, V> aging(const bool speciesChange = false, const bool shelterWood = false) {
            return aging(mai, speciesChange, shelterWood);
        }

        // MG: reforestation with harvested area but zero biomass; real reforestation occurs in the new forest in the same cell
        [[nodiscard]] pair<V, V> aging(const double aMai, const bool speciesChange, const bool shelterWood) {
            V retThin, retHarvest, retDamage;
            double areaShift = 0;
            gapArea = 0;
            gapArea0 = 0;
            mai = aMai;
            qMai.pop_front();
            qMai.push_back(mai);
            calcAvgMai();
            setRotationTime();
            setMinRot();
            if ((objOfProd == 1 || objOfProd == 2) && !selectiveLogging) { //Fulfill an amount of harvest
                retThin = thinAndGrow();
                if (objOfProd == 1)
                    retHarvest = finalCut(minSw - retThin.sw, minRw - retThin.rw, minHarv - (retThin.sw + retThin.rw),
                                          true, false);
                else
                    retHarvest = finalCut(minSw - retThin.sw, minRw - retThin.rw, minHarv - (retThin.sw + retThin.rw),
                                          true, true);
            } else if (!speciesChange && !selectiveLogging && !shelterWood) { //We have a rotation time to fulfill
                retHarvest = finalCut(area * timeStep / u, true); //do final cut
                retThin = thinAndGrow();
                //retThin = thinAndGrowOLD();
            } else if (speciesChange && !selectiveLogging && !shelterWood) { // We have a rotation time to fulfill
                retHarvest = finalCut(area * timeStep / u, true); //do final cut
                retThin = thinAndGrow(speciesChange);
                //retThin = thinAndGrowOLD();
            } else if (!speciesChange && selectiveLogging && !shelterWood) { // We have a rotation time to fulfill
                //retHarvest = finalCutSL((area*timeStep/u)/slShare, true); // do final cut
                //retHarvest = finalCutSL(true); //do final cut sustainable (i.e., respect minRot) and economic
                //retHarvest = finalCutSL(false); //do final cut sustainable but non-economic
                double dbhHarvest = dbhHarvestSL;
                double bmRmShare = bmRmSL;
                retHarvest = finalCutSL(dbhHarvest, bmRmShare); //do final cut
                //retHarvest = finalCutSL(false, -1, -1, -1, -1, dbhHarvest, -1, false);
                retDamage = damage(retHarvest.area);
                tie(retThin, areaShift) = thinAndGrowSL(retHarvest.area);  // structured binding isn't possible
            } else if (!selectiveLogging && shelterWood) { //We have a rotation time to fulfill
                retHarvest = finalCut(area * timeStep / u, true, true); //do final cut
                retThin = thinAndGrowShelterWood(speciesChange);
            }
            if (speciesChange && selectiveLogging)
                reforestSL(areaShift);  // changeSpecies to be added for the SL case
            else
                reforest(retHarvest.area, false);
            return {retThin, retHarvest};
        }

        [[nodiscard]] size_t getMaxAge() const noexcept {
            return static_cast<size_t>(static_cast<double>(maxNumberOfAgeClasses) * timeStep);
        }

        [[nodiscard]] size_t getDatSize() const noexcept {
            return dat.size();
        }

        void setMaxAge(const size_t maxAge) noexcept {
            maxNumberOfAgeClasses = static_cast<size_t>(ceil(static_cast<double>(maxAge) / timeStep));
        }

        //  https://en.cppreference.com/w/cpp/compiler_support/23
        //  wait for P0330R8 in MSVC, update to C++23 and change 0ll to 0z
        // MG: added : Find "active Age" - the oldest age class with area > 0
        [[nodiscard]] double getActiveAge() const noexcept {
            if (dat.empty())
                return 0;
            ptrdiff_t i = distance(ranges::find_if(dat | rv::reverse, [](const auto &dat_i) { return dat_i.area > 0; }),
                                dat.rend());
            i = max(0ll, i - 1);
            return static_cast<double>(i) * timeStep;
        }

        // MG: Thinned wood weighted average diameter of harvested (at thinning) trees
        [[nodiscard]] double getDBHThinned() const noexcept {
            return hDbh;
        }

        // MG: Thinned wood weighted average height of harvested (at thinning) trees
        [[nodiscard]] double getHThinned() const noexcept {
            return hH;
        }

        // MG: weighted average diameter of harvested (at final cut) trees
        [[nodiscard]] double getDBHFinalCut() const noexcept {
            return fcDBH;
        }

        // MG: weighted average height of harvested (at final cut) trees
        [[nodiscard]] double getHFinalCut() const noexcept {
            return fcH;
        }

        // MG:  net annual increment averaged over all age classes
        [[nodiscard]] double getAvgNetInc() const noexcept {
            return netInc / timeStep;
        }

        // MG:  gross annual increment averaged over all age classes
        [[nodiscard]] double getAvgGrossInc() const noexcept {
            return grossInc / timeStep;
        }

        // MG: get rotation time
        [[nodiscard]] double getU() const noexcept {
            return u;
        }

        // MG: get stocking degree ([StockingDegreeMin + StockingDegreeMax] / 2)
        [[nodiscard]] double getStockingDegree() const noexcept {
            return midpoint(sdMin, sdMax);
        }

        // MG: get age structure Area
        [[nodiscard]] vector<double> getAgeStructArea() const noexcept {
            vector<double> ageStructArea(dat.size());
            for (size_t i = 0; i < dat.size(); ++i)
                ageStructArea[i] = dat[i].area;
            return ageStructArea;
        }

        // MG: get age structure Bm
        [[nodiscard]] vector<double> getAgeStructBm() const noexcept {
            vector<double> ageStructBm(dat.size());
            for (size_t i = 0; i < dat.size(); ++i)
                ageStructBm[i] = dat[i].bm;
            return ageStructBm;
        }

        // MG: get age structure Dbh
        [[nodiscard]] vector<double> getAgeStructD() const noexcept {
            vector<double> ageStructD(dat.size());
            for (size_t i = 0; i < dat.size(); ++i)
                ageStructD[i] = dat[i].d;
            return ageStructD;
        }

        // MG: get age structure Height
        [[nodiscard]] vector<double> getAgeStructH() const noexcept {
            vector<double> ageStructH(dat.size());
            for (size_t i = 0; i < dat.size(); ++i)
                ageStructH[i] = dat[i].h;
            return ageStructH;
        }

        // MG: set age structure Area
        void setAgeStructArea(const span<const double> area_set) noexcept {
            dat.resize(area_set.size());
            for (size_t i = 0; i < area_set.size(); ++i)
                dat[i].area = area_set[i];
        }

        // MG: set age structure Bm
        void setAgeStructBm(const span<const double> bm_set) noexcept {
            dat.resize(bm_set.size());
            for (size_t i = 0; i < bm_set.size(); ++i)
                dat[i].bm = bm_set[i];
        }

        // MG: set age structure DBH
        void setAgeStructD(const span<const double> d_set) noexcept {
            dat.resize(d_set.size());
            for (size_t i = 0; i < d_set.size(); ++i)
                dat[i].d = d_set[i];
        }

        // MG: set age structure Height
        void setAgeStructH(const span<const double> h_set) noexcept {
            dat.resize(h_set.size());
            for (size_t i = 0; i < h_set.size(); ++i)
                dat[i].h = h_set[i];
        }

        void setDbhHarvestSL(const double aDbhHarvestSL = 50) noexcept {
            dbhHarvestSL = aDbhHarvestSL;
        }

        void setBmRmSL(const double aBmRmSL = 0.02) noexcept {
            bmRmSL = aBmRmSL;
        }

        // MG: get area of shelters that should be cut in current year and age of which is less than Max biomass
        [[nodiscard]] double getShelter2CutArea() const noexcept {
            if (timerSW.empty()) {
                ERROR("timerSW is empty");
                return 0;
            }
            double maxBmAge_d = 0;
            double sd = midpoint(sdMin, sdMax);
            if (sd > 0)
                maxBmAge_d = sd == 1 ? it->getTOptT(avgMai, 1) : it->getTOptSdTab(avgMai, sd, 1);
            else
                maxBmAge_d = sd == -1 ? it->getTOpt(avgMai, 1) : it->getTOptSdNat(avgMai, abs(sd), 1);
            int maxBmAge = static_cast<int>(maxBmAge_d);
            double areaShelterCut = 0;
            for (const auto &swt: timerSW)
                if (swt.age < maxBmAge && swt.timer == 0)
                    areaShelterCut += swt.area;
            return areaShelterCut;
        }

        // MG: if i > 0 postpone logging of shelters that should be cut in current year to i years, which age of which is less than Max biomass;
        // if i < 0 - force logging of shelters, which age is > 0.8 of shelter age, in current year
        void postponeShelter2Cut(const int i) {
            if (timerSW.empty()) {
                ERROR("timerSW is empty");
                return;
            }
            if (i > 0)
                postponeShelter2CutPosI(i);
            else if (i < 0)
                postponeShelter2CutNegI(i);
        }

        // MG: Clean area of those age classes where Bm<=0; possible problem with low MAI when a few beginning age classes have 0 biomass
        void rectifyArea() noexcept {
            if (dat.size() <= 1)
                return;
            for (auto &dat_i: dat)
                if (dat_i.area > 0 && dat_i.bm <= 0)
                    dat_i.area = 0;
            calcArea();
        }

        // MG: Get area-weighted thinned wood, tC/ha
        [[nodiscard]] double getThinnedWeight() const noexcept {
            return (thinned_weight);
        }

        // MG: Get stem deadwood in current timeStep (dead trees of Dbh>10cm), tC/ha
        [[nodiscard]] double getDeadwood() const noexcept {
            return deadwood;
        }

        // MG: Get stem litter in current timeStep (dead trees of Dbh<=10cm), tC/ha
        // MG: Stem litter, trees of Dbh<=10cm, in current timeStep, tC/ha
        // only thinning and mortality, no harvest residues or disturbance damage
        [[nodiscard]] double getLitter() const noexcept {
            return litter;
        }

        // MG: Get area-weighted stem deadwood in current timeStep (dead trees of Dbh > 10cm), tC/ha
        [[nodiscard]] double getDeadwoodWeight() const noexcept {
            return deadwood_weight;
        }

        // MG: Get area-weighted stem litter in current timeStep (dead trees of Dbh <= 10cm), tC/ha
        // only thinning and mortality, no harvest residues or disturbance damage
        [[nodiscard]] double getLitterWeight() const noexcept {
            return litter_weight;
        }

        // MG: Deadwood weighted average diameter of dead trees (not thinned; DBH > 10cm), cm
        // only thinning and mortality, no harvest residues or disturbance damage
        [[nodiscard]] double getDBHMortDw() const noexcept {
            return mortDwDBH;
        }

        // MG:  Deadwood weighted average height of dead trees (not thinned; DBH > 10cm), m
        // only thinning and mortality, no harvest residues or disturbance damage
        [[nodiscard]] double getHMortDw() const noexcept {
            return mortDwH;
        }

        // MG: Deadwood weighted average diameter of dead trees (not thinned; DBH <= 10cm), cm
        // only thinning and mortality, no harvest residues or disturbance damage
        [[nodiscard]] double getDBHMortLitter() const noexcept {
            return (mortLitterDbh);
        }

        // MG: Deadwood weighted average height of dead trees (not thinned; DBH <= 10cm), m
        // only thinning and mortality, no harvest residues or disturbance damage
        [[nodiscard]] double getHMortLitter() const noexcept {
            return mortLitterH;
        }

        // MG: BM weighted average diameter of damaged trees and trees logged from slash
        [[nodiscard]] double getDBHSlashCut() const noexcept {
            return slashDBH;
        }

        // MG: BM weighted average height of damaged trees and trees logged from slash
        [[nodiscard]] double getHSlashCut() const noexcept {
            return slashH;
        }

        // disturbance damaged wood and wood harvested from the slash
        [[nodiscard]] V
        disturbanceDamage(const double targetDamaged, const uint32_t targetAge, const double targetHeight,
                          const double targetDbh, const double harvestableShare) {
            // 30.06.2023
            // disturbance damaged wood and wood harvested from the slash
            // agent is wind, fire or biotic
            // targetDamaged is amount of damaged merchantable wood caused by one of the agents, tC/ha
            // targetAge is a minimal (average) age of trees, years, when the disturbance agent is active. targetAge = 0 if age is not important; 30 years for wind and fire
            // targetHeight - is a minimal (average) height of trees when the disturbance agent is active. targetHeight = 0 if height is not important; 15m for wind
            // targetDbh - is a minimal (average) DBH when the disturbance agent is active. targetDbh = 0 if DBH is not important; 15 cm for biotic
            // harvestableShare - a share of the damaged merchantable wood that is removed as round-wood or fuel-wood
            V ret;
            auto endYear = static_cast<size_t>(static_cast<double>(targetAge) / timeStep);

            array<double, 3> dbhHBm{};  // array including current year Dbh, H and Bm
            array<double, 2> dbhBm{};   // dbhBm

            double fcDBHTmp = 0; // DBH of harvested trees, average weighted by clearcut area and harvested biomass
            double fcHTmp = 0; // height of harvested trees, average weighted by clearcut area and harvested biomass
            double hWoodArea = 0; // clearcut area X harvested biomass

            for (ptrdiff_t i = ssize(dat) - 1; i > endYear && ret.bm < targetDamaged; --i)  // ssize() to avoid overflow
                if (dat[i].area > 0 && dat[i].bm > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    //  The Stands get half increment of the next growing period
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    double ih = it->getIncHeight(age, avgMai) * 0.5;

                    dat[i].bm = max(0., dat[i].bm); // MG: we don't allow negative biomass in any age group
                    dbhHBm[0] = max(0., dat[i].d + id);
                    dbhHBm[1] = max(0., dat[i].h + ih);
                    dbhHBm[2] = max(0., dat[i].bm + dbm);
                    dbhBm[0] = dbhHBm[0];
                    dbhBm[1] = dbhHBm[2];

                    if (dbhHBm[0] > targetDbh && dbhHBm[1] > targetHeight) {  // given amount of damage
                        double harvestShare = 1;
                        double totalWood = dat[i].area * dbhHBm[2];
                        double harvestedWood = totalWood * harvestableShare * fc->getHle().ip(dbhHBm[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhHBm[0]);
                        double hArea = 0;  // clearcut area in current age class

                        if (ret.bm + totalWood < targetDamaged) {  // Take all age class
                            ret.area += dat[i].area;
                            area -= dat[i].area;
                            hArea = dat[i].area;
                            dat[i].area = 0;
                        } else {  // Take a share of age class
                            harvestShare = 0;
                            double tmp_arg = targetDamaged - ret.bm;
                            if (tmp_arg > 0 && totalWood > 0) {
                                tmp_arg /= totalWood;
                                harvestShare = max(harvestShare, tmp_arg);
                            }
                            harvestShare = clamp(harvestShare, 0., 1.);
                            tmp_arg = dat[i].area * harvestShare;
                            ret.area += tmp_arg;
                            area -= tmp_arg;
                            hArea = tmp_arg;
                            dat[i].area *= (1 - harvestShare);
                        }

                        double tmp_arg = totalWood * hArea;
                        fcDBHTmp += dat[i].d * tmp_arg;
                        fcHTmp += dat[i].h * tmp_arg;
                        hWoodArea += tmp_arg;

                        ret.bm += totalWood *
                                  harvestShare;  // total amount of damaged stem-wood wood including the burnt biomass and potential harvest losses
                        ret.sw += sawnWood * harvestShare;  // harvested sawn wood from the slash
                        ret.rw += (harvestedWood - sawnWood) * harvestShare;  // harvested rest-wood from the slash
                        ret.co += totalWood * coe->ip(dbhBm) *
                                  harvestShare;  // harvesting costs - costs for normal harvest used, not well estimated yet
                    }
                }

            if (hWoodArea > 0) {
                slashDBH = fcDBHTmp / hWoodArea;
                slashH = fcHTmp / hWoodArea;
            } else {
                slashDBH = 0;
                slashH = 0;
            }
            if (ret.area > 0) {  // Values per hectare
                ret.sw /= ret.area;
                ret.rw /= ret.area;
                ret.co /= ret.area;
                ret.bm /= ret.area;
            }
            reforest(ret.area, false); // we reforest the damaged forest here
            // area_disturbDamaged += ret.area;
            // reforest(ret.area, false); // replant the damaged and logged stands
            return ret;
        }

        // returns harvest
        [[nodiscard]] double cohortRes() const {
            auto cohortTmp = *this;
            auto res = cohortTmp.aging();
            double realArea = cohortTmp.getArea();
            return cohortRes(realArea, res);
        }

        // Create copy and set U inline for the sake of encapsulation
        [[nodiscard]] AgeStruct createSetU(const double aU) const {
            auto cohortTmp = *this;
            cohortTmp.setU(aU);
            return cohortTmp;
        }

    private:
        // pointer variables
        IncrementTab *it = nullptr;
        FFIpol<double> *sws = nullptr;
        FFIpolsCountry *fc = nullptr;
        FFIpolM<double> *cov = nullptr;
        FFIpolM<double> *coe = nullptr;
        FFIpolM<double> *dov = nullptr; // was ffipolm<bool>
        FFIpolM<double> *doe = nullptr; // was ffipolm<bool>

        double mai = 0;
        double avgMai = 0;
        int objOfProd = 0;              // Objective of production
        double minSw = 0;               // Min sawn-wood to harvest
        double minRw = 0;               // Min rest-wood to harvest
        double minHarv = 0;             // Minimum total harvest

        deque<double> qMai;             // Queue to store the mai's of previous years (youngest mai is at the end of queue)
        double u = 0;                   // Rotation time

        size_t maxNumberOfAgeClasses;
        vector<ShelterWoodTimer> timerSW;

        double uRef = 0;
        int sdDef = 0;
        double sdMin = 0, sdMax = 0;    // Target stocking degree (> 0 Table, < 0 natural)
        double area = 0;                // Total forest area (sum of dat.area)
        double areaSL1 = 0;             // Total area of the 1 canopy layer
        double minRot = 0;              // minimal age when final harvest will be done
        double minRotVal = 0;
        int minRotRef = 0;

        double timeStep = 0;            // How long is one time step
        double flexSd = 0;
        double hH = 0;                  // MG: average height of trees in age classes that are thinned
        double hDbh = 0;                // MG: average diameter of trees in age classes that are thinned
        double fcH = 0;                 // MG: average height of trees in age classes that are clearcut
        double fcDBH = 0;               // MG: average DBH of trees in age classes that are clearcut
        double slashH = 0;              // MG: average height of trees in age classes that are slash-logged
        double slashDBH = 0;            // MG: average DBH of trees in age classes that are slash-logged
        double netInc = 0;              // MG:  net annual increment averaged over all age classes, tC/(ha year)
        double grossInc = 0;            // MG:  gross annual increment averaged over all age classes, tC/(ha year)
        double slShare = 0;             // MG: share of biomass and increment in the first canopy layer (if selectiveLogging = TRUE)
        size_t slAge = 0;               // MG: age from which first canopy layer is populated (if selectiveLogging = TRUE)
        double gapArea = 0;             // MG: area where trees were removed in the 1 canopy layer
        double gapArea0 = 0;            // MG: area where trees were removed in the 2 canopy layer
        bool selectiveLogging = false;
        double dbhHarvestSL = 0;        // Minimum diameter of trees to be harvested at selective logging > dbhHarvestSL;
        double bmRmSL = 0;              // Share of stem biomass of the canopy layer to be removed at selective logging;
        double deadwood = 0;            // Average stem deadwood, trees of Dbh>10cm, tC/ha
        double litter = 0;              // Average stem litter, trees of Dbh<=10cm, tC/ha
        double mortDwH = 0;             // MG: average height of trees in age classes that die and are not thinned, DBH > 10cm
        double mortDwDBH = 0;           // MG: average diameter of trees in age classes that die and are not thinned, DBH > 10cm
        double mortLitterH = 0;         // MG: average height of trees in age classes that die and are not thinned, DBH <= 10cm
        double mortLitterDbh = 0;       // MG: average diameter of trees in age classes that die and are not thinned, DBH <= 10cm
        double thinned_weight = 0;      // MG: area-weighted thinned wood (no harvest losses)
        double deadwood_weight = 0;     // MG: area & bm-weighted deadwood
        double litter_weight = 0;       // MG: area & bm-weighted litter

        vector<Cohort> dat;
        vector<Cohort> dat0;            // MG: cohort of second canopy layer (if selectiveLogging = TRUE)

        void setRotationTime() {
            switch (objOfProd) {
                case 0:
                    u = uRef;
                    break;
                case 1:
                case 2:
                    u = -1;
                    break;
                case 3:
                case 4:
                case 5:
                case 6:
                case 7: {
                    const int type = objOfProd - 3;
                    double sd = midpoint(sdMin, sdMax);
                    if (sd > 0)
                        u = sd == 1 ? it->getTOptT(avgMai, type) : it->getTOptSdTab(avgMai, sd, type);
                    else
                        u = sd == -1 ? it->getTOpt(avgMai, type) : it->getTOptSdNat(avgMai, abs(sd), type);
                    break;
                }
                default:
                    ERROR("objOfProd = {} is not implemented", objOfProd);
                    u = 0;
            }
        }

        // Calculate average mai with vector qMai
        void calcAvgMai() noexcept {
            if (qMai.empty()) {
                avgMai = mai;
                return;
            }
            double product = 1;
            double sWeight = 0;
            double weight = 0;
            double step = 2 / static_cast<double>(qMai.size());
            for (const auto mai_q: qMai) {
                weight += step;
                if (mai_q <= 0) {   //The years before 0 are unimportant
                    product = 1;
                    sWeight = 0;
                } else {
                    product *= pow(mai_q, weight);
                    sWeight += weight;
                }
            }
            avgMai = sWeight > 0 ? avgMai = pow(product, 1 / sWeight) : 0;
        }

        // Set the minimal rotation time "minRot"
        void setMinRot() noexcept {
            switch (minRotRef) {
                case 0:
                    minRot = minRotVal;
                    break;
                case 1:
                    if (u > 0) {
                        minRot = minRotVal * u;
                        break;
                    }
                case 2:
                case 3:
                case 4:
                case 5:
                case 6: {
                    const int type = minRotRef - 2;
                    double sd = midpoint(sdMin, sdMax);
                    minRot = minRotVal;
                    if (sd > 0)
                        minRot *= sd == 1 ? it->getTOptT(avgMai, type) : it->getTOptSdTab(avgMai, sd, type);
                    else
                        minRot *= sd == -1 ? it->getTOpt(avgMai, type) : it->getTOptSdNat(avgMai, abs(sd), type);
                    break;
                }
                default:
                    ERROR("minRotRef = {} is not implemented", minRotRef);
                    minRot = -1;
            }
        }

        void setAgeSL() {
            slAge = static_cast<size_t>(it->getTOpt((1 - slShare) * mai, 0) / 3);
        }

        // MG: initialize cohorts for the 1 and 2 canopy layer (if selectiveLogging = TRUE)
        void initCohort(const size_t ageClassL, const size_t ageClassH) {
            maxNumberOfAgeClasses = max(maxNumberOfAgeClasses, dat.size());
            Cohort tmp;
            double sd = midpoint(sdMin, sdMax);
            for (size_t i = ageClassL; i < ageClassH; ++i) {
                double age = static_cast<double>(i) * timeStep;
                if (sd > 0) {
                    if (sd == 1) {
                        tmp.bm = it->getBmT(age, avgMai);
                        tmp.d = it->getDbh(age, avgMai);
                    } else {
                        tmp.bm = it->getBmSdTab(age, avgMai, sd);
                        tmp.d = it->getDbhSdTab(age, avgMai, sd);
                    }
                } else {
                    if (sd == -1) {
                        tmp.bm = it->getBm(age, avgMai);
                        tmp.d = it->getDbh(age, avgMai);
                    } else {
                        tmp.bm = it->getBmSdNat(age, avgMai, abs(sd));
                        tmp.d = it->getDbhSdNat(age, avgMai, abs(sd));
                    }
                }
                tmp.h = it->getHeight(age, avgMai);
                dat[i] = tmp;
            }
        }

        // MG: initialize cohorts for the 2 canopy layer (if selectiveLogging = TRUE)
        void initCohortSL0(const size_t ageClassL, const size_t ageClassH) {
            Cohort tmp;
            double sd = midpoint(sdMin, sdMax);
            double tmp_arg = (1 - slShare) * avgMai;
            for (size_t i = ageClassL; i < ageClassH; ++i) {
                double age = static_cast<double>(i) * timeStep;
                tmp.bm = it->getBm(age, tmp_arg);
                tmp.d = it->getDbh(age, tmp_arg);
                tmp.h = it->getHeight(age, tmp_arg);
                dat0[i] = tmp;
            }
        }

        // MG: initialize cohorts for the 1 canopy layer (if selectiveLogging = TRUE)
        void initCohortSL1(const size_t ageClassL, const size_t ageClassH) {
            Cohort tmp;
            double sd = abs(midpoint(sdMin, sdMax));
            double tmp_arg = slShare * sd;
            auto slAge_d = static_cast<double>(slAge);
            for (size_t i = ageClassL; i < ageClassH; ++i) {
                double age = static_cast<double>(i) * timeStep;
                if (age >= slAge_d) {
                    tmp.bm = it->getBmSdNat(age, avgMai, tmp_arg); // should be only for older age classes, to be done!
                    tmp.d = it->getDbhSdNat(age, avgMai, tmp_arg); // should be only for older age classes, to be done!
                    tmp.h = it->getHeight(age, avgMai);
                } else {
                    tmp.bm = 0;
                    tmp.d = 0;
                    tmp.h = 0;
                }
                dat[i] = tmp;
            }
        }

        // Calculates the forest area with dat[].area
        void calcArea() noexcept {
            area = 0;
            for (const auto &dat_i: dat)
                area += dat_i.area;
        }

        [[nodiscard]] V finalCut(const double aArea, const double aMinSw, const double aMinRw, const double aMinHarv,
                                 const bool eco, const bool sustainable) {
            V ret;
            size_t endYear = eco || sustainable ? static_cast<size_t>(minRot / timeStep) : 0;
            array<double, 2> dbhBm{}; // Key to ask if harvest is economic

            double fcDBHTmp = 0;    // DBH of harvested trees, average weighted by clearcut area and harvested biomass
            double fcHTmp = 0;      // height of harvested trees, average weighted by clearcut area and harvested biomass
            double hWoodArea = 0;   // clearcut area X harvested biomass

            for (ptrdiff_t i = ssize(dat) - 1; i >= endYear && (ret.area < aArea ||
                                                                (ret.sw < aMinSw || ret.rw < aMinRw ||
                                                                 (ret.sw + ret.rw) < aMinHarv)); --i)
                if (dat[i].area > 0 && dat[i].bm > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    // The Stands get half increment of the next growing period
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    dat[i].bm = max(0., dat[i].bm);  // MG: we don't allow negative biomass in any age group
                    dbhBm[0] = max(0., dat[i].d + id);
                    dbhBm[1] = max(0., dat[i].bm + dbm);
                    if (static_cast<bool>(doe->ip(dbhBm)) || !eco) {  // do harvest if it is economic
                        if (aArea >= 0) { //Given area to harvest
                            double totalWood = 0;
                            double hArea = 0;  // clearcut area in current age class
                            if (ret.area + dat[i].area < aArea) { //Harvest all of this age class
                                totalWood = dat[i].area * dbhBm[1];
                                ret.area += dat[i].area;
                                area -= dat[i].area;
                                hArea = dat[i].area; // 30.06.2023 corrected bug
                                dat[i].area = 0;

                            } else {
                                double tmp_arg = aArea - ret.area;
                                totalWood = tmp_arg * dbhBm[1];
                                area -= tmp_arg;
                                dat[i].area -= tmp_arg;
                                hArea = tmp_arg;
                                ret.area = aArea;
                            }
                            double tmp_arg = totalWood * hArea;
                            fcDBHTmp += dat[i].d * tmp_arg;
                            fcHTmp += dat[i].h * tmp_arg;
                            hWoodArea += tmp_arg;
                            ret.bm += totalWood;
                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            ret.sw += sawnWood;
                            ret.rw += harvestedWood - sawnWood;
                            ret.co += totalWood * coe->ip(dbhBm);
                        } else {  // given amount of harvest
                            double harvestShare = 1;
                            double totalWood = dat[i].area * dbhBm[1];
                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            if (ret.sw + sawnWood < minSw || ret.rw + harvestedWood - sawnWood < minRw ||
                                ret.sw + ret.rw + harvestedWood < minHarv) {//Harvest all
                                ret.area += dat[i].area;
                                area -= dat[i].area;
                                dat[i].area = 0;
                            } else {  // Harvest part of age class
                                harvestShare = 0;
                                double tmp = minSw - ret.sw;
                                if (tmp > 0 && sawnWood > 0) {
                                    tmp /= sawnWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                tmp = minRw - ret.rw;
                                if (tmp > 0 && harvestedWood > sawnWood) {
                                    tmp /= harvestedWood - sawnWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                tmp = minHarv - (ret.sw + ret.rw);
                                if (tmp > 0 && harvestedWood > 0) {
                                    tmp /= harvestedWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                harvestShare = clamp(harvestShare, 0., 1.);
                                ret.area += dat[i].area * harvestShare;
                                area -= dat[i].area * harvestShare;
                                dat[i].area *= 1 - harvestShare;
                            }
                            ret.bm += totalWood * harvestShare;
                            ret.sw += sawnWood * harvestShare;
                            ret.rw += (harvestedWood - sawnWood) * harvestShare;
                            ret.co += totalWood * coe->ip(dbhBm) * harvestShare;
                        }
                    }
                }

            if (hWoodArea > 0) {
                fcDBH = fcDBHTmp / hWoodArea;
                fcH = fcHTmp / hWoodArea;
            } else {
                fcDBH = 0;
                fcH = 0;
            }

            if (ret.area > 0) {  // Values per hectare
                ret.sw /= ret.area;
                ret.rw /= ret.area;
                ret.co /= ret.area;
                ret.bm /= ret.area;
            }
            return ret;
        }

        // MG: Cut only when diameter > doe and Fulfill area
        [[nodiscard]] V finalCutSL(const double aArea, const double aMinSw, const double aMinRw, const double aMinHarv,
                                   const double dbhHarv, double bmRemovedShare,
                                   const bool eco, const bool sustainable) {
            V ret;
            size_t endYear = eco || sustainable ? static_cast<size_t>(minRot / timeStep) : 0;
            if (bmRemovedShare < 0)
                bmRemovedShare = 1;
            array<double, 2> dbhBm{}; // Key to ask if harvest is economic
            double bmBefore = getBmSL(1) * getAreaSL(1);
            double bmRemoved = 0;
            for (size_t i = ssize(dat) - 1; i >= endYear && (ret.area < aArea || ret.sw < aMinSw || ret.rw < aMinRw ||
                                                             ret.sw + ret.rw < aMinHarv ||
                                                             bmRemoved < bmRemovedShare * bmBefore); --i)
                if (dat[i].area > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    // The Stands get half increment of the next growing period
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    dbhBm[0] = max(0., dat[i].d + id);
                    dbhBm[1] = max(0., dat[i].bm + dbm);
                    if (static_cast<bool>(doe->ip(dbhBm)) || !eco) {  // do harvest if it is economic
                        if (aArea >= 0) {  // Given area to harvest
                            double totalWood = 0;
                            if (ret.area + dat[i].area < aArea) {  // Harvest all of this age class
                                totalWood = dat[i].area * dbhBm[1];
                                ret.area += dat[i].area;
                                dat[i].area = 0;
                            } else {
                                totalWood = (aArea - ret.area) * dbhBm[1];
                                dat[i].area -= aArea - ret.area;
                                ret.area = aArea;
                            }
                            ret.bm += totalWood;
                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            ret.sw += sawnWood;
                            ret.rw += harvestedWood - sawnWood;
                            ret.co += totalWood * coe->ip(dbhBm);
                        } else if (minHarv >= 0 || minRw >= 0 || minSw >= 0) {  // given amount of harvest
                            double harvestShare = 1;
                            double totalWood = dat[i].area * dbhBm[1];
                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            if (ret.sw + sawnWood < minSw || ret.rw + harvestedWood - sawnWood < minRw ||
                                ret.sw + ret.rw + harvestedWood < minHarv) {  // Harvest all
                                ret.area += dat[i].area;
                                dat[i].area = 0;
                            } else {  // Harvest part of age class
                                harvestShare = 0;
                                double tmp = minSw - ret.sw;
                                if (tmp > 0 && sawnWood > 0) {
                                    tmp /= sawnWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                tmp = minRw - ret.rw;
                                if (tmp > 0 && harvestedWood > sawnWood) {
                                    tmp /= harvestedWood - sawnWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                tmp = minHarv - (ret.sw + ret.rw);
                                if (tmp > 0 && harvestedWood > 0) {
                                    tmp /= harvestedWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                harvestShare = clamp(harvestShare, 0., 1.);
                                ret.area += dat[i].area * harvestShare;
                                dat[i].area *= 1 - harvestShare;
                            }
                            ret.bm += totalWood * harvestShare;
                            ret.sw += sawnWood * harvestShare;
                            ret.rw += (harvestedWood - sawnWood) * harvestShare;
                            ret.co += totalWood * coe->ip(dbhBm) * harvestShare;
                        } else if (dat[i].d >
                                   dbhHarv) { // harvest all trees of diameter > doe if economically feasible (dbh > dbhHarv)
                            double harvestShare = 1;
                            double totalWood = 0;
                            totalWood = dat[i].area * dat[i].bm;
                            if (totalWood > 0 &&
                                (bmRemoved + totalWood) / bmBefore < bmRemovedShare) {  // Harvest all age class
                                ret.area += dat[i].area;
                                ret.bm += totalWood;

                                dat[i].reset();
                            } else if (totalWood > 0) {
                                harvestShare = (bmRemovedShare * bmBefore - bmRemoved) / totalWood;
                                harvestShare = clamp(harvestShare, 0., 1.);
                                totalWood *= harvestShare;
                                ret.area += dat[i].area * harvestShare;
                                dat[i].area *= 1 - harvestShare;
                            }

                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            ret.sw += sawnWood;
                            ret.rw += harvestedWood - sawnWood;
                            ret.co += totalWood * coe->ip(dbhBm);
                            bmRemoved += totalWood;
                        }
                    }
                }
            if (ret.area > 0) {  // Values per hectare
                ret.sw /= ret.area;
                ret.rw /= ret.area;
                ret.co /= ret.area;
                ret.bm /= ret.area;
            }
            return ret;
        }

        // Fulfill area
        [[nodiscard]] V finalCut(const double aArea, const bool eco = true) {
            return finalCut(aArea, -1, -1, -1, eco, true);
        }

        // Fulfill amount
        [[nodiscard]] V finalCut(const double aMinSw, const double aMinRw, const double aMinHarv, const bool eco = true,
                                 const bool sustainable = true) {
            return finalCut(-1, aMinSw, aMinRw, aMinHarv, eco, sustainable);
        }

        // MG: Cut only when diameter > doe and Fulfill area
        [[nodiscard]] V finalCutSL(const double aArea, const bool eco = true) {
            return finalCutSL(aArea, -1, -1, -1, -1, 1, eco, true);
        }

        // MG: Cut all trees diameter > doe
        [[nodiscard]] V finalCutSL(const bool eco = true) {
            return finalCutSL(-1, -1, -1, -1, -1, 1, eco, true);
        }

        // MG: Cut all trees diameter > dbhHarV and while removed biomass share is less than bmRemovedShare
        [[nodiscard]] V finalCutSL(const double dbhHarv, const double bmRemovedShare = 1) {
            return finalCutSL(-1, -1, -1, -1, dbhHarv, bmRemovedShare, false, true);
        }

        [[nodiscard]] V finalCutShelterWood(const double aArea, const double aMinSw, const double aMinRw,
                                            const double aMinHarv, const bool eco, const bool sustainable) {
            V ret;
            int timeTo2Cut = 20; // Time, years, after which the shelter will be cut
            // int timeTo2Cut = ceil(0.4 * it->getTOptT(mai, 0)); // Expert suggestion by Fulvio Di Fulvio, 07.08.2019.
            int maxAge = ceil(it->getTOptT(mai, 1));
            double areaShelterCut_total = 0;
            size_t endYear = eco || sustainable ? static_cast<size_t>(minRot / timeStep) : 0;
            array<double, 2> dbhBm{}; // Key to ask if harvest is economic
            for (ptrdiff_t i = ssize(dat) - 1; i >= endYear && (ret.area < aArea * 0.5 || ret.sw < aMinSw ||
                                                                ret.rw < aMinRw || ret.sw + ret.rw < aMinHarv); --i)
                if (dat[i].area > 0 && dat[i].bm > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    //The Stands get half increment of the next growing period
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    dat[i].bm = max(0., dat[i].bm); // MG: we don't allow negative biomass in any age group
                    dbhBm[0] = max(0., dat[i].d + id);
                    dbhBm[1] = max(0., dat[i].bm + dbm);
                    if (static_cast<bool>(doe->ip(dbhBm)) || !eco) {  // do harvest if it is economic
                        if (aArea >= 0) {  // Given area to harvest
                            timeTo2Cut = clamp(timeTo2Cut, 0, static_cast<int>(maxAge - i));
                            double totalWood = 0;
                            double areaShelter = 0;  // Shelter area in current age class
                            double areaShelterCut = 0; // Shelter area in current age class to be cut in current year / timeStep
                            if (!timerSW.empty()) {
                                for (auto &swt: timerSW)
                                    if (swt.age == i) {
                                        areaShelter += swt.area; // All shelter area of current age
                                        if (swt.timer == 0) {
                                            areaShelterCut += swt.area;
                                            swt.area = 0;
                                        }
                                    }
                                const auto rit = ranges::find_if(timerSW | rv::reverse, [i](const auto &swt) {
                                    return swt.age == i && swt.timer == 0;
                                });
                                if (rit != timerSW.rend())
                                    timerSW.erase(next(rit).base());  // why next ? vvv
                                // https://stackoverflow.com/questions/16609041/c-stl-what-does-base-do
                            }

                            double area_tmp = 0;
                            if (ret.area + dat[i].area - areaShelter < aArea) {  // Harvest all of this age class
                                area_tmp = (dat[i].area - areaShelter) * 0.5;
                                dat[i].area = area_tmp + areaShelter - areaShelterCut;
                            } else {
                                area_tmp = (aArea - ret.area * 2) * 0.5;
                                dat[i].area -= area_tmp + areaShelterCut;
                            }
                            totalWood = (area_tmp + areaShelterCut) * dbhBm[1];
                            ret.area += area_tmp;
                            area -= area_tmp + areaShelterCut;
                            if (area_tmp > 0)
                                timerSW.emplace_back(area_tmp, static_cast<int>(i), timeTo2Cut);
                            areaShelterCut_total += areaShelterCut;

                            ret.bm += totalWood;
                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            ret.sw += sawnWood;
                            ret.rw += harvestedWood - sawnWood;
                            ret.co += totalWood * coe->ip(dbhBm);
                        } else {  // given amount of harvest
                            double harvestShare = 1;
                            double totalWood = dat[i].area * dbhBm[1];
                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            if (ret.sw + sawnWood < minSw || ret.rw + harvestedWood - sawnWood < minRw ||
                                ret.sw + ret.rw + harvestedWood < minHarv) {  // Harvest all
                                ret.area += dat[i].area;
                                area -= dat[i].area;
                                dat[i].area = 0;
                            } else {  // Harvest part of age class
                                harvestShare = 0;
                                double tmp = minSw - ret.sw;
                                if (tmp > 0 && sawnWood > 0) {
                                    tmp /= sawnWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                tmp = minRw - ret.rw;
                                if (tmp > 0 && harvestedWood > sawnWood) {
                                    tmp /= harvestedWood - sawnWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                tmp = minHarv - (ret.sw + ret.rw);
                                if (tmp > 0 && harvestedWood > 0) {
                                    tmp /= harvestedWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                harvestShare = clamp(harvestShare, 0., 1.);
                                ret.area += dat[i].area * harvestShare;
                                area -= dat[i].area * harvestShare;
                                dat[i].area *= 1 - harvestShare;
                            }
                            ret.bm += totalWood * harvestShare;
                            ret.sw += sawnWood * harvestShare;
                            ret.rw += (harvestedWood - sawnWood) * harvestShare;
                            ret.co += totalWood * coe->ip(dbhBm) * harvestShare;
                        }
                    }
                }

            if (!timerSW.empty()) {  // cutting shelters which time has come to end in all age classes
                for (auto &swt: timerSW)
                    if (swt.timer == 0) {
                        double area2Cut = swt.area;
                        size_t age_idx = swt.age;
                        double age = swt.age * timeStep;

                        if (age_idx >= dat.size() - 1)
                            FATAL("P1458: age_idx >= dat.size()!!!!!  age_idx= {}\tdat.size= {}\tmaxNumberOfAgeClasses= {}",
                                  age_idx, dat.size(), maxNumberOfAgeClasses);

                        double sdNat = it->getSdNat(age, avgMai, dat[age_idx].bm);
                        double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                        double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                        // MG: we don't allow negative biomass in any age group
                        dat[age_idx].bm = max(0., dat[age_idx].bm);
                        dbhBm[0] = max(0., dat[age_idx].d + id);
                        dbhBm[1] = max(0., dat[age_idx].bm + dbm);
                        if (static_cast<bool>(doe->ip(dbhBm)) || !eco) {  // do harvest if it is economic
                            area2Cut = min(area2Cut, dat[age_idx].area);
                            double totalWood = area2Cut * dbhBm[1];
                            area -= area2Cut;
                            dat[age_idx].area -= area2Cut;
                            ret.bm += totalWood;
                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            ret.sw += sawnWood;
                            ret.rw += harvestedWood - sawnWood;
                            ret.co += totalWood * coe->ip(dbhBm);
                            areaShelterCut_total += area2Cut;
                        }
                    }
                erase_if(timerSW, [](const auto &swt) { return swt.timer == 0; });
            }

            ret.area += areaShelterCut_total;
            if (ret.area > 0) {  // Values per hectare
                ret.sw /= ret.area;
                ret.rw /= ret.area;
                ret.co /= ret.area;
                ret.bm /= ret.area;
            }
            return ret;
        }

        [[nodiscard]] V finalCut(const double aArea, const bool eco, const bool shelterWood) {
            return shelterWood ? finalCutShelterWood(aArea, -1, -1, -1, eco, true) : finalCut(aArea, eco);
        }

        [[nodiscard]] V thinAndGrow() {
            switch (sdDef) {
                case 0:  // Constant stocking degree
                    return thinAndGrowStatic();
                default:  // Alternate between varying stocking degrees
                    return thinAndGrowOLD();
            }
        }

        [[nodiscard]] V thinAndGrow(const bool changeSpecies) {
            return thinAndGrowStatic(changeSpecies);
        }

        // MG: (if selectiveLogging = TRUE)
        [[nodiscard]] pair<V, double> thinAndGrowSL(const double harvestedArea) {
            return thinAndGrowStaticSL(harvestedArea);
        }

        [[nodiscard]] V thinAndGrowShelterWood(const bool speciesChange) {
            if (!timerSW.empty()) {
                for (auto &swt: timerSW) {
                    ++swt.age;
                    --swt.timer;
                }
                auto maxAgeSW = ranges::max_element(timerSW,
                                                    [](const auto &lhs, const auto &rhs) { return lhs.age < rhs.age; });
                maxNumberOfAgeClasses = max(static_cast<size_t>(maxAgeSW->age + 1), maxNumberOfAgeClasses);
                if (maxAgeSW->age > dat.size() - 1) {
                    size_t oldSize = dat.size();
                    dat.resize(maxAgeSW->age + 1);
                    initCohort(oldSize, maxAgeSW->age + 1);
                }
            }
            return thinAndGrowStatic(speciesChange);
        }

        [[nodiscard]] V thinAndGrowStatic() {
            V ret;
            double shBm = 0;                // MG: sum of BM in age classes which are thinned
            double sumArea = 0;             // MG: sum of area of all age classes
            double area_dw = 0;             // sum of area where the deadwood was collected
            double area_lt = 0;             // sum of area where the litter was collected
            double mortDwBm = 0;            // sum of area x DW
            double mortLitterBm = 0;        // sum of area x litter
            double mortDwDBH_tmp = 0;
            double mortDwH_tmp = 0;
            double mortLitterH_tmp = 0;
            double mortLitterDbh_tmp = 0;

            hDbh = 0;
            hH = 0;         // MG: average diameter and height of trees in age classes that are thinned
            netInc = 0;     // MG:  net annual increment averaged over all age classes
            grossInc = 0;   // MG:  gross annual increment averaged over all age classes
            for (ptrdiff_t i = ssize(dat) - 1; i >= 0; --i) //11.05.2016 MG:Thin and grow the oldest age class
                if (dat[i].area > 0) {
                    auto [sd, iGwl, bmT, id] = incStatic(i);
                    if (flexSd > 0) {  // The typical amount of harvest
                        // standing biomass (growing stock) at the next age class step, not used if flexSd = 0
                        double bmTCom = dat[i].bm + incCommon(i, sd, iGwl);
                        bmT = bmT * (1 - flexSd) + bmTCom * flexSd;
                    }
                    double totalWood = max(0., dat[i].area * (iGwl - (bmT - dat[i].bm)));
                    double bm_cur = dat[i].bm;  // MG: current biomass before growth
                    array<double, 3> dbhBmSh{};
                    dbhBmSh[0] = dat[i].d + id * 0.5;
                    dbhBmSh[1] = dat[i].bm + iGwl * 0.5;
                    dbhBmSh[2] = totalWood / dbhBmSh[1];
                    double age = static_cast<double>(i) * timeStep;

                    if (static_cast<bool>(dov->ip(dbhBmSh)) && totalWood > 0) {  // Do Thinning if it is economic
                        double harvestedWood = totalWood * fc->getHlv().ip(dbhBmSh[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhBmSh[0]);
                        ret.area += dat[i].area;
                        ret.bm += totalWood;
                        ret.sw += sawnWood;
                        ret.rw += harvestedWood - sawnWood;
                        ret.co += totalWood * cov->ip(dbhBmSh);
                        double incBm = dat[i].area > 0 ? iGwl - totalWood / dat[i].area : iGwl;
                        dat[i].bm = max(0., dat[i].bm + incBm);
                        // MG: estimation of BM weighted average d and h of harvested (at thinning) trees
                        double tmp_arg = harvestedWood * dat[i].area;
                        hDbh += dat[i].d * tmp_arg;
                        hH += dat[i].h * tmp_arg;
                        shBm += tmp_arg;
                    } else {  // No thinning
                        dat[i].bm += iGwl;
                        double bmMax = it->getBm(age + timeStep, avgMai);
                        double tmp_arg = (dat[i].bm - bmMax) * dat[i].area;
                        if (bmMax < dat[i].bm) {
                            if (dat[i].d + id * 0.5 > 10) {  // deadwood
                                deadwood += tmp_arg;
                                area_dw += dat[i].area;
                                mortDwDBH_tmp += dat[i].d * tmp_arg;
                                mortDwH_tmp += dat[i].h * tmp_arg;
                                mortDwBm += tmp_arg;  // MG: estimation of BM weighted average d and h of dead trees (deadwood) that are not thinned
                            } else if (dat[i].d + id * 0.5 <= 10 && dat[i].d > 0.1) {  // litter
                                litter += tmp_arg;
                                area_lt += dat[i].area;
                                mortLitterDbh_tmp += dat[i].d * tmp_arg;
                                mortLitterH_tmp += dat[i].h * tmp_arg;
                                mortLitterBm += tmp_arg;  // MG: estimation of BM weighted average d and h of dead trees (litter) that are not thinned
                            }
                            dat[i].bm = max(0., bmMax);
                        }
                    }
                    dat[i].d += id;
                    dat[i].h += it->getIncHeight(age, avgMai);
                    //netInc += dat[i].area * (dat[i].bm-bm_cur);  // MG thinning and mortality are out
                    netInc += dat[i].area *
                              (dat[i].bm + ret.bm - bm_cur);  // MG thinning is included and mortality is out
                    //		netInc += dat[i].area * (bmT-dat[i].bm);
                    grossInc += dat[i].area * iGwl;
                    sumArea += dat[i].area;
                }

            if (shBm > 0) {
                hDbh /= shBm;
                hH /= shBm;
                thinned_weight = shBm / ret.area;
            } else {
                hDbh = 0;
                hH = 0;
                thinned_weight = 0;
            }  // MG: estimation of BM weighted average d and h of harvested (at thinning) trees
            if (mortDwBm > 0) {
                mortDwDBH = mortDwDBH_tmp / mortDwBm;
                mortDwH = mortDwH_tmp / mortDwBm;
            } else {
                mortDwDBH = 0;
                mortDwH = 0;
            }  // MG: estimation of BM weighted average d and h of dead trees (deadwood) that were not thinned
            if (mortLitterBm > 0) {
                mortLitterDbh = mortLitterDbh_tmp / mortLitterBm;
                mortLitterH = mortLitterH_tmp / mortLitterBm;
            } else {
                mortLitterDbh = 0;
                mortLitterH = 0;
            }  // MG: estimation of BM weighted average d and h of dead trees (deadwood) that were not thinned
            deadwood_weight = area_dw > 0 ? deadwood / area_dw : 0;  // area & bm-weighted dw, tC/ha
            litter_weight = area_lt > 0 ? litter / area_lt : 0;  // area & bm-weighted litter, tC/ha

            if (sumArea > 0) {
                netInc /= sumArea;
                grossInc /= sumArea;
            } else {
                netInc = 0;
                grossInc = 0;
            }  // MG: estimation of net and gross increments averaged over all age classes for timeStep, tC/(ha timeStep)

            if (isinf(mortLitterDbh))
                WARN("mortLitterBm = ", mortLitterBm);
            if (isinf(mortDwDBH))
                WARN("mortDwBm = ", mortDwBm);

            cohortShift();
            return ret;
        }

        [[nodiscard]] V thinAndGrowStatic(const bool speciesChange) {
            V ret;
            double shBm = 0;            // MG: sum of harvested wood*area in age classes which are thinned
            double sumArea = 0;         // MG: sum of area of all age classes
            double area_dw = 0;
            double area_lt = 0;
            double mortDwBm = 0;        // MG: sum of deadwood*area
            double mortLitterBm = 0;    // MG: sum of litter-wood * area
            hDbh = 0;
            hH = 0;         // MG: average diameter and height of trees in age classes that are thinned
            netInc = 0;     // MG:  net annual increment averaged over all age classes
            grossInc = 0;   // MG:  gross annual increment averaged over all age classes

            for (auto i = ssize(dat) - 1; i >= 0; --i)  // 11.05.2016 MG: Thin and grow the oldest age class
                if (dat[i].area > 0 && !speciesChange || dat[0].area >= 0 && dat[i].bm >= 0 && speciesChange) {
                    auto [sd, iGwl, bmT, id] = incStatic(i);
                    if (flexSd > 0) {  // The typical amount of harvest
                        double bmTCom = dat[i].bm + incCommon(i, sd, iGwl);
                        bmT = bmT * (1 - flexSd) + bmTCom * flexSd;
                    }
                    double totalWood = max(0., dat[i].area * (iGwl - (bmT - dat[i].bm)));
                    double bm_cur = dat[i].bm;  // MG: current biomass before growth

                    array<double, 3> dbhBmSh{};
                    dbhBmSh[0] = dat[i].d + id * 0.5;
                    dbhBmSh[1] = dat[i].bm + iGwl * 0.5;
                    dbhBmSh[2] = totalWood / dbhBmSh[1];
                    double age = static_cast<double>(i) * timeStep;
                    if (static_cast<bool>(dov->ip(dbhBmSh))) { //Do Thinning if it is economic
                        double harvestedWood = totalWood * fc->getHlv().ip(dbhBmSh[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhBmSh[0]);
                        ret.area += dat[i].area;
                        ret.bm += totalWood;
                        ret.sw += sawnWood;
                        ret.rw += harvestedWood - sawnWood;
                        ret.co += totalWood * cov->ip(dbhBmSh);
                        double incBm = dat[i].area > 0 ? iGwl - totalWood / dat[i].area : iGwl;
                        dat[i].bm = max(0., dat[i].bm + incBm);
                        //  MG: estimation of BM weighted average d and h of harvested (at thinning) trees
                        double tmp_arg = harvestedWood * dat[i].area;
                        hDbh += dat[i].d * tmp_arg;
                        hH += dat[i].h * tmp_arg;
                        shBm += tmp_arg;
                    } else {  // No thinning
                        dat[i].bm += iGwl;
                        double bmMax = it->getBm(age + timeStep, avgMai);
                        double tmp_arg = (bmMax - dat[i].bm) * dat[i].area;
                        if (bmMax > dat[i].bm) {
                            if (dat[i].d + id * 0.5 > 10) {
                                deadwood += tmp_arg;
                                area_dw += dat[i].area;
                                mortDwDBH += dat[i].d * tmp_arg;
                                mortDwH += dat[i].h * tmp_arg;
                                mortDwBm += tmp_arg;  // MG: estimation of BM weighted average d and h of dead trees (deadwood) that are not thinned
                            } else if (dat[i].d + id * 0.5 <= 10) {
                                litter += tmp_arg;
                                area_lt += dat[i].area;
                                mortLitterDbh += dat[i].d * tmp_arg;
                                mortLitterH += dat[i].h * tmp_arg;
                                mortLitterBm += tmp_arg;  // MG: estimation of BM weighted average d and h of dead trees (litter) that are not thinned
                            }
                            dat[i].bm = max(0., bmMax);
                        }
                    }
                    dat[i].d += id;
                    dat[i].h += it->getIncHeight(age, avgMai);
                    netInc += dat[i].area * (dat[i].bm - bm_cur);
                    grossInc += dat[i].area * iGwl;
                    sumArea += dat[i].area;
                }

            if (shBm > 0) {
                hDbh /= shBm;
                hH /= shBm;
            } else {
                hDbh = 0;
                hH = 0;
            }  // MG: estimation of BM weighted average d and h of harvested (at thinning) trees
            if (mortDwBm > 0) {
                mortDwDBH /= mortDwBm;
                mortDwH /= mortDwBm;
            } else {
                mortDwDBH = 0;
                mortDwH = 0;
            }  // MG: estimation of BM weighted average d and h of dead trees (deadwood) that were not thinned
            if (mortLitterBm > 0) {
                mortLitterDbh /= mortLitterBm;
                mortLitterH /= mortLitterBm;
            } else {
                mortLitterDbh = 0;
                mortLitterH = 0;
            }  // MG: estimation of BM weighted average d and h of dead trees (deadwood) that were not thinned

            if (sumArea > 0) {
                netInc /= sumArea;
                grossInc /= sumArea;
            } else {
                netInc = 0;
                grossInc = 0;
            }  // MG: estimation of net and gross increments averaged over all age classes for timeStep, tC/(ha timeStep)
            cohortShift();
            return ret;
        }

        // MG: (if selectiveLogging = TRUE)
        [[nodiscard]] pair<V, double> thinAndGrowStaticSL(const double harvestedArea) {
            V ret;
            double shBm = 0;        // MG: sum of BM in age classes which are thinned
            double sumArea = 0;     // MG: sum of area of all age classes
            hDbh = 0;
            hH = 0;                 //MG: average diameter and height of trees in age classes that are thinned
            netInc = 0;             //MG: net annual increment averaged over all age classes
            grossInc = 0;           //MG: gross annual increment averaged over all age classes
            double tmp_arg = (1 - slShare) * mai;
            double bm0 = max(0.01, it->getBm(10, tmp_arg));
            double mort0 = it->getRemBm(10, tmp_arg) / bm0;

            for (ptrdiff_t i = ssize(dat) - 1; i >= 0; --i) {  // 11.05.2016 MG: Thin and grow the oldest age class
                if (dat[i].area > 0) {
                    const auto [sd, iGwl, bmT, id] = incStaticSL1(i);

                    //        if(flexSd > 0.) { //The typical amount of harvest
                    //          double bmTCom =  dat[i].bm + incCommon(i, sd, iGwl);
                    //          bmT = bmT * (1. - flexSd) + bmTCom * flexSd;
                    //        }
                    double totalWood = max(0., dat[i].area * (iGwl - (bmT - dat[i].bm)));
                    double bm_cur = dat[i].bm;  // MG: current biomass before growth

                    array<double, 3> dbhBmSh{};
                    dbhBmSh[0] = dat[i].d + id * 0.5;
                    dbhBmSh[1] = dat[i].bm + iGwl * 0.5;
                    dbhBmSh[2] = totalWood / dbhBmSh[1];
                    double age = static_cast<double>(i) * timeStep;
                    if (static_cast<bool>(dov->ip(dbhBmSh))) {  // Do Thinning if it is economic
                        double harvestedWood = totalWood * fc->getHlv().ip(dbhBmSh[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhBmSh[0]);
                        ret.area += dat[i].area;
                        ret.bm += totalWood;
                        ret.sw += sawnWood;
                        ret.rw += harvestedWood - sawnWood;
                        ret.co += totalWood * cov->ip(dbhBmSh);
                        dat[i].bm += iGwl - totalWood / dat[i].area;
                        // we assume that the area taken by a tree is proportional to its biomass
                        gapArea += totalWood / dat[i].bm;
                        // gapArea += (totalWood/dat[i].bm) * slShare; // we assume that the area taken by a tree is proportional to its biomass
                        hDbh += dat[i].d * dat[i].bm;
                        hH += dat[i].h * dat[i].bm;
                        shBm += dat[i].bm; //MG: estimation of BM weighted average d and h of harvested (at thinning) trees
                    } else {  // No thinning
                        dat[i].bm += iGwl;
                        // double bmMax = slShare*it->getBm((i+1)*timeStep, avgMai);
                        // MG: should it be bmMax = it->getBmSdNat((i+1)*timeStep, avgMai, sd) ?
                        double bmMax = it->getBm(age + timeStep, avgMai);
                        dat[i].bm = clamp(dat[i].bm, 0., bmMax);
                        // if(dat[i].bm > bmMax) {gapArea += ((dat[i].bm-bmMax)/bmMax)*dat[i].area; dat[i].bm = bmMax;} // MG: Should we postpone the growing on place of standing dead trees????
                    }
                    // if (i*timeStep > slAge) {
                    dat[i].d += id;
                    dat[i].h += it->getIncHeight(age, avgMai);
                    netInc += dat[i].area * (dat[i].bm - bm_cur);//MG
                    // netInc += dat[i].area * (bmT-dat[i].bm);
                    grossInc += dat[i].area * iGwl;
                    // sumArea += dat[i].area;
                    //}
                }
            }
            // 11.05.2016 MG: Thin and grow the oldest age class
            for (ptrdiff_t i = ssize(dat0) - 1; i >= 0; --i) {
                if (dat0[i].area > 0) {
                    const auto [sd, iGwl, bmT, id] = incStaticSL0(i);
//        if(flexSd > 0.) { //The typical amount of harvest
//          double bmTCom =  dat0[i].bm + incCommon(i, sd, iGwl);
//          bmT = bmT * (1. - flexSd) + bmTCom * flexSd;
//        }
                    //double totalWood = dat0[i].area * (iGwl - (bmT-dat0[i].bm));

                    // mortality from Weng et al. 2015 Scaling from individual trees to forests in an Earth system modelling framework ..... Biogeosciences, 12, 2655-2694
                    double dbhI = dat0[i].d <= 0.01 ? 0.0001 : dat0[i].d * 0.01;
                    double tmp = exp(-30 * dbhI);
                    double mortI = clamp(mort0 * (1 + 10 * tmp) / (1 + 2 * tmp), 0., 1.);
                    // Do we introduce double counting of mortality in younger age classes as the mortality is already in the bm data?
                    double totalWood = max(0., dat0[i].area * (iGwl - (bmT - dat0[i].bm) * (1 - mortI)));
                    double bm_cur = dat0[i].bm;  // MG: current biomass before growth

                    array<double, 3> dbhBmSh{};
                    dbhBmSh[0] = dat0[i].d + id * 0.5;
                    dbhBmSh[1] = dat0[i].bm + iGwl * 0.5;
                    dbhBmSh[2] = totalWood / dbhBmSh[1];
                    double age = static_cast<double>(i) * timeStep;
                    double tmp_arg_1 = (1 - slShare) * avgMai;
                    double tmp_arg_2 = (1 - slShare) * mai;
                    if (static_cast<bool>(dov->ip(dbhBmSh))) {  // Do Thinning if it is economic
                        double harvestedWood = totalWood * fc->getHlv().ip(dbhBmSh[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhBmSh[0]);
                        ret.area += dat0[i].area;
                        ret.bm += totalWood;
                        ret.sw += sawnWood;
                        ret.rw += harvestedWood - sawnWood;
                        ret.co += totalWood * cov->ip(dbhBmSh);

                        double bm_test1 = 0;
                        double bm_test2 = 0;
                        double area_test = dat0[i].area;
                        double dbm_test = iGwl - totalWood / dat0[i].area;

                        bm_test1 = dat0[i].bm;
                        dat0[i].bm += iGwl - totalWood / dat0[i].area;
                        bm_test2 = dat0[i].bm;
                        // if (dat0[i].bm<0) dat0[i].bm=0;
                        // we assume that the area taken by a tree is proportional to its biomass
                        gapArea0 += totalWood / dat0[i].bm;
                        hDbh += dat0[i].d * dat0[i].bm;
                        hH += dat0[i].h * dat0[i].bm;
                        shBm += dat0[i].bm;  // MG: estimation of BM weighted average d and h of harvested (at thinning) trees
                    } else {  // No thinning
                        dat0[i].bm += iGwl;
                        //double bmMax = it->getBm((i+1)*timeStep, (1-slShare)*avgMai);
                        double bmMax = it->getBm(age + timeStep, tmp_arg_1 * (1 - mortI));
                        if (dat0[i].bm > bmMax) {
                            gapArea0 += ((dat0[i].bm - bmMax) / bmMax) * dat0[i].area;
                            dat0[i].bm = bmMax;
                        } // MG: Should we postpone the growing on place of standing dead trees????
                    }
                    double dbhMax = it->getDbh(age + timeStep, tmp_arg_2);
                    double hMax = it->getHeight(age + timeStep, tmp_arg_2);
                    dat0[i].d = min(dbhMax, dat0[i].d + id);
                    dat0[i].h = min(hMax, dat0[i].h + it->getIncHeight(age, tmp_arg_1));
                    netInc += dat0[i].area * (dat0[i].bm - bm_cur);
                    // netInc += dat0[i].area * (bmT-dat0[i].bm);
                    // grossInc += dat0[i].area * iGwl;
                    sumArea += dat0[i].area;
                }
            }

            if (shBm > 0) {
                hDbh /= shBm;
                hH /= shBm;
            } else {
                hDbh = 0;
                hH = 0;
            }//MG: estimation of BM weighted average d and h of harvested (at thinning) trees
            if (sumArea > 0) {
                netInc /= sumArea;
                grossInc /= sumArea;
            } else {
                netInc = 0;
                grossInc = 0;
            }//MG: estimation of net and gross increments averaged over all age classes for timeStep, tC/(ha timeStep)
            return {ret, cohortShiftSL(harvestedArea)};
        }

        // Needs to be removed after new restructuring
        [[nodiscard]] V thinAndGrowOLD() {
            V ret;
            bool constSd = sdDef == 0;
            for (ptrdiff_t i = ssize(dat) - 1; i >= 0; --i)
                if (dat[i].area > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    double sdNat = 0;
                    if (i > 0)
                        sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    else  // Afforestation has a stocking degree of sdMin
                        sdNat = sdMin > 0 ? sdMin * it->getSdNat(0, avgMai) : -sdMin;
                    double gwl = it->getIncGwlSdNat(age, mai, sdNat);
                    double sd = 0;  //Stocking degree after growing period
                    if (sdMax > 0)
                        sd = (gwl + dat[i].bm) / it->getBmT(age + timeStep, avgMai);
                    else
                        sd = (gwl + dat[i].bm) / it->getBm(age + timeStep, avgMai);
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat);
                    //The oldest age class has no increment
                    if (i == dat.size() - 1) {
                        gwl = 0;
                        id = 0;
                    } else
                        dat[i].h += it->getIncHeight(age, avgMai);
                    bool thinningWasDone = false;
                    // Key to ask if harvest is economic
                    array<double, 3> dbhBmSh{};
                    // Stocking degree to high or typical thinning are forced . make thinning
                    if (sd > abs(sdMax) || flexSd > 0) {
                        //Thinning caused by stand density
                        double reduce = 0;
                        if (constSd)
                            reduce = abs(sdMax) / sd;
                        else if (sd > abs(sdMax)) {
                            if (sdMin > 0)
                                reduce = sdMin / (gwl * 0.5 + dat[i].bm) * it->getBmT(age + timeStep, avgMai);
                            else
                                reduce = -sdMin / (gwl * 0.5 + dat[i].bm) * it->getBm(age + timeStep, avgMai);
                        }
                        reduce = clamp(reduce, 0., 1.);
                        //Thinning caused by typical thinning
                        double reduceB = 0;
                        if (flexSd > 0) {
                            double cutVol = gwl - it->getIncBmSdNat(age, mai, sdNat);
                            if (cutVol > 0) {
                                //A weighting by sd / sdMax is done to come sometime to sdMax
                                double weight = weight = sdMax > 0 ? it->getSdTab(age, avgMai, dat[i].bm) / sdMax :
                                                         sdNat / -sdMax;
                                reduceB = 1 - cutVol * dat[i].bm * weight / gwl;
                            }
                        }
                        reduceB = clamp(reduceB, 0., 1.);
                        //Bring in the thinning fluctuation softener
                        //flexSd > 0 && flexSd <= 1
                        reduce = reduce * (1 - flexSd) + reduceB * flexSd;
                        dbhBmSh[0] = dat[i].d + id * 0.5;
                        dbhBmSh[1] = dat[i].bm + gwl * 0.5;
                        dbhBmSh[2] = 1 - reduce;
                        if (reduce > 0 && static_cast<bool>(dov->ip(dbhBmSh))) {  // Do Thinning if it is economic
                            thinningWasDone = true;
                            if (constSd) {
                                dat[i].bm += gwl;
                                dat[i].d += id;
                            } else {
                                // The Stands get half increment at high stand density
                                dat[i].bm += gwl * 0.5;
                                dat[i].d += id * 0.5;
                            }
                            double totalWood = dat[i].area * dat[i].bm * (1 - reduce);
                            dat[i].bm *= reduce;
                            double harvestedWood = totalWood * fc->getHlv().ip(dat[i].d);
                            double sawnWood = harvestedWood * sws->ip(dat[i].d);
                            ret.area += dat[i].area;
                            ret.bm += totalWood;
                            ret.sw += sawnWood;
                            ret.rw += harvestedWood - sawnWood;
                            ret.co += totalWood * cov->ip(dbhBmSh);
                            if (!constSd) {
                                //The Stands get half increment at low stand density
                                sdNat = it->getSdNat(age + 0.5 * timeStep, avgMai, dat[i].bm);
                                dat[i].bm += it->getIncBmSdNat(age + 0.5 * timeStep, avgMai, sdNat) * 0.5;
                                dat[i].d += it->getIncDbhSdNat(age + 0.5 * timeStep, avgMai, sdNat) * 0.5;
                            }
                        } else {  // No thinning
                            dat[i].bm += gwl;
                            dat[i].d += id;
                        }
                    } else {  // No thinning
                        dat[i].bm += gwl;
                        dat[i].d += id;
                    }
                    //Check if Bm is not higher than maximum possible at end of period
                    double bmMax = it->getBm(age + timeStep, avgMai);
                    if (dat[i].bm > bmMax) {
                        if (!constSd && thinningWasDone) {
                            double totalWood = dat[i].area * (dat[i].bm - bmMax);
                            double harvestedWood = totalWood * fc->getHlv().ip(dat[i].d);
                            double sawnWood = harvestedWood * sws->ip(dat[i].d);
                            ret.bm += totalWood;
                            ret.sw += sawnWood;
                            ret.rw += harvestedWood - sawnWood;
                            ret.co += totalWood * cov->ip(dbhBmSh);
                        }
                        dat[i].bm = bmMax;
                    }
                    // Bring the Data to the next age class
                    if (i < dat.size() - 2)
                        dat[i + 1] = dat[i];
                    else {
                        double tmp_arg = 1 / (dat[i + 1].area + dat[i].area);
                        dat[i + 1].d = (dat[i + 1].d * dat[i + 1].area + dat[i].d * dat[i].area) * tmp_arg;
                        dat[i + 1].h = (dat[i + 1].h * dat[i + 1].area + dat[i].h * dat[i].area) * tmp_arg;
                        dat[i + 1].bm = (dat[i + 1].bm * dat[i + 1].area + dat[i].bm * dat[i].area) * tmp_arg;
                        dat[i + 1].area += dat[i].area;
                    }
                    dat[i].reset();
                }

            if (ret.area > 0) {  // Values per hectare
                ret.sw /= ret.area;
                ret.rw /= ret.area;
                ret.co /= ret.area;
                ret.bm /= ret.area;
            }
            return ret;
        }

        [[nodiscard]] array<double, 4> incStatic(const size_t i) const {
            double sd = 0;   // estimated SD
            double iGwl = 0; // total increment
            double bmT = 0;  // target theoretical biomass at the next age class step
            double id = 0;   // dbh increment
            double age = static_cast<double>(i) * timeStep;
            if (sdMax > 0) {  // Yield table stocking degree
                // MG: why t+1???
                bmT = sdMax == 1 ? it->getBmT(age + timeStep, mai) : it->getBmSdTab(age + timeStep, mai, sdMax);
                sd = i > 0 ? it->getSdTab(age, avgMai, dat[i].bm) : 1;  // New plantations have a stocking degree of 1
                if (sd == 1) {
                    iGwl = it->getIncGwlT(age, mai);
                    id = it->getIncDbhT(age, mai);
                } else {
                    iGwl = it->getIncGwlSdTab(age, mai, sd);
                    id = it->getIncDbhSdTab(age, mai, sd);
                }
            } else {  // Natural stocking degree
                bmT = sdMax == -1 ? it->getBm(age + timeStep, mai) : it->getBmSdNat(age + timeStep, mai, abs(sdMax));
                sd = i > 0 ? it->getSdNat(age, avgMai, dat[i].bm) : 1;  // New plantations have a stocking degree of 1
                if (sd == 1) {
                    iGwl = it->getIncGwl(age, mai);
                    id = it->getIncDbh(age, mai);
                } else {
                    iGwl = it->getIncGwlSdNat(age, mai, sd);
                    id = it->getIncDbhSdNat(age, mai, sd);
                }
            }
            return {sd, iGwl, bmT, id};
        }

        // MG: for the 2 canopy layer (if selectiveLogging = TRUE)
        [[nodiscard]] array<double, 4> incStaticSL0(const size_t i) const {
            double sd = 0;
            double iGwl = 0;
            double bm = 0;
            double id = 0;
            // Natural stocking degree
            double age = static_cast<double>(i) * timeStep;
            double tmp_arg = (1 - slShare) * mai;
            bm = it->getBm(age + timeStep, tmp_arg);
            sd = i > 0 ? it->getSdNat(age, (1 - slShare) * avgMai, dat0[i].bm)
                       : 1; // New plantations have a stocking degree of 1
            if (sd == 1) {
                iGwl = it->getIncGwl(age, tmp_arg);
                id = it->getIncDbh(age, tmp_arg);
            } else {
                iGwl = it->getIncGwlSdNat(age, tmp_arg, sd);
                id = it->getIncDbhSdNat(age, tmp_arg, sd);
            }
            return {sd, iGwl, bm, id};
        }

        // MG:for the 1 canopy layer (if selectiveLogging = TRUE)
        [[nodiscard]] array<double, 4> incStaticSL1(const size_t i) const {
            double sd = 0;
            double iGwl = 0;
            double bm = 0;
            double id = 0;
            //Natural stocking degree
            double sdTmp = abs(sdMax);
            double age = static_cast<double>(i) * timeStep;
            bm = it->getBmSdNat(age + timeStep, mai, slShare * sdTmp);
            sd = i > 0 ? it->getSdNat(age, avgMai, dat[i].bm) : slShare * sdTmp;
            iGwl = it->getIncGwlSdNat(age, mai, sd);
            id = it->getIncDbhSdNat(age, mai, sd);
            return {sd, iGwl, bm, id};
        }

        [[nodiscard]] double incCommon(const size_t i, const double sd, const double iGwl) const {
            double sdTarget = abs(sdMax);
            double bmInc = 0;
            double age = static_cast<double>(i + 1) * timeStep;
            if (sd > 0) {
                if (sdMax > 0)  // Yield table stocking degree
                    bmInc = sd == 1 ? it->getIncBmT(age, mai) : it->getIncBmSdTab(age, mai, sd);
                else  // Natural stocking degree
                    bmInc = sd == 1 ? it->getIncBm(age, mai) : it->getIncBmSdNat(age, mai, sd);
                bmInc = min(bmInc, iGwl);
                if (sd > sdTarget)
                    bmInc *= sdTarget / sd;
                else
                    bmInc += (iGwl - bmInc) * (1 - 1 / (1 + sdTarget - sd));
            } else  // Current stocking degree is 0
                bmInc = sdTarget > 0 ? iGwl : 0;
            return bmInc;
        }

        void cohortShift() {
            if (dat.size() > 1) {
                size_t i = dat.size() - 2;
                if (dat[i + 1].area > 0) {
                    if (i + 2 < maxNumberOfAgeClasses) {  // Add one more age class
                        size_t oldSize = dat.size();
                        dat.resize(i + 3);
                        // MG BUG: new class is not populated by bm and d data !!! corrected 15.04.2016
                        initCohort(oldSize, i + 3);
                        dat[i + 2] = dat[i + 1];  // MG BUG: disappears area of last age class !!! corrected 15.04.2016
                        dat[i + 1] = dat[i];
                    } else { // It would be good to allow one more age class
                        double tmp_arg = 1 / (dat[i + 1].area + dat[i].area);
                        dat[i + 1].d = (dat[i + 1].d * dat[i + 1].area + dat[i].d * dat[i].area) * tmp_arg;
                        dat[i + 1].h = (dat[i + 1].h * dat[i + 1].area + dat[i].h * dat[i].area) * tmp_arg;
                        dat[i + 1].bm = (dat[i + 1].bm * dat[i + 1].area + dat[i].bm * dat[i].area) * tmp_arg;
                        dat[i + 1].area += dat[i].area;
                    }
                } else
                    dat[i + 1] = dat[i];
                ranges::shift_right(dat | rv::take(dat.size() - 1), 1);  // shift right 1 dat except last element
                dat[0].bm = 0;
                dat[0].d = 0;
                dat[0].h = 0;
            } else if (maxNumberOfAgeClasses > 1 && dat.size() == 1) {
                dat.resize(2);
                initCohort(0, 1);
                dat[1] = dat[0];
                dat[0].reset();
            }
        }

        // MG: shift cohorts of the 2 and 1 canopy layers,
        // populate gapArea in the 1 layer from the 2 layer (if selectiveLogging = TRUE)
        [[nodiscard]] double cohortShiftSL(const double aGapArea) {
            double areaShift = 0;
            double areaDebt = getAreaSL(0) - (getAreaSL(1) + aGapArea);
            if (dat.size() > slAge) {
                ptrdiff_t i = ssize(dat) - 2;
                if (dat[i + 1].area > 0) {
                    if (i + 2 < maxNumberOfAgeClasses) { //Add one more age class
                        size_t oldSize = dat.size();
                        dat.resize(i + 3);
                        // MG BUG: new class is not populated by bm and d data !!! corrected 15.04.2016
                        initCohortSL1(oldSize, i + 3);
                        dat[i + 2] = dat[i + 1];  // MG BUG: disappears area of last age class !!! corrected 15.04.2016
                        dat[i + 1] = dat[i];
                    } else {  // It would be good to allow one more age class // MG: To be checked!!! 07.01.2019
                        double tmp_arg = 1 / (dat[i + 1].area + dat[i].area);
                        dat[i + 1].d = (dat[i + 1].d * dat[i + 1].area + dat[i].d * dat[i].area) * tmp_arg;
                        dat[i + 1].h = (dat[i + 1].h * dat[i + 1].area + dat[i].h * dat[i].area) * tmp_arg;
                        dat[i + 1].bm = (dat[i + 1].bm * dat[i + 1].area + dat[i].bm * dat[i].area) * tmp_arg;
                        dat[i + 1].area += dat[i].area;
                    }
                } else
                    dat[i + 1] = dat[i];
                ranges::shift_right(dat | rv::take(dat.size() - 1), 1);  // shift right 1 dat except last element

                vector<Cohort> dat_tmp = dat;
                dat[0].reset();
                if (dat0.size() > slAge) {
                    double treeShift = 0;
                    areaDebt = max(0., areaDebt);
                    for (i = ssize(dat0) - 1; treeShift < aGapArea + areaDebt &&
                                              static_cast<double>(i) * timeStep > static_cast<double>(slAge) &&
                                              i >= 0; --i)
                        if (dat0[i].area > 0 && dat0[i].bm > 0) {
                            double dbh0 = dat0[i].d;
                            size_t j = 1;
                            const auto [sd, iGwl, bmT, id] = incStaticSL1(i);
                            double ih = it->getIncHeight(static_cast<double>(j) * timeStep, avgMai);
                            while (it->getDbhSdNat(static_cast<double>(j), mai, slShare) < dbh0 + 2 * id)
                                ++j;  // find respective diameter class for the undergrowth
                            j = min(dat.size() - 1, j);
                            double bm1 = it->getBmSdNat(static_cast<double>(j), mai, slShare);
                            double d1 = it->getDbhSdNat(static_cast<double>(j), mai, slShare);
                            double h1 = it->getHeight(static_cast<double>(j), mai);
                            double bm0 = min(dat0[i].bm + 2 * iGwl, bm1);
                            double d0 = min(dat0[i].d + 2 * id, d1);
                            double h0 = min(dat0[i].h + 2 * ih, h1);

                            if (treeShift + dat0[i].area < aGapArea + areaDebt) { // Shift all age class
                                dat[j].area += dat0[i].area;
                                dat[j].bm += bm0 * dat0[i].area;
                                dat[j].d += d0 * dat0[i].area;
                                dat[j].h += h0 * dat0[i].area;
                                treeShift += dat0[i].area;
                                dat0[i].reset();
                            } else {
                                double treeShiftShare = (aGapArea + areaDebt - treeShift) / dat0[i].area;
                                double tmp_arg = aGapArea + areaDebt - treeShift;
                                dat[j].area += tmp_arg;
                                dat[j].bm += bm0 * tmp_arg;
                                dat[j].d += d0 * tmp_arg;
                                dat[j].h += h0 * tmp_arg;
                                treeShift += tmp_arg;
                                dat0[i].area *= 1 - treeShiftShare;
                            }
                        }
                    if (treeShift > 0) {
                        //for (i = static_cast<int>(dat.size()) - 1; i > slAge; --i) {
                        for (i = 0; i < dat.size(); ++i)
                            if (dat[i].area > 0) {
                                dat[i].bm += dat_tmp[i].bm * dat_tmp[i].area - dat_tmp[i].bm;
                                dat[i].d += dat_tmp[i].d * dat_tmp[i].area - dat_tmp[i].d;
                                dat[i].h += dat_tmp[i].h * dat_tmp[i].area - dat_tmp[i].h;

                                dat[i].bm /= dat[i].area;
                                dat[i].d /= dat[i].area;
                                dat[i].h /= dat[i].area;
                            }
/*
			  if (dat[slAge].area > 0) {
				  dat[slAge].bm /= dat[slAge].area;
				  dat[slAge].d /= dat[slAge].area;
				  dat[slAge].h /= dat[slAge].area;
			  }
			  else {
				  dat[slAge].bm = 0;
				  dat[slAge].d = 0;
				  dat[slAge].h = 0;
			  }
*/
                    }
                    areaShift = treeShift;
                }
            } else if (maxNumberOfAgeClasses > slAge && dat.size() == slAge) {
                dat.resize(slAge + 1);
                initCohortSL1(slAge, slAge + 1);
                dat[slAge + 1] = dat[slAge];
                //	  dat[slAge].reset();
                dat[0].reset();

                //int jumpAge = it->getTOpt((1-slShare)*mai,0);
                if (dat0.size() > slAge) {
                    double treeShift = 0;
                    auto slAge_d = static_cast<double>(slAge);
                    for (ptrdiff_t i = ssize(dat0) - 1;
                         treeShift < aGapArea + areaDebt && static_cast<double>(i) * timeStep > slAge_d && i >= 0; --i)
                        if (dat0[i].area > 0 && dat0[i].bm > 0) {
                            const auto [sd, iGwl, bmT, id] = incStaticSL1(i);
                            double ih = it->getIncHeight(slAge_d * timeStep, avgMai);
                            double bm1 = it->getBmSdNat(slAge_d, mai, slShare);
                            double d1 = it->getDbhSdNat(slAge_d, mai, slShare);
                            double h1 = it->getHeight(slAge_d, mai);
                            double bm0 = min(dat0[i].bm + 2 * iGwl, bm1);
                            double d0 = min(dat0[i].d + 2 * id, d1);
                            double h0 = min(dat0[i].h + 2 * ih, h1);

                            if (treeShift + dat0[i].area < aGapArea + areaDebt) {  // Shift all age class
                                dat[slAge].area += dat0[i].area;
                                dat[slAge].bm += bm0 * dat0[i].area;
                                dat[slAge].d += d0 * dat0[i].area;
                                dat[slAge].h += h0 * dat0[i].area;
                                treeShift += dat0[i].area;
                                dat0[i].reset();
                            } else {
                                double treeShiftShare = (aGapArea + areaDebt - treeShift) / dat0[i].area;
                                double tmp_arg = aGapArea + areaDebt - treeShift;
                                dat[slAge].area += tmp_arg;
                                dat[slAge].bm += bm0 * tmp_arg;
                                dat[slAge].d += d0 * tmp_arg;
                                dat[slAge].h += h0 * tmp_arg;
                                treeShift += tmp_arg;
                                dat0[i].area *= 1 - treeShiftShare;
                            }
                        }
                    if (dat[slAge].area > 0) {
                        dat[slAge].bm /= dat[slAge].area;
                        dat[slAge].d /= dat[slAge].area;
                        dat[slAge].h /= dat[slAge].area;
                    } else {
                        dat[slAge].bm = 0;
                        dat[slAge].d = 0;
                        dat[slAge].h = 0;
                    }
                    areaShift = treeShift;
                }

            }
            double areaSL1_tmp = area;
            area -= areaShift;
            double slShare_tmp = slShare;
            areaSL1 = getAreaSL(1);
            slShare_tmp = (areaSL1 / areaSL1_tmp) * sdMin;
            setShareSL(slShare_tmp);
            setAgeSL();
            if (dat0.size() > 1) {
                size_t i = dat0.size() - 2;
                if (dat0[i + 1].area > 0) {
                    if (i + 2 < maxNumberOfAgeClasses) { //Add one more age class
                        size_t oldSize = dat0.size();
                        dat0.resize(i + 3);
                        initCohortSL0(oldSize, i + 3);
                        dat0[i + 2] = dat0[i + 1];
                        dat0[i + 1] = dat0[i];
                    } else { //It would be good to allow one more age class
                        double tmp_arg = 1 / (dat0[i + 1].area + dat0[i].area);
                        dat0[i + 1].d = (dat0[i + 1].d * dat0[i + 1].area + dat0[i].d * dat0[i].area) * tmp_arg;
                        dat0[i + 1].h = (dat0[i + 1].h * dat0[i + 1].area + dat0[i].h * dat0[i].area) * tmp_arg;
                        dat0[i + 1].bm = (dat0[i + 1].bm * dat0[i + 1].area + dat0[i].bm * dat0[i].area) * tmp_arg;
                        dat0[i + 1].area += dat0[i].area;
                    }
                } else
                    dat0[i + 1] = dat0[i];
                ranges::shift_right(dat0 | rv::take(dat0.size() - 1), 1);  // shift right 1 dat0 except last element
                dat0[0].reset();
            } else if (maxNumberOfAgeClasses > 1 && dat0.size() == 1) {
                dat0.resize(2);
                initCohortSL0(0, 1);
                dat0[1] = dat0[0];
                dat0[0].reset();
            }
            return areaShift;
        }

        void createNormalForestSLTrue(double rotationPeriod, double aArea, double sd) {
            //slAge = 20; // should be defined as optimal increment age (e.g. jumpAge = it->getTOpt((1-slShare)*mai,0);)
            //slShare = 0.2;
            sd = abs(sd);
            if (slShare < 0)
                setShareSL(sd);
            //slAge = it->getTOpt((1-slShare)*mai,0);
            setAgeSL();
            setDbhHarvestSL();
            setBmRmSL();
            areaSL1 = aArea;
            aArea = max(0., aArea);
            area = aArea;
            rotationPeriod = clamp(rotationPeriod, 1., it->getTMax() - 1);
            size_t ageClassesL0 = ceil(0.5 + rotationPeriod / timeStep);
            dat0.resize(max(dat0.size(), ageClassesL0));
            maxNumberOfAgeClasses = max(maxNumberOfAgeClasses, ageClassesL0);
            double aArea1 = aArea;
            aArea /= rotationPeriod;
            double maxRotationL1 = it->getTOptSdNat(mai, slShare * sd, 1); // maximum average biomass
            double rotationPeriodL1 = min(rotationPeriod, maxRotationL1);
            size_t ageClassesL1 = ceil(rotationPeriodL1 / timeStep);
            dat.resize(max(dat.size(), ageClassesL1));

            auto slAge_d = static_cast<double>(slAge);
            double aAreaL1 = aArea1 / (rotationPeriodL1 - slAge_d);
            Cohort tmp, tmp0;
            tmp.area = aAreaL1 * timeStep;
            tmp0.area = aArea * timeStep;
            for (size_t i = 0; i < ageClassesL0; ++i) {
                double age = static_cast<double>(i) * timeStep;
                double tmp_arg = (1 - slShare) * avgMai;
                tmp0.bm = it->getBm(age, tmp_arg);
                tmp0.d = it->getDbh(age, tmp_arg);
                tmp0.h = it->getHeight(age, tmp_arg);
                dat0[i] = tmp0;
            }

            for (size_t i = 0; i < ageClassesL1; ++i) {
                double age = static_cast<double>(i) * timeStep;
                double tmp_arg = slShare * sd;
                if (age >= slAge_d) {
                    tmp.bm = it->getBmSdNat(age, avgMai, tmp_arg);
                    tmp.d = it->getDbhSdNat(age, avgMai, tmp_arg);
                    tmp.h = it->getHeight(age, avgMai);
                    tmp.area = aAreaL1 * timeStep;
                } else
                    tmp.reset();
                dat[i] = tmp;
            }
            dat[0].area *= 0.5;
            dat0[0].area *= 0.5;

            double tmp_arg = rotationPeriod / timeStep;
            double shareL0 = 0.5 + tmp_arg - lround(tmp_arg);
            tmp_arg = rotationPeriodL1 / timeStep;
            double shareL1 = 0.5 + tmp_arg - lround(tmp_arg);

            if (shareL0 > 0)
                dat0[ageClassesL0 - 1].area = aArea * timeStep * shareL0;
            for (auto &dat0_i: dat)
                dat0_i.area = 0;
            for (auto &dat_i: dat)
                dat_i.area = 0;

            setShareSL(max(0., getAreaSL(1) / getAreaSL(0) * sd));
        }

        void createNormalForestSLFalse(double rotationPeriod, double aArea, double sd) {
            aArea = max(0., aArea);
            area = aArea;
            rotationPeriod = clamp(rotationPeriod, 1., it->getTMax() - 1);
            size_t ageClasses = ceil(0.5 + rotationPeriod / timeStep);
            dat.resize(max(dat.size(), ageClasses));
            maxNumberOfAgeClasses = max(maxNumberOfAgeClasses, ageClasses);
            aArea /= rotationPeriod;
            Cohort tmp;
            tmp.area = aArea * timeStep;

            for (size_t i = 0; i < ageClasses; ++i) {
                double age = static_cast<double>(i) * timeStep;
                if (sd > 0) {
                    if (sd == 1) {
                        tmp.bm = it->getBmT(age, avgMai);
                        tmp.d = it->getDbhT(age, avgMai);
                    } else {
                        tmp.bm = it->getBmSdTab(age, avgMai, sd);
                        tmp.d = it->getDbhSdTab(age, avgMai, sd);
                    }
                } else {
                    if (sd == -1) {
                        tmp.bm = it->getBm(age, avgMai);
                        tmp.d = it->getDbh(age, avgMai);
                    } else {
                        tmp.bm = it->getBmSdNat(age, avgMai, -sd);
                        tmp.d = it->getDbhSdNat(age, avgMai, -sd);
                    }
                }
                tmp.h = it->getHeight(age, avgMai);
                dat[i] = tmp;
            }
            dat[0].area *= 0.5;
            double tmp_arg = rotationPeriod / timeStep;
            double share = 0.5 + tmp_arg - lround(tmp_arg);
            if (share > 0)
                dat[ageClasses - 1].area = aArea * timeStep * share;
            for (auto &dat_i: dat)
                dat_i.area = 0;
        }

        // area is a non-negative number, return the deforested biomass
        V deforestSLTrue(double aArea) {
            if (area <= 0)
                return {};
            aArea = min(aArea, area);
            V ret;
            ret.area = aArea;
            double mul = 1 - aArea / area;
            array<double, 2> dbhBm{}; // Key of dbh and biomass

            for (size_t i = 0; i < dat.size(); ++i)
                if (dat[i].area > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    dbhBm[0] = max(0., dat[i].d + id);
                    dbhBm[1] = max(0., dat[i].bm + dbm);
                    double totalWood = dat[i].area * (1 - mul) * dbhBm[1];
                    ret.bm += totalWood;
                    double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                    double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                    ret.sw += sawnWood;
                    ret.rw += harvestedWood - sawnWood;
                    ret.co += totalWood * coe->ip(dbhBm);
                    area -= dat[i].area * (1 - mul);
                    dat[i].area *= mul;
                }

            double tmp_arg = (1 - slShare) * avgMai;

            for (size_t i = 0; i < dat0.size(); ++i)
                if (dat0[i].area > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    double sdNat = it->getSdNat(age, tmp_arg, dat0[i].bm);
                    double dbm = it->getIncBmSdNat(age, tmp_arg, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, tmp_arg, sdNat) * 0.5;
                    dbhBm[0] = max(0., dat0[i].d + id);
                    dbhBm[1] = max(0., dat0[i].bm + dbm);
                    double totalWood = dat0[i].area * (1 - mul) * dbhBm[1];
                    ret.bm += totalWood;
                    double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                    double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                    ret.sw += sawnWood;
                    ret.rw += harvestedWood - sawnWood;
                    ret.co += totalWood * coe->ip(dbhBm);
                    dat0[i].area *= mul;
                }

            if (ret.area > 0) {  // Values per hectare
                ret.sw /= ret.area;
                ret.rw /= ret.area;
                ret.co /= ret.area;
                ret.bm /= ret.area;
            } // MG: now per ha of deforested land
            return ret;
        }

        // type: 0. Take from all age classes, 1. Take from the eldest age classes
        // area is a non-negative number, return the deforested biomass
        V deforestSLFalse(double aArea, const int type) {
            if (area <= 0)
                return {};

            switch (type) {
                case 0: { // Clearcut FM
                    // Take from all age classes
                    aArea = min(aArea, area);
                    V ret;
                    ret.area = aArea;
                    double mul = 1 - aArea / area;
                    array<double, 2> dbhBm{};  // Key of dbh and biomass

                    for (size_t i = 0; i < dat.size(); ++i)
                        if (dat[i].area > 0) {
                            double age = static_cast<double>(i) * timeStep;
                            double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                            double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                            double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                            dbhBm[0] = max(0., dat[i].d + id);
                            dbhBm[1] = max(0., dat[i].bm + dbm);
                            double totalWood = dat[i].area * (1 - mul) * dbhBm[1];
                            ret.bm += totalWood;
                            double harvestedWood = totalWood * fc->getHle().ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            ret.sw += sawnWood;
                            ret.rw += harvestedWood - sawnWood;
                            ret.co += totalWood * coe->ip(dbhBm);
                            area -= dat[i].area * (1 - mul);
                            dat[i].area *= mul;
                        }

                    if (ret.area > 0) { // Values per hectare
                        ret.sw /= ret.area;
                        ret.rw /= ret.area;
                        ret.co /= ret.area;
                        ret.bm /= ret.area;
                    } // MG: now per ha of deforested land
                    return ret;
                }
                case 1:  // Take from the old age classes
                    return finalCut(aArea, false);
                default:
                    ERROR("type = {} is not implemented", type);
                    return {};
            }
        }

        void postponeShelter2CutPosI(const int i) {
            int maxBmAge = ceil(it->getTOptT(mai, 1));

            for (auto &swt: timerSW)
                if (swt.age + i < maxBmAge && swt.timer == 0) {
                    swt.timer = i;
                    if (swt.age + i < maxNumberOfAgeClasses) {  // Add one more age class
                        if (swt.age + i >= dat.size()) {
                            DEBUG("postponeShelter2cut: swt.age + i > dat.size() - 1, dat.size() has been changed from {} to {} maxBmAge= {}",
                                  dat.size(), swt.age + i + 1, maxBmAge);
                            size_t oldSize = dat.size();
                            dat.resize(swt.age + i + 1);
                            initCohort(oldSize, swt.age + i + 1);
                        }
                    } else {
                        DEBUG("postponeShelter2cut: swt.age + i >= maxNumberOfAgeClasses, maxNumberOfAgeClasses has been changed from {} to {} maxBmAge= {}",
                              dat.size(), swt.age + i + 1, maxBmAge);
                        maxNumberOfAgeClasses = swt.age + i + 1;
                        size_t oldSize = dat.size();
                        dat.resize(swt.age + i + 1);
                        initCohort(oldSize, swt.age + i + 1);

                    }
                }
        }

        void postponeShelter2CutNegI(const int i) {
            double maxMaiAge = 0;
            double sd = midpoint(sdMin, sdMax);
            if (sd > 0)
                maxMaiAge = sd == 1 ? it->getTOptT(avgMai, 0) : it->getTOptSdTab(avgMai, sd, 0);
            else
                maxMaiAge = sd == -1 ? it->getTOpt(avgMai, 0) : it->getTOptSdNat(avgMai, abs(sd), 0);
            for (auto &swt: timerSW)
                if (swt.age > u + 0.8 * 0.4 * maxMaiAge && swt.timer == 1)
                    swt.timer = 0;
        }
    };
}

#endif
