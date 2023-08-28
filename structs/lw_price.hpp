#ifndef G4M_EUROPE_DG_LWPRICE_HPP
#define G4M_EUROPE_DG_LWPRICE_HPP

#include <string>

#include "../misc/concrete/ipol.hpp"

using namespace std;
using namespace g4m::misc::concrete;

namespace g4m::structs {

    struct LWPrice {
        int REG_ID = 0;
        string REG;
        string SCENARIO;
        Ipol<float> LP;
        Ipol<float> WP;
    };
}

#endif
