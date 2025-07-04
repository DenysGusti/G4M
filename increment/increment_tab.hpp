#ifndef G4M_EUROPE_DG_INCREMENT_TAB_HPP
#define G4M_EUROPE_DG_INCREMENT_TAB_HPP

#include <cmath>
#include <limits>
#include <vector>
#include <array>
#include <span>

#include "increment_curves.hpp"
#include "opt_rot_times.hpp"

using namespace std;

namespace g4m::increment {
    // Forest stand growth functions and a set of instruments for estimating forest stand parameters.
    class IncrementTab {
    public:
        IncrementTab() = default;

        IncrementTab
                (const span<const double> a, const double maiMax_, const double aMaiStep, const double atMax,
                 const double tStep_, const double sdNatStep_, const double sdTabMax_,
                 const double sdTabStep_, const double timeframe_ = -1)
                : ic{a, 1}, maiHi{max(0., maiMax_)}, maiStep{aMaiStep}, tHi{max(0., atMax)}, tStep{tStep_},
                  sdNatStep{sdNatStep_}, sdTabHi{sdTabMax_}, sdTabStep{sdTabStep_}, timeframe{timeframe_} {
            if (tStep <= 0. && tHi > 0)
                tStep = tHi / 100;

            if (maiStep <= 0 && maiHi > 0)
                maiStep = maiHi * 0.50;

            if (tHi > 0 && tStep > 0)
                nt = 1 + static_cast<size_t>(ceil(tHi / tStep));

            if (maiHi > 0 && maiStep > 0)
                nmai = 1 + static_cast<size_t>(ceil(maiHi / maiStep));

            if (sdNatStep > 0)
                nsdNat = 1 + static_cast<size_t>(ceil(1 / sdNatStep));

            if (sdTabHi > 0 && sdTabStep > 0)
                nsdTab = 1 + static_cast<size_t>(ceil(sdTabHi / sdTabStep));

            size_t table_size = nt * nmai;
            abm.assign(table_size, 0);
            abmT.assign(table_size, 0);
            abmSdTab.assign(table_size * nsdTab, 0);  // sd below 1 is not used
            bm.assign(table_size, 0);
            bmT.assign(table_size, 0);
            gwl.assign(table_size, 0);
            gwlT.assign(table_size, 0);
            gwlSdNat.assign(table_size * nsdNat, 0);
            gwlSdTab.assign(table_size * nsdTab, 0);
            dbh.assign(table_size, 0);
            dbhT.assign(table_size, 0);
            height.assign(table_size, 0);
            sdNat.assign(table_size, 0);
            incGwl.assign(table_size, 0);  // The inc arrays could be in size (nt - 1)
            incGwlT.assign(table_size, 0);
            incBm.assign(table_size, 0);
            incBmT.assign(table_size, 0);
            incDbh.assign(table_size, 0);
            incDbhT.assign(table_size, 0);
            incHeight.assign(table_size, 0);
            incGwlSdNat.assign(table_size * nsdNat, 0);
            incDbhSdNat.assign(table_size * nsdNat, 0);
            incGwlSdTab.assign(table_size * nsdTab, 0);
            incDbhSdTab.assign(table_size * nsdTab, 0);
            dbhSdTab.assign(table_size * nsdTab, 0);
            optTime.assign(nmai, {});
            optTimeT.assign(nmai, {});
            optTimeSdNat.assign(nmai * nsdNat, {});
            optTimeSdTab.assign(nmai * nsdTab, {});
            fillTables();
        }

        void setCoef(const span<const double> a) {
            ic.setCoef(a);
        }

        enum class TableMode {
            N,
            T,
            SdNat,
            SdTab
        };

        [[nodiscard]] static TableMode getTableModeBySD(const double sd) {
            if (sd == 0)
                throw invalid_argument{"getTableModeBySD: sd = 0!"};
            if (sd > 0) // Yield table stocking degree
                return sd == 1 ? TableMode::T : TableMode::SdTab;
            else    // Natural stocking degree
                return sd == -1 ? TableMode::N : TableMode::SdNat;
        }

        [[nodiscard]] double
        getAvgBm(const TableMode mode, const double u, const double mai,
                 const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getAvgBm(u, mai);
                case TableMode::T:
                    return getAvgBmT(u, mai);
                case TableMode::SdNat:
                    return getAvgBmSdNat(u, mai, sd);
                case TableMode::SdTab:
                    return getAvgBmSdTab(u, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        [[nodiscard]] double getAvgBm(const double u, const double mai, const optional<double> sd) const {
            return ip(u, mai, abm);
        }

        // get Average biomass, u .. rotation time, mai .. Site index
        [[nodiscard]] double getAvgBm(const double u, const double mai) const {
            return ip(u, mai, abm);
        }

        // With thinning like yield table
        [[nodiscard]] double getAvgBmT(const double u, const double mai) const {
            return ip(u, mai, abmT);
        }

        // natural stocking degree
        [[nodiscard]] double getAvgBmSdNat(const double u, const double mai, const double sd) const {
            return ip(u, mai, abm) * clamp(sd, 0., 1.);
        }

        // yield table SD
        [[nodiscard]] double getAvgBmSdTab(const double u, const double mai, const double sd) const {
            if (sd <= 1)
                return ip(u, mai, abmT) * max(0., sd);

            return ip(u, mai, max(0., sd), abmSdTab, false);
        }

        [[nodiscard]] double
        getBm(const TableMode mode, const double age, const double mai,
              const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getBm(age, mai);
                case TableMode::T:
                    return getBmT(age, mai);
                case TableMode::SdNat:
                    return getBmSdNat(age, mai, sd);
                case TableMode::SdTab:
                    return getBmSdTab(age, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // Get stocking Biomass
        [[nodiscard]] double getBm(const double age, const double mai) const {
            return ip(age, mai, bm);
        }

        [[nodiscard]] double getBmT(const double age, const double mai) const {
            return ip(age, mai, bmT);
        }

        [[nodiscard]] double getBmSdNat(const double age, const double mai, const double sd) const {
            return ip(age, mai, bm) * clamp(sd, 0., 1.);
            // TODO Check is SD should be <= 1 or can be > 1?
        }

        [[nodiscard]] double getBmSdTab(const double age, const double mai, const double sd) const {
            if (sd <= 1)
                return ip(age, mai, bmT) * max(0., sd);

            double t1 = ip(age, mai, bmT) * max(0., sd);
            double t2 = ip(age, mai, bm);
            return min(t1, t2);
        }

        [[nodiscard]] double
        getDbh(const TableMode mode, const double age, const double mai,
               const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getDbh(age, mai);
                case TableMode::T:
                    return getDbhT(age, mai);
                case TableMode::SdNat:
                    return getDbhSdNat(age, mai, sd);
                case TableMode::SdTab:
                    return getDbhSdTab(age, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        //Get Diameter
        [[nodiscard]] double getDbh(const double age, const double mai) const {
            return ip(age, mai, dbh);
        }

        [[nodiscard]] double getDbhT(const double age, const double mai) const {
            return ip(age, mai, dbhT);
        }

        [[nodiscard]] double getDbhSdNat(const double age, const double mai, const double sd) const {
            return ip(age, mai, dbh) * ic.getDmul(clamp(sd, 0., 1.));
        }

        [[nodiscard]] double getDbhSdTab(const double age, const double mai, const double sd) const {
            return ip(age, mai, sd, dbhSdTab, false);
        }

        [[nodiscard]] double
        getGwl(const TableMode mode, const double age, const double mai,
               const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getGwl(age, mai);
                case TableMode::T:
                    return getGwlT(age, mai);
                case TableMode::SdNat:
                    return getGwlSdNat(age, mai, sd);
                case TableMode::SdTab:
                    return getGwlSdTab(age, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // Get Total increment
        [[nodiscard]] double getGwl(const double age, const double mai) const {
            return ip(age, mai, gwl);
        }

        [[nodiscard]] double getGwlT(const double age, const double mai) const {
            return ip(age, mai, gwlT);
        }

        [[nodiscard]] double getGwlSdNat(const double age, const double mai, const double sd) const {
            return ip(age, mai, sd, gwlSdNat, true);
        }

        [[nodiscard]] double getGwlSdTab(const double age, const double mai, const double sd) const {
            return ip(age, mai, sd, gwlSdTab, false);
        }

        // Get tree height
        [[nodiscard]] double getHeight(const double age, const double mai) const {
            return ip(age, mai, height);
        }

        // Get natural stocking degree if yield table stocking degree is 1
        [[nodiscard]] double getSdNat(const double age, const double mai) const {
            return ip(age, mai, sdNat);
        }

        [[nodiscard]] double
        getIncGwl(const TableMode mode, const double age, const double mai,
                  const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getIncGwl(age, mai);
                case TableMode::T:
                    return getIncGwlT(age, mai);
                case TableMode::SdNat:
                    return getIncGwlSdNat(age, mai, sd);
                case TableMode::SdTab:
                    return getIncGwlSdTab(age, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // Get Total increment increments
        [[nodiscard]] double getIncGwl(const double age, const double mai) const {
            return ip(age, mai, incGwl);
        }

        [[nodiscard]] double getIncGwlT(const double age, const double mai) const {
            return ip(age, mai, incGwlT);
        }

        [[nodiscard]] double getIncGwlSdNat(const double age, const double mai, const double sd) const {
            return ip(age, mai, sd, incGwlSdNat, true);
        }

        [[nodiscard]] double getIncGwlSdTab(const double age, const double mai, const double sd) const {
            return ip(age, mai, sd, incGwlSdTab, false);
        }

        [[nodiscard]] double
        getIncBm(const TableMode mode, const double age, const double mai,
                 const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getIncBm(age, mai);
                case TableMode::T:
                    return getIncBmT(age, mai);
                case TableMode::SdNat:
                    return getIncBmSdNat(age, mai, sd);
                case TableMode::SdTab:
                    return getIncBmSdTab(age, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // Get Biomass change
        [[nodiscard]] double getIncBm(const double age, const double mai) const {
            return ip(age, mai, incBm);
        }

        [[nodiscard]] double getIncBmT(const double age, const double mai) const {
            return ip(age, mai, incBmT);
        }

        [[nodiscard]] double getIncBmSdNat(const double age, const double mai, const double sd) const {
            return ip(age, mai, incBm) * clamp(sd, 0., 1.);
        }

        [[nodiscard]] double getIncBmSdTab(const double age, const double mai, const double sd) const {
            if (sd <= 1)
                return ip(age, mai, incBmT) * max(0., sd);

            double t1 = ip(age, mai, incBmT) * sd;
            double t2 = ip(age, mai, incBm);
            return min(t1, t2);
        }

        [[nodiscard]] double
        getIncDbh(const TableMode mode, const double age, const double mai,
                  const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getIncDbh(age, mai);
                case TableMode::T:
                    return getIncDbhT(age, mai);
                case TableMode::SdNat:
                    return getIncDbhSdNat(age, mai, sd);
                case TableMode::SdTab:
                    return getIncDbhSdTab(age, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // Get DBH-Increment without thinning
        [[nodiscard]] double getIncDbh(const double age, const double mai) const {
            return ip(age, mai, incDbh);
        }

        [[nodiscard]] double getIncDbhT(const double age, const double mai) const {
            return ip(age, mai, incDbhT);
        }

        [[nodiscard]] double getIncDbhSdNat(const double age, const double mai, const double sd) const {
            return ip(age, mai, sd, incDbhSdNat, true);
        }

        [[nodiscard]] double getIncDbhSdTab(double age, double mai, double sd) const {
            return ip(age, mai, sd, incDbhSdTab, false);
        }

        // Get height increment
        [[nodiscard]] double getIncHeight(const double age, const double mai) const {
            return ip(age, mai, incHeight);
        }

        [[nodiscard]] double
        getRemBm(const TableMode mode, const double age, const double mai,
                 const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getRemBm(age, mai);
                case TableMode::T:
                    return getRemBmT(age, mai);
                case TableMode::SdNat:
                    return getRemBmSdNat(age, mai, sd);
                case TableMode::SdTab:
                    return getRemBmSdTab(age, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // Get Biomass which comes from mortality or thinning and can be used
        // If IncBm is also needed it will be faster to calculate by own
        [[nodiscard]] double getRemBm(const double age, const double mai) const {
            return getIncGwl(age, mai) - getIncBm(age, mai);
        }

        [[nodiscard]] double getRemBmT(const double age, const double mai) const {
            return getIncGwlT(age, mai) - getIncBmT(age, mai);
        }

        [[nodiscard]] double getRemBmSdNat(const double age, const double mai, const double sd) const {
            return getIncGwlSdNat(age, mai, sd) - getIncBmSdNat(age, mai, sd);
        }

        [[nodiscard]] double getRemBmSdTab(const double age, const double mai, const double sd) const {
            return getIncGwlSdTab(age, mai, sd) - getIncBmSdTab(age, mai, sd);
        }

        [[nodiscard]] double
        getMai(const TableMode mode, const double u, const double mai,
               const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getMai(u, mai);
                case TableMode::T:
                    return getMaiT(u, mai);
                case TableMode::SdNat:
                    return getMaiSdNat(u, mai, sd);
                case TableMode::SdTab:
                    return getMaiSdTab(u, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // Get Mean annual increment with specific rotation time
        [[nodiscard]] double getMai(const double u, const double mai) const {
            return u > 0 ? getGwl(u, mai) / u : 0;
        }

        [[nodiscard]] double getMaiT(const double u, const double mai) const {
            return u > 0 ? getGwlT(u, mai) / u : 0;
        }

        [[nodiscard]] double getMaiSdNat(const double u, const double mai, const double sd) const {
            return u > 0 ? getGwlSdNat(u, mai, sd) / u : 0;
        }

        [[nodiscard]] double getMaiSdTab(const double u, const double mai, const double sd) const {
            return u > 0 ? getGwlSdTab(u, mai, sd) / u : 0;
        }

        [[nodiscard]] double
        getTOpt(const TableMode mode, const double mai, const OptRotTimes::Mode type,
                const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getTOpt(mai, type);
                case TableMode::T:
                    return getTOptT(mai, type);
                case TableMode::SdNat:
                    return getTOptSdNat(mai, sd, type);
                case TableMode::SdTab:
                    return getTOptSdTab(mai, sd, type);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        //Get optimal rotation time
        //0 … Highest average increment
        //1 … Maximum average Biomass
        //2 … Highest possible age
        //3 … Maximum harvest at final cut
        //4 … Average Maximum harvest at final cut
        [[nodiscard]] double getTOpt(const double mai, const OptRotTimes::Mode type) const {
            return ip(mai, optTime, type);
        }

        [[nodiscard]] double getTOptT(const double mai, const OptRotTimes::Mode type) const {
            return ip(mai, optTimeT, type);
        }

        [[nodiscard]] double
        getTOptSdNat(const double mai, const double sd, const OptRotTimes::Mode type) const {
            return ip(mai, sd, optTimeSdNat, type, true);
        }

        [[nodiscard]] double
        getTOptSdTab(const double mai, const double sd, const OptRotTimes::Mode type) const {
            return ip(mai, sd, optTimeSdTab, type, false);
        }

        [[nodiscard]] double
        getU(const TableMode mode, const double avgBm, const double mai,
             const double sd = numeric_limits<double>::quiet_NaN()) const {
            switch (mode) {
                case TableMode::N:
                    return getU(avgBm, mai);
                case TableMode::T:
                    return getUT(avgBm, mai);
                case TableMode::SdNat:
                    return getUSdNat(avgBm, mai, sd);
                case TableMode::SdTab:
                    return getUSdTab(avgBm, mai, sd);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        //Get rotation time out of mean bm
        [[nodiscard]] double getU(const double avgBm, const double mai) const {
            int tL = 0;
            int tH = ceil(getTOpt(mai, OptRotTimes::Mode::MaxBm));
            double bmL = 0;
            double bmH = getAvgBm(tH, mai);

            if (bmL == bmH)
                return tH;

            double t = 0;
            if (avgBm < bmH) {
                int tX = floor(0.5 + tH * 0.5);
                double bmX = getAvgBm(tX, mai);
                do {
                    if (bmX > avgBm) {
                        tH = tX;
                        bmH = bmX;
                    } else {
                        tL = tX;
                        bmL = bmX;
                    }
                    tX = floor(0.5 + lerp(tL, tH, (avgBm - bmL) / (bmH - bmL)));
                    bmX = getAvgBm(tX, mai);
                } while (tX != tL && tX != tH);
                t = lerp(tL, tH, (avgBm - bmL) / (bmH - bmL));
            } else
                t = tH;

            return t;
        }

        [[nodiscard]] double getUT(const double avgBm, const double mai) const {
            int tL = 0;
            int tH = ceil(getTOptT(mai, OptRotTimes::Mode::MaxBm));
            double bmL = 0;
            double bmH = getAvgBmT(tH, mai);

            if (bmL == bmH)
                return tH;

            double t = 0;
            if (avgBm < bmH) {
                int tX = floor(0.5 + tH * 0.5);
                double bmX = getAvgBmT(tX, mai);
                do {
                    if (bmX > avgBm) {
                        tH = tX;
                        bmH = bmX;
                    } else {
                        tL = tX;
                        bmL = bmX;
                    }
                    tX = floor(0.5 + lerp(tL, tH, (avgBm - bmL) / (bmH - bmL)));
                    bmX = getAvgBmT(tX, mai);
                } while (tX != tL && tX != tH);
                t = lerp(tL, tH, (avgBm - bmL) / (bmH - bmL));
            } else
                t = tH;

            return t;
        }

        [[nodiscard]] double getUSdNat(const double avgBm, const double mai, const double sd) const {
            if (avgBm <= 0)
                return 0;

            if (sd > 0)
                return getU(avgBm / min(1., sd), mai);

            return getTOpt(mai, OptRotTimes::Mode::MaxBm);
        }

        [[nodiscard]] double getUSdTab(const double avgBm, const double mai, const double sd) const {
            if (sd <= 1) {
                if (sd > 0)
                    return getUT(avgBm / sd, mai);

                if (avgBm <= 0)
                    return 0;

                return getTOptT(mai, OptRotTimes::Mode::MaxBm);
            }
            int tL = 0;
            int tH = ceil(getTOptSdTab(mai, sd, OptRotTimes::Mode::MaxBm));
            double bmL = 0;
            double bmH = getAvgBmSdTab(tH, mai, sd);

            if (bmL == bmH)
                return tH;

            double t = 0;
            if (avgBm < bmH) {
                int tX = floor(0.5 + tH * 0.5);
                double bmX = getAvgBmSdTab(tX, mai, sd);
                do {
                    if (bmX > avgBm) {
                        tH = tX;
                        bmH = bmX;
                    } else {
                        tL = tX;
                        bmL = bmX;
                    }
                    tX = floor(0.5 + lerp(tL, tH, (avgBm - bmL) / (bmH - bmL)));
                    bmX = getAvgBmSdTab(tX, mai, sd);
                } while (tX != tL && tX != tH);
                t = lerp(tL, tH, (avgBm - bmL) / (bmH - bmL));
            } else
                t = tH;

            return t;
        }

        [[nodiscard]] double getSd(const TableMode mode, const double age, const double mai, const double avgBm) const {
            switch (mode) {
                case TableMode::N:
                case TableMode::SdNat:
                    return getSdNat(age, mai, avgBm);
                case TableMode::T:
                case TableMode::SdTab:
                    return getSdTab(age, mai, avgBm);
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // Get stocking degree with age, mai, biomass
        [[nodiscard]] double getSdNat(const double age, const double mai, const double avgBm) const {
            if (getBm(age, mai) <= 0)
                return 0;

            return clamp(avgBm / getBm(age, mai), 0., 1.);
        }

        [[nodiscard]] double getSdTab(const double age, const double mai, const double avgBm) const {
            double sd = 0;
            double gBmT_tmp = getBmT(age, mai);

            if (gBmT_tmp > 0)
                sd = max(0., avgBm / gBmT_tmp);

            if (sd <= 1)
                return sd;

            double sdnat = getSdNat(age, mai);
            return sd * sdnat > 1 ? 1 / sdnat : sd;
        }

        // Get average stocking degree with rotation time, mai, average biomass
        [[nodiscard]] double getAvgSdNat(const double u, const double mai, const double avgBm) const {
            return clamp(avgBm / getAvgBm(u, mai), 0., 1.);
        }

        [[nodiscard]] double getAvgSdTab(const double u, const double mai, const double avgBm) const {
            double sd = max(0., avgBm / getAvgBmT(u, mai));

            if (sd <= 1)
                return sd;

            double sdnat = getAvgSdNat(u, mai, avgBm);
            if (sdnat >= 1 || avgBm <= 0)
                return numeric_limits<double>::infinity();

            int count = 0;
            double bmEst = 0;
            double dif = 1;
            do {
                sd /= dif;
                bmEst = getAvgBmSdTab(u, mai, sd);
                dif = bmEst / avgBm;
                ++count;
            } while (count < 20 && abs(dif - 1) > 1.e-32);

            return sd;
        }

        // Get time step width
        [[nodiscard]] double getTimeframe() const {
            return timeframe <= 0 ? tStep : timeframe;
        }

        // Get the oldest age in the table
        [[nodiscard]] double getTMax() const {
            return tHi;
        }

        // MG: get rotation when DBH > DBH0
        // This can also be stored in a table for faster access
        [[nodiscard]] double getU_dbh(const double dbh0, const double mai) const {
            return u_param(dbh0, mai, dbh);
        }

    private:
        IncrementCurves ic;             // Class of the increment curves
        double maiHi = 0;               // Highest MAI until table should be created
        double maiStep = 0;             // Step width in MAI
        double tHi = 0;                 // Highest age until table should be created
        double tStep = 0;               // Step width in time
        double sdNatStep = 0;           // Step width in Natural stocking degree
        double sdTabHi = 0;             // Highest Yield Table stocking degree
        double sdTabStep = 0;           // Step width in Yield Table stocking degree
        // Time steps for which the increment should be calculated (<=0 .. use tStep)
        double timeframe = 0;
        size_t nt = 0;                  // Number of time steps
        size_t nmai = 0;                // Number of mai steps
        size_t nsdNat = 0;              // Number of natural stand density steps
        size_t nsdTab = 0;              // Number of yield table stand density steps
        vector<double> abm;             // average above ground biomass without thinning
        vector<double> abmT;            // average above ground biomass with thinning
        vector<double> abmSdTab;        // average above ground biomass at specific stocking degree
        vector<double> bm;              // above ground biomass without thinning
        vector<double> bmT;             // above ground biomass with thinning
        vector<double> dbh;             // Diameter at maximum stand density
        vector<double> dbhT;            // Diameter at managed stand density
        vector<double> dbhSdTab;        // BHD at different yield table stocking degrees
        vector<double> gwl;             // Total increment without thinning
        vector<double> gwlT;            // Total increment with thinning
        vector<double> gwlSdNat;        // Total increment at specific Natural stocking degree
        vector<double> gwlSdTab;        // Total increment at specific yield table stocking degree
        vector<double> height;          // Height
        vector<double> sdNat;           // Natural stocking degree at increment optimal stand density
        vector<double> incGwl;          // Total increment change without thinning
        vector<double> incGwlT;         // Total increment change with thinning
        vector<double> incBm;           // Biomass change without thinning
        vector<double> incBmT;          // Biomass change with thinning
        vector<double> incDbh;          // BHD-Increment without thinning
        vector<double> incDbhT;         // BHD-Increment with thinning
        vector<double> incHeight;       // Height increment
        vector<double> incGwlSdNat;     // Total increment change at specific Natural stocking degree
        vector<double> incDbhSdNat;     // BHD-Increment at specific Natural stocking degree
        vector<double> incGwlSdTab;     // Total increment change at specific yield table stocking degree
        vector<double> incDbhSdTab;     // BHD-Increment at specific yield table stocking degree
        // Optimal rotation time array
        vector<OptRotTimes> optTime;
        vector<OptRotTimes> optTimeT;
        vector<OptRotTimes> optTimeSdNat;
        vector<OptRotTimes> optTimeSdTab;

        // Interpolate rotation time between mai
        [[nodiscard]] double ip(double mai, const span<const OptRotTimes> tab, const OptRotTimes::Mode type) const {
            mai /= maiStep;

            auto mail = static_cast<size_t>(clamp(floor(mai), 0., static_cast<double>(nmai - 1)));
            auto maih = static_cast<size_t>(clamp(ceil(mai), 0., static_cast<double>(nmai - 1)));

            int ul = tab[mail](type);
            int uh = tab[maih](type);

            double t_mai = mail == maih ? 0 : (mai - static_cast<double>(mail)) / static_cast<double>(maih - mail);

            return lerp(ul, uh, t_mai);
        }

        // Interpolate rotation time between mai and stocking degree (sdNat: True..sdNat, false..sdTab)
        [[nodiscard]] double ip(double mai, double sd, const span<const OptRotTimes> tab, const OptRotTimes::Mode type,
                                const bool sdNatFlag) const {
            mai = clamp(mai / maiStep, 0., static_cast<double>(nmai - 1));
            size_t maih = ceil(mai);
            size_t mail = floor(mai);
            size_t sdMax = 1;  // MG: 25 March 2013
            if (sdNatFlag) {
                sd /= sdNatStep;
                if (sd > static_cast<double>(nsdNat - 1)) {
                    sd = static_cast<double>(nsdNat - 1);
                    sdMax = nsdNat - 1;
                }  // MG: Suggestion of Georg 25 March 2013
            } else {
                sd /= sdTabStep;
                if (sd >= static_cast<double>(nsdTab)) {
                    sd = static_cast<double>(nsdTab - 1);
                    sdMax = nsdTab - 1;
                }
            }
            sd = clamp(sd, 0., static_cast<double>(sdMax));  // sd < 0?
            size_t sdh = ceil(sd);
            size_t sdl = floor(sd);
            if (abs(sd) < 1e-11) {
                sdl = 0;
                sdh = 0;
            }  // MG: 7 July 2019
            int mhsh = tab[maih + sdh * nmai](type);
            int mhsl = tab[maih + sdl * nmai](type);
            int mlsh = tab[mail + sdh * nmai](type);
            int mlsl = tab[mail + sdl * nmai](type);
            //	MG: hot fix check
            double t_mai = mail == maih ? 0 : (mai - static_cast<double>(mail)) / static_cast<double>(maih - mail);
            double t0 = lerp(mlsl, mhsl, t_mai);
            double t1 = lerp(mlsh, mhsh, t_mai);
            double t_sd = sdl == sdh ? 0 : (sd - static_cast<double>(sdl)) / static_cast<double>(sdh - sdl);

            return lerp(t0, t1, t_sd);
        }

        // Interpolate between age and mai
        [[nodiscard]] double ip(double u, double mai, const span<const double> tab) const {
            mai = clamp(mai / maiStep, 0., static_cast<double>(nmai - 1));
            size_t maih = ceil(mai);
            size_t mail = floor(mai);
            u = clamp(u / tStep, 0., static_cast<double>(nt - 1));
            size_t uh = ceil(u);
            size_t ul = floor(u);

            double tmp1 = tab[ul + mail * nt];  // MG
            double tmp2 = tab[uh + mail * nt];  // MG
            double tmp3 = tab[ul + maih * nt];  // MG
            double tmp4 = tab[uh + maih * nt];  // MG

            double t_u = ul == uh ? 0 : (u - static_cast<double>(ul)) / static_cast<double>(uh - ul);
            double t1 = lerp(tmp1, tmp2, t_u);

            double t2 = mail == maih ? t1 : lerp(tmp3, tmp4, t_u);

            double t_mai = mail == maih ? 0 : (mai - static_cast<double>(mail)) / static_cast<double>(maih - mail);
            return lerp(t1, t2, t_mai);
        }

        // Interpolate between stocking degree, age and mai (sdNat: True..sdNat, false..sdTab)
        [[nodiscard]] double
        ip(double u, double mai, double sd, const span<const double> tab, const bool sdNatFlag) const {
            mai = clamp(mai / maiStep, 0., static_cast<double>(nmai - 1));
            size_t maih = ceil(mai);
            size_t mail = floor(mai);
            u = clamp(u / tStep, 0., static_cast<double>(nt - 1));
            size_t uh = ceil(u);
            size_t ul = floor(u);

            size_t maxSd = 0;
            if (sdNatFlag) {
                sd = min(sd / sdNatStep, static_cast<double>(nsdNat - 1)); // MG: Suggestion of Georg 25 March 2013
                maxSd = nsdNat;
            } else {
                sd = min(sd / sdTabStep, static_cast<double>(nsdTab - 1));
                maxSd = nsdTab;
            }
            sd = clamp(sd, 0., static_cast<double>(maxSd - 1));
            size_t sdh = ceil(sd);
            size_t sdl = floor(sd);

            double tmp1 = tab[ul + mail * nt + sdl * nt * nmai];  // MG
            double tmp2 = tab[uh + mail * nt + sdl * nt * nmai];  // MG
            double tmp3 = tab[ul + maih * nt + sdl * nt * nmai];  // MG
            double tmp4 = tab[uh + maih * nt + sdl * nt * nmai];  // MG

            double t_u = ul == uh ? 0 : (u - static_cast<double>(ul)) / static_cast<double>(uh - ul);

            double t1 = lerp(tmp1, tmp2, t_u);

            double t2 = lerp(tmp3, tmp4, t_u);
            double t_mai = mail == maih ? 0 : (mai - static_cast<double>(mail)) / static_cast<double>(maih - mail);
            double t0 = lerp(t1, t2, t_mai);

            tmp1 = tab[ul + mail * nt + sdh * nt * nmai];
            tmp2 = tab[uh + mail * nt + sdh * nt * nmai];
            tmp3 = tab[ul + maih * nt + sdh * nt * nmai];
            tmp4 = tab[uh + maih * nt + sdh * nt * nmai];

            t1 = lerp(tmp1, tmp2, t_u);
            t2 = lerp(tmp3, tmp4, t_u);
            t1 = lerp(t1, t2, t_mai);
            double t_sd = sdl == sdh ? 0 : (sd - static_cast<double>(sdl)) / static_cast<double>(sdh - sdl);

            return lerp(t0, t1, t_sd);
        }

        // Function to fill up the tables using incrementCurves ic
        void fillTables() {
            const double minMAI = 0.01;
            fillArraysGLWBiomass(minMAI);
            getIncTimeSteps(minMAI);
            searchORT();
        }

        // Fill the arrays with gwl, biomass, ... for age/yield
        void fillArraysGLWBiomass(const double minMAI) {
            for (size_t cmai = 0; cmai < nmai; ++cmai) {
                ic.setMai(max(static_cast<double>(cmai) * maiStep, minMAI));

                double sbm = 0;
                double sbmT = 0; // Sum of Biomass (with treatment)
                vector<double> sbmSdTab(nsdTab);
                for (size_t ct = 0; ct < nt; ++ct) {
                    double t = static_cast<double>(ct) * tStep;
                    // Stocking degree where 95% of the increment of a full stocked forest is reached
                    double sd95inc = ic.getManDens(t);
                    sdNat[ct + cmai * nt] = sd95inc;
                    dbh[ct + cmai * nt] = ic.getD(t);
                    height[ct + cmai * nt] = ic.getH(t);
                    double gwlMaxDens = ic.getTcp(t);
                    gwl[ct + cmai * nt] = gwlMaxDens;
                    double bmMaxDens = gwlMaxDens * ic.getMaxDens(t);
                    bm[ct + cmai * nt] = bmMaxDens;
                    double bmSd95inc = bmMaxDens * sd95inc;
                    bmT[ct + cmai * nt] = bmSd95inc;
                    sbm += bmMaxDens;
                    sbmT += bmSd95inc;
                    if (t > 0) {
                        abm[ct + cmai * nt] = sbm * tStep / t;
                        abmT[ct + cmai * nt] = sbmT * tStep / t;
                    } else {
                        abm[ct + cmai * nt] = 0;
                        abmT[ct + cmai * nt] = 0;
                    }
                    // Go to all natural stocking degree steps
                    for (size_t csd = 0; csd < nsdTab; ++csd) {
                        // Yield table stocking degree; tabSd, bmSd
                        sbmSdTab[csd] += min(bmSd95inc * static_cast<double>(csd) * sdTabStep, bmMaxDens);
                        abmSdTab[ct + cmai * nt + csd * nt * nmai] = t > 0 ? sbmSdTab[csd] * tStep / t : 0;
                    }
                }
            }
        }

        // Go to all time steps and get the increments
        void getIncTimeSteps(const double minMAI) {
            for (size_t cmai = 0; cmai < nmai; ++cmai) {
                ic.setMai(max(static_cast<double>(cmai) * maiStep, minMAI));
                for (size_t ct = 1; ct < nt; ++ct) {
                    // Natural stocking degree at begin (0) and end (1) of period of
                    // Yield table stocking degree = 1
                    double sdNat0 = sdNat[ct - 1 + cmai * nt];
                    double sdNat1 = sdNat[ct + cmai * nt];
                    double t0 = static_cast<double>(ct - 1) * tStep;
                    double t1 = static_cast<double>(ct) * tStep;
                    auto cmai_d = static_cast<double>(cmai);
                    if (timeframe <= 0)
                        incHeight[ct - 1 + cmai * nt] = height[ct + cmai * nt] - height[ct - 1 + cmai * nt];
                    else
                        incHeight[ct - 1 + cmai * nt] =
                                ip(t0 + timeframe, cmai_d * maiStep, height) - height[ct - 1 + cmai * nt];
                    double incDbhMaxDens = dbh[ct + cmai * nt] - dbh[ct - 1 + cmai * nt];
                    double incDbhMaxDensY = incDbhMaxDens;
                    if (timeframe <= 0)
                        incDbh[ct - 1 + cmai * nt] = incDbhMaxDens;
                    else {
                        incDbhMaxDensY = ip(t0 + timeframe, cmai_d * maiStep, dbh) - dbh[ct - 1 + cmai * nt];
                        incDbh[ct - 1 + cmai * nt] = incDbhMaxDensY;
                    }
                    double incDbhT0 = incDbhMaxDens * (ic.getDmul(sdNat1) + ic.getDmul(sdNat0)) * 0.5;
                    if (timeframe <= 0)
                        incDbhT[ct - 1 + cmai * nt] = incDbhT0;
                    else
                        incDbhT[ct - 1 + cmai * nt] =
                                incDbhMaxDensY * (ic.getDmul(ip(t0 + timeframe, cmai_d * maiStep,
                                                                sdNat)) + ic.getDmul(sdNat0)) * 0.5;
                    dbhT[ct + cmai * nt] = dbhT[ct - 1 + cmai * nt] + incDbhT0;
                    double incGwlMaxDens = gwl[ct + cmai * nt] - gwl[ct - 1 + cmai * nt];
                    double incGwlMaxDensY = incGwlMaxDens;
                    if (timeframe <= 0)
                        incGwl[ct - 1 + cmai * nt] = incGwlMaxDens;
                    else {
                        incGwlMaxDensY = ip(t0 + timeframe, cmai_d * maiStep, gwl) - gwl[ct - 1 + cmai * nt];
                        incGwl[ct - 1 + cmai * nt] = incGwlMaxDensY;
                    }
                    double incGwlT0 = incGwlMaxDens * (ic.getImul(sdNat1, t1) + ic.getImul(sdNat0, t0)) * 0.5;
                    if (timeframe <= 0)
                        incGwlT[ct - 1 + cmai * nt] = incGwlT0;
                    else {
                        incGwlT[ct - 1 + cmai * nt] = incGwlMaxDensY *
                                                      (ic.getImul(ip(t0 + timeframe, cmai_d * maiStep, sdNat),
                                                                  t0 + timeframe) + ic.getImul(sdNat0, t0)) * 0.5;
                    }
                    gwlT[ct + cmai * nt] = gwlT[ct - 1 + cmai * nt] + incGwlT0;
                    if (timeframe <= 0) {
                        incBm[ct - 1 + cmai * nt] = bm[ct + cmai * nt] - bm[ct - 1 + cmai * nt];
                        incBmT[ct - 1 + cmai * nt] = bmT[ct + cmai * nt] - bmT[ct - 1 + cmai * nt];
                    } else {
                        incBm[ct - 1 + cmai * nt] = ip(t0 + timeframe, cmai_d * maiStep, bm) - bm[ct - 1 + cmai * nt];
                        incBmT[ct - 1 + cmai * nt] =
                                ip(t0 + timeframe, cmai_d * maiStep, bmT) - bmT[ct - 1 + cmai * nt];
                    }
                    // Go to all natural stocking degree steps
                    for (int csd = 0; csd < nsdNat; ++csd) {
                        double natSd = csd * sdNatStep;  // Natural stocking degree
                        incDbhSdNat[ct - 1 + cmai * nt + csd * nt * nmai] = incDbhMaxDensY * ic.getDmul(natSd);
                        incGwlSdNat[ct - 1 + cmai * nt + csd * nt * nmai] =
                                incGwlMaxDensY * (ic.getImul(natSd, t0) + ic.getImul(natSd, t0 + timeframe)) * 0.5;
                        gwlSdNat[ct + cmai * nt + csd * nt * nmai] = gwlSdNat[ct - 1 + cmai * nt + csd * nt * nmai] +
                                                                     incGwlMaxDens *
                                                                     (ic.getImul(natSd, t0) + ic.getImul(natSd, t1)) *
                                                                     0.5;
                    }
                    // Go to all yield table stocking degree steps
                    for (size_t csd = 0; csd < nsdTab; ++csd) {
                        double tabSd = static_cast<double>(csd) * sdTabStep;  // Yield table stocking degree
                        double sdNat2 = ip(t0 + timeframe, cmai_d * maiStep, sdNat);
                        incDbhSdTab[ct - 1 + cmai * nt + csd * nt * nmai] =
                                incDbhMaxDensY * (ic.getDmul(tabSd * sdNat2) + ic.getDmul(tabSd * sdNat0)) * 0.5;
                        dbhSdTab[ct + cmai * nt + csd * nt * nmai] = dbhSdTab[ct - 1 + cmai * nt + csd * nt * nmai] +
                                                                     incDbhMaxDens * (ic.getDmul(tabSd * sdNat1) +
                                                                                      ic.getDmul(tabSd * sdNat0)) * 0.5;
                        // in case "tabSd * sdNatX > 1" getImul returns 1 what is OK
                        incGwlSdTab[ct - 1 + cmai * nt + csd * nt * nmai] = incGwlMaxDensY *
                                                                            (ic.getImul(tabSd * sdNat2,
                                                                                        t0 + timeframe) +
                                                                             ic.getImul(tabSd * sdNat0, t0)) * 0.5;
                        gwlSdTab[ct + cmai * nt + csd * nt * nmai] = gwlSdTab[ct - 1 + cmai * nt + csd * nt * nmai] +
                                                                     incGwlMaxDens * (ic.getImul(tabSd * sdNat1, t1) +
                                                                                      ic.getImul(tabSd * sdNat0, t0)) *
                                                                     0.5;
                    }
                }
            }
        }

        // Search optimal rotation times
        void searchORT() {
            for (size_t cmai = 0; cmai < nmai; ++cmai) {
                // Highest Possible Age
                optTime[cmai].maxAge = static_cast<int>(static_cast<double>(nt - 1) * tStep);
                for (int ct = static_cast<int>(nt) - 2; ct >= 0 && bm[ct + cmai * nt] <= 0; --ct)
                    optTime[cmai].maxAge = static_cast<int>(static_cast<double>(ct) * tStep);
                optTimeT[cmai].maxAge = optTime[cmai].maxAge;
                for (int csd = 0; csd < nsdNat; ++csd)
                    optTimeSdNat[cmai + csd * nmai].maxAge = optTime[cmai].maxAge;
                for (int csd = 0; csd < nsdTab; ++csd)
                    optTimeSdTab[cmai + csd * nmai].maxAge = optTime[cmai].maxAge;

                auto tStep_i = static_cast<int>(tStep);
                // Highest average increment
                {
                    int ct = 1;      // Current rotation time
                    int maxInc;  // last rotation time
                    double cBm = gwl[ct + cmai * nt] / ct;    // current biomass
                    double maxBm;  // last biomass
                    do {
                        maxInc = ct++;
                        maxBm = cBm;
                        cBm = gwl[ct + cmai * nt] / ct;
                    } while (cBm > maxBm && ct < nt);
                    maxInc *= tStep_i;
                    optTime[cmai].maxInc = maxInc;
                    ct = 1;
                    cBm = gwlT[ct + cmai * nt] / ct;
                    do {
                        maxInc = ct++;
                        maxBm = cBm;
                        cBm = gwlT[ct + cmai * nt] / ct;
                    } while (cBm > maxBm && ct < nt);
                    maxInc *= tStep_i;
                    optTimeT[cmai].maxInc = maxInc;
                    for (size_t csd = 0; csd < nsdNat; ++csd) {
                        ct = 1;
                        cBm = gwlSdNat[ct + cmai * nt + csd * nt * nmai] / ct;
                        do {
                            maxInc = ct++;
                            maxBm = cBm;
                            cBm = gwlSdNat[ct + cmai * nt + csd * nt * nmai] / ct;
                        } while (cBm > maxBm && ct < nt);
                        maxInc *= tStep_i;
                        optTimeSdNat[cmai + csd * nmai].maxInc = maxInc;
                    }
                    for (size_t csd = 0; csd < nsdTab; ++csd) {
                        ct = 1;
                        cBm = gwlSdTab[ct + cmai * nt + csd * nt * nmai] / ct;
                        do {
                            maxInc = ct++;
                            maxBm = cBm;
                            cBm = gwlSdTab[ct + cmai * nt + csd * nt * nmai] / ct;
                        } while (cBm > maxBm && ct < nt);
                        maxInc *= tStep_i;
                        optTimeSdTab[cmai + csd * nmai].maxInc = maxInc;
                    }
                }
                // Maximum average Biomass
                {
                    int maxBm = 0;
                    int ct = 0;
                    for (ct = 1; abm[ct + cmai * nt] > abm[maxBm + cmai * nt] && ct < nt; ++ct)
                        maxBm = ct;
                    maxBm *= tStep_i;
                    optTime[cmai].maxBm = maxBm;
                    for (size_t csd = 0; csd < nsdNat; ++csd) {
                        optTimeSdNat[cmai + csd * nmai].maxBm = maxBm;
                    }
                    maxBm = 0;
                    for (ct = 1; abmT[ct + cmai * nt] > abmT[maxBm + cmai * nt] && ct < nt; ++ct)
                        maxBm = ct;
                    maxBm *= tStep_i;
                    optTimeT[cmai].maxBm = maxBm;
                    for (size_t csd = 0; csd < nsdTab; ++csd)
                        optTimeSdTab[cmai + csd * nmai].maxBm = maxBm;
                }
                // Maximum harvest at final cut
                {
                    int maxHarv = 0;
                    int ct = 0;
                    for (ct = 1; bm[ct + cmai * nt] > bm[maxHarv + cmai * nt] && ct < nt; ++ct)
                        maxHarv = ct;
                    maxHarv *= tStep_i;
                    optTime[cmai].maxHarv = maxHarv;
                    for (int csd = 0; csd < nsdNat; ++csd) {
                        optTimeSdNat[cmai + csd * nmai].maxHarv = maxHarv;
                    }
                    maxHarv = 0;
                    for (ct = 1; bmT[ct + cmai * nt] > bmT[maxHarv + cmai * nt] && ct < nt; ++ct)
                        maxHarv = ct;
                    maxHarv *= tStep_i;
                    optTimeT[cmai].maxHarv = maxHarv;
                    for (size_t csd = 0; csd < nsdTab; ++csd)
                        optTimeSdTab[cmai + csd * nmai].maxHarv = maxHarv;
                }
                // Average Maximum harvest at final cut
                {
                    int maxAvgHarv = 1;
                    int ct = 0;
                    for (ct = 2; bm[ct + cmai * nt] / ct > bm[maxAvgHarv + cmai * nt] / maxAvgHarv && ct < nt; ++ct)
                        maxAvgHarv = ct;
                    maxAvgHarv *= tStep_i;
                    optTime[cmai].maxAvgHarv = maxAvgHarv;
                    for (size_t csd = 0; csd < nsdNat; ++csd)
                        optTimeSdNat[cmai + csd * nmai].maxAvgHarv = maxAvgHarv;
                    maxAvgHarv = 1;
                    for (ct = 2; bmT[ct + cmai * nt] / ct > bmT[maxAvgHarv + cmai * nt] / maxAvgHarv && ct < nt; ++ct)
                        maxAvgHarv = ct;
                    maxAvgHarv *= tStep_i;
                    optTimeT[cmai].maxAvgHarv = maxAvgHarv;
                    for (size_t csd = 0; csd < nsdTab; ++csd)
                        optTimeSdTab[cmai + csd * nmai].maxAvgHarv = maxAvgHarv;
                }
            }
        }

        // MG find U when param > param0
        [[nodiscard]] double u_param(const double param0, const double mai, const span<const double> tab) const {
            double tmp = clamp(mai / maiStep, 0., static_cast<double>(nmai) - 1.);
            size_t mail = floor(tmp);

            for (size_t i = 0; i < nt; ++i)
                if (tab[i + mail * nt] > param0)
                    return static_cast<double>(i) * tStep;
            return -tStep;
        }
    };
}

#endif
