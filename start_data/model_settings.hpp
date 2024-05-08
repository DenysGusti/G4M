#ifndef G4M_EUROPE_DG_MODEL_SETTINGS_HPP
#define G4M_EUROPE_DG_MODEL_SETTINGS_HPP

#include "../init/settings.hpp"
#include "../init/coef_struct.hpp"

using namespace g4m::init;

// separate file to break circular dependency!
namespace g4m::StartData {
    Settings settings;
    CoefStruct coef;
}

#endif
