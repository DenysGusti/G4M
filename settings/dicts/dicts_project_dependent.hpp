#ifndef G4M_EUROPE_DG_DICTS_PROJECT_DEPENDENT_HPP
#define G4M_EUROPE_DG_DICTS_PROJECT_DEPENDENT_HPP

#include <unordered_set>
#include <set>
#include "../../init/species.hpp"

using namespace std;

using namespace g4m::init;

namespace g4m::Dicts {
    // Setting years for output
    [[nodiscard]] unordered_set<uint16_t>
    yearsToConsider(const uint16_t startYear, const uint16_t endYear, const uint16_t yearStep = 1) noexcept {
        unordered_set<uint16_t> fun_years;
        fun_years.reserve((endYear - startYear) / yearStep + 1);

        for (uint16_t year = startYear; year <= endYear; year += yearStep)
            fun_years.insert(year);

        return fun_years;
    }

    // Setting regions for calculations
    [[nodiscard]] unordered_set<uint8_t> regionsToConsider() noexcept {
        unordered_set<uint8_t> fun_regions;
        fun_regions.reserve(256);

        //  fun_fun_regions.insert(1);      // Australia&New Zealand
        //  fun_fun_regions.insert(3);      // Canada
        fun_regions.insert(5);          // EU_Baltic
        fun_regions.insert(6);          // EU_CentralEast
        fun_regions.insert(7);          // EU_MidWest
        fun_regions.insert(8);          // EU_North
        fun_regions.insert(9);          // EU_South
        //  fun_regions.insert(10);     // Former USSR
        fun_regions.insert(17);         // RCEU (Rest of Central Europe)
        fun_regions.insert(18);         // ROWE (Rest of Western Europe)
        //  fun_regions.insert(12);     // JapanReg
        //  fun_regions.insert(26);     // TurkeyReg
        //  fun_regions.insert(27);     // USAReg
        //  fun_regions.insert(2);      // BrazilReg
        //  fun_regions.insert(4);      // ChinaReg
        //  fun_regions.insert(11);     // IndiaReg
        //  fun_regions.insert(13);     // MexicoReg
        //  fun_regions.insert(14);     // MidEastNorthAfr
        //  fun_regions.insert(15);     // Pacific_Islands
        //  fun_regions.insert(16);     // RCAM (Rest of Central America)
        //  fun_regions.insert(19);     // RSAM (Rest of South America)
        //  fun_regions.insert(20);     // RSAS (Rest of South Asia)
        //  fun_regions.insert(21);     // RSEA_OPA (Rest of South-East Asia Pacific)
        //  fun_regions.insert(22);     // RSEA_PAC (Rest of South-East Asia Pacific)
        //  fun_regions.insert(23);     // SouthAfrReg
        //  fun_regions.insert(24);     // SouthKorea
        //  fun_regions.insert(25);     // SubSaharanAfr (Sub-Saharan Africa)

        return fun_regions;
    }

    // CountryRegMix to be considered
    [[nodiscard]] unordered_set<uint8_t> countryRegionsToConsider() noexcept {
        unordered_set<uint8_t> fun_countryRegList;
        fun_countryRegList.reserve(256);

        //    fun_countryRegList.insert(0);
        fun_countryRegList.insert(1);           // Austria
        fun_countryRegList.insert(2);           // Belgium
        fun_countryRegList.insert(3);           // Bulgaria
        fun_countryRegList.insert(4);           // Croatia
        fun_countryRegList.insert(5);           // Cyprus
        fun_countryRegList.insert(6);           // Czech
        fun_countryRegList.insert(7);           // Denmark
        fun_countryRegList.insert(8);           // Estonia
        fun_countryRegList.insert(9);           // Finland
        fun_countryRegList.insert(10);          // France
        fun_countryRegList.insert(11);          // Germany
        fun_countryRegList.insert(12);          // Greece
        fun_countryRegList.insert(13);          // Hungary
        fun_countryRegList.insert(14);          // Ireland
        fun_countryRegList.insert(15);          // Italy
        fun_countryRegList.insert(16);          // Latvia
        fun_countryRegList.insert(17);          // Lithuania
        fun_countryRegList.insert(18);          // Luxembourg
        fun_countryRegList.insert(19);          // Malta
        fun_countryRegList.insert(20);          // Netherlands
        fun_countryRegList.insert(21);          // Poland
        fun_countryRegList.insert(22);          // Portugal
        fun_countryRegList.insert(23);          // Romania
        fun_countryRegList.insert(24);          // Slovakia
        fun_countryRegList.insert(25);          // Slovenia
        fun_countryRegList.insert(26);          // Spain
        fun_countryRegList.insert(27);          // Sweden
        fun_countryRegList.insert(28);          // UK
        //    fun_countryRegList.insert(33);    // Former USSR
        //    fun_countryRegList.insert(29);    // ANZ
        //    fun_countryRegList.insert(30);    // Brazil
        //    fun_countryRegList.insert(31);    // Canada
        //    fun_countryRegList.insert(32);    // China
        //    fun_countryRegList.insert(34);    // India
        //    fun_countryRegList.insert(35);    // Japan
        //    fun_countryRegList.insert(36);
        //    fun_countryRegList.insert(37);
        //    fun_countryRegList.insert(38);
        //    fun_countryRegList.insert(39);
        //    fun_countryRegList.insert(40);
        //    fun_countryRegList.insert(41);    // "ROWE (Rest of Western Europe)" for Norway and Switzerland
        //    fun_countryRegList.insert(42);
        //    fun_countryRegList.insert(43);
        //    fun_countryRegList.insert(44);
        //    fun_countryRegList.insert(45);
        //    fun_countryRegList.insert(46);
        //    fun_countryRegList.insert(47);
        //    fun_countryRegList.insert(48);
        //    fun_countryRegList.insert(49);    // Turkey
        //    fun_countryRegList.insert(50);    // US

        return fun_countryRegList;
    }

    // List of Annex-1 countries for FMCPol
    [[nodiscard]] unordered_set<uint8_t> countriesFmCPolToConsider() noexcept {
        unordered_set<uint8_t> fun_countriesFmCPol;
        fun_countriesFmCPol.reserve(256);

        //    fun_countriesFmCPol.insert(16);
        fun_countriesFmCPol.insert(17);         // Austria
        fun_countriesFmCPol.insert(20);         // Belgium
        fun_countriesFmCPol.insert(24);         // Bulgaria
        //    fun_countriesFmCPol.insert(28);   // Belarus
        //    fun_countriesFmCPol.insert(39);
        //    fun_countriesFmCPol.insert(41);   // Switzerland
        //    fun_countriesFmCPol.insert(56);   // Cyprus
        fun_countriesFmCPol.insert(57);         // Czech
        //    fun_countriesFmCPol.insert(58);
        fun_countriesFmCPol.insert(61);
        fun_countriesFmCPol.insert(68);
        fun_countriesFmCPol.insert(69);
        fun_countriesFmCPol.insert(71);
        fun_countriesFmCPol.insert(74);
        fun_countriesFmCPol.insert(78);
        fun_countriesFmCPol.insert(87);
        fun_countriesFmCPol.insert(97);         // Croatia
        fun_countriesFmCPol.insert(99);
        fun_countriesFmCPol.insert(103);
        //    fun_countriesFmCPol.insert(106);
        fun_countriesFmCPol.insert(108);
        //    fun_countriesFmCPol.insert(111);
        fun_countriesFmCPol.insert(128);
        fun_countriesFmCPol.insert(129);
        fun_countriesFmCPol.insert(130);
        //    fun_countriesFmCPol.insert(141);  // Malta
        fun_countriesFmCPol.insert(161);
        //    fun_countriesFmCPol.insert(162);  // Norway
        //    fun_countriesFmCPol.insert(165);
        fun_countriesFmCPol.insert(174);        // Poland
        fun_countriesFmCPol.insert(177);        //  Portugal
        fun_countriesFmCPol.insert(183);
        //   fun_countriesFmCPol.insert(184);
        fun_countriesFmCPol.insert(202);
        fun_countriesFmCPol.insert(203);
        fun_countriesFmCPol.insert(204);
        //    fun_countriesFmCPol.insert(219);
        //    fun_countriesFmCPol.insert(224);  // Ukraine
        //    fun_countriesFmCPol.insert(227);

        return fun_countriesFmCPol;
    }

    // countries to be considered
    // 192 out of 244 countries in which there is forest or environmental conditions are suitable for forestry
    [[nodiscard]] unordered_set<uint8_t> countriesToConsider() noexcept {
        unordered_set<uint8_t> fun_countriesList;
        fun_countriesList.reserve(256);

        //    fun_countriesList.insert(2);
        //    fun_countriesList.insert(3);
        //    fun_countriesList.insert(6);
        //    fun_countriesList.insert(7);
        //    fun_countriesList.insert(9);
        //    fun_countriesList.insert(10);
        //    fun_countriesList.insert(11);
        //    fun_countriesList.insert(15);
        //    fun_countriesList.insert(16);         // Australia

        fun_countriesList.insert(17);               // Austria

        //    fun_countriesList.insert(18);
        //    fun_countriesList.insert(19);
        fun_countriesList.insert(20);               // Belgium
        //    fun_countriesList.insert(21);
        //    fun_countriesList.insert(22);
        //    fun_countriesList.insert(23);
        fun_countriesList.insert(24);               // Bulgaria
        //    fun_countriesList.insert(25);
        //    fun_countriesList.insert(26);
        //    fun_countriesList.insert(27);
        //    fun_countriesList.insert(28);         // Belarus
        //    fun_countriesList.insert(29);
        //    fun_countriesList.insert(31);
        //    fun_countriesList.insert(32);
        //    fun_countriesList.insert(33);
        //    fun_countriesList.insert(34);
        //    fun_countriesList.insert(35);
        //    fun_countriesList.insert(37);
        //    fun_countriesList.insert(38);
        //    fun_countriesList.insert(39);         // Canada
        //    fun_countriesList.insert(41);         // Switzerland
        //    fun_countriesList.insert(42);
        //    fun_countriesList.insert(43);
        //    fun_countriesList.insert(44);
        //    fun_countriesList.insert(45);
        //    fun_countriesList.insert(46);
        //    fun_countriesList.insert(47);
        //    fun_countriesList.insert(49);
        //    fun_countriesList.insert(50);
        //    fun_countriesList.insert(52);
        //    fun_countriesList.insert(53);

        fun_countriesList.insert(56);               // Cyprus

        fun_countriesList.insert(57);               // Czech Republic
        fun_countriesList.insert(58);               // Germany
        //    fun_countriesList.insert(59);
        //    fun_countriesList.insert(60);
        fun_countriesList.insert(61);               // Denmark
        //    fun_countriesList.insert(62);
        //    fun_countriesList.insert(63);
        //    fun_countriesList.insert(64);
        //    fun_countriesList.insert(65);
        //    fun_countriesList.insert(66);
        //    fun_countriesList.insert(67);
        fun_countriesList.insert(68);               // Spain
        fun_countriesList.insert(69);               // Estonia
        //    fun_countriesList.insert(70);
        fun_countriesList.insert(71);               // Finland
        //    fun_countriesList.insert(72);
        //    fun_countriesList.insert(73);
        fun_countriesList.insert(74);               // France
        //    fun_countriesList.insert(75);
        //    fun_countriesList.insert(77);
        fun_countriesList.insert(78);               // UK
        //    fun_countriesList.insert(79);
        //    fun_countriesList.insert(80);
        //    fun_countriesList.insert(82);
        //    fun_countriesList.insert(83);
        //    fun_countriesList.insert(84);
        //    fun_countriesList.insert(85);
        //    fun_countriesList.insert(86);
        fun_countriesList.insert(87);               // Greece
        //    fun_countriesList.insert(88);
        //    fun_countriesList.insert(89);
        //    fun_countriesList.insert(90);
        //    fun_countriesList.insert(91);
        //    fun_countriesList.insert(93);
        //    fun_countriesList.insert(96);
        fun_countriesList.insert(97);               // Croatia
        //    fun_countriesList.insert(98);
        fun_countriesList.insert(99);               // Hungary
        //    fun_countriesList.insert(100);
        //    fun_countriesList.insert(101);
        fun_countriesList.insert(103);              // Ireland
        //    fun_countriesList.insert(104);
        //    fun_countriesList.insert(105);
        //    fun_countriesList.insert(106);        // Iceland
        //    fun_countriesList.insert(107);
        fun_countriesList.insert(108);              // Italy
        //    fun_countriesList.insert(109);
        //    fun_countriesList.insert(110);
        //    fun_countriesList.insert(111);        // Japan
        //    fun_countriesList.insert(112);
        //    fun_countriesList.insert(113);
        //    fun_countriesList.insert(114);
        //    fun_countriesList.insert(115);
        //    fun_countriesList.insert(117);
        //    fun_countriesList.insert(118);
        //    fun_countriesList.insert(119);
        //    fun_countriesList.insert(120);
        //    fun_countriesList.insert(121);
        //    fun_countriesList.insert(122);
        //    fun_countriesList.insert(123);
        //    fun_countriesList.insert(124);
        //    fun_countriesList.insert(125);        // Liechtenstein
        //    fun_countriesList.insert(126);
        //    fun_countriesList.insert(127);
        fun_countriesList.insert(128);              // Lithuania
        fun_countriesList.insert(129);              // Luxembourg
        fun_countriesList.insert(130);              // Latvia
        //    fun_countriesList.insert(132);
        //    fun_countriesList.insert(133);        // Monaco
        //    fun_countriesList.insert(134);
        //    fun_countriesList.insert(135);
        //    fun_countriesList.insert(137);
        //    fun_countriesList.insert(139);
        //    fun_countriesList.insert(140);
        //    fun_countriesList.insert(141);              // Malta
        //    fun_countriesList.insert(142);
        //    fun_countriesList.insert(144);
        //    fun_countriesList.insert(145);
        //    fun_countriesList.insert(146);
        //    fun_countriesList.insert(147);
        //    fun_countriesList.insert(149);
        //    fun_countriesList.insert(150);
        //    fun_countriesList.insert(151);
        //    fun_countriesList.insert(152);
        //    fun_countriesList.insert(154);
        //    fun_countriesList.insert(155);
        //    fun_countriesList.insert(158);
        //    fun_countriesList.insert(159);
        fun_countriesList.insert(161);              // Netherlands
        //    fun_countriesList.insert(162);        // Norway
        //    fun_countriesList.insert(163);
        //    fun_countriesList.insert(165);        // New Zealand
        //    fun_countriesList.insert(166);
        //    fun_countriesList.insert(167);
        //    fun_countriesList.insert(168);
        //    fun_countriesList.insert(170);
        //    fun_countriesList.insert(171);
        //    fun_countriesList.insert(173);
        fun_countriesList.insert(174);              // Poland
        //    fun_countriesList.insert(175);
        //    fun_countriesList.insert(176);
        fun_countriesList.insert(177);              // Portugal
        //    fun_countriesList.insert(178);
        //    fun_countriesList.insert(179);
        //    fun_countriesList.insert(181);
        //    fun_countriesList.insert(182);
        fun_countriesList.insert(183);              // Romania
        //    fun_countriesList.insert(184);        // Russia
        //    fun_countriesList.insert(185);
        //    fun_countriesList.insert(186);
        //    fun_countriesList.insert(187);
        //    fun_countriesList.insert(188);
        //    fun_countriesList.insert(189);
        //    fun_countriesList.insert(192);
        //    fun_countriesList.insert(193);
        //    fun_countriesList.insert(194);
        //    fun_countriesList.insert(195);
        //    fun_countriesList.insert(196);
        //    fun_countriesList.insert(197);
        //    fun_countriesList.insert(199);
        //    fun_countriesList.insert(200);
        //    fun_countriesList.insert(201);
        fun_countriesList.insert(202);              // Slovakia
        fun_countriesList.insert(203);              // Slovenia
        fun_countriesList.insert(204);              // Sweden
        //    fun_countriesList.insert(205);
        //    fun_countriesList.insert(207);
        //    fun_countriesList.insert(208);
        //    fun_countriesList.insert(209);
        //    fun_countriesList.insert(210);
        //    fun_countriesList.insert(211);
        //    fun_countriesList.insert(212);
        //    fun_countriesList.insert(214);
        //    fun_countriesList.insert(217);
        //    fun_countriesList.insert(218);
        //    fun_countriesList.insert(219);        // Turkey
        //    fun_countriesList.insert(222);
        //    fun_countriesList.insert(223);
        //    fun_countriesList.insert(224);        // Ukraine
        //    fun_countriesList.insert(226);
        //    fun_countriesList.insert(227);        // US
        //    fun_countriesList.insert(228);
        //    fun_countriesList.insert(230);
        //    fun_countriesList.insert(231);
        //    fun_countriesList.insert(234);
        //    fun_countriesList.insert(235);
        //    fun_countriesList.insert(238);
        //    fun_countriesList.insert(239);
        //    fun_countriesList.insert(240);
        //    fun_countriesList.insert(241);

        return fun_countriesList;
    }

    // A list of countries and species where stumps can be harvested as compiled by Fulvio
    [[nodiscard]] set <pair<uint8_t, Species>> setCountrySpecies() noexcept {
        set<pair<uint8_t, Species> > fun_stumpHarvCountrySpecies;

        fun_stumpHarvCountrySpecies.emplace(58, Species::Spruce);
        fun_stumpHarvCountrySpecies.emplace(58, Species::Pine);
        fun_stumpHarvCountrySpecies.emplace(69, Species::Spruce);
        fun_stumpHarvCountrySpecies.emplace(71, Species::Spruce);
        fun_stumpHarvCountrySpecies.emplace(74, Species::PinusHalepensis);
        fun_stumpHarvCountrySpecies.emplace(74, Species::Birch);
        fun_stumpHarvCountrySpecies.emplace(78, Species::Spruce);
        fun_stumpHarvCountrySpecies.emplace(78, Species::Pine);
        fun_stumpHarvCountrySpecies.emplace(99, Species::Pine);
        fun_stumpHarvCountrySpecies.emplace(103, Species::Spruce);
        fun_stumpHarvCountrySpecies.emplace(103, Species::Pine);
        fun_stumpHarvCountrySpecies.emplace(108, Species::Birch);
        fun_stumpHarvCountrySpecies.emplace(130, Species::Spruce);
        fun_stumpHarvCountrySpecies.emplace(130, Species::Pine);
        fun_stumpHarvCountrySpecies.emplace(177, Species::Pine);
        fun_stumpHarvCountrySpecies.emplace(202, Species::Pine);
        fun_stumpHarvCountrySpecies.emplace(204, Species::Spruce);
        fun_stumpHarvCountrySpecies.emplace(204, Species::Pine);

        return fun_stumpHarvCountrySpecies;
    }

    [[nodiscard]] unordered_map<string, string, StringHash, equal_to<> > initFileNames() {
        unordered_map<string, string, StringHash, equal_to<> > fun_fileNames = {
                {"lp0",                "output_glo4myk_LandRent_myk_01042023.csv"},
                {"wp0",                "output_glo4myk_SupplyWood_myk_price_01042023.csv"},
                // 18 Jan 2021 NL corrected according to NFAP / a share of wood reported to FAOSTAT comes from non-forest sources (communication with NL experts in Dec 2020)
                {"wd0",                "output_glo4myk_SupplyWood_CntDet_01042023.csv"},
                {"rd0",                "output_glo4myk_Residues_myk_01042023.csv"}, // in m^3

                {"lp",                 "output_glo4myk_LandRent_myk_04072023.csv"},
                {"wp",                 "output_glo4myk_SupplyWood_myk_price_04072023.csv"},
                {"wd",                 "output_glo4myk_SupplyWood_CntDet_04072023.csv"},
                {"rd",                 "output_glo4myk_Residues_myk_04072023.csv"}, // in th t

                // could be empty

                // GLOBIOM land cell scale
                {"gl_0",               "GLOBIOM_LC_MCS_BIOCLIMA_GRSCor9_devANBEXT_02052023_newClasses_updated_2000_2020.csv"},
                {"gl",                 "GLOBIOM_LC_MCS_BIOCLIMA_GRSCor9_FinJune11_11062023_newClasses_updated_CORRECTED.csv"},
                {"gl_country_0",       "GLOBIOM_LC_CountryLevel_Ref2070_05042023.csv"},
                {"gl_country",         "GLOBIOM_LC_CountryLevel_CTP_S2_CC_04072023_04072023.csv"},

                {"co2p",               "output_glo4myk_CO2price_myk_04072023.csv"},
                {"nuts2",              "nuts2_xy_05.csv"},

                {"maic",               "shifters_g4m_v2.csv"},  // MAI climate shifters
                {"disturbance",        "disturbances_g4m_m3ha_29062023.csv"},  // disturbance damage
                {"disturbanceExtreme", "disturbances_g4m_m3ha_29062023.csv"},  // disturbance damage

                // As ageStructData_EUCLIMIT2020_03012021, but IR and DE are corrected according to the countries' comments in 2022
                {"ageStruct",          "ageStructData_EUCLIMIT2020_15092022.csv"},
                // as _30032023 but unfccc managed forest area increased for a number of countries
                {"dat",                "b2_euclimit6_bioclima_feu2020_nai2010_11052023.csv"},
        };
        return fun_fileNames;
    }
}

#endif
