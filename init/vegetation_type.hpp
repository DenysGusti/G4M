#ifndef G4M_EUROPE_DG_VEGETATION_TYPE_HPP
#define G4M_EUROPE_DG_VEGETATION_TYPE_HPP

#include <string>
#include <unordered_map>

#include "../helper/enum_hasher.hpp"

using namespace std;
using namespace g4m::helper;

namespace g4m::init {
    /*
    vegetation type
    0 - no type (default)
    1 - Tropical Evergreen Forest/Woodland
    2 - Tropical Deciduous Forest/Woodland
    3 - Temperate Broadleaf Evergreen Forest/Woodland
    4 - Temperate Needleleaf Evergreen Forest/Woodland
    5 - Temperate Deciduous Forest/Woodland
    6 - Boreal Evergreen Forest/Woodland
    7 - Boreal Deciduous Forest/Woodland
    8 - Mixed Forest
    9 - Savanna
    10 - Grassland/Steppe
    11 - Dense Shrubland
    12 - Open Shrubland
    13 - Tundra
    14 - Desert
    15 - Polar desert/Rock/Ice
    */
    enum class VegetationType : uint8_t {
        NoType,
        TropicalEvergreenForest,
        TropicalDeciduousForest,
        TemperateBroadleafEvergreenForest,
        TemperateNeedleleafEvergreenForest,
        TemperateDeciduousForest,
        BorealEvergreenForest,
        BorealDeciduousForest,
        MixedForest,
        Savanna,
        Grassland,
        DenseShrubland,
        OpenShrubland,
        Tundra,
        Desert,
        PolarDesert
    };

    const unordered_map<VegetationType, string, EnumHasher> vegetationTypeName = {
            {VegetationType::TropicalEvergreenForest,            "Tropical Evergreen Forest"},
            {VegetationType::TropicalDeciduousForest,            "Tropical Deciduous Forest"},
            {VegetationType::TemperateBroadleafEvergreenForest,  "Temperate Broadleaf Evergreen Forest"},
            {VegetationType::TemperateNeedleleafEvergreenForest, "Temperate Needleleaf Evergreen Forest"},
            {VegetationType::TemperateDeciduousForest,           "Temperate Deciduous Forest"},
            {VegetationType::BorealEvergreenForest,              "Boreal Evergreen Forest"},
            {VegetationType::BorealDeciduousForest,              "Boreal Deciduous Forest"},
            {VegetationType::MixedForest,                        "Mixed Forest"},
            {VegetationType::Savanna,                            "Savanna"},
            {VegetationType::Grassland,                          "Grassland"},
            {VegetationType::DenseShrubland,                     "Dense Shrubland"},
            {VegetationType::OpenShrubland,                      "Open Shrubland"},
            {VegetationType::Tundra,                             "Tundra"},
            {VegetationType::Desert,                             "Desert"},
            {VegetationType::PolarDesert,                        "PolarDesert"}
    };
}

#endif
