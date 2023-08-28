#ifndef G4M_EUROPE_DG_VIPOL_HPP
#define G4M_EUROPE_DG_VIPOL_HPP

#include <iostream>
#include <string>
#include <concepts>

using namespace std;

namespace g4m::misc::abstract {

    template<floating_point T>
    class VIpol {
    public:
        virtual ~VIpol() = default;

        // string representation
        [[nodiscard]] virtual string str() const noexcept = 0;

        // print to a stream
        friend ostream &operator<<(ostream &os, const VIpol &obj) {
            os << obj.str();
            return os;
        }

        virtual VIpol &operator+=(T) noexcept = 0;

        virtual VIpol &operator*=(T) noexcept = 0;
    };
}

#endif
