#ifndef G4M_EUROPE_DG_CANOPY_LAYER_HPP
#define G4M_EUROPE_DG_CANOPY_LAYER_HPP

#include <ranges>
#include <print>

#include "age_class.hpp"
#include "increment_tab.hpp"
#include "FM_result.hpp"
#include "../parameters/decisions.hpp"
#include "../log.hpp"

using namespace std;
namespace rv = ranges::views;
using namespace g4m::parameters;

namespace g4m::increment {
    // CanopyLayer doesn't own IncrementTab!
    class CanopyLayer {
    public:
        // TODO move to tests
        // test area's ||f - sum f_i||
        static void Test_createNormalForest() {
            Species speciesType = Species::Spruce;
            println("speciesType = {}", speciesName.at(speciesType));

            double max_relative_error = 0;
            for (double rotationPeriod = 1; rotationPeriod <= 500; rotationPeriod += 0.5)
                for (double area = 0; area <= 1; area += 0.5)
                    for (double sd = -2; sd <= 2; sd += 0.5)
                        for (double avgMai = 0; avgMai <= 10; avgMai += 0.5) {
                            if (sd == 0)
                                continue;
                            CanopyLayer fl{&species.at(speciesType),
                                           nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
                            fl.createNormalForest(rotationPeriod, area, sd, avgMai);
                            double real_area = fl.getTotalArea();
                            double relative_error = abs(real_area / area - 1);
//                            println("rotationPeriod = {}\tarea = {}\tsd = {}\tavgMai = {}\trelative_error = {}",
//                                    rotationPeriod, area, sd, avgMai, relative_error);
                            max_relative_error = max(max_relative_error, relative_error);
                        }
            println("max_relative_error = {}", max_relative_error);
        }

        CanopyLayer(
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
                const Decisions *const decisions_) : it{it_}, sws{sws_}, hlv{hlv_}, hle{hle_}, cov{cov_}, coe{coe_},
                                                     decisions{decisions_} {
            if (timeStep <= 0)
                throw invalid_argument{"CanopyLayer: timeStep <= 0!"};

            dat.reserve(incrementTableSize);
        }

        // for dat0: sd = -1, avgMai = (1 - slShare) * avgMai
        void
        createNormalForest(const double rotationPeriod_, const double area_, const double sd,
                           const double avgMai) {
            if (!dat.empty()) {
                ERROR("createNormalForest: the forest with size {} was already created", dat.size());
                return;
            }

            const double maxRotationPeriod = it->getTMax() - 1;
            const double rotationPeriod = clamp(rotationPeriod_, 1., maxRotationPeriod);
            const double area = max(0., area_);

            if (rotationPeriod != rotationPeriod_ || area_ != area)
                ERROR("createNormalForest: input parameter correction:\nrotationPeriod: {} -> {}\tarea: {} -> {}",
                      rotationPeriod_, rotationPeriod, area_, area);

            const double adjustedRotationPeriod = rotationPeriod / timeStep;
            const size_t numberOfAgeClasses = ceil(0.5 + adjustedRotationPeriod);
            const double ageClassArea = area / adjustedRotationPeriod;

            currentMaxSize = max(currentMaxSize, numberOfAgeClasses);
            initNewAgeClasses(numberOfAgeClasses, sd, avgMai);

            for (auto &cohort: dat)
                cohort.area = ageClassArea;

            dat.front().area *= 0.5;
            // graph: ./|./|./|; share = 0 if adjustedRotationPeriod = int + 0.5
            double share = 0.5 + adjustedRotationPeriod - static_cast<double>(llround(adjustedRotationPeriod));
            if (share > 0)  // abs(modf(adjustedRotationPeriod, nullptr)) == 0.5
                dat.back().area *= share;
        }

        [[nodiscard]] double getTotalArea() const {
            double totalArea = 0;
            for (const auto &cohort: dat)
                totalArea += cohort.area;
            return totalArea;
        }

        // get average biomass per ha
        [[nodiscard]] double getAverageBiomass() const {
            double totalArea = getTotalArea();
            if (totalArea <= 0)
                return 0;

            double bm = 0;
            for (const auto &cohort: dat)
                bm += cohort.getBiomass_tC();
            return bm / totalArea;
        }

        [[nodiscard]] double getAreaByAge(const double age_) const {
            const double age = max(0., age_);

            if (age != age_)
                ERROR("getAreaByAge: input parameter correction:\nage: {} -> {}", age_, age);

            const double adjustedAge = age / timeStep;
            const size_t ageH = ceil(adjustedAge);
            return ageH < dat.size() ? dat[ageH].area / timeStep : 0;
        }

        [[nodiscard]] double getBiomassByAge(const double age_) const {
            return getInterpolatedCohortValueByAge(age_, AgeClass::Mode::Bm);
        }

        [[nodiscard]] double getDiameterByAge(const double age_) const {
            return getInterpolatedCohortValueByAge(age_, AgeClass::Mode::D);
        }

        [[nodiscard]] double getHeightByAge(const double age_) const {
            return getInterpolatedCohortValueByAge(age_, AgeClass::Mode::H);
        }

        void
        setAreaToAgeClassIdx(const size_t idx, const double area_) {
            if (idx >= dat.size()) [[unlikely]] {
                ERROR("setAreaToAgeClassIdx: idx ({}) >= dat.size() ({})!", idx, dat.size());
                return;
            }
            const double area = max(0., area_);
            if (area != area_)
                ERROR("setAreaToAgeClassIdx: input parameter correction:\nidx: {} -> {}", area_, area);

            dat[idx].area = area;
        }

        // for dat0: sd = -1, avgMai = (1 - slShare) * avgMai, for dat: sd = slShare * sd
        void setBiomassToAgeClassIdx(const size_t idx, const double biomass_, const double avgMai) {
            if (idx >= dat.size()) [[unlikely]] {
                ERROR("setBiomassToAgeClassIdx: idx ({}) >= dat.size() ({})!", idx, dat.size());
                return;
            }
            const double age = static_cast<double>(idx) * timeStep;
            const double maxBiomass = it->getBm(age, avgMai) * 1.2;
            const double biomass = clamp(biomass_, 0., maxBiomass);

            if (biomass != biomass_)
                ERROR("setBiomassToAgeClassIdx: input parameter correction:\nbiomass: {} -> {}", biomass_, biomass);

            dat[idx].bm = biomass;
        }

        void setDiameterToAgeClassIdx(const size_t idx, const double dbh_, const double avgMai) {
            if (idx >= dat.size()) [[unlikely]] {
                ERROR("setDiameterToAgeClassIdx: idx ({}) >= dat.size() ({})!", idx, dat.size());
                return;
            }
            const double age = static_cast<double>(idx + 1) * timeStep;
            const double minD = it->getDbh(age, avgMai);
            const double maxD = it->getDbhSdNat(age, avgMai, 0);
            const double dbh = clamp(dbh_, minD, maxD);

            if (dbh != dbh_)
                ERROR("setDiameterToAgeClassIdx: input parameter correction:\ndbh: {} -> {}", dbh_, dbh);

            dat[idx].d = dbh;
        }

        void setHeightToAgeClassIdx(const size_t idx, const double height_) {
            if (idx >= dat.size()) [[unlikely]] {
                ERROR("setHeightToAgeClassIdx: idx ({}) >= dat.size() ({})!", idx, dat.size());
                return;
            }
            const double height = clamp(height_, 0., 50.);

            if (height != height_)
                ERROR("setDiameterToAgeClassIdx: input parameter correction:\ndbh: {} -> {}", height_, height);

            dat[idx].h = height;
        }

        void increaseSize(const size_t newSize, const double sd, const double avgMai) {
            if (dat.empty()) {
                ERROR("setCohortValueByAgeClassIdx: forest was not created!");
                return;
            }
            if (newSize > dat.size())
                initNewAgeClasses(newSize - dat.size(), sd, avgMai);
        }

        [[nodiscard]] size_t getCurrentNumberOfAgeClasses() const {
            return dat.size();
        }

        [[nodiscard]] size_t getCurrentMaxNumberOfAgeClasses() const {
            return currentMaxSize;
        }

        [[nodiscard]] size_t getMaxPossibleNumberOfAgeClasses() const {
            return incrementTableSize;
        }

        [[nodiscard]] double getMaxAge() const {
            return static_cast<double>(currentMaxSize) * timeStep;
        }

        void setMaxAge(const double maxAge_) {
            const double maxAge = clamp(maxAge_, 0., it->getTMax() - 1);
            if (maxAge != maxAge_)
                ERROR("setMaxAge: input parameter correction:\nmaxAge: {} -> {}", maxAge_, maxAge);

            const size_t newMaxSize_ = ceil(maxAge / timeStep);
            const size_t newMaxSize = clamp(newMaxSize_, dat.size(), incrementTableSize);
            if (newMaxSize != newMaxSize_)
                ERROR("setMaxAge: input parameter correction:\nnewMaxSize: {} -> {}", newMaxSize_, newMaxSize);

            currentMaxSize = newMaxSize;
        }

        void afforest(const double area_) {
            if (dat.empty()) {
                ERROR("afforest: forest was not created, afforesting is not possible!");
                return;
            }
            const double area = max(0., area_);
            if (area != area_)
                ERROR("afforest: input parameter correction:\nidx: {} -> {}", area_, area);

            if (area == 0)
                return;

            dat[0].area += area;
        }

        // for dat0: sd = -1, mai = (1 - slShare) * mai
        void reforest(const double area_, const double sd, const double mai) {
            if (dat.size() <= 1) {
                ERROR("reforest: forest was not created, reforesting is not possible or forest is too small!");
                return;
            }
            const double area = max(0., area_);
            if (area != area_)
                ERROR("afforest: input parameter correction:\nidx: {} -> {}", area_, area);

            if (area == 0)
                return;

            const double halfArea = area * 0.5;
            dat[0].area += halfArea;
            const IncrementTab::TableMode mode = IncrementTab::getTableModeBySD(sd);
            const double tableBiomass = it->getBm(mode, 0.5 * timeStep, mai, abs(sd));
            const double newBiomass = (dat[1].getBiomass_tC() + tableBiomass * halfArea) / (dat[1].area + halfArea);
            dat[1].area += halfArea;
            setBiomassToAgeClassIdx(1, newBiomass, mai);
        }

        void changeSpecies(const double area_) {
            if (dat.empty()) {
                ERROR("changeSpecies: forest was not created, changing species is not possible!");
                return;
            }
            const double area = max(0., area_);
            if (area != area_)
                ERROR("afforest: input parameter correction:\nidx: {} -> {}", area_, area);
            dat[0].area += area;
            dat.front().bm = 0;
        }

        // take from all age classes
        // area is a non-negative number, return the deforested biomass
        [[nodiscard]] FMResult deforest(const double area_, const double avgMai_) {
            const double totalArea = getTotalArea();
            if (totalArea == 0)
                return {};

            const double area = clamp(area_, 0., totalArea);
            const double avgMai = clamp(avgMai_, 0., 50.);
            if (area != area_ || avgMai != avgMai_)
                ERROR("deforest: input parameter correction:\narea: {} -> {}\tavgMai: {} -> {}", area_, area,
                      avgMai_, avgMai);

            FMResult ret;
            ret.area = area;
            const double mul = 1 - area / totalArea;
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
                    ret.biomass += totalWood;
                    double harvestedWood = totalWood * hle->ip(dbhBm[0]);
                    double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                    ret.sawnWood += sawnWood;
                    ret.restWood += harvestedWood - sawnWood;
                    ret.harvestCosts += totalWood * coe->ip(dbhBm);
                    dat[i].area *= mul;  // no check because 0 <= mul <= 1
                }

            if (ret.area > 0) { // Values per hectare
                const double reciprocalArea = 1 / ret.area;
                ret.sawnWood *= reciprocalArea;
                ret.restWood *= reciprocalArea;
                ret.harvestCosts *= reciprocalArea;
                ret.biomass *= reciprocalArea;
            } // MG: now per ha of deforested land
            return ret;
        }

        // returns {sd, iGwl, bm, id}
        // for dat0: sdMax = slShare * sdMax < 0, for dat: sdMax = -1
        [[nodiscard]] array<double, 4> incStatic(const size_t ageClassIdx_, const double sdMax_, const double mai_,
                                                 const double avgMai_, const double newForestSD_) const {
            if (dat.empty()) {
                ERROR("incStatic: forest was not created!");
                return {};
            }
            const size_t ageClassIdx = min(ageClassIdx_, dat.size() - 1);
            const double sdMax = clamp(sdMax_, -2., 2.);
            const double mai = clamp(mai_, 0., 50.);
            const double avgMai = clamp(avgMai_, 0., 50.);
            const double newForestSD = clamp(newForestSD_, -2., 2.);
            if (ageClassIdx != ageClassIdx_ || sdMax != sdMax_ || mai != mai_ || avgMai != avgMai_)
                ERROR("incStatic: input parameter correction:\nageClassIdx: {} -> {}\tavgMai: {} -> {}\tnewForestSD: {} -> {}",
                      ageClassIdx_, ageClassIdx, mai_, mai, avgMai_, avgMai, newForestSD_, newForestSD);

            const IncrementTab::TableMode mode = IncrementTab::getTableModeBySD(sdMax);
            double age = static_cast<double>(ageClassIdx) * timeStep;
            // MG: why t + 1???
            // target theoretical biomass at the next age class step
            double bm = it->getBm(mode, age + timeStep, mai, abs(sdMax));
            // estimated SD
            double sd = ageClassIdx > 0 ? it->getSd(mode, age, avgMai, dat[ageClassIdx].bm) : newForestSD;
            double iGwl = it->getIncGwl(mode, age, mai, sd);    // total increment
            double id = it->getIncDbh(mode, age, mai, sd);  // dbh increment
            return {sd, iGwl, bm, id};
        }

        void cohortShift(const double sd, const double avgMai) {
            if (currentMaxSize <= 1) {
                ERROR("cohortShift: currentMaxSize ({}) <= 1!", currentMaxSize);
                return;
            }
            // change back to ranges later
            if (dat.size() + 1 <= currentMaxSize) {
                initNewAgeClasses(1, sd, avgMai);
                ranges::shift_right(dat, 1);  // shift right 1
                dat[0] = AgeClass{};
                return;
            }
            if (dat.back().area == 0) {
                ranges::shift_right(dat, 1);  // shift right 1
                dat[0] = AgeClass{};
                return;
            }
            dat.back() = dat.back().getWeightedAgeClass(dat.end()[-2]);
            ranges::shift_right(dat | rv::take(dat.size() - 1), 1);  // shift right 1 dat except last element
            dat[0] = AgeClass{};
        }

        // bring the data to the next age class
        void shiftAgeClassIdx(const size_t idx) {
            if (dat.empty()) {
                ERROR("shiftAgeClassIdx: forest was not created!");
                return;
            }
            if (idx >= dat.size()) [[unlikely]] {
                ERROR("shiftAgeClassIdx: idx ({}) > dat.size() ({})!", idx, dat.size());
                return;
            }
            if (dat.size() == 1)
                return;
            if (idx + 1 < dat.size())
                dat[idx + 1] = dat[idx];
            else
                dat[idx + 1] = dat[idx + 1].getWeightedAgeClass(dat[idx]);
            dat[idx] = AgeClass{};
        }

        // get forest area where Bm > 0, including the first age class
        [[nodiscard]] double getAreaNonZero() const {
            double area = 0;
            for (const auto &ageClass: dat)
                if (ageClass.bm > 0 || &ageClass == &dat.front())
                    area += ageClass.area;
            return area;
        }

        // get average biomass per ha for age classes with Bm > 0 (including 0 age class)
        [[nodiscard]] double getBmNonZero() const {
            double area = 0;
            double bm = 0;
            for (const auto &ageClass: dat)
                if (ageClass.bm > 0 || &ageClass == &dat.front()) {
                    area += ageClass.area;
                    bm += ageClass.area * ageClass.bm;
                }
            return area > 0 ? bm / area : 0;
        }

        // MG 09052022: get standing stem biomass with a diameter greater than dbh0 cm
        [[nodiscard]] double getBmGDbh(const double dbh0) const {
            double area_ = 0;
            double bm = 0;

            for (const auto &ageClass: dat)
                if (ageClass.d > dbh0) {
                    area_ += ageClass.area;
                    bm += ageClass.area * ageClass.bm;
                }
            return area_ > 0 ? bm / area_ : 0;
        }

        [[nodiscard]] const vector<AgeClass> &getDat() const {
            return dat;
        }

        // MG: added : Find "active Age" - the oldest age class with area > 0
        [[nodiscard]] double getActiveAge() const {
            return static_cast<double>(getActiveAgeClassIdx()) * timeStep;
        }

        // find the oldest age class index with area > 0
        [[nodiscard]] size_t getActiveAgeClassIdx() const {
            if (dat.empty())
                return 0;
            auto iter = ranges::find_if(dat | rv::reverse, [](const auto &dat_i) { return dat_i.area > 0; });
            ptrdiff_t i = distance(iter, dat.rend());
            i = max(ptrdiff_t{0}, i - 1);
            return i;
        }

        // MG: Clean area of those age classes where Bm <= 0;
        // possible problem with low MAI when a few beginning age classes have 0 biomass
        void rectifyArea() {
            if (dat.size() <= 1)
                return;
            for (auto &ageClass: dat)
                if (ageClass.area > 0 && ageClass.bm <= 0)
                    ageClass.area = 0;
        }

        [[nodiscard]] FMResult
        thinAndGrowStatic(const double flexSd, const double sdMin, const double sdMax, const double mai,
                          const double avgMai) {
            FMResult ret;
            double shBm = 0;            // MG: sum of harvested wood*area in age classes which are thinned
            double sumArea = 0;         // MG: sum of area of all age classes
            double area_dw = 0;
            double area_lt = 0;
            double mortDwBm = 0;        // MG: sum of deadwood * area
            double mortLitterBm = 0;    // MG: sum of litter-wood * area

            for (ptrdiff_t i = ssize(dat) - 1; i >= 0; --i)  // 11.05.2016 MG: Thin and grow the oldest age class
                if (dat[i].area > 0) {
                    auto [sd, iGwl, bmT, id] = incStatic(i, sdMax, mai, avgMai, 1);
                    if (flexSd > 0) {  // The typical amount of harvest
                        double bmTCom = dat[i].bm + incCommon(i, sd, iGwl, sdMax, mai);
                        bmT = lerp(bmT, bmTCom, flexSd);  // bmT * (1 - flexSd) + bmTCom * flexSd
                    }
                    double totalWood = max(0., dat[i].area * (iGwl - (bmT - dat[i].bm)));
                    double bm_cur = dat[i].bm;  // MG: current biomass before growth

                    array<double, 3> dbhBmSh{};
                    dbhBmSh[0] = dat[i].d + id * 0.5;
                    dbhBmSh[1] = dat[i].bm + iGwl * 0.5;
                    dbhBmSh[2] = totalWood / dbhBmSh[1];
                    double age = static_cast<double>(i) * timeStep;
                    if (decisions->DOV(dbhBmSh[0], dbhBmSh[1], dbhBmSh[2])) { // do thinning if it is economic
                        double harvestedWood = totalWood * hlv->ip(dbhBmSh[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhBmSh[0]);
                        ret.area += dat[i].area;
                        ret.biomass += totalWood;
                        ret.sawnWood += sawnWood;
                        ret.restWood += harvestedWood - sawnWood;
                        ret.harvestCosts += totalWood * cov->ip(dbhBmSh);
                        double incBm = dat[i].area > 0 ? iGwl - totalWood / dat[i].area : iGwl;
                        dat[i].bm = max(0., dat[i].bm + incBm);
                        //  MG: estimation of BM weighted average d and h of harvested (at thinning) trees
                        double tmp_arg = harvestedWood * dat[i].area;
                        ret.DBH += dat[i].d * tmp_arg;
                        ret.H += dat[i].h * tmp_arg;
                        shBm += tmp_arg;
                    } else {  // No thinning
                        dat[i].bm += iGwl;
                        double bmMax = it->getBm(age + timeStep, avgMai);
                        double tmp_arg = (dat[i].bm - bmMax) * dat[i].area;
                        if (bmMax < dat[i].bm) {
                            if (dat[i].d + id * 0.5 > 10) {
                                ret.deadwood += tmp_arg;
                                area_dw += dat[i].area;
                                ret.mortDeadwoodDBH += dat[i].d * tmp_arg;
                                ret.mortDeadwoodH += dat[i].h * tmp_arg;
                                mortDwBm += tmp_arg;  // MG: estimation of BM weighted average d and h of dead trees (deadwood) that are not thinned
                            } else {
                                ret.litter += tmp_arg;
                                area_lt += dat[i].area;
                                ret.mortLitterDBH += dat[i].d * tmp_arg;
                                ret.mortLitterH += dat[i].h * tmp_arg;
                                mortLitterBm += tmp_arg;  // MG: estimation of BM weighted average d and h of dead trees (litter) that are not thinned
                            }
                            dat[i].bm = max(0., bmMax);
                        }
                    }
                    dat[i].d = max(0., dat[i].d + id);
                    dat[i].h = max(0., dat[i].h + it->getIncHeight(age, avgMai));
                    ret.netInc += dat[i].area * (dat[i].bm - bm_cur);
                    ret.grossInc += dat[i].area * iGwl;
                    sumArea += dat[i].area;
                }
            // MG: estimation of BM weighted average d and h of harvested (at thinning) trees
            if (shBm > 0) {
                ret.DBH /= shBm;
                ret.H /= shBm;
                ret.thinnedWeight = shBm / ret.area;
            }
            // MG: estimation of BM weighted average d and h of dead trees (deadwood) that were not thinned
            if (mortDwBm > 0) {
                ret.mortDeadwoodDBH /= mortDwBm;
                ret.mortDeadwoodH /= mortDwBm;
            }
            // MG: estimation of BM weighted average d and h of dead trees (deadwood) that were not thinned
            if (mortLitterBm > 0) {
                ret.mortLitterDBH /= mortLitterBm;
                ret.mortLitterH /= mortLitterBm;
            }
            // area & bm-weighted dw, tC/ha
            if (area_dw > 0)
                ret.deadwoodWeight = ret.deadwood / area_dw;
            // area & bm-weighted litter, tC/ha
            if (area_lt > 0)
                ret.litterWeight = ret.litter / area_lt;
            // MG: estimation of net and gross increments averaged over all age classes for timeStep, tC/(ha timeStep)
            if (sumArea > 0) {
                ret.netInc /= sumArea * timeStep;
                ret.grossInc /= sumArea * timeStep;
            }
            cohortShift(midpoint(sdMin, sdMax), avgMai);
            return ret;
        }

        [[nodiscard]] FMResult
        finalCutArea(const double area_, const bool eco, const bool sustainable, const double minRot_,
                     const double avgMai_) {
            const double area = max(0., area_);
            const double minRot = max(0., minRot_);
            const double avgMai = clamp(avgMai_, 0., 50.);
            if (area != area_ || avgMai != avgMai_)
                ERROR("finalCut: input parameter correction:\narea: {} -> {}\tminRot: {} -> {}\tavgMai: {} -> {}",
                      area_, area, minRot_, minRot, avgMai_, avgMai);

            FMResult ret;
            ptrdiff_t endYear = eco || sustainable ? static_cast<ptrdiff_t>(minRot / timeStep) : 0;
            array<double, 2> dbhBm{}; // Key to ask if harvest is economic

            double fcDBHTmp = 0;    // DBH of harvested trees, average weighted by clear-cut area and harvested biomass
            double fcHTmp = 0;      // height of harvested trees, average weighted by clear-cut area and harvested biomass
            double hWoodArea = 0;   // clear-cut area X harvested biomass

            for (ptrdiff_t i = ssize(dat) - 1; i >= endYear && ret.area < area; --i)
                if (dat[i].area > 0 && dat[i].bm > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    // The Stands get half increment of the next growing period
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    // MG: we don't allow negative biomass in any age group
                    dbhBm[0] = dat[i].d + id;
                    dbhBm[1] = dat[i].bm + dbm;
                    if (decisions->DOE(dbhBm[0], dbhBm[1]) || !eco) {  // do harvest if it is economic
                        double totalWood = 0;
                        double hArea = 0;  // clear-cut area in current age class
                        if (ret.area + dat[i].area < area) { // harvest all of this age class
                            totalWood = dat[i].area * dbhBm[1];
                            ret.area += dat[i].area;
                            hArea = dat[i].area; // 30.06.2023 corrected bug
                            dat[i].area = 0;
                        } else {
                            double tmp_arg = area - ret.area;
                            totalWood = tmp_arg * dbhBm[1];
                            dat[i].area -= tmp_arg;
                            hArea = tmp_arg;
                            ret.area = area;
                        }
                        double tmp_arg = totalWood * hArea;
                        fcDBHTmp += dat[i].d * tmp_arg;
                        fcHTmp += dat[i].h * tmp_arg;
                        hWoodArea += tmp_arg;
                        ret.biomass += totalWood;
                        double harvestedWood = totalWood * hle->ip(dbhBm[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                        ret.sawnWood += sawnWood;
                        ret.restWood += harvestedWood - sawnWood;
                        ret.harvestCosts += totalWood * coe->ip(dbhBm);
                    }
                }
            if (hWoodArea > 0) {
                ret.DBH = fcDBHTmp / hWoodArea;
                ret.H = fcHTmp / hWoodArea;
            }
            if (ret.area > 0) {  // Values per hectare
                const double reciprocalArea = 1 / ret.area;
                ret.sawnWood *= reciprocalArea;
                ret.restWood *= reciprocalArea;
                ret.harvestCosts *= reciprocalArea;
                ret.biomass *= reciprocalArea;
            }
            return ret;
        }

        [[nodiscard]] FMResult
        finalCutAmount(const double minSw_, const double minRw_, const double minHarv_,
                       const bool eco, const bool sustainable, const double minRot_, const double avgMai_) {
            const double minSw = max(0., minSw_);
            const double minRw = max(0., minRw_);
            const double minHarv = max(0., minHarv_);
            const double minRot = max(0., minRot_);
            const double avgMai = clamp(avgMai_, 0., 50.);
            if (avgMai != avgMai_)
                ERROR("finalCut: input parameter correction:\nminSw: {} -> {}\tminRw: {} -> {}\tminHarv: {} -> {}\tminRot: {} -> {}\tavgMai: {} -> {}",
                      minSw_, minSw, minRw_, minRw, minHarv_, minHarv, minRot_, minRot, avgMai_, avgMai);

            FMResult ret;
            ptrdiff_t endYear = eco || sustainable ? static_cast<ptrdiff_t>(minRot / timeStep) : 0;
            array<double, 2> dbhBm{}; // Key to ask if harvest is economic

            double fcDBHTmp = 0;    // DBH of harvested trees, average weighted by clear-cut area and harvested biomass
            double fcHTmp = 0;      // height of harvested trees, average weighted by clear-cut area and harvested biomass
            double hWoodArea = 0;   // clear-cut area X harvested biomass

            for (ptrdiff_t i = ssize(dat) - 1;
                 i >= endYear && (ret.sawnWood < minSw || ret.restWood < minRw || ret.getWood() < minHarv); --i)
                if (dat[i].area > 0 && dat[i].bm > 0) {
                    double age = static_cast<double>(i) * timeStep;
                    // The Stands get half increment of the next growing period
                    double sdNat = it->getSdNat(age, avgMai, dat[i].bm);
                    double dbm = it->getIncBmSdNat(age, avgMai, sdNat) * 0.5;
                    double id = it->getIncDbhSdNat(age, avgMai, sdNat) * 0.5;
                    // MG: we don't allow negative biomass in any age group
                    dbhBm[0] = dat[i].d + id;
                    dbhBm[1] = dat[i].bm + dbm;
                    if (decisions->DOE(dbhBm[0], dbhBm[1]) || !eco) {  // do harvest if it is economic
                        double harvestShare = 1;
                        double totalWood = dat[i].area * dbhBm[1];
                        double harvestedWood = totalWood * hle->ip(dbhBm[0]);
                        double sawnWood = harvestedWood * sws->ip(dbhBm[0]);
                        if (ret.sawnWood + sawnWood < minSw || ret.restWood + harvestedWood - sawnWood < minRw ||
                            ret.getWood() + harvestedWood < minHarv) {  // harvest all
                            ret.area += dat[i].area;
                            dat[i].area = 0;
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
                            tmp = minHarv - ret.getWood();
                            if (tmp > 0 && harvestedWood > 0) {
                                tmp /= harvestedWood;
                                harvestShare = max(tmp, harvestShare);
                            }
                            harvestShare = clamp(harvestShare, 0., 1.);
                            const double area_to_harvest = dat[i].area * harvestShare;
                            ret.area += area_to_harvest;
                            dat[i].area -= area_to_harvest;  // no check because 0 <= harvestShare <= 1
                        }
                        ret.biomass += totalWood * harvestShare;
                        ret.sawnWood += sawnWood * harvestShare;
                        ret.restWood += (harvestedWood - sawnWood) * harvestShare;
                        ret.harvestCosts += totalWood * coe->ip(dbhBm) * harvestShare;
                    }
                }
            if (hWoodArea > 0) {
                ret.DBH = fcDBHTmp / hWoodArea;
                ret.H = fcHTmp / hWoodArea;
            }
            if (ret.area > 0) {  // Values per hectare
                const double reciprocalArea = 1 / ret.area;
                ret.sawnWood *= reciprocalArea;
                ret.restWood *= reciprocalArea;
                ret.harvestCosts *= reciprocalArea;
                ret.biomass *= reciprocalArea;
            }
            return ret;
        }

        [[nodiscard]] string str() const {
            string str;
            str.reserve(75 * dat.size());
            for (const auto &[i, cohort]: dat | rv::enumerate)
                str += format("dat[{}] = [{}]", i, cohort.str());
            return str;
        }

        friend ostream &operator<<(ostream &os, const CanopyLayer &obj) {
            os << obj.str();
            return os;
        }

    private:
        vector<AgeClass> dat;

        // non-owned pointers
        const IncrementTab *it = nullptr;
        const IIpol<double> *sws = nullptr;
        const IIpol<double> *hlv = nullptr;
        const IIpol<double> *hle = nullptr;
        const IIpolM<double> *cov = nullptr;
        const IIpolM<double> *coe = nullptr;
        const Decisions *decisions = nullptr;

        double timeStep = it->getTimeframe();
        size_t incrementTableSize = static_cast<size_t>(it->getTMax() / timeStep);
        size_t currentMaxSize = 0;

        // get interpolated biomass, diameter or height per ha by age
        [[nodiscard]] double
        getInterpolatedCohortValueByAge(const double age_, const AgeClass::Mode type) const {
            if (dat.empty())
                return 0;

            const double age = max(0., age_);
            if (age != age_)
                ERROR("getInterpolatedCohortValueByAge: input parameter correction:\nage: {} -> {}", age_, age);

            const double adjustedAge = age / timeStep;
            const size_t ageH = ceil(adjustedAge);

            if (ageH == 0)
                return dat.front()(type);
            if (ageH >= dat.size())
                return dat.back()(type);

            const double t_age = age - static_cast<double>(ageH - 1);
            return lerp(dat[ageH - 1](type), dat[ageH](type), t_age);
        }

        // former initCohort with resizing, increases number of age classes and initializes them
        void initNewAgeClasses(const size_t numberOfNewAgeClasses, const double sd_, const double avgMai_) {
            size_t oldSize = dat.size();
            size_t newSize = oldSize + numberOfNewAgeClasses;
            if (newSize > incrementTableSize) {
                ERROR("initNewAgeClasses: newSize ({}) > incrementTableSize ({})!", newSize, incrementTableSize);
                return;
            }
            if (newSize > currentMaxSize) {
                ERROR("initNewAgeClasses: newSize ({}) > currentMaxSize ({})!", newSize, currentMaxSize);
                return;
            }
            const double sd = clamp(sd_, -2., 2.);
            const double avgMai = clamp(avgMai_, 0., 50.);
            if (sd != sd_ || avgMai != avgMai_)
                ERROR("initNewAgeClasses: input parameter correction:\nsd: {} -> {}\tavgMai: {} -> {}", sd_, sd,
                      avgMai_, avgMai);

            dat.resize(oldSize + numberOfNewAgeClasses);

            const IncrementTab::TableMode mode = IncrementTab::getTableModeBySD(sd);
            for (size_t i = oldSize; i < dat.size(); ++i) {
                const double age = static_cast<double>(i) * timeStep;
                dat[i].bm = it->getBm(mode, age, avgMai, abs(sd));
                dat[i].d = it->getDbh(mode, age, avgMai, abs(sd));
                dat[i].h = it->getHeight(age, avgMai);
            }
        }

        [[nodiscard]] double incCommon(const size_t i, const double sd, const double iGwl, const double sdMax,
                                       const double mai) const {
            double sdTarget = abs(sdMax);
            double bmInc = 0;
            double age = static_cast<double>(i + 1) * timeStep;
            if (sd > 0) {
                const IncrementTab::TableMode mode = IncrementTab::getTableModeBySD(copysign(sd, sdMax));
                bmInc = it->getIncBm(mode, age, mai, sd);
                bmInc = min(bmInc, iGwl);
                if (sd > sdTarget)
                    bmInc *= sdTarget / sd;
                else
                    bmInc += (iGwl - bmInc) * (1 - 1 / (1 + sdTarget - sd));
            } else  // Current stocking degree is 0
                bmInc = sdTarget > 0 ? iGwl : 0;
            return bmInc;
        }
    };
}

#endif
