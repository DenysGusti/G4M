#ifndef G4M_EUROPE_DG_IIPOL_HPP
#define G4M_EUROPE_DG_IIPOL_HPP

#include <concepts>

using namespace std;

namespace g4m::misc::abstract {

    template<floating_point T>
    class IIpol {
    public:
        virtual ~IIpol() = default;

        virtual T ip(T i) const = 0;

        T operator()(const T i) const {
            return ip(i);
        };
    };
}

#endif
