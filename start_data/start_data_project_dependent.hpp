#ifndef G4M_EUROPE_DG_START_DATA_PROJECT_DEPENDENT_HPP
#define G4M_EUROPE_DG_START_DATA_PROJECT_DEPENDENT_HPP

#include "start_data.hpp"

namespace g4m::StartData {

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

    // List of Annex-1 countries for FMcpol
    [[nodiscard]] unordered_set<uint8_t> countriesFmcpolToConsider() noexcept {
        unordered_set<uint8_t> fun_countriesFmcpol;
        fun_countriesFmcpol.reserve(256);

        //    fun_countriesFmcpol.insert(16);
        fun_countriesFmcpol.insert(17);         // Austria
        fun_countriesFmcpol.insert(20);         // Belgium
        fun_countriesFmcpol.insert(24);         // Bulgaria
        //    fun_countriesFmcpol.insert(28);   // Belarus
        //    fun_countriesFmcpol.insert(39);
        //    fun_countriesFmcpol.insert(41);   // Switzerland
        //    fun_countriesFmcpol.insert(56);   // Cyprus
        fun_countriesFmcpol.insert(57);         // Czech
        //    fun_countriesFmcpol.insert(58);
        fun_countriesFmcpol.insert(61);
        fun_countriesFmcpol.insert(68);
        fun_countriesFmcpol.insert(69);
        fun_countriesFmcpol.insert(71);
        fun_countriesFmcpol.insert(74);
        fun_countriesFmcpol.insert(78);
        fun_countriesFmcpol.insert(87);
        fun_countriesFmcpol.insert(97);         // Croatia
        fun_countriesFmcpol.insert(99);
        fun_countriesFmcpol.insert(103);
        //    fun_countriesFmcpol.insert(106);
        fun_countriesFmcpol.insert(108);
        //    fun_countriesFmcpol.insert(111);
        fun_countriesFmcpol.insert(128);
        fun_countriesFmcpol.insert(129);
        fun_countriesFmcpol.insert(130);
        //    fun_countriesFmcpol.insert(141);  // Malta
        fun_countriesFmcpol.insert(161);
        //    fun_countriesFmcpol.insert(162);  // Norway
        //    fun_countriesFmcpol.insert(165);
        fun_countriesFmcpol.insert(174);        // Poland
        fun_countriesFmcpol.insert(177);        //  Portugal
        fun_countriesFmcpol.insert(183);
        //   fun_countriesFmcpol.insert(184);
        fun_countriesFmcpol.insert(202);
        fun_countriesFmcpol.insert(203);
        fun_countriesFmcpol.insert(204);
        //    fun_countriesFmcpol.insert(219);
        //    fun_countriesFmcpol.insert(224);  // Ukraine
        //    fun_countriesFmcpol.insert(227);
        return fun_countriesFmcpol;
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
        fun_countriesList.insert(141);              // Malta
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

    void setCountryData(const unordered_set<uint8_t> &countries_list) noexcept {
        countriesNforCover.setListOfCountries(countries_list);
        countriesAfforHaYear.setListOfCountries(countries_list);

        countriesNforTotC.setListOfCountries(countries_list);
        countriesAfforCYear.setListOfCountries(countries_list);
        countriesAfforCYear_ab.setListOfCountries(countries_list);
        countriesAfforCYear_bl.setListOfCountries(countries_list);
        countriesAfforCYear_biom.setListOfCountries(countries_list);
        countriesAfforCYear_dom.setListOfCountries(countries_list);
        countriesAfforCYear_soil.setListOfCountries(countries_list);
//---------
        countriesOforCover.setListOfCountries(countries_list);
        countriesDeforHaYear.setListOfCountries(countries_list);

        countriesOfor_ab_C.setListOfCountries(countries_list);
        countriesOforC_biom.setListOfCountries(countries_list);
        countriesDeforCYear.setListOfCountries(countries_list);
        countriesDeforCYear_bl.setListOfCountries(countries_list);
        countriesDeforCYear_ab.setListOfCountries(countries_list);
        countriesDeforCYear_biom.setListOfCountries(countries_list);
        countriesDeforCYear_dom.setListOfCountries(countries_list);
        countriesDeforCYear_soil.setListOfCountries(countries_list);

//---------

        countriesWoodHarvestM3Year.setListOfCountries(countries_list);
        countriesWoodHarvestPlusM3Year.setListOfCountries(countries_list);
        countriesWoodHarvestFmM3Year.setListOfCountries(countries_list);
        countriesWoodHarvestDfM3Year.setListOfCountries(countries_list);
        countriesWoodLoosCYear.setListOfCountries(countries_list);
        countriesHarvLossesYear.setListOfCountries(countries_list);
//---------
        countriesManagedForHa.setListOfCountries(countries_list);
        countriesManagedCount.setListOfCountries(countries_list);

        countriesMAI.setListOfCountries(countries_list);
        countriesCAI.setListOfCountries(countries_list);
        countriesCAI_new.setListOfCountries(countries_list);

        countriesFM.setListOfCountries(countries_list);
        countriesFMbm.setListOfCountries(countries_list);
//---------
        //countriesWprod.setListOfCountries(countries_list);
//---------
        countriesProfit.setListOfCountries(countries_list);
    }

    // A list of countries and species where stumps can be harvested as compiled by Fulvio
    [[nodiscard]] vector<CountrySpecies> setCountrySpecies() noexcept {
        vector<CountrySpecies> fun_stumpHarvCountrySpecies;
        fun_stumpHarvCountrySpecies.reserve(18);

        fun_stumpHarvCountrySpecies.emplace_back(58, 2);
        fun_stumpHarvCountrySpecies.emplace_back(58, 3);
        fun_stumpHarvCountrySpecies.emplace_back(69, 2);
        fun_stumpHarvCountrySpecies.emplace_back(71, 2);
        fun_stumpHarvCountrySpecies.emplace_back(74, 4);
        fun_stumpHarvCountrySpecies.emplace_back(74, 5);
        fun_stumpHarvCountrySpecies.emplace_back(78, 2);
        fun_stumpHarvCountrySpecies.emplace_back(78, 3);
        fun_stumpHarvCountrySpecies.emplace_back(99, 3);
        fun_stumpHarvCountrySpecies.emplace_back(103, 2);
        fun_stumpHarvCountrySpecies.emplace_back(103, 3);
        fun_stumpHarvCountrySpecies.emplace_back(108, 5);
        fun_stumpHarvCountrySpecies.emplace_back(130, 2);
        fun_stumpHarvCountrySpecies.emplace_back(130, 3);
        fun_stumpHarvCountrySpecies.emplace_back(177, 3);
        fun_stumpHarvCountrySpecies.emplace_back(202, 3);
        fun_stumpHarvCountrySpecies.emplace_back(204, 2);
        fun_stumpHarvCountrySpecies.emplace_back(204, 3);
        return fun_stumpHarvCountrySpecies;
    }
//-----------------------------------------------------------------
}

#endif
