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

#include "../misc/abstract/iipol.hpp"
#include "../misc/abstract/iipolm.hpp"
#include "../log.hpp"
#include "FM_result.hpp"
#include "cohort_res.hpp"
#include "salvage_logging_res.hpp"
#include "age_class.hpp"
#include "canopy_layer.hpp"
#include "shelter_wood_timer.hpp"
#include "increment_tab.hpp"
#include "../settings/constants.hpp"
#include "../parameters/decisions.hpp"

using namespace std;
namespace rv = ranges::views;
using namespace g4m::misc::concrete;
using namespace g4m::init;
using namespace g4m::parameters;

namespace g4m::increment {
    // simulator of forest growth, development, and forest management
    // this class doesn't own IIpols, they are created on stack and freed automatically, here are just pointers!
    class AgeStruct {
    public:
        AgeStruct(
                // Increment table which will be used, the time step width (simulation period length) of *it will also be used in ageStruct
                const IncrementTab *const it_,
                // Sawn-wood share of harvested wood depending on dbh
                const IIpol<double> *const sws_,
                // 1-harvesting losses thinning (Vornutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
                const IIpol<double> *const hlv_,
                // 1-harvesting losses final felling (Endnutzung) (depending on d) in relation to standing timber (Vorratsfestmeter)
                const IIpol<double> *const hle_,
                // Thinning costs depending on d and removed volume per hectare in relation to standing timber (Vorratsfestmeter)
                const IIpolM<double> *const cov_,
                // Harvesting costs depending on d and vol
                const IIpolM<double> *const coe_,
                // do thinning (depending on d and removed volume per hectare) and final felling
                // (depending on d and stocking volume per hectare) in relation to standing timber (Vorratsfestmeter)
                const Decisions *const decisions_,
                // mean annual increment in tC stem-wood per hectare and year at increment optimal rotation time
                double mai_,
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
                int objOfProd_ = 3,
                // Rotation time if objOfProd 0
                double u_ = 0,
                // if objOfProd 1, 2 amount of sawn-wood to harvest
                double minSw_ = 0,
                // if objOfProd 1, 2 amount of rest-wood to harvest
                double minRw_ = 0,
                // if objOfProd 1, 2 amount of total harvest
                double minHarv_ = 0,
                // Usage of stocking degree:
                // 0. Keep all the time sdMax,
                // 1. alternate between sdMin and sdMax,
                // 3. alternate between sdMinH and sdMaxH
                int sdDef_ = 0,
                // Stocking degree: if sd exceeds sdMax do thinning until sdMin.
                // d > 0 stockingDegree yield table,sd -1 to 0 natural stocking degree
                // Maybe sdMin and sdMax can be made dependent on h/hmax and MAI
                double sdMax_ = 1,
                double sdMin_ = 1,
                // Years to calculate average mai
                size_t maiYears_ = 30,
                // Minimal rotation time in years or as share given in minRotRef which needs to be exceeded until final harvests are done
                double minRotVal_ = 0.75,
                // meaning of minRotVal value
                // 0. use it as years,
                // 1. minRotVal * u (u > 0),
                // 2. * uMaxAvgIncrement,
                // 3. * uMaxAvgBiomass,
                // 4. * uMaxAge,
                // 5. * uMaxHarvest,
                // 6. * uAvgMaxHarvest
                int minRotRef_ = 2,
                // how fast should the stoking degree target be reached
                // 0. do only remove caused by stand density  to  1. do only typical removes
                double flexSd_ = 0,
                uint32_t maxAge_ = 300
                // , FFIpol<double> *sdMaxH  // Stocking degree depending on max tree height
                // , FFIpol<double> *sdMinH  // Stocking degree depending on min tree height
        ) : it{it_},
            sws{sws_},
            hlv{hlv_},
            hle{hle_},
            cov{cov_},
            coe{coe_},
            decisions{decisions_},
            mai{mai_},
            uRef{u_},
            objOfProd{objOfProd_},
            minSw{minSw_},
            minRw{minRw_},
            minHarv{minHarv_},
            sdDef{sdDef_},
            sdMin{sdMin_},
            sdMax{sdMax_},
            minRotVal{minRotVal_},
            minRotRef{minRotRef_},
            flexSd{flexSd_},
            canopyLayer{it, sws, hlv, hle, cov, coe, decisions} {
            avgMai = mai;
            qMai.assign(maiYears_, mai);
            setRotationTime();
            setMinRot();
            timeStep = it->getTimeframe();  // Check if this should be tStep or timeframe
        }

        void createNormalForest(const double rotationPeriod, const double area_, const double sd = 1) {
            canopyLayer.createNormalForest(rotationPeriod, area_, sd, avgMai);
            area = area_;
        }

        // MG: Clear vector<Cohort> dat and create normal forest with new parameters
        void recreateNormalForest(const double rotationPeriod, const double area_, const double sd = 1) {
            canopyLayer = CanopyLayer{it, sws, hlv, hle, cov, coe, decisions};
            canopyLayer.createNormalForest(rotationPeriod, area_, sd, avgMai);
            area = area_;
        }

        // get biomass per ha by age
        [[nodiscard]] inline double getBm(double age) const noexcept {
            return canopyLayer.getBiomassByAge(age);
        }

        // get average biomass per ha
        [[nodiscard]] inline double getBm() const noexcept {
            return canopyLayer.getAverageBiomass();
        }

        // MG 09052022: get standing stem biomass with a diameter greater than dbh0 cm
        [[nodiscard]] double getBmGDbh(const double dbh0) const noexcept {
            return canopyLayer.getBmGDbh(dbh0);
        }

        // get average biomass per ha for age classes with Bm > 0 (including 0 age class)
        [[nodiscard]] inline double getBmNonZero() const noexcept {
            return canopyLayer.getBmNonZero();
        }

        // get forest area by age (consider age class size)
        [[nodiscard]] inline double getArea(const size_t age) const noexcept {
            return canopyLayer.getDat()[age].area;
        }

        // get forest area by age (consider age class size)
        [[nodiscard]] inline double getArea(const double age) const noexcept {
            return canopyLayer.getAreaByAge(age);
        }

        // get forest area
        [[nodiscard]] inline double getArea() const noexcept {
            return area;
        }

        // get forest area where Bm > 0, including the first age class
        [[nodiscard]] inline double getAreaNonZero() const noexcept {
            return canopyLayer.getAreaNonZero();
        }

        // get Diameter
        [[nodiscard]] inline double getD(double age) const noexcept {
            return canopyLayer.getDiameterByAge(age);
        }

        // get Height / canopyLayer:
        // 1 - first canopy layer,
        // 0 - second (ground) canopy layer that is active if selective logging option is used
        [[nodiscard]] inline double getH(double age) const noexcept {
            return canopyLayer.getHeightByAge(age);
        }

        // Set area for a specific ageCLASS for specified canopy layer
        void setArea(size_t ageClass, double area_) {
            canopyLayer.setAreaToAgeClassIdx(ageClass, area_);
            area += area_; // MG: can be a problem here as area must be the same for L0 and L1
        }

        // Set biomass per hectare for a specific ageCLASS for specified canopy layer
        inline void setBm(size_t ageClass, double biomass) {
            canopyLayer.setBiomassToAgeClassIdx(ageClass, biomass, avgMai);
        }

        // Set dbh for a specific ageClass
        inline void setD(size_t ageClass, double dbh) {
            canopyLayer.setDiameterToAgeClassIdx(ageClass, dbh, avgMai);
        }

        // Just set mai but don't influence avgMai
        inline void setMai(const double mai_) noexcept {
            mai = mai_;
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

        void setMaiAndAvgMai(const double mai_) {
            setMai(mai_);
            setAvgMai(mai_);
        }

        [[nodiscard]] inline double getMai() const noexcept {
            return mai;
        }

        [[nodiscard]] inline double getAvgMai() const noexcept {
            return avgMai;
        }

        inline void setObjOfProd(const int aObjOfProd) noexcept {
            objOfProd = aObjOfProd;
        }

        inline void setU(const double aU) {
            uRef = aU;
            setRotationTime();
            setMinRot();
        }

        [[nodiscard]] inline double getURef() const noexcept {
            return uRef;
        }

        inline void setStockingDegreeMin(const double sd) noexcept {
            sdMin = sd;
        }

        inline void setStockingDegreeMax(const double sd) noexcept {
            sdMax = sd;
        }

        // min and max
        void setStockingDegree(const double sd) noexcept {
            sdMin = sd;
            sdMax = sd;
        }

        inline void setMinRotRef(const int aMinRotRef) noexcept {
            minRotRef = aMinRotRef;
        }

        void setMinRotVal(const double minRotVal_) {
            minRotVal = minRotVal_;
            setMinRot();
        }

        inline void setFlexSd(const double flexSd_) noexcept {
            flexSd = flexSd_;
        }

        // Insert in the youngest age class
        void afforest(const double area_) noexcept {
            canopyLayer.afforest(area_);
            area += area_;
        }

        // Make afforestation in age class 0 and 1
        // MG: correct problem with 0 biomass in 1st age class
        // Proposed by Georg 21 August 2015
        // Make fake afforestation in age class 0; new species are planted in the new forest instead
        void reforest(const double area_) noexcept {
            canopyLayer.reforest(area_, 1, mai);
            area += area_;
        }

        void changeSpecies(const double area_) noexcept {
            canopyLayer.changeSpecies(area_);
            area += area_;
        }

        // take from all age classes
        // area is a non-negative number, return the deforested biomass
        [[nodiscard]] FMResult deforest(double area_) {
            FMResult ret = canopyLayer.deforest(area_, avgMai);
            area = canopyLayer.getTotalArea();
            return ret;
        }

        // take from the eldest age classes
        // area is a non-negative number, return the deforested biomass
        [[nodiscard]] FMResult deforestOldest(double area_) {
            return finalCutAreaWrapper(area_, false, true);
        }

        // .first = thinning, .second = harvest
        // MG: reforestation with harvested area but zero biomass; real reforestation occurs in the new forest in the same cell
        [[nodiscard]] inline CohortRes aging() {
            return agingDetailed(mai, false);
        }

        [[nodiscard]] inline double getMaxAge() const noexcept {
            return canopyLayer.getMaxAge();
        }

        [[nodiscard]] inline size_t getDatSize() const noexcept {
            return canopyLayer.getCurrentNumberOfAgeClasses();
        }

        inline void setMaxAge(const double maxAge) noexcept {
            canopyLayer.setMaxAge(maxAge);
        }

        // MG: added : Find "active Age" - the oldest age class with area > 0
        [[nodiscard]] inline double getActiveAge() const noexcept {
            return canopyLayer.getActiveAge();
        }

        // find the oldest age class index with area > 0
        [[nodiscard]] inline size_t getActiveAgeClassIdx() const noexcept {
            return canopyLayer.getActiveAgeClassIdx();
        }

        // MG: get rotation time
        [[nodiscard]] inline double getU() const noexcept {
            return u;
        }

        // MG: get stocking degree ([StockingDegreeMin + StockingDegreeMax] / 2)
        [[nodiscard]] inline double getStockingDegree() const noexcept {
            return midpoint(sdMin, sdMax);
        }

        // MG: get age structure
        [[nodiscard]] inline const CanopyLayer &getCanopyLayer() const noexcept {
            return canopyLayer;
        }

        // MG: set age structure
        void setCanopyLayer(const CanopyLayer &other) noexcept {
            canopyLayer = other;
        }

        // MG: get area of shelters that should be cut in current year and age of which is less than Max biomass
        [[nodiscard]] double getShelter2CutArea() const noexcept {
            if (timerSW.empty()) {
                ERROR("timerSW is empty");
                return 0;
            }
            double sd = midpoint(sdMin, sdMax);
            const IncrementTab::TableMode mode = IncrementTab::getTableModeBySD(sd);
            double maxBmAge_d = it->getTOpt(mode, avgMai, OptRotTimes::Mode::MaxBm, abs(sd));
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
            if (i > 0) {
                int maxBmAge = ceil(it->getTOptT(mai, OptRotTimes::Mode::MaxBm));
                for (auto &swt: timerSW)
                    if (swt.age + i < maxBmAge && swt.timer == 0) {
                        swt.timer = i;
                        if (swt.age + i < canopyLayer.getCurrentMaxNumberOfAgeClasses()) {  // add one more age class
                            if (swt.age + i >= canopyLayer.getCurrentNumberOfAgeClasses())
                                DEBUG("postponeShelter2cut: swt.age + i > dat.size() - 1, dat.size() has been changed from {} to {} maxBmAge= {}",
                                      canopyLayer.getCurrentNumberOfAgeClasses(), swt.age + i + 1, maxBmAge);
                        } else {
                            DEBUG("postponeShelter2cut: swt.age + i >= maxNumberOfAgeClasses, maxNumberOfAgeClasses has been changed from {} to {} maxBmAge= {}",
                                  canopyLayer.getCurrentNumberOfAgeClasses(), swt.age + i + 1, maxBmAge);
                            canopyLayer.setMaxAge((swt.age + i + 1) * timeStep);
                        }
                        canopyLayer.increaseSize(swt.age + i + 1, midpoint(sdMin, sdMax), avgMai);
                    }
            } else if (i < 0) {
                const double sd = midpoint(sdMin, sdMax);
                const IncrementTab::TableMode mode = IncrementTab::getTableModeBySD(sd);
                double maxMaiAge = it->getTOpt(mode, avgMai, OptRotTimes::Mode::MAI, abs(sd));
                for (auto &swt: timerSW)
                    if (swt.age > u + 0.8 * 0.4 * maxMaiAge && swt.timer == 1)
                        swt.timer = 0;
            }
        }

        // MG: Clean area of those age classes where Bm<=0; possible problem with low MAI when a few beginning age classes have 0 biomass
        void rectifyArea() noexcept {
            canopyLayer.rectifyArea();
            area = canopyLayer.getTotalArea();
        }

        // disturbance damaged wood and wood harvested from the slash
        [[nodiscard]] FMResult
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
            FMResult ret;
            auto endYear = static_cast<size_t>(static_cast<double>(targetAge) / timeStep);

            array<double, 3> dbhHBm{};  // array including current year Dbh, H and Bm
            array<double, 2> dbhBm{};   // dbhBm

            double fcDBHTmp = 0; // DBH of harvested trees, average weighted by clear-cut area and harvested biomass
            double fcHTmp = 0; // height of harvested trees, average weighted by clear-cut area and harvested biomass
            double hWoodArea = 0; // clear-cut area X harvested biomass

            const vector<AgeClass> &dat = canopyLayer.getDat();
            // ssize() to avoid underflow
            for (ptrdiff_t i = ssize(dat) - 1; i > endYear && ret.biomass < targetDamaged; --i)
                if (dat[i].area > 0 && dat[i].bm > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    //  The Stands get half increment of the next growing period
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    double ih = it->getIncHeight(age, avgMai) * 0.5;

                    // MG: we don't allow negative biomass in any age group
                    dbhHBm[0] = dat[i].d + id;
                    dbhHBm[1] = dat[i].h + ih;
                    dbhHBm[2] = dat[i].bm + dbm;
                    dbhBm[0] = dbhHBm[0];
                    dbhBm[1] = dbhHBm[2];

                    if (dbhHBm[0] > targetDbh && dbhHBm[1] > targetHeight) {  // given amount of damage
                        double harvestShare = 1;
                        double totalWood = dat[i].area * dbhHBm[2];
                        double harvestedWood = totalWood * harvestableShare * hle->ip(dbhHBm[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhHBm[0]);
                        double hArea = 0;  // clear-cut area in current age class

                        if (ret.biomass + totalWood < targetDamaged) {  // Take all age class
                            ret.area += dat[i].area;
                            area -= dat[i].area;
                            hArea = dat[i].area;
                            canopyLayer.setAreaToAgeClassIdx(i, 0);
                        } else {  // Take a share of age class
                            harvestShare = 0;
                            double tmp_arg = targetDamaged - ret.biomass;
                            if (tmp_arg > 0 && totalWood > 0) {
                                tmp_arg /= totalWood;
                                harvestShare = max(harvestShare, tmp_arg);
                            }
                            harvestShare = clamp(harvestShare, 0., 1.);
                            tmp_arg = dat[i].area * harvestShare;
                            ret.area += tmp_arg;
                            area -= tmp_arg;
                            hArea = tmp_arg;
                            canopyLayer.setAreaToAgeClassIdx(i, dat[i].area * (1 - harvestShare));
                        }

                        double tmp_arg = totalWood * hArea;
                        fcDBHTmp += dat[i].d * tmp_arg;
                        fcHTmp += dat[i].h * tmp_arg;
                        hWoodArea += tmp_arg;

                        // total amount of damaged stem-wood wood including the burnt biomass and potential harvest losses
                        ret.biomass += totalWood * harvestShare;
                        ret.sawnWood += sawnWood * harvestShare;  // harvested sawn wood from the slash
                        // harvested rest-wood from the slash
                        ret.restWood += (harvestedWood - sawnWood) * harvestShare;
                        // harvesting costs - costs for normal harvest used, not well estimated yet
                        ret.harvestCosts += totalWood * coe->ip(dbhBm) * harvestShare;
                    }
                }

            if (hWoodArea > 0) {
                ret.DBH = fcDBHTmp / hWoodArea;
                ret.H = fcHTmp / hWoodArea;
            } else {
                ret.DBH = 0;
                ret.H = 0;
            }
            if (ret.area > 0) {  // Values per hectare
                ret.sawnWood /= ret.area;
                ret.restWood /= ret.area;
                ret.harvestCosts /= ret.area;
                ret.biomass /= ret.area;
            }
            reforest(ret.area); // we reforest the damaged forest here
            // area_disturbDamaged += ret.area;
            // reforest(ret.area, false); // replant the damaged and logged stands
            return ret;
        }

        // returns harvestW, bmH, bmTh, performs aging and doesn't modify the cohort
        [[nodiscard]] CohortRes cohortRes() const {
            return AgeStruct{*this}.aging();
        }

        // Create copy and set U inline for the sake of encapsulation
        [[nodiscard]] AgeStruct createSetU(const double u_) const {
            auto cohortTmp{*this};
            cohortTmp.setU(u_);
            return cohortTmp;
        }

        // returns salvage logging after disturbances: harvest, bmH, damagedFire, harvArea
        [[nodiscard]] SalvageLoggingRes
        salvageLoggingAllAgents(const double damagedWindForced, const double damagedFireForced,
                                const double damagedBioticForced, const double harvestableWind,
                                const double harvestableFire, const double harvestableBiotic) {
            if (area <= 0)
                return {};

            FMResult wind, fire, biotic;

            if (damagedWindForced > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                wind = disturbanceDamage(damagedWindForced, 30, 15, 0, harvestableWind);
            if (damagedFireForced > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                fire = disturbanceDamage(damagedFireForced, 30, 0, 0, harvestableFire);
            if (damagedBioticForced > 0)  // disturbance damaged wood and wood harvested from the damaged trees
                biotic = disturbanceDamage(damagedBioticForced, 0, 0, 15, harvestableBiotic);

            return {area, wind, fire, biotic};
        }

        [[nodiscard]] inline const vector<AgeClass> &getDat() const noexcept {
            return canopyLayer.getDat();
        }

    private:
        // non-owned pointers
        const IncrementTab *it = nullptr;
        const IIpol<double> *sws = nullptr;
        const IIpol<double> *hlv = nullptr;
        const IIpol<double> *hle = nullptr;
        const IIpolM<double> *cov = nullptr;
        const IIpolM<double> *coe = nullptr;
        const Decisions *decisions = nullptr;

        double mai = 0;
        double avgMai = 0;
        int objOfProd = 0;              // Objective of production
        double minSw = 0;               // Min sawn-wood to harvest
        double minRw = 0;               // Min rest-wood to harvest
        double minHarv = 0;             // Minimum total harvest

        deque<double> qMai;             // Queue to store the mai's of previous years (youngest mai is at the end of queue)
        double u = 0;                   // Rotation time

        vector<ShelterWoodTimer> timerSW;

        double uRef = 0;
        int sdDef = 0;
        double sdMin = 0, sdMax = 0;    // Target stocking degree (> 0 Table, < 0 natural)
        double area = 0;                // Total forest area (sum of dat.area)
        double minRot = 0;              // minimal age when final harvest will be done
        double minRotVal = 0;
        int minRotRef = 0;
        double timeStep = 0;            // How long is one time step
        double flexSd = 0;

        CanopyLayer canopyLayer;

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
                    auto type = static_cast<OptRotTimes::Mode>(objOfProd - 3);
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
        void setMinRot() {
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
                    auto type = static_cast<OptRotTimes::Mode>(minRotRef - 2);
                    const double sd = midpoint(sdMin, sdMax);
                    const IncrementTab::TableMode mode = IncrementTab::getTableModeBySD(sd);
                    minRot = minRotVal * it->getTOpt(mode, avgMai, type, abs(sd));
                    break;
                }
                default:
                    ERROR("minRotRef = {} is not implemented", minRotRef);
                    minRot = -1;
            }
        }

        // .first = thinning, .second = harvest
        // MG: reforestation with harvested area but zero biomass; real reforestation occurs in the new forest in the same cell
        [[nodiscard]] CohortRes agingDetailed(const double mai_, const bool shelterWood) {
            FMResult retThin, retHarvest;
            mai = mai_;
            qMai.pop_front();
            qMai.push_back(mai);
            calcAvgMai();
            setRotationTime();
            setMinRot();
            if (u == 0) {
                FATAL("agingDetailed: rotation time is 0!");
                throw runtime_error{"agingDetailed: rotation time is 0!"};
            }
            if (objOfProd == 1 || objOfProd == 2) { // fulfill an amount of harvest
                retThin = thinAndGrowStaticWrapper();
                const bool sustainable = objOfProd == 2;
                retHarvest = finalCutAmountWrapper(minSw - retThin.sawnWood, minRw - retThin.restWood,
                                                   minHarv - retThin.getWood(), true, sustainable);
            } else if (!shelterWood) { // we have a rotation time to fulfill
                retHarvest = finalCutAreaWrapper(area * timeStep / u, true, true); // do final cut
                retThin = thinAndGrowStaticWrapper();
            } else if (shelterWood) { //We have a rotation time to fulfill
                retHarvest = finalCutShelterWood(area * timeStep / u, -1, -1, -1, true, true); // do final cut
                retThin = thinAndGrowShelterWood();
            }
            reforest(retHarvest.area);
            return {area, retThin, retHarvest};
        }

        [[nodiscard]] FMResult finalCutShelterWood(const double area_, const double minSw_, const double minRw_,
                                                   const double minHarv_, const bool eco, const bool sustainable) {
            FMResult ret;
            int timeTo2Cut = 20; // Time, years, after which the shelter will be cut
            // int timeTo2Cut = ceil(0.4 * it->getTOptT(mai, 0)); // Expert suggestion by Fulvio Di Fulvio, 07.08.2019.
            int maxAge = ceil(it->getTOptT(mai, OptRotTimes::Mode::MaxBm));
            double areaShelterCut_total = 0;
            size_t endYear = eco || sustainable ? static_cast<size_t>(minRot / timeStep) : 0;
            array<double, 2> dbhBm{}; // key to ask if harvest is economic
            const vector<AgeClass> &dat = canopyLayer.getDat();
            for (ptrdiff_t i = ssize(dat) - 1; i >= endYear && (ret.area < area_ * 0.5 || ret.sawnWood < minSw_ ||
                                                                ret.restWood < minRw_ || ret.getWood() < minHarv_); --i)
                if (dat[i].area > 0 && dat[i].bm > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    //The Stands get half increment of the next growing period
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    // MG: we don't allow negative biomass in any age group
                    dbhBm[0] = max(0., dat[i].d + id);
                    dbhBm[1] = max(0., dat[i].bm + dbm);
                    if (decisions->DOE(dbhBm[0], dbhBm[1]) || !eco) {  // do harvest if it is economic
                        if (area_ >= 0) {  // Given area to harvest
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
                                // https://stackoverflow.harvestCostsm/questions/16609041/c-stl-what-does-base-do
                            }

                            double area_tmp = 0;
                            if (ret.area + dat[i].area - areaShelter < area_) {  // Harvest all of this age class
                                area_tmp = (dat[i].area - areaShelter) * 0.5;
                                canopyLayer.setAreaToAgeClassIdx(i, area_tmp + areaShelter - areaShelterCut);
                            } else {
                                area_tmp = (area_ - ret.area * 2) * 0.5;
                                canopyLayer.setAreaToAgeClassIdx(i, dat[i].area - area_tmp - areaShelterCut);
                            }
                            totalWood = (area_tmp + areaShelterCut) * dbhBm[1];
                            ret.area += area_tmp;
                            area -= area_tmp + areaShelterCut;
                            if (area_tmp > 0)
                                timerSW.emplace_back(area_tmp, static_cast<int>(i), timeTo2Cut);
                            areaShelterCut_total += areaShelterCut;

                            ret.biomass += totalWood;
                            double harvestedWood = totalWood * hle->ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            ret.sawnWood += sawnWood;
                            ret.restWood += harvestedWood - sawnWood;
                            ret.harvestCosts += totalWood * coe->ip(dbhBm);
                        } else {  // given amount of harvest
                            double harvestShare = 1;
                            double totalWood = dat[i].area * dbhBm[1];
                            double harvestedWood = totalWood * hle->ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            if (ret.sawnWood + sawnWood < minSw || ret.restWood + harvestedWood - sawnWood < minRw ||
                                ret.getWood() + harvestedWood < minHarv) {  // Harvest all
                                ret.area += dat[i].area;
                                area -= dat[i].area;
                                canopyLayer.setAreaToAgeClassIdx(i, 0);
                            } else {  // Harvest part of age class
                                harvestShare = 0;
                                double tmp = minSw - ret.sawnWood;
                                if (tmp > 0 && sawnWood > 0) {
                                    tmp /= sawnWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                tmp = minRw - ret.restWood;
                                if (tmp > 0 && harvestedWood > sawnWood) {
                                    tmp /= harvestedWood - sawnWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                tmp = minHarv - (ret.sawnWood + ret.restWood);
                                if (tmp > 0 && harvestedWood > 0) {
                                    tmp /= harvestedWood;
                                    harvestShare = max(tmp, harvestShare);
                                }
                                harvestShare = clamp(harvestShare, 0., 1.);
                                ret.area += dat[i].area * harvestShare;
                                area -= dat[i].area * harvestShare;
                                canopyLayer.setAreaToAgeClassIdx(i, dat[i].area * (1 - harvestShare));
                            }
                            ret.biomass += totalWood * harvestShare;
                            ret.sawnWood += sawnWood * harvestShare;
                            ret.restWood += (harvestedWood - sawnWood) * harvestShare;
                            ret.harvestCosts += totalWood * coe->ip(dbhBm) * harvestShare;
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
                                  age_idx, dat.size(), canopyLayer.getCurrentMaxNumberOfAgeClasses());

                        double sdNat = it->getSdNat(age, avgMai, dat[age_idx].bm);
                        double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                        double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                        // MG: we don't allow negative biomass in any age group
                        dbhBm[0] = dat[age_idx].d + id;
                        dbhBm[1] = dat[age_idx].bm + dbm;
                        if (decisions->DOE(dbhBm[0], dbhBm[1]) || !eco) {  // do harvest if it is economic
                            area2Cut = min(area2Cut, dat[age_idx].area);
                            double totalWood = area2Cut * dbhBm[1];
                            area -= area2Cut;
                            canopyLayer.setAreaToAgeClassIdx(age_idx, dat[age_idx].area - area2Cut);
                            ret.biomass += totalWood;
                            double harvestedWood = totalWood * hle->ip(dbhBm[0]);
                            double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                            ret.sawnWood += sawnWood;
                            ret.restWood += harvestedWood - sawnWood;
                            ret.harvestCosts += totalWood * coe->ip(dbhBm);
                            areaShelterCut_total += area2Cut;
                        }
                    }
                erase_if(timerSW, [](const auto &swt) { return swt.timer == 0; });
            }

            ret.area += areaShelterCut_total;
            if (ret.area > 0) {  // Values per hectare
                ret.sawnWood /= ret.area;
                ret.restWood /= ret.area;
                ret.harvestCosts /= ret.area;
                ret.biomass /= ret.area;
            }
            return ret;
        }

        [[nodiscard]] FMResult thinAndGrowShelterWood() {
            if (!timerSW.empty()) {
                for (auto &swt: timerSW) {
                    ++swt.age;
                    --swt.timer;
                }
                auto maxAgeSW = ranges::max_element(timerSW,
                                                    [](const auto &lhs, const auto &rhs) { return lhs.age < rhs.age; });
                // We always keep one extra age class to allow aging
                if (maxAgeSW->age >= canopyLayer.getCurrentMaxNumberOfAgeClasses() - 1)
                    canopyLayer.setMaxAge(maxAgeSW->age + 1);
                if (maxAgeSW->age >= canopyLayer.getCurrentNumberOfAgeClasses())
                    canopyLayer.increaseSize(maxAgeSW->age + 1, midpoint(sdMin, sdMax), avgMai);
            }
            return thinAndGrowStaticWrapper();
        }

        [[nodiscard]] FMResult thinAndGrowStaticWrapper() {
            return canopyLayer.thinAndGrowStatic(flexSd, sdMin, sdMax, mai, avgMai);
        }

        [[nodiscard]] FMResult finalCutAreaWrapper(const double area_, const bool eco, const bool sustainable) {
            FMResult ret = canopyLayer.finalCutArea(area_, eco, sustainable, minRot, avgMai);
            area = canopyLayer.getTotalArea();
            return ret;
        }

        [[nodiscard]] FMResult
        finalCutAmountWrapper(const double minSw_, const double minRw_, const double minHarv_, const bool eco,
                              const bool sustainable) {
            FMResult ret = canopyLayer.finalCutAmount(minSw_, minRw_, minHarv_, eco, sustainable, minRot, avgMai);
            area = canopyLayer.getTotalArea();
            return ret;
        }
    };
}

#endif
