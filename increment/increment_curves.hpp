#ifndef G4M_EUROPE_DG_INCREMENT_CURVES_HPP
#define G4M_EUROPE_DG_INCREMENT_CURVES_HPP

#include <cmath>
#include <limits>
#include <array>
#include <algorithm>

#include "coef.hpp"

using namespace std;

namespace g4m::increment {

    class IncrementCurves {
    public:
        IncrementCurves(const span<const double> a, const double aMai) : coef{a}, mai{aMai} {
            calc();
        }

        void setCoef(const span<const double> a) {
            coef.setCoef(a);
            calc();
        }

        void setMai(const double amai) noexcept {
            mai = amai;
            calc();
        }

        // increment optimal rotation time
        [[nodiscard]] double getTOpt() const noexcept {
            return tOpt;
        }

        [[nodiscard]] double getTcpMax() const noexcept {
            return tcpMax;
        }

        // Total carbon production (at maximum stocking degree)
        [[nodiscard]] double getTcp(const double t) const noexcept {
            if (t <= 0 || tMax <= 0)
                return 0;
            if (t >= tMax)
                return tcpMax;
            double tmp = log(t / tMax);
            return max(0., tcpMax * exp(k * tmp * tmp));
        }

        // Reduction of total carbon production (TCP) to get maximum possible Volume
        [[nodiscard]] double getMaxDens(const double t) const noexcept {
            if (t <= 0 || tOpt <= 0)
                return 1;

            array<double, 4> f{};
            const auto &c = coef.maxDens;

            f[2] = c[10] + c[11] * exp(c[12] + c[13] * mai);
            double tmp = f[2] * t / tOpt;

            if (tmp <= 0)
                return 1;
            if (tmp >= 1)
                return 0;

            f[0] = c[0] + c[1] / (1 + exp(c[2] + c[3] * mai));
            f[1] = c[4] / (1 + exp(c[5] + c[6] * mai)) + c[7] / (1 + exp(c[8] + c[9] * mai));
            f[3] = c[14];

            return clamp((f[0] + f[1] * log(t / tOpt)) * pow(1 - tmp, f[3]), 0., 1.);
        }

        // Natural Stocking degree for managed forests
        [[nodiscard]] double getManDens(const double t) const noexcept {
            double up = 1;
            double lo = 0;
            double x = 0.8;
            double vup = 1;
            double vlo = 0;
            double vx = getImul(x, t);
            for (int n = 0; abs(vx - coef.iCrit) > 0.01 && n < 10; ++n) {
                if (vx > coef.iCrit) {
                    up = x;
                    vup = vx;
                } else {
                    lo = x;
                    vlo = vx;
                }
                x = lerp(lo, up, (coef.iCrit - vlo) / (vup - vlo));
                vx = getImul(x, t);
            }
            return x;
        }

        // Tree height
        [[nodiscard]] double getH(const double t) const noexcept {
            if (t <= 0)
                return 0;

            return max(0.,
                       coef.h[0] * pow(mai, coef.h[1]) * pow(1. - exp(coef.h[2] * t), coef.h[3] * pow(mai, coef.h[4])));
        }

        // Tree diameter at maximum stand density
        [[nodiscard]] double getD(const double t) const noexcept {
            if (t <= th13 || mai <= 0)
                return 0;

            double a = coef.d[0] + coef.d[1] * mai;
            double b = coef.d[2] / (1. + coef.d[3] * pow(mai, coef.d[4]));
            double c = coef.d[5] / (1. + coef.d[6] * pow(mai, coef.d[7]));
            return max(0., a * pow(1. - exp(b * (t - th13)), c));
        }

        // diameter multiplier for stocking degree v/vMax
        [[nodiscard]] double getDmul(const double bg) const noexcept {
            return 2 - pow(clamp(bg, 0., 1.), coef.dmul);
        }

        // increment multiplier for stocking degree v/vMax
        [[nodiscard]] double getImul(const double bg, const double t) const noexcept {
            if (bg <= 0)
                return 0;
            if (bg >= 1 && t <= 0)
                return 1;

            const auto &c = coef.imul;
            double o0 = 1. + pow(t, c[0]) / c[1] * 1. / (c[2] + c[3] * pow(mai, c[4]));
            double o1 = 1. + c[5] / (1. + c[6] * pow(t, c[7]) * 1. / (c[8] + pow(mai, c[9])));
            double p = pow(1. / o0, 1. / (o0 - 1.));
            double p2 = p - pow(p, o0);
            double p4 = p * o1;
            double p3 = p;
            p3 = min(p3, bg * p4);
            return (p3 - pow(p3, o0)) / p2;
        }

    private:
        Coef coef;
        double mai = 0;
        double k = 0;
        double tOpt = 0;
        double tMax = 0;
        double th13 = 0;
        double tcpMax = 0;

        void calcK() noexcept {
            k = min(0., coef.k[0] + coef.k[1] * exp(coef.k[2] * pow(mai, coef.k[3])));
        }

        void calcTcpMax() noexcept {
            tcpMax = k < 0 ? max(0., mai * tMax * exp(0.25 / k)) : 0;
        }

        void calcTMax() noexcept {
            tMax = max(0., coef.tMax[0] + coef.tMax[1] / (1 + exp(coef.tMax[2] + coef.tMax[3] * mai)));
        }

        void calcTOpt() noexcept {
            tOpt = k < 0 ? max(0., tMax * exp(0.5 / k)) : 0;
        }

        void calcTh13() noexcept {
            if (mai <= 0)
                th13 = numeric_limits<double>::infinity();
            else {
                double t1 = coef.h[0] * pow(mai, coef.h[1]);
                double t2 = coef.h[3] * pow(mai, coef.h[4]);
                th13 = max(0., log(1 - pow(1.3 / t1, 1 / t2)) / coef.h[2]);
            }
        }

        void calc() noexcept {
            calcK();
            calcTMax();
            calcTcpMax();
            calcTOpt();
            calcTh13();
        }
    };
}

#endif
