#ifndef G4M_EUROPE_DG_SPECIES_HPP
#define G4M_EUROPE_DG_SPECIES_HPP

#include <string>
#include <array>
#include <unordered_map>

#include "../diagnostics/enum_hasher.hpp"
#include "../increment/increment_tab.hpp"

using namespace std;
using namespace g4m::increment;

namespace g4m::init {
    // a tree species code
    // 1 - fir
    // 2 - spruce
    // 3 - pine
    // 4 - Pinus halepensis
    // 5 - birch / alder / Alnus incana
    // 6 - beech
    // 7 - oak
    // 8 - larch
    enum class Species : uint8_t {
        NoTree,
        Fir,
        Spruce,
        Pine,
        PinusHalepensis,
        Birch,
        Beech,
        Oak,
        Larch
    };

    const unordered_map<Species, string, EnumHasher> speciesName = {
            {Species::Fir,             "Fir"},
            {Species::Spruce,          "Spruce"},
            {Species::Pine,            "Pine"},
            {Species::PinusHalepensis, "Pinus Halepensis"},
            {Species::Birch,           "Birch"},
            {Species::Beech,           "Beech"},
            {Species::Oak,             "Oak"},
            {Species::Larch,           "Larch"},
    };
}

#endif
