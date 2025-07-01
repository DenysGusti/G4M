#ifndef G4M_EUROPE_DG_IIPOLM_HPP
#define G4M_EUROPE_DG_IIPOLM_HPP

#include <concepts>
#include <span>

using namespace std;

namespace g4m::misc::abstract {

    template<floating_point T>
    class IIpolM {
    public:
        virtual ~IIpolM() = default;

        virtual T ip(span<const T> i) const = 0;

        T operator()(const span<const T> i) const {
            return ip(i);
        };
    };
}

#endif
