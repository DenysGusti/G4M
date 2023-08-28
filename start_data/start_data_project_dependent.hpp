#ifndef G4M_EUROPE_DG_START_DATA_PROJECT_DEPENDENT_HPP
#define G4M_EUROPE_DG_START_DATA_PROJECT_DEPENDENT_HPP

#include "start_data.hpp"

namespace g4m::StartData {

    // Setting years for output
    void
    yearsToConsider(const uint16_t startYear, const uint16_t endYear, const uint16_t yearStep = 1) noexcept {
        years.reserve((endYear - startYear) / yearStep + 1);

        for (uint16_t year = startYear; year <= endYear; year += yearStep)
            years.insert(year);
    }

    // Setting regions for calculations
    void regionsToConsider() noexcept {
        regions.reserve(256);

        //  regions.insert(1);      // Australia&New Zealand
        //  regions.insert(3);      // Canada
        regions.insert(5);          // EU_Baltic
        regions.insert(6);          // EU_CentralEast
        regions.insert(7);          // EU_MidWest
        regions.insert(8);          // EU_North
        regions.insert(9);          // EU_South
        //  regions.insert(10);     // Former USSR
        regions.insert(17);         // RCEU (Rest of Central Europe)
        regions.insert(18);         // ROWE (Rest of Western Europe)
        //  regions.insert(12);     // JapanReg
        //  regions.insert(26);     // TurkeyReg
        //  regions.insert(27);     // USAReg
        //  regions.insert(2);      // BrazilReg
        //  regions.insert(4);      // ChinaReg
        //  regions.insert(11);     // IndiaReg
        //  regions.insert(13);     // MexicoReg
        //  regions.insert(14);     // MidEastNorthAfr
        //  regions.insert(15);     // Pacific_Islands
        //  regions.insert(16);     // RCAM (Rest of Central America)
        //  regions.insert(19);     // RSAM (Rest of South America)
        //  regions.insert(20);     // RSAS (Rest of South Asia)
        //  regions.insert(21);     // RSEA_OPA (Rest of South-East Asia Pacific)
        //  regions.insert(22);     // RSEA_PAC (Rest of South-East Asia Pacific)
        //  regions.insert(23);     // SouthAfrReg
        //  regions.insert(24);     // SouthKorea
        //  regions.insert(25);     // SubSaharanAfr (Sub-Saharan Africa)
    }

    // CountryRegMix to be considered
    void countryRegionsToConsider() noexcept {
        countryRegList.reserve(256);

        //    countryRegList.insert(0);
        countryRegList.insert(1);           // Austria
        countryRegList.insert(2);           // Belgium
        countryRegList.insert(3);           // Bulgaria
        countryRegList.insert(4);           // Croatia
        countryRegList.insert(5);           // Cyprus
        countryRegList.insert(6);           // Czech
        countryRegList.insert(7);           // Denmark
        countryRegList.insert(8);           // Estonia
        countryRegList.insert(9);           // Finland
        countryRegList.insert(10);          // France
        countryRegList.insert(11);          // Germany
        countryRegList.insert(12);          // Greece
        countryRegList.insert(13);          // Hungary
        countryRegList.insert(14);          // Ireland
        countryRegList.insert(15);          // Italy
        countryRegList.insert(16);          // Latvia
        countryRegList.insert(17);          // Lithuania
        countryRegList.insert(18);          // Luxembourg
        countryRegList.insert(19);          // Malta
        countryRegList.insert(20);          // "Netherlands"
        countryRegList.insert(21);          // Poland
        countryRegList.insert(22);          // Portugal
        countryRegList.insert(23);          // Romania
        countryRegList.insert(24);          // Slovakia
        countryRegList.insert(25);          // Slovenia
        countryRegList.insert(26);          // Spain
        countryRegList.insert(27);          // Sweden
        countryRegList.insert(28);          // UK
        //    countryRegList.insert(33);    // Former USSR
        //    countryRegList.insert(29);    // ANZ
        //    countryRegList.insert(30);    // Brazil
        //    countryRegList.insert(31);    // Canada
        //    countryRegList.insert(32);    // China
        //    countryRegList.insert(34);    // India
        //    countryRegList.insert(35);    // Japan
        //    countryRegList.insert(36);
        //    countryRegList.insert(37);
        //    countryRegList.insert(38);
        //    countryRegList.insert(39);
        //    countryRegList.insert(40);
        //    countryRegList.insert(41);    // "ROWE (Rest of Western Europe)" for Norway and Switzerland
        //    countryRegList.insert(42);
        //    countryRegList.insert(43);
        //    countryRegList.insert(44);
        //    countryRegList.insert(45);
        //    countryRegList.insert(46);
        //    countryRegList.insert(47);
        //    countryRegList.insert(48);
        //    countryRegList.insert(49);    // Turkey
        //    countryRegList.insert(50);    // US
    }

    // List of Annex-1 countries for FMcpol
    void countriesFmcpolToConsider() noexcept {
        countriesFmcpol.reserve(256);

        //    countriesFmcpol.insert(16);
        countriesFmcpol.insert(17);         // Austria
        countriesFmcpol.insert(20);         // Belgium
        countriesFmcpol.insert(24);         // Bulgaria
        //    countriesFmcpol.insert(28);   // Belarus
        //    countriesFmcpol.insert(39);
        //    countriesFmcpol.insert(41);   // Switzerland
        //    countriesFmcpol.insert(56);   // Cyprus
        countriesFmcpol.insert(57);         // Czech
        //    countriesFmcpol.insert(58);
        countriesFmcpol.insert(61);
        countriesFmcpol.insert(68);
        countriesFmcpol.insert(69);
        countriesFmcpol.insert(71);
        countriesFmcpol.insert(74);
        countriesFmcpol.insert(78);
        countriesFmcpol.insert(87);
        countriesFmcpol.insert(97);         // Croatia
        countriesFmcpol.insert(99);
        countriesFmcpol.insert(103);
        //    countriesFmcpol.insert(106);
        countriesFmcpol.insert(108);
        //    countriesFmcpol.insert(111);
        countriesFmcpol.insert(128);
        countriesFmcpol.insert(129);
        countriesFmcpol.insert(130);
        //    countriesFmcpol.insert(141);  // Malta
        countriesFmcpol.insert(161);
        //    countriesFmcpol.insert(162);  // Norway
        //    countriesFmcpol.insert(165);
        countriesFmcpol.insert(174);        // Poland
        countriesFmcpol.insert(177);        //  Portugal
        countriesFmcpol.insert(183);
        //   countriesFmcpol.insert(184);
        countriesFmcpol.insert(202);
        countriesFmcpol.insert(203);
        countriesFmcpol.insert(204);
        //    countriesFmcpol.insert(219);
        //    countriesFmcpol.insert(224);  // Ukraine
        //    countriesFmcpol.insert(227);
    }

    // countries to be considered
    // 192 out of 244 countries in which there is forest or environmental conditions are suitable for forestry
    void countriesToConsider() noexcept {
        countriesList.reserve(256);

        //    countriesList.insert(2);
        //    countriesList.insert(3);
        //    countriesList.insert(6);
        //    countriesList.insert(7);
        //    countriesList.insert(9);
        //    countriesList.insert(10);
        //    countriesList.insert(11);
        //    countriesList.insert(15);
        //    countriesList.insert(16);         // Australia

        countriesList.insert(17);               // Austria


        //    countriesList.insert(18);
        //    countriesList.insert(19);
        countriesList.insert(20);               // Belgium
        //    countriesList.insert(21);
        //    countriesList.insert(22);
        //    countriesList.insert(23);
        countriesList.insert(24);               // Bulgaria
        //    countriesList.insert(25);
        //    countriesList.insert(26);
        //    countriesList.insert(27);
        //    countriesList.insert(28);         // Belarus
        //    countriesList.insert(29);
        //    countriesList.insert(31);
        //    countriesList.insert(32);
        //    countriesList.insert(33);
        //    countriesList.insert(34);
        //    countriesList.insert(35);
        //    countriesList.insert(37);
        //    countriesList.insert(38);
        //    countriesList.insert(39);         // Canada
        //    countriesList.insert(41);         // Switzerland
        //    countriesList.insert(42);
        //    countriesList.insert(43);
        //    countriesList.insert(44);
        //    countriesList.insert(45);
        //    countriesList.insert(46);
        //    countriesList.insert(47);
        //    countriesList.insert(49);
        //    countriesList.insert(50);
        //    countriesList.insert(52);
        //    countriesList.insert(53);

        countriesList.insert(56);               // Cyprus

        countriesList.insert(57);               // Czech Republic
        countriesList.insert(58);               // Germany
        //    countriesList.insert(59);
        //    countriesList.insert(60);
        countriesList.insert(61);               // Denmark
        //    countriesList.insert(62);
        //    countriesList.insert(63);
        //    countriesList.insert(64);
        //    countriesList.insert(65);
        //    countriesList.insert(66);
        //    countriesList.insert(67);
        countriesList.insert(68);               // Spain
        countriesList.insert(69);               // Estonia
        //    countriesList.insert(70);
        countriesList.insert(71);               // Finland
        //    countriesList.insert(72);
        //    countriesList.insert(73);
        countriesList.insert(74);               // France
        //    countriesList.insert(75);
        //    countriesList.insert(77);
        countriesList.insert(78);               // UK
        //    countriesList.insert(79);
        //    countriesList.insert(80);
        //    countriesList.insert(82);
        //    countriesList.insert(83);
        //    countriesList.insert(84);
        //    countriesList.insert(85);
        //    countriesList.insert(86);
        countriesList.insert(87);               // Greece
        //    countriesList.insert(88);
        //    countriesList.insert(89);
        //    countriesList.insert(90);
        //    countriesList.insert(91);
        //    countriesList.insert(93);
        //    countriesList.insert(96);
        countriesList.insert(97);               // Croatia
        //    countriesList.insert(98);
        countriesList.insert(99);               // Hungary
        //    countriesList.insert(100);
        //    countriesList.insert(101);
        countriesList.insert(103);              // Ireland
        //    countriesList.insert(104);
        //    countriesList.insert(105);
        //    countriesList.insert(106);        // Iceland
        //    countriesList.insert(107);
        countriesList.insert(108);              // Italy
        //    countriesList.insert(109);
        //    countriesList.insert(110);
        //    countriesList.insert(111);        // Japan
        //    countriesList.insert(112);
        //    countriesList.insert(113);
        //    countriesList.insert(114);
        //    countriesList.insert(115);
        //    countriesList.insert(117);
        //    countriesList.insert(118);
        //    countriesList.insert(119);
        //    countriesList.insert(120);
        //    countriesList.insert(121);
        //    countriesList.insert(122);
        //    countriesList.insert(123);
        //    countriesList.insert(124);
        //    countriesList.insert(125);        // Liechtenstein
        //    countriesList.insert(126);
        //    countriesList.insert(127);
        countriesList.insert(128);              // Lithuania
        countriesList.insert(129);              // Luxembourg
        countriesList.insert(130);              // Latvia
        //    countriesList.insert(132);
        //    countriesList.insert(133);        // Monaco
        //    countriesList.insert(134);
        //    countriesList.insert(135);
        //    countriesList.insert(137);
        //    countriesList.insert(139);
        //    countriesList.insert(140);
        countriesList.insert(141);              // Malta
        //    countriesList.insert(142);
        //    countriesList.insert(144);
        //    countriesList.insert(145);
        //    countriesList.insert(146);
        //    countriesList.insert(147);
        //    countriesList.insert(149);
        //    countriesList.insert(150);
        //    countriesList.insert(151);
        //    countriesList.insert(152);
        //    countriesList.insert(154);
        //    countriesList.insert(155);
        //    countriesList.insert(158);
        //    countriesList.insert(159);
        countriesList.insert(161);              // Netherlands
        //    countriesList.insert(162);        // Norway
        //    countriesList.insert(163);
        //    countriesList.insert(165);        // New Zealand
        //    countriesList.insert(166);
        //    countriesList.insert(167);
        //    countriesList.insert(168);
        //    countriesList.insert(170);
        //    countriesList.insert(171);
        //    countriesList.insert(173);
        countriesList.insert(174);              // Poland
        //    countriesList.insert(175);
        //    countriesList.insert(176);
        countriesList.insert(177);              // Portugal
        //    countriesList.insert(178);
        //    countriesList.insert(179);
        //    countriesList.insert(181);
        //    countriesList.insert(182);
        countriesList.insert(183);              // Romania
        //    countriesList.insert(184);        // Russia
        //    countriesList.insert(185);
        //    countriesList.insert(186);
        //    countriesList.insert(187);
        //    countriesList.insert(188);
        //    countriesList.insert(189);
        //    countriesList.insert(192);
        //    countriesList.insert(193);
        //    countriesList.insert(194);
        //    countriesList.insert(195);
        //    countriesList.insert(196);
        //    countriesList.insert(197);
        //    countriesList.insert(199);
        //    countriesList.insert(200);
        //    countriesList.insert(201);
        countriesList.insert(202);              // Slovakia
        countriesList.insert(203);              // Slovenia
        countriesList.insert(204);              // Sweden
        //    countriesList.insert(205);
        //    countriesList.insert(207);
        //    countriesList.insert(208);
        //    countriesList.insert(209);
        //    countriesList.insert(210);
        //    countriesList.insert(211);
        //    countriesList.insert(212);
        //    countriesList.insert(214);
        //    countriesList.insert(217);
        //    countriesList.insert(218);
        //    countriesList.insert(219);        // Turkey
        //    countriesList.insert(222);
        //    countriesList.insert(223);
        //    countriesList.insert(224);        // Ukraine
        //    countriesList.insert(226);
        //    countriesList.insert(227);        // US
        //    countriesList.insert(228);
        //    countriesList.insert(230);
        //    countriesList.insert(231);
        //    countriesList.insert(234);
        //    countriesList.insert(235);
        //    countriesList.insert(238);
        //    countriesList.insert(239);
        //    countriesList.insert(240);
        //    countriesList.insert(241);
    }

    void setCountryData() noexcept {
        countriesNforCover.setListOfCountries(countriesList);
        countriesAfforHaYear.setListOfCountries(countriesList);

        countriesNforTotC.setListOfCountries(countriesList);
        countriesAfforCYear.setListOfCountries(countriesList);
        countriesAfforCYear_ab.setListOfCountries(countriesList);
        countriesAfforCYear_bl.setListOfCountries(countriesList);
        countriesAfforCYear_biom.setListOfCountries(countriesList);
        countriesAfforCYear_dom.setListOfCountries(countriesList);
        countriesAfforCYear_soil.setListOfCountries(countriesList);
//---------
        countriesOforCover.setListOfCountries(countriesList);
        countriesDeforHaYear.setListOfCountries(countriesList);

        countriesOfor_ab_C.setListOfCountries(countriesList);
        countriesOforC_biom.setListOfCountries(countriesList);
        countriesDeforCYear.setListOfCountries(countriesList);
        countriesDeforCYear_bl.setListOfCountries(countriesList);
        countriesDeforCYear_ab.setListOfCountries(countriesList);
        countriesDeforCYear_biom.setListOfCountries(countriesList);
        countriesDeforCYear_dom.setListOfCountries(countriesList);
        countriesDeforCYear_soil.setListOfCountries(countriesList);

//---------

        countriesWoodHarvestM3Year.setListOfCountries(countriesList);
        countriesWoodHarvestPlusM3Year.setListOfCountries(countriesList);
        countriesWoodHarvestFmM3Year.setListOfCountries(countriesList);
        countriesWoodHarvestDfM3Year.setListOfCountries(countriesList);
        countriesWoodLoosCYear.setListOfCountries(countriesList);
        countriesHarvLossesYear.setListOfCountries(countriesList);
//---------
        countriesManagedForHa.setListOfCountries(countriesList);
        countriesManagedCount.setListOfCountries(countriesList);

        countriesMAI.setListOfCountries(countriesList);
        countriesCAI.setListOfCountries(countriesList);
        countriesCAI_new.setListOfCountries(countriesList);

        countriesFM.setListOfCountries(countriesList);
        countriesFMbm.setListOfCountries(countriesList);
//---------
        //countriesWprod.setListOfCountries(countriesList);
//---------
        countriesProfit.setListOfCountries(countriesList);
    }

    // A list of countries and species where stumps can be harvested as compiled by Fulvio
    void setCountrySpecies() noexcept {
        stumpHarvCountrySpecies.reserve(18);
        stumpHarvCountrySpecies.emplace_back(58, 2);
        stumpHarvCountrySpecies.emplace_back(58, 3);
        stumpHarvCountrySpecies.emplace_back(69, 2);
        stumpHarvCountrySpecies.emplace_back(71, 2);
        stumpHarvCountrySpecies.emplace_back(74, 4);
        stumpHarvCountrySpecies.emplace_back(74, 5);
        stumpHarvCountrySpecies.emplace_back(78, 2);
        stumpHarvCountrySpecies.emplace_back(78, 3);
        stumpHarvCountrySpecies.emplace_back(99, 3);
        stumpHarvCountrySpecies.emplace_back(103, 2);
        stumpHarvCountrySpecies.emplace_back(103, 3);
        stumpHarvCountrySpecies.emplace_back(108, 5);
        stumpHarvCountrySpecies.emplace_back(130, 2);
        stumpHarvCountrySpecies.emplace_back(130, 3);
        stumpHarvCountrySpecies.emplace_back(177, 3);
        stumpHarvCountrySpecies.emplace_back(202, 3);
        stumpHarvCountrySpecies.emplace_back(204, 2);
        stumpHarvCountrySpecies.emplace_back(204, 3);
    }
//-----------------------------------------------------------------
}

#endif
