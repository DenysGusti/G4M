#ifndef G4M_EUROPE_DG_COEF_HPP
#define G4M_EUROPE_DG_COEF_HPP

#include <array>

using namespace std;

namespace g4m::increment {
    struct Coef {
        array<double, 4> k{};
        array<double, 4> tMax{};
        array<double, 5> h{};
        array<double, 8> d{};
        array<double, 15> maxDens{};
        array<double, 4> imul{};
        double dmul = 0;
        double iCrit = 0;  // Critical stocking density (increment = 95% of full stoked)

        Coef() = default;

        explicit Coef(const span<const double> a) {
            setCoef(a);
        }

        void setCoef(const span<const double> a) noexcept {
            copy(a.begin(), a.begin() + 4, k.begin());
            copy(a.begin() + 4, a.begin() + 8, tMax.begin());
            copy(a.begin() + 8, a.begin() + 23, maxDens.begin());
            copy(a.begin() + 23, a.begin() + 33, imul.begin());
            copy(a.begin() + 23, a.begin() + 33, imul.begin());
            copy(a.begin() + 33, a.begin() + 38, h.begin());
            copy(a.begin() + 38, a.begin() + 46, d.begin());
            dmul = a[46];
            iCrit = a[47];
        }
    };
}

#endif
