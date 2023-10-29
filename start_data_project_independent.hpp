#ifndef G4M_EUROPE_DG_START_DATA_PROJECT_INDEPENDENT_HPP
#define G4M_EUROPE_DG_START_DATA_PROJECT_INDEPENDENT_HPP

#include <future>

#include "start_data_project_dependent.hpp"
#include "../data_io/reading.hpp"
#include "../data_io/printing.hpp"
#include "../increment/dima.hpp"

using namespace g4m;
using namespace g4m::DataIO::reading;
using namespace g4m::DataIO::printing;

namespace g4m::StartData {

    [[nodiscard]] unordered_map<uint8_t, string> setIdCountryGLOBIOM() noexcept {
        unordered_map<uint8_t, string> fun_idCountryGLOBIOM;
        fun_idCountryGLOBIOM.reserve(256);

        fun_idCountryGLOBIOM[6] = "Albania";
        fun_idCountryGLOBIOM[63] = "Algeria";
        fun_idCountryGLOBIOM[3] = "Angola";
        fun_idCountryGLOBIOM[10] = "Argentina";
        fun_idCountryGLOBIOM[11] = "Armenia";
        fun_idCountryGLOBIOM[16] = "Australia";
        fun_idCountryGLOBIOM[17] = "Austria";
        fun_idCountryGLOBIOM[18] = "Azerbaijan";
        fun_idCountryGLOBIOM[26] = "Bahamas";
        fun_idCountryGLOBIOM[25] = "Bahrain";
        fun_idCountryGLOBIOM[23] = "Bangladesh";
        fun_idCountryGLOBIOM[28] = "Belarus";
        fun_idCountryGLOBIOM[20] = "Belgium";
        fun_idCountryGLOBIOM[29] = "Belize";
        fun_idCountryGLOBIOM[21] = "Benin";
        fun_idCountryGLOBIOM[35] = "Bhutan";
        fun_idCountryGLOBIOM[31] = "Bolivia";
        fun_idCountryGLOBIOM[27] = "BosniaHerzg";
        fun_idCountryGLOBIOM[37] = "Botswana";
        fun_idCountryGLOBIOM[32] = "Brazil";
        fun_idCountryGLOBIOM[34] = "BruneiDarsm";
        fun_idCountryGLOBIOM[24] = "Bulgaria";
        fun_idCountryGLOBIOM[22] = "BurkinaFaso";
        fun_idCountryGLOBIOM[19] = "Burundi";
        fun_idCountryGLOBIOM[115] = "Cambodia";
        fun_idCountryGLOBIOM[45] = "Cameroon";
        fun_idCountryGLOBIOM[39] = "Canada";
        fun_idCountryGLOBIOM[51] = "CapeVerde";
        fun_idCountryGLOBIOM[38] = "CentAfrRep";
        fun_idCountryGLOBIOM[209] = "Chad";
        fun_idCountryGLOBIOM[42] = "Chile";
        fun_idCountryGLOBIOM[43] = "China";
        fun_idCountryGLOBIOM[49] = "Colombia";
        fun_idCountryGLOBIOM[50] = "Comoros";
        fun_idCountryGLOBIOM[46] = "CongoDemR";
        fun_idCountryGLOBIOM[47] = "CongoRep";
        fun_idCountryGLOBIOM[52] = "CostaRica";
        fun_idCountryGLOBIOM[44] = "CotedIvoire";
        fun_idCountryGLOBIOM[97] = "Croatia";
        fun_idCountryGLOBIOM[53] = "Cuba";
        fun_idCountryGLOBIOM[56] = "Cyprus";
        fun_idCountryGLOBIOM[57] = "CzechRep";
        fun_idCountryGLOBIOM[61] = "Denmark";
        fun_idCountryGLOBIOM[59] = "Djibouti";
        fun_idCountryGLOBIOM[62] = "DominicanRp";
        fun_idCountryGLOBIOM[64] = "Ecuador";
        fun_idCountryGLOBIOM[65] = "Egypt";
        fun_idCountryGLOBIOM[195] = "ElSalvador";
        fun_idCountryGLOBIOM[86] = "EqGuinea";
        fun_idCountryGLOBIOM[66] = "Eritrea";
        fun_idCountryGLOBIOM[69] = "Estonia";
        fun_idCountryGLOBIOM[70] = "Ethiopia";
        fun_idCountryGLOBIOM[73] = "FalklandIs";
        fun_idCountryGLOBIOM[72] = "FijiIslands";
        fun_idCountryGLOBIOM[71] = "Finland";
        fun_idCountryGLOBIOM[74] = "France";
        fun_idCountryGLOBIOM[91] = "FrGuiana";
        fun_idCountryGLOBIOM[180] = "FrPolynesia";
        fun_idCountryGLOBIOM[77] = "Gabon";
        fun_idCountryGLOBIOM[84] = "Gambia";
        fun_idCountryGLOBIOM[79] = "Georgia";
        fun_idCountryGLOBIOM[58] = "Germany";
        fun_idCountryGLOBIOM[80] = "Ghana";
        fun_idCountryGLOBIOM[87] = "Greece";
        fun_idCountryGLOBIOM[89] = "Greenland";
        fun_idCountryGLOBIOM[83] = "Guadeloupe";
        fun_idCountryGLOBIOM[90] = "Guatemala";
        fun_idCountryGLOBIOM[82] = "Guinea";
        fun_idCountryGLOBIOM[85] = "GuineaBissau";
        fun_idCountryGLOBIOM[93] = "Guyana";
        fun_idCountryGLOBIOM[98] = "Haiti";
        fun_idCountryGLOBIOM[96] = "Honduras";
        fun_idCountryGLOBIOM[99] = "Hungary";
        fun_idCountryGLOBIOM[106] = "Iceland";
        fun_idCountryGLOBIOM[101] = "India";
        fun_idCountryGLOBIOM[100] = "Indonesia";
        fun_idCountryGLOBIOM[104] = "Iran";
        fun_idCountryGLOBIOM[105] = "Iraq";
        fun_idCountryGLOBIOM[103] = "Ireland";
        fun_idCountryGLOBIOM[107] = "Israel";
        fun_idCountryGLOBIOM[108] = "Italy";
        fun_idCountryGLOBIOM[109] = "Jamaica";
        fun_idCountryGLOBIOM[111] = "Japan";
        fun_idCountryGLOBIOM[110] = "Jordan";
        fun_idCountryGLOBIOM[112] = "Kazakhstan";
        fun_idCountryGLOBIOM[113] = "Kenya";
        fun_idCountryGLOBIOM[176] = "KoreaDPRp";
        fun_idCountryGLOBIOM[118] = "KoreaRep";
        fun_idCountryGLOBIOM[119] = "Kuwait";
        fun_idCountryGLOBIOM[114] = "Kyrgyzstan";
        fun_idCountryGLOBIOM[120] = "Laos";
        fun_idCountryGLOBIOM[130] = "Latvia";
        fun_idCountryGLOBIOM[121] = "Lebanon";
        fun_idCountryGLOBIOM[127] = "Lesotho";
        fun_idCountryGLOBIOM[122] = "Liberia";
        fun_idCountryGLOBIOM[123] = "Libya";
        fun_idCountryGLOBIOM[128] = "Lithuania";
        fun_idCountryGLOBIOM[129] = "Luxembourg";
        fun_idCountryGLOBIOM[139] = "Macedonia";
        fun_idCountryGLOBIOM[135] = "Madagascar";
        fun_idCountryGLOBIOM[151] = "Malawi";
        fun_idCountryGLOBIOM[152] = "Malaysia";
        fun_idCountryGLOBIOM[140] = "Mali";
        fun_idCountryGLOBIOM[141] = "Malta";
        fun_idCountryGLOBIOM[147] = "Mauritania";
        fun_idCountryGLOBIOM[150] = "Mauritius";
        fun_idCountryGLOBIOM[137] = "Mexico";
        fun_idCountryGLOBIOM[134] = "MoldovaRep";
        fun_idCountryGLOBIOM[144] = "Mongolia";
        fun_idCountryGLOBIOM[132] = "Morocco";
        fun_idCountryGLOBIOM[146] = "Mozambique";
        fun_idCountryGLOBIOM[142] = "Myanmar";
        fun_idCountryGLOBIOM[154] = "Namibia";
        fun_idCountryGLOBIOM[163] = "Nepal";
        fun_idCountryGLOBIOM[161] = "Netherlands";
        fun_idCountryGLOBIOM[155] = "NewCaledonia";
        fun_idCountryGLOBIOM[165] = "NewZealand";
        fun_idCountryGLOBIOM[159] = "Nicaragua";
        fun_idCountryGLOBIOM[156] = "Niger";
        fun_idCountryGLOBIOM[158] = "Nigeria";
        fun_idCountryGLOBIOM[162] = "Norway";
        fun_idCountryGLOBIOM[166] = "Oman";
        fun_idCountryGLOBIOM[167] = "Pakistan";
        fun_idCountryGLOBIOM[172] = "Palestin";
        fun_idCountryGLOBIOM[168] = "Panama";
        fun_idCountryGLOBIOM[173] = "PapuaNGuin";
        fun_idCountryGLOBIOM[178] = "Paraguay";
        fun_idCountryGLOBIOM[170] = "Peru";
        fun_idCountryGLOBIOM[171] = "Philippines";
        fun_idCountryGLOBIOM[174] = "Poland";
        fun_idCountryGLOBIOM[177] = "Portugal";
        fun_idCountryGLOBIOM[175] = "PuertoRico";
        fun_idCountryGLOBIOM[181] = "Qatar";
        fun_idCountryGLOBIOM[182] = "Reunion";
        fun_idCountryGLOBIOM[183] = "Romania";
        fun_idCountryGLOBIOM[184] = "RussianFed";
        fun_idCountryGLOBIOM[185] = "Rwanda";
        fun_idCountryGLOBIOM[237] = "Samoa";
        fun_idCountryGLOBIOM[186] = "SaudiArabia";
        fun_idCountryGLOBIOM[188] = "Senegal";
        fun_idCountryGLOBIOM[199] = "Serbia-Monte";
        fun_idCountryGLOBIOM[194] = "SierraLeone";
        fun_idCountryGLOBIOM[189] = "Singapore";
        fun_idCountryGLOBIOM[202] = "Slovakia";
        fun_idCountryGLOBIOM[203] = "Slovenia";
        fun_idCountryGLOBIOM[193] = "SolomonIs";
        fun_idCountryGLOBIOM[197] = "Somalia";
        fun_idCountryGLOBIOM[239] = "SouthAfrica";
        fun_idCountryGLOBIOM[68] = "Spain";
        fun_idCountryGLOBIOM[126] = "SriLanka";
        fun_idCountryGLOBIOM[187] = "Sudan";
        fun_idCountryGLOBIOM[201] = "Suriname";
        fun_idCountryGLOBIOM[205] = "Swaziland";
        fun_idCountryGLOBIOM[204] = "Sweden";
        fun_idCountryGLOBIOM[41] = "Switzerland";
        fun_idCountryGLOBIOM[207] = "Syria";
        fun_idCountryGLOBIOM[212] = "Tajikistan";
        fun_idCountryGLOBIOM[222] = "Tanzania";
        fun_idCountryGLOBIOM[211] = "Thailand";
        fun_idCountryGLOBIOM[215] = "TimorLeste";
        fun_idCountryGLOBIOM[210] = "Togo";
        fun_idCountryGLOBIOM[217] = "TrinidadTob";
        fun_idCountryGLOBIOM[218] = "Tunisia";
        fun_idCountryGLOBIOM[219] = "Turkey";
        fun_idCountryGLOBIOM[214] = "Turkmenistan";
        fun_idCountryGLOBIOM[223] = "Uganda";
        fun_idCountryGLOBIOM[78] = "UK";
        fun_idCountryGLOBIOM[224] = "Ukraine";
        fun_idCountryGLOBIOM[9] = "UntdArabEm";
        fun_idCountryGLOBIOM[226] = "Uruguay";
        fun_idCountryGLOBIOM[227] = "USA";
        fun_idCountryGLOBIOM[228] = "Uzbekistan";
        fun_idCountryGLOBIOM[235] = "Vanuatu";
        fun_idCountryGLOBIOM[231] = "Venezuela";
        fun_idCountryGLOBIOM[234] = "VietNam";
        fun_idCountryGLOBIOM[67] = "WestSahara";
        fun_idCountryGLOBIOM[238] = "Yemen";
        fun_idCountryGLOBIOM[240] = "Zambia";
        fun_idCountryGLOBIOM[241] = "Zimbabwe";

        return fun_idCountryGLOBIOM;
    }

    [[nodiscard]] unordered_map<string, uint8_t, string_hash, equal_to<> > setCountryGLOBIOMId() noexcept {
        unordered_map<string, uint8_t, string_hash, equal_to<> > fun_countryGLOBIOMId;
        fun_countryGLOBIOMId.reserve(256);

        fun_countryGLOBIOMId["Albania"] = 6;
        fun_countryGLOBIOMId["Algeria"] = 63;
        fun_countryGLOBIOMId["Angola"] = 3;
        fun_countryGLOBIOMId["Argentina"] = 10;
        fun_countryGLOBIOMId["Armenia"] = 11;
        fun_countryGLOBIOMId["Australia"] = 16;
        fun_countryGLOBIOMId["Austria"] = 17;
        fun_countryGLOBIOMId["Azerbaijan"] = 18;
        fun_countryGLOBIOMId["Bahamas"] = 26;
        fun_countryGLOBIOMId["Bahrain"] = 25;
        fun_countryGLOBIOMId["Bangladesh"] = 23;
        fun_countryGLOBIOMId["Belarus"] = 28;
        fun_countryGLOBIOMId["Belgium"] = 20;
        fun_countryGLOBIOMId["Belize"] = 29;
        fun_countryGLOBIOMId["Benin"] = 21;
        fun_countryGLOBIOMId["Bhutan"] = 35;
        fun_countryGLOBIOMId["Bolivia"] = 31;
        fun_countryGLOBIOMId["BosniaHerzg"] = 27;
        fun_countryGLOBIOMId["Botswana"] = 37;
        fun_countryGLOBIOMId["Brazil"] = 32;
        fun_countryGLOBIOMId["BruneiDarsm"] = 34;
        fun_countryGLOBIOMId["Bulgaria"] = 24;
        fun_countryGLOBIOMId["BurkinaFaso"] = 22;
        fun_countryGLOBIOMId["Burundi"] = 19;
        fun_countryGLOBIOMId["Cambodia"] = 115;
        fun_countryGLOBIOMId["Cameroon"] = 45;
        fun_countryGLOBIOMId["Canada"] = 39;
        fun_countryGLOBIOMId["CapeVerde"] = 51;
        fun_countryGLOBIOMId["CentAfrRep"] = 38;
        fun_countryGLOBIOMId["Chad"] = 209;
        fun_countryGLOBIOMId["Chile"] = 42;
        fun_countryGLOBIOMId["China"] = 43;
        fun_countryGLOBIOMId["Colombia"] = 49;
        fun_countryGLOBIOMId["Comoros"] = 50;
        fun_countryGLOBIOMId["CongoDemR"] = 46;
        fun_countryGLOBIOMId["CongoRep"] = 47;
        fun_countryGLOBIOMId["CostaRica"] = 52;
        fun_countryGLOBIOMId["CotedIvoire"] = 44;
        fun_countryGLOBIOMId["Croatia"] = 97;
        fun_countryGLOBIOMId["Cuba"] = 53;
        fun_countryGLOBIOMId["Cyprus"] = 56;
        fun_countryGLOBIOMId["CzechRep"] = 57;
        fun_countryGLOBIOMId["Denmark"] = 61;
        fun_countryGLOBIOMId["Djibouti"] = 59;
        fun_countryGLOBIOMId["DominicanRp"] = 62;
        fun_countryGLOBIOMId["Ecuador"] = 64;
        fun_countryGLOBIOMId["Egypt"] = 65;
        fun_countryGLOBIOMId["ElSalvador"] = 195;
        fun_countryGLOBIOMId["EqGuinea"] = 86;
        fun_countryGLOBIOMId["Eritrea"] = 66;
        fun_countryGLOBIOMId["Estonia"] = 69;
        fun_countryGLOBIOMId["Ethiopia"] = 70;
        fun_countryGLOBIOMId["FalklandIs"] = 73;
        fun_countryGLOBIOMId["FijiIslands"] = 72;
        fun_countryGLOBIOMId["Finland"] = 71;
        fun_countryGLOBIOMId["France"] = 74;
        fun_countryGLOBIOMId["FrGuiana"] = 91;
        fun_countryGLOBIOMId["FrPolynesia"] = 180;
        fun_countryGLOBIOMId["Gabon"] = 77;
        fun_countryGLOBIOMId["Gambia"] = 84;
        fun_countryGLOBIOMId["Georgia"] = 79;
        fun_countryGLOBIOMId["Germany"] = 58;
        fun_countryGLOBIOMId["Ghana"] = 80;
        fun_countryGLOBIOMId["Greece"] = 87;
        fun_countryGLOBIOMId["Greenland"] = 89;
        fun_countryGLOBIOMId["Guadeloupe"] = 83;
        fun_countryGLOBIOMId["Guatemala"] = 90;
        fun_countryGLOBIOMId["Guinea"] = 82;
        fun_countryGLOBIOMId["GuineaBissau"] = 85;
        fun_countryGLOBIOMId["Guyana"] = 93;
        fun_countryGLOBIOMId["Haiti"] = 98;
        fun_countryGLOBIOMId["Honduras"] = 96;
        fun_countryGLOBIOMId["Hungary"] = 99;
        fun_countryGLOBIOMId["Iceland"] = 106;
        fun_countryGLOBIOMId["India"] = 101;
        fun_countryGLOBIOMId["Indonesia"] = 100;
        fun_countryGLOBIOMId["Iran"] = 104;
        fun_countryGLOBIOMId["Iraq"] = 105;
        fun_countryGLOBIOMId["Ireland"] = 103;
        fun_countryGLOBIOMId["Israel"] = 107;
        fun_countryGLOBIOMId["Italy"] = 108;
        fun_countryGLOBIOMId["Jamaica"] = 109;
        fun_countryGLOBIOMId["Japan"] = 111;
        fun_countryGLOBIOMId["Jordan"] = 110;
        fun_countryGLOBIOMId["Kazakhstan"] = 112;
        fun_countryGLOBIOMId["Kenya"] = 113;
        fun_countryGLOBIOMId["KoreaDPRp"] = 176;
        fun_countryGLOBIOMId["KoreaRep"] = 118;
        fun_countryGLOBIOMId["Kuwait"] = 119;
        fun_countryGLOBIOMId["Kyrgyzstan"] = 114;
        fun_countryGLOBIOMId["Laos"] = 120;
        fun_countryGLOBIOMId["Latvia"] = 130;
        fun_countryGLOBIOMId["Lebanon"] = 121;
        fun_countryGLOBIOMId["Lesotho"] = 127;
        fun_countryGLOBIOMId["Liberia"] = 122;
        fun_countryGLOBIOMId["Libya"] = 123;
        fun_countryGLOBIOMId["Lithuania"] = 128;
        fun_countryGLOBIOMId["Luxembourg"] = 129;
        fun_countryGLOBIOMId["Macedonia"] = 139;
        fun_countryGLOBIOMId["Madagascar"] = 135;
        fun_countryGLOBIOMId["Malawi"] = 151;
        fun_countryGLOBIOMId["Malaysia"] = 152;
        fun_countryGLOBIOMId["Mali"] = 140;
        fun_countryGLOBIOMId["Malta"] = 141;
        fun_countryGLOBIOMId["Mauritania"] = 147;
        fun_countryGLOBIOMId["Mauritius"] = 150;
        fun_countryGLOBIOMId["Mexico"] = 137;
        fun_countryGLOBIOMId["MoldovaRep"] = 134;
        fun_countryGLOBIOMId["Mongolia"] = 144;
        fun_countryGLOBIOMId["Morocco"] = 132;
        fun_countryGLOBIOMId["Mozambique"] = 146;
        fun_countryGLOBIOMId["Myanmar"] = 142;
        fun_countryGLOBIOMId["Namibia"] = 154;
        fun_countryGLOBIOMId["Nepal"] = 163;
        fun_countryGLOBIOMId["Netherlands"] = 161;
        fun_countryGLOBIOMId["NewCaledonia"] = 155;
        fun_countryGLOBIOMId["NewZealand"] = 165;
        fun_countryGLOBIOMId["Nicaragua"] = 159;
        fun_countryGLOBIOMId["Niger"] = 156;
        fun_countryGLOBIOMId["Nigeria"] = 158;
        fun_countryGLOBIOMId["Norway"] = 162;
        fun_countryGLOBIOMId["Oman"] = 166;
        fun_countryGLOBIOMId["Pakistan"] = 167;
        fun_countryGLOBIOMId["Palestin"] = 172;
        fun_countryGLOBIOMId["Panama"] = 168;
        fun_countryGLOBIOMId["PapuaNGuin"] = 173;
        fun_countryGLOBIOMId["Paraguay"] = 178;
        fun_countryGLOBIOMId["Peru"] = 170;
        fun_countryGLOBIOMId["Philippines"] = 171;
        fun_countryGLOBIOMId["Poland"] = 174;
        fun_countryGLOBIOMId["Portugal"] = 177;
        fun_countryGLOBIOMId["PuertoRico"] = 175;
        fun_countryGLOBIOMId["Qatar"] = 181;
        fun_countryGLOBIOMId["Reunion"] = 182;
        fun_countryGLOBIOMId["Romania"] = 183;
        fun_countryGLOBIOMId["RussianFed"] = 184;
        fun_countryGLOBIOMId["Rwanda"] = 185;
        fun_countryGLOBIOMId["Samoa"] = 237;
        fun_countryGLOBIOMId["SaudiArabia"] = 186;
        fun_countryGLOBIOMId["Senegal"] = 188;
        fun_countryGLOBIOMId["Serbia-Monte"] = 199;
        fun_countryGLOBIOMId["SierraLeone"] = 194;
        fun_countryGLOBIOMId["Singapore"] = 189;
        fun_countryGLOBIOMId["Slovakia"] = 202;
        fun_countryGLOBIOMId["Slovenia"] = 203;
        fun_countryGLOBIOMId["SolomonIs"] = 193;
        fun_countryGLOBIOMId["Somalia"] = 197;
        fun_countryGLOBIOMId["SouthAfrica"] = 239;
        fun_countryGLOBIOMId["Spain"] = 68;
        fun_countryGLOBIOMId["SriLanka"] = 126;
        fun_countryGLOBIOMId["Sudan"] = 187;
        fun_countryGLOBIOMId["Suriname"] = 201;
        fun_countryGLOBIOMId["Swaziland"] = 205;
        fun_countryGLOBIOMId["Sweden"] = 204;
        fun_countryGLOBIOMId["Switzerland"] = 41;
        fun_countryGLOBIOMId["Syria"] = 207;
        fun_countryGLOBIOMId["Tajikistan"] = 212;
        fun_countryGLOBIOMId["Tanzania"] = 222;
        fun_countryGLOBIOMId["Thailand"] = 211;
        fun_countryGLOBIOMId["TimorLeste"] = 215;
        fun_countryGLOBIOMId["Togo"] = 210;
        fun_countryGLOBIOMId["TrinidadTob"] = 217;
        fun_countryGLOBIOMId["Tunisia"] = 218;
        fun_countryGLOBIOMId["Turkey"] = 219;
        fun_countryGLOBIOMId["Turkmenistan"] = 214;
        fun_countryGLOBIOMId["Uganda"] = 223;
        fun_countryGLOBIOMId["UK"] = 78;
        fun_countryGLOBIOMId["Ukraine"] = 224;
        fun_countryGLOBIOMId["UntdArabEm"] = 9;
        fun_countryGLOBIOMId["Uruguay"] = 226;
        fun_countryGLOBIOMId["USA"] = 227;
        fun_countryGLOBIOMId["Uzbekistan"] = 228;
        fun_countryGLOBIOMId["Vanuatu"] = 235;
        fun_countryGLOBIOMId["Venezuela"] = 231;
        fun_countryGLOBIOMId["VietNam"] = 234;
        fun_countryGLOBIOMId["WestSahara"] = 67;
        fun_countryGLOBIOMId["Yemen"] = 238;
        fun_countryGLOBIOMId["Zambia"] = 240;
        fun_countryGLOBIOMId["Zimbabwe"] = 241;

        return fun_countryGLOBIOMId;
    }

    [[nodiscard]] CountryData setCountriesWoodProdStat() noexcept {
        CountryData fun_countriesWoodProdStat;

        for (size_t i = 0; i < woodProdEUStats.size(); ++i)
            for (size_t j = 0; j < woodProdEUStats[0].size(); ++j)
                // Malta is in countryNwp[18], there are no wood production data for Malta
                fun_countriesWoodProdStat.setVal(countryNwp[i + (i >= 18)], 1990 + j, woodProdEUStats[i][j]);

        return fun_countriesWoodProdStat;
    }

    [[nodiscard]] CountryData setCountriesFmEmission_unfccc() noexcept {
        CountryData fun_countriesFmEmission_unfccc;

        for (size_t i = 0; i < fmEmission_unfccc_CRF.size(); ++i)
            for (size_t j = 0; j < fmEmission_unfccc_CRF[0].size(); ++j)
                fun_countriesFmEmission_unfccc.setVal(eu28OrderCode[i], 1990 + j, fmEmission_unfccc_CRF[i][j]);

        return fun_countriesFmEmission_unfccc;
    }

    void calcAvgFM_sink_stat() noexcept {
        for (size_t i = 0; i < fmEmission_unfccc_CRF.size(); ++i) {
            int count = 0;
            double fmSinkSumTmp = 0;
            for (size_t j = 0; j < min(adjustLength, fmEmission_unfccc_CRF[0].size()) &&
                               coef.bYear - 1990 + j < fmEmission_unfccc_CRF[0].size(); ++j) {
                fmSinkSumTmp += fmEmission_unfccc_CRF[i][coef.bYear - 1990 + j] * -1;
                ++count;
            }
            FM_sink_stat[eu28OrderCode[i] - 1] = count > 0 ? fmSinkSumTmp * 1'000 / count : 0; // GgCO2/year
        }
    }

    [[nodiscard]] vector<DataStruct> filterPlots(const span<const DataStruct> plots) noexcept {
        vector<DataStruct> filteredPlots;
        filteredPlots.reserve(plots.size());

        for (const auto &plot: plots)
            if (regions.contains(plot.polesReg) && countriesList.contains(plot.country))
                filteredPlots.push_back(plot);

        return filteredPlots;
    }

    void convertUnitsDatamaps() noexcept {
        for (auto &[scenario, datamap]: woodDemandScenarios)
            for (auto &[id, ipol]: datamap)
                for (auto &[key, value]: ipol.data)
                    value *= 1'000;
        for (auto &[scenario, datamap]: residuesDemandScenarios)
            for (auto &[id, ipol]: datamap)
                for (auto &[key, value]: ipol.data)
                    value *= 250;
    }

    void correctNUTS2Data(const span<const DataStruct> plots) noexcept {
        for (const auto &plot: plots)
            // Test only some regions and some countries
            // plot.protect.data.at(2000) == 0)
        {
            // locate the struct with asID == asID within the country
            string_view countryISO = countryOrderISO2[countryCodeOrder[plot.country - 1]];
            if (countryISO == "GB")
                countryISO = "UK";

            auto findNeighbour = [&](const uint32_t radius) -> optional<string> {
                nuts2grid.setNeighNum(radius, radius);
                auto neighbours = nuts2grid.getNeighValues(plot.x, plot.y);
                auto it_nearbyCountry = ranges::find_if(neighbours, [countryISO](const string_view el) {
                    return el.substr(0, 2) == countryISO;
                });
                if (it_nearbyCountry == neighbours.end())
                    return {};
                return *it_nearbyCountry;
            };

            const uint32_t MAX_RADIUS = 3;  // 3 for remote islands

            if (auto it_nuts2 = nuts2id.find({plot.x, plot.y}); it_nuts2 != nuts2id.end()) {
                auto &[coords, NUTS2] = *it_nuts2;

                if (NUTS2.substr(0, 2) != countryISO)
                    for (uint32_t radius = 1; radius <= MAX_RADIUS; ++radius) {
                        optional<string> opt_neighbour = findNeighbour(radius);
                        if (opt_neighbour) {
                            NUTS2 = *opt_neighbour;
                            DEBUG("x = {}, y = {}, NUTS2 = {}, countryISO = {}, *opt_neighbour = {}, radius = {}",
                                  plot.x, plot.y, NUTS2, countryISO, *opt_neighbour, radius);
                            break;
                        }

                        if (radius == MAX_RADIUS)
                            ERROR("!No x = {}, y = {}, NUTS2 = {}, countryISO = {}",
                                  plot.x, plot.y, NUTS2, countryISO);
                    }

            } else
                for (uint32_t radius = 1; radius <= MAX_RADIUS; ++radius) {
                    optional<string> opt_neighbour = findNeighbour(radius);
                    if (opt_neighbour) {
                        nuts2id[{plot.x, plot.y}] = *opt_neighbour;
                        DEBUG("x = {}, y = {}, countryISO = {}, *opt_neighbour = {}, radius = {}",
                              plot.x, plot.y, countryISO, *opt_neighbour, radius);
                        break;
                    }

                    if (radius == MAX_RADIUS)
                        ERROR("!No x = {}, y = {}, countryISO = {}", plot.x, plot.y, countryISO);
                }
        }
    }

    // Setup forest increment table
    [[nodiscard]] vector<IncrementTab> defineSpecies() {
        vector<IncrementTab> fun_species;
        fun_species.reserve(8);

        // fir [0]
        fun_species.emplace_back(
                array{-0.4562, -0.7403, -1.0772, 1.4803, 0.6713, 300., -0.2151, -0.9929, 0.5, 0.2, -0.7642, 0.3156,
                      -0.4, 0.4468, 0.1425, 0., 0., 0., 0.25, -1., -2., -0.5, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8,
                      0.002, 2., 0.01, 0.5, 24.82935, 0.60708, -0.0212, 2.41308, -0.48246, 16.11, 17.78, -0.01436,
                      0.37424, -1.52401, 2.282, 1.2718, -0.7707, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // spruce [1]
        fun_species.emplace_back(
                array{0., -0.9082, -0.2728, 0.6483, 209.6889, 300., 1.8536, 0.4811, 0., 0.9, -0.143, -0.5915, -0.4,
                      0.4507, 0.3713, 0., 0., 0., 0.1, 1., -2., -0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2.,
                      0.01, 0.5, 22.58866, 0.6168, -0.02102, 2.4176, -0.35816, 16.11, 17.78, -0.01436, 0.37424,
                      -1.52401, 2.282, 1.2718, -0.7707, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // pine [2]
        fun_species.emplace_back(array{-0.3835, -0.2416, -1.7576, 1.1638,                 // k
                                       170., 114.343, -2.804, 1.044,                      // tMax
                                       0., 0.9, -0.8242, -0.4273,                         // maxDens0
                                       -0.4, -1.476, 4.283, -0.3, 3.610, -1.071,          // maxDens1
                                       0.1, 1., -2.0670, -0.3028,                         // maxDens2
                                       0.5,                                               // maxDens3
                                       1.5, 150., 0.01, 0.5, 0.5,                         // imul0
                                       0.8, 0.002, 2., 0.01, 0.5,                         // imul0
                                       22.09082, 0.62065, -0.01965, 1.50614, -0.25346,    // h
                                       22.70, 16.56,                                      // d0
                                       -0.01068, 0.24754, -1.81373,                       // d1
                                       1.0945, 0.0999, -1.6033,                           // d2
                                       1.6,                                               // dmul
                                       0.95},                                             // iCrit
                                 5,       // maiMax
                                 0.25,    // maiStep
                                 600,     // tMax
                                 1,       // tStep
                                 0.25,    // sdNatStep
                                 1.5,     // sdTabMax
                                 0.25,    // sdTabStep
                                 1);      // timeframe
        // Pinus halepensis [3]
        fun_species.emplace_back(
                array{-0.3, -0.306, -2.052, 1.673, 150., 130., 1.898, -1.141, 0.92, 0.07, -4.25, 6.168, -0.4, 0.93237,
                      -0.00468, 0., 0., 0., 0.25, -1., -2., -0.5, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2., 0.01,
                      0.5, 26.59488, 0.62839, -0.02023, 1.05953, -0.03489, 18.7252, 46.3833, -0.2643, 14.1425, -0.6368,
                      0.8948, 0.000422233, -4.9625, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // birch / alder / Alnus incana [4]
        fun_species.emplace_back(
                array{0., -0.7422, -0.54, 0.5719, 136.958, 100., 0.2972, -0.7543, 0., 0.9, -0.953, -0.9236, -0.4, 1.052,
                      0.108, 0., 0., 0., 0.1, 1., -2.5, -0.4, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2., 0.01, 0.5,
                      23.23597, 0.44554, -0.02485, 1.36973, -0.42941, 13.61, 10.69, -0.02688, 0.24196, -0.7015, 1.337,
                      0.0708, -2.1515, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // beech [5]
        fun_species.emplace_back(
                array{0., -0.5998, -0.2467, 0.7674, 245.5516, 100., 2.6345, -0.8978, 0.69135, 0., 0., 0., -0.03177, 0.,
                      0., 0., 0., 0., -0.875, 1., 0., 0., 0.5, 1.5, 300., 0.01, 0.5, 0.5, 0.8, 0.001, 2., 0.01, 0.5,
                      21.29459, 0.48722, -0.01967, 1.81479, -0.29145, 30.707, 7.008, -0.01051, -0.19793, 0.2977, 1.423,
                      1.025, -16.85, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // oak [6]
        fun_species.emplace_back(
                array{0., -0.6, -0.4419, 0.3179, 16.6688, 300., -0.6066, -1.1243, 0.7, 0.3, -0.4339, 0.5288, -0.4,
                      2.01561, -0.07354, 0., 0., 0., 0.1, 1., -3.5, 0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2.,
                      0.01, 0.5, 21.26281, 0.51987, -0.01901, 1.34081, -0.10979, -7.511, 41.689, -0.02201, 0.58055,
                      1.72465, 3.6757, 1.7544, 0.3264, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // larch [7]
        fun_species.emplace_back(
                array{0., -0.388, -0.01226, 0.85934, 195.3724, 600., 0.9883, 1.0784, 0., 0.9, -2.1347, -0.3437, -0.4,
                      1.3238, 0.4061, 0., 0., 0., 0.1, 1., -2.5, -0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2.,
                      0.01, 0.5, 23.63487, 0.50281, -0.01557, 1.16199, -0.18673, 25.196, 9.118, -0.01376, 0.64637,
                      -0.79909, 1.0817, 0.1667, -0.9408, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);

        return fun_species;
    }

    // Setup forest management parameters similar for all countries (cells)
    void setupFMP() noexcept {
        sws.data[10] = 0;
        sws.data[30] = 0.6;

        // Georg's recommendation
        // 7: 0
        // 25: 1 - HarvestingLosesCountry
        // 50: 1 - (0.7 * HarvestingLosesCountry)
        // This is for hle, for the thinning (hlv) multiply the values of
        // harvestable biomass with 0.8.
        hlv.data[0] = 0;
        hlv.data[27] = 0;  // Testing!

        hle.data[0] = 0;
        hle.data[27] = 0;  // Testing!

        sdMaxH.data[0] = 1;
        sdMinH.data[0] = 1;

        // diameter, stocking volume [tC stem-wood/ha], share of harvest (0 - 1)
        cov.data[{0, 2, 0.3}] = 4;
        cov.data[{40, 30, 0.2}] = 2;

        // We allow thinning from Dbh 3 cm to allow more energy wood (discussion with Fulvio 14 May 2020)
        dov.data[{0, 0, 0}] = 0;
        dov.data[{3, 5, 0.001}] = 0;  // Testing!
        dov.data[{10, 20, 0.01}] = 1;  // Testing!

        coe.data[{0, 2}] = 3;
        coe.data[{40, 30}] = 1;

        doe.data[{15, 10}] = 0;
        doe.data[{16, 11}] = 1;

        ffsws.overwrite(sws);
        ffhlv.overwrite(hlv);
        ffhle.overwrite(hle);
        ffsdMinH.overwrite(sdMinH);
        ffsdMaxH.overwrite(sdMaxH);

        ffcov = FFIpolM<double>{cov};
        ffcoe = FFIpolM<double>{coe};
        ffdov = FFIpolM<double>{dov};
        ffdoe = FFIpolM<double>{doe};
    }

    void correctMAI(const span<DataStruct> plots) {
        for (auto &plot: plots)
            // Test only some regions and some countries
            if (plot.protect.data.at(2000) == 0) {  // if there is no lerp, why not simple map then?
                // forest with specified age structure
                plot.MAIE.data[2000] *= maiCoefficients[plot.country];
                plot.MAIN.data[2000] *= maiCoefficients[plot.country];
            }
    }

    [[nodiscard]] array<double, numberOfCountries> calculateAverageMAI(const span<const DataStruct> plots) {
        INFO("calculating average MAI");
        array<double, numberOfCountries> fun_MAI_CountryUprotect{};
        array<double, numberOfCountries> forestAreaCountry{};
        for (const auto &plot: plots)
            if (plot.protect.data.at(2000) == 0) {
                double forestArea0 = plot.landArea * 100 * clamp(plot.forest, 0., 1.);
                if (forestArea0 > 0) {
                    // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
                    fun_MAI_CountryUprotect[plot.country - 1] += plot.MAIE.data.at(2000) * forestArea0;
                    forestAreaCountry[plot.country - 1] += forestArea0;
                }
            }

        for (auto &&[MAI, area]: rv::zip(fun_MAI_CountryUprotect, forestAreaCountry))
            if (area > 0)
                MAI /= area;

        for (size_t i = 0; i < fun_MAI_CountryUprotect.size(); ++i)
            if (fun_MAI_CountryUprotect[i] > 0)
                DEBUG("fun_MAI_CountryUprotect[{}] = {}", i, fun_MAI_CountryUprotect[i]);

        INFO("calculated average MAI");
        return fun_MAI_CountryUprotect;
    }

    [[nodiscard]] unordered_set<uint32_t> initPlotsSimuID(const span<const DataStruct> plots) noexcept {
        unordered_set<uint32_t> fun_plotsSimuID;
        fun_plotsSimuID.reserve(plots.size());

        for (const auto &plot: plots)
            fun_plotsSimuID.insert(plot.simuID);

        return fun_plotsSimuID;
    }

    [[nodiscard]] map<pair<uint32_t, uint32_t>, uint32_t>
    initPlotsXY_SimuID(const span<const DataStruct> plots) noexcept {
        map<pair<uint32_t, uint32_t>, uint32_t> fun_plotsXYSimuID;

        for (const auto &plot: plots)
            fun_plotsXYSimuID[{plot.x, plot.y}] = plot.simuID;

        return fun_plotsXYSimuID;
    }

    void correctAndConvertCO2Prices(heterDatamapScenariosType &datamapScenarios) {
        for (auto &[scenario, datamap]: datamapScenarios)
            for (auto &[id, ipol]: datamap)
                for (auto &[year, CO2Price]: ipol.data)
                    CO2Price = CO2Price < 0.011 ? 0 : CO2Price * deflator * 44 / 12.;  // M(CO2) / M(C)
    }

    // Scaling the MAI climate shifters to the 2020 value (i.e., MAIShifter_year = MAIShifter_year/MAIShifter_2000, so the 2000 value = 1);
    void scaleMAIClimate2020(heterSimuIdScenariosType &simuIdScenarios) {
        if (!scaleMAIClimate) {
            INFO("scaleMAIClimate is turned off");
            return;
        }

        INFO("Scaling MAI climate shifters to the 2020 value!");
        for (auto &[scenario, MAI]: simuIdScenarios)
            for (auto &[simu_id, ipol]: MAI) {
                double reciprocal_value_2020 = 1 / ipol.data.at(2020);
                ipol *= reciprocal_value_2020;
            }
    }

    void add2020Disturbances() {
        for (auto &[id, ipol]: commonDisturbWind)
            ipol.data[2020] = ipol.data.at(2030) / 1.025;
        for (auto &[id, ipol]: commonDisturbFire)
            ipol.data[2020] = ipol.data.at(2030) / 1.05;
        for (auto &[id, ipol]: commonDisturbBiotic)
            ipol.data[2020] = ipol.data.at(2030) / 1.05;
    }

    void scaleDisturbance(simuIdType &disturbance, const uint16_t scaleYear) {
        for (auto &[id, ipol]: disturbance)
            ipol += -ipol.data.at(scaleYear);
    }

    void scaleDisturbances2020() {
        if (!scaleDisturbance2020) {
            INFO("scaleDisturbance2020 is turned off");
            return;
        }

        const uint16_t scaleYear = 2020;
        scaleDisturbance(commonDisturbWind, scaleYear);
        scaleDisturbance(commonDisturbFire, scaleYear);
        scaleDisturbance(commonDisturbBiotic, scaleYear);

        INFO("Disturbances are scaled to the {} value!", scaleYear);
    }

    void initGlobiomLandAndManagedForestGlobal() {
        array<double, numberOfCountries> woodHarvest{};
        array<double, numberOfCountries> woodLost{};

        double sawnW = 0;      // MG: get harvestable sawn-wood for the set (old) forest tC/ha for final cut.
        double restW = 0;      // MG: get harvestable rest-wood for the set (old) forest tC/ha for final cut.
        double sawnThW = 0;    // MG: get harvestable sawn-wood for the set (old) forest tC/ha for thinning.
        double restThW = 0;    // MG: get harvestable rest-wood for the set (old) forest tC/ha for thinning.
        double bmH = 0;        // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for final cut
        double bmTh = 0;       // MG: get total harvestable biomass including harvest losses for the set (old) forest tC/ha for thinning
        double harvRes = 0;    // MG: usable harvest residues for the set (old) forest tC/ha

        for (auto &plot: commonPlots) {
            plot.initForestArrange();  // initGlobiomLandGlobal included here
            double forestShare0 = max(0., plot.forest);
            plot.forestsArrangement();
            commonOForestShGrid.country(plot.x, plot.y) = plot.country;
            double maxAffor = plot.getMaxAffor();

            if (forestShare0 > maxAffor) {
                optional<double> opt_dfor = plot.initForestArea(forestShare0 - maxAffor);
                if (opt_dfor)
                    simuIdDfor[plot.simuID] = *opt_dfor;  // subtraction later in initManagedForestLocal
                forestShare0 = maxAffor;
            }

            commonOForestShGrid(plot.x, plot.y) = forestShare0;
            commonOForestShGrid.update1YearForward();  // populate the OForestShGridPrev with forestShare0 data
            double forestArea0 = plot.landArea * 100 * forestShare0; // all forest area in the cell, ha

            double biomassRot = 0;     // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
            double biomassRotTh = 0;   // MG: rotation time fitted to get certain biomass under certain MAI (with thinning)
            double harvWood = 0;    // MG: harvestable wood, m3
            double abBiomassO = 0;
            // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
            // Max mean annual increment of New forest (with uniform age structure and managed with rotation length maximizing MAI)
            double MAI = max(0., forestShare0 > 0 ? plot.MAIE(coef.bYear) : plot.MAIN(coef.bYear));
            double defIncome = 0;

            double rotUnmanaged = 0, rotMAI = 0, rotMaxBm = 0, rotMaxBmTh = 0, rotHarvFin = 0, rotHarvAve = 0;

            if (plot.protect.data.at(2000) == 1)
                plot.managedFlag = false;

            commonMaiForest(plot.x, plot.y) = MAI;
            double harvMAI = MAI * plot.fTimber(coef.bYear) * (1 - coef.harvLoos);

            if (plot.CAboveHa > 0 && commonMaiForest(plot.x, plot.y) > 0) {
                if (plot.speciesType == 0)
                    DEBUG("plot.speciesType = {}", plot.speciesType);
                // rotation time to get current biomass (without thinning)
                biomassRot = species[plot.speciesType - 1].getU(plot.CAboveHa, MAI);
                // rotation time to get current biomass (with thinning)
                biomassRotTh = species[plot.speciesType - 1].getUT(plot.CAboveHa, MAI);
            }

            if (commonMaiForest(plot.x, plot.y) > 0) {
                rotMAI = species[plot.speciesType - 1].getTOptT(MAI, optimMAI);
                rotMaxBm = species[plot.speciesType - 1].getTOpt(MAI, optimMaxBm);
                rotMaxBmTh = species[plot.speciesType - 1].getTOptT(MAI, optimMaxBm);
            }

            DIMA decision{1990, plot.NPP, plot.sPopDens, plot.sAgrSuit, plot.priceIndex, coef.priceIndexE, plot.R,
                          coef.priceC, coef.plantingCostsR, coef.priceLandMinR, coef.priceLandMaxR, coef.maxRotInter,
                          coef.minRotInter, coef.decRateL, coef.decRateS, plot.fracLongProd, coef.baseline,
                          plot.fTimber, coef.priceTimberMaxR, coef.priceTimberMinR, coef.fCUptake, plot.GDP,
                          coef.harvLoos, forestShare0, woodPriceScenarios.at(s_bauScenario).at(plot.country),
                          rotMAI, harvMAI};

            double rotation = 0;
            if (plot.protect.data.at(2000) < 1) {
                rotation = max(biomassRotTh + 1, rotMAI);

                double pDefIncome =
                        plot.CAboveHa * (decision.priceTimber() * plot.fTimber(coef.bYear) * (1 - coef.harvLoos));
                // Immediate Pay if deforested (Slash and Burn)
                double sDefIncome = pDefIncome;
                defIncome =
                        pDefIncome * (1 - plot.slashBurn(coef.bYear)) + sDefIncome * plot.slashBurn(coef.bYear);

                if (plot.managedFlag) {
                    commonThinningForest(plot.x, plot.y) = 1;
                    commonRotationType(plot.x, plot.y) = 11;

                    if (MAI > MAI_CountryUprotect[plot.country - 1])
                        commonManagedForest(plot.x, plot.y) = 3;
                    else {
                        if (decision.forValNC() * hurdle_opt[plot.country - 1] > decision.agrVal() + defIncome)
                            commonManagedForest(plot.x, plot.y) = 2;
                        else
                            commonManagedForest(plot.x, plot.y) = 1;
                    }

                } else {
                    commonThinningForest(plot.x, plot.y) = -1;
                    commonRotationType(plot.x, plot.y) = 10;

                    if (MAI > MAI_CountryUprotect[plot.country - 1])
                        commonManagedForest(plot.x, plot.y) = 0;
                    else {
                        if (decision.forValNC() * hurdle_opt[plot.country - 1] > decision.agrVal() + defIncome)
                            commonManagedForest(plot.x, plot.y) = -1;
                        else
                            commonManagedForest(plot.x, plot.y) = -2;
                    }
                }

            } else {
                commonThinningForest(plot.x, plot.y) = -1;
                rotation = biomassRot + 1;
            }

            commonRotationForest(plot.x, plot.y) = rotation;
            commonThinningForest10(plot.x, plot.y) = -1;
            commonThinningForest30(plot.x, plot.y) = commonThinningForest(plot.x, plot.y);
        }
    }

    void setAsIds(const span<DataStruct> plots) noexcept {
        for (auto &&[i, plot]: plots | rv::enumerate)
            plot.asID = i;
    }

    [[nodiscard]] unordered_map<uint8_t, FFIpolsCountry>
    initCountriesFFIpols(const span<const DataStruct> plots) noexcept {
        unordered_map<uint8_t, FFIpolsCountry> fun_countriesFFIpols;
        fun_countriesFFIpols.reserve(256);

        for (const auto &plot: plots)
            fun_countriesFFIpols.emplace(plot.country, plot.country);

        return fun_countriesFFIpols;
    }

    pair<string, vector<vector<double> > > findAndReadBau(const string_view prefix) {
        string bauName = string{prefix} + string{suffix};
        for (const auto &dir_entry: fs::directory_iterator{settings.inputPath})
            if (dir_entry.path().string().contains(bauName))
                return {dir_entry.path().stem().string().substr(bauName.size())
                        | rv::transform(::toupper) | ranges::to<string>(),
                        readBau(dir_entry.path().filename().string(), prefix)};

        FATAL("file with bauName = {} is not found in {}", bauName, settings.inputPath);
        throw runtime_error{"Missing bau file"};
    }

    void initLoop() {
        INFO("Start initialising cohorts");
        commonCohort_all.reserve(commonPlots.size());
        commonCohort30_all.reserve(commonPlots.size());
        commonCohort10_all.reserve(commonPlots.size());
        commonCohort_primary_all.reserve(commonPlots.size());
        commonNewCohort_all.reserve(commonPlots.size());
        commonDat_all.reserve(commonPlots.size());
        harvestResiduesCountry.reserve(256);

        const unordered_map<uint8_t, vector<double> > ageStructData = readAgeStructData();

        // type and size will be deduced
        constexpr array priceCiS = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 70, 100, 150};
        constexpr array ageBreaks = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 999};
        constexpr array ageSize = {11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

        for (size_t asId = 0; const auto &plot: commonPlots) {
            coef.priceC = priceCiS[0] * plot.corruption(coef.bYear);

            double forestShare = clamp(plot.getForestShare(), 0., plot.getMaxAffor());

            // MG: 15 December 2020: make soft condition for Ireland (allowing harvesting of stands younger than MaiRot
            // according to the explanation of the national experts in December 2020 the actual rotation time is 30-40%
            // lover than the MAI rotation
            double minRotVal = (plot.country == 103) ? 0.6 : 1;

            double MAIRot = 1;         // MG: optimal rotation time (see above)
            double rotation = 1;
            double rotMaxBm = 1;
            double rotMaxBmTh = 1;
            double biomassRot = 1;     // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
            double biomassRotTh = 1;   // MG: rotation time fitted to get certain biomass under certain MAI (with thinning)

            bool forFlag = false;   // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlag10 = false; // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlag30 = false; // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest
            bool forFlagP = false;  // MG: a forest area for fitting existing forest in the grid: 0 - no forest; 1 - 1 ha of normal forest

            double mai_tmp = commonMaiForest(plot.x, plot.y);
            double thinning_tmp = commonThinningForest(plot.x, plot.y);
            size_t species_tmp = plot.speciesType - 1;

            if (mai_tmp > 0) {
                MAIRot = max(1., species[species_tmp].getTOptT(mai_tmp, optimMAI));
                rotMaxBm = max(1., species[species_tmp].getTOpt(mai_tmp, optimMaxBm));
                rotMaxBmTh = max(1., species[species_tmp].getTOptT(mai_tmp, optimMaxBm));

                if (plot.CAboveHa > 0) {
                    biomassRot = max(1., species[species_tmp].getU(plot.CAboveHa, mai_tmp));
                    biomassRotTh = max(1., species[species_tmp].getUSdTab(plot.CAboveHa, mai_tmp,
                                                                          abs(thinning_tmp))); // with thinning

                    forFlag = plot.forest > 0;
                    forFlag10 = plot.oldGrowthForest_ten > 0;
                    forFlag30 = plot.oldGrowthForest_thirty > 0;
                    forFlagP = plot.strictProtected > 0;
                }
            }

            rotation = thinning_tmp > 0 ? biomassRotTh : biomassRot;

            double abBiomass0 = 0;  // Modelled biomass at time 0, tC/ha

            // Stocking degree depending on tree height is not implemented
            // saving results to initial vectors
            commonCohort_all.emplace_back(&species[species_tmp], &ffsws, &countriesFFIpols.at(plot.country), &ffcov,
                                          &ffcoe,
                                          &ffdov, &ffdoe, mai_tmp, 0, 1, 0, 0, 0, 0, thinning_tmp * sdMaxCoef,
                                          thinning_tmp * sdMinCoef, 30, minRotVal, 1, 0, 1);
            commonCohort30_all.push_back(commonCohort_all.back());
            commonCohort10_all.emplace_back(&species[species_tmp], &ffsws, &countriesFFIpols.at(plot.country), &ffcov,
                                            &ffcoe,
                                            &ffdov, &ffdoe, mai_tmp, 0, 1, 0, 0, 0, 0, -sdMaxCoef, -sdMinCoef, 30,
                                            minRotVal,
                                            1, 0, 1);
            commonCohort_primary_all.push_back(commonCohort10_all.back());

            AgeStruct &cohort = commonCohort_all.back();
            AgeStruct &cohort30 = commonCohort30_all.back();
            AgeStruct &cohort10 = commonCohort10_all.back();
            AgeStruct &cohort_primary = commonCohort_primary_all.back();

            size_t oldestAgeGroup = 0;
            double oldestAge = 0;

            if (ageStructData.contains(plot.country)) {
                oldestAgeGroup = distance(ranges::find_if(ageStructData.at(plot.country) | rv::reverse,
                                                          [](const auto x) { return x > 0; }),
                                          ageStructData.at(plot.country).rend()) - 1;  // last positive

                oldestAge = ageBreaks.at(oldestAgeGroup);
                if (oldestAge > 150)
                    oldestAge = rotMaxBm * 0.7;

            } else
                WARN("ageStructData doesn't contain plot.country = {} ({})",
                     plot.country, idCountryGLOBIOM.at(plot.country));

            if (plot.forest + plot.oldGrowthForest_thirty > 0 && mai_tmp > 0) {
                if (ageStructData.contains(plot.country) && thinning_tmp > 0 && (forFlag || forFlag30) &&
                    plot.potVeg.data.at(2000) < 10) {
                    cohort.createNormalForest(321, 0, 1);

                    for (size_t i = 1; i < 161; ++i) {
                        size_t ageGroup = distance(ageBreaks.begin(),
                                                   ranges::lower_bound(ageBreaks, 161));  // first x <= i
                        cohort.setArea(i, ageStructData.at(plot.country)[ageGroup] /
                                          static_cast<double>(ageSize[ageGroup]));
                    }

                    double biomass = cohort.getBm() * plot.BEF(cohort.getBm());
                    //Tune age structure for current cell
                    if (cohort.getArea() > 0 && biomass > 0) {

                        if (biomass < 0.95 * plot.CAboveHa) {

                            for (size_t young = 0, oag = oldestAgeGroup;
                                 biomass < 0.95 * plot.CAboveHa && oag < 30; ++young, ++oag)
                                if (ageSize[young] > 0 && oag > young)
                                    for (int i = 0; i < 10; ++i) {
                                        double halfAreaTmp = cohort.getArea(i + young * 10 + 1) * 0.5;
                                        cohort.setArea(i + young * 10 + 1, halfAreaTmp);
                                        cohort.setArea(i + (oag + 1) * 10 + 1, halfAreaTmp);
                                        biomass = cohort.getBm() * plot.BEF(cohort.getBm());
                                    }

                        } else if (biomass > 2 * plot.CAboveHa) {  // v_24_11

                            for (size_t young = 0, oag = oldestAgeGroup;
                                 biomass > 2 * plot.CAboveHa && oag > 2; ++young, --oag) {
                                if (oag > young) {
                                    if (ageSize[oag] > 0 && ageSize[young] > 0) {
                                        for (int i = 0; i < ageSize[oag]; ++i) {
                                            double areaTmp_oag = cohort.getArea(i + oag * 10 + 1);
                                            double areaTmp_young = cohort.getArea(i + young * 10 + 1);
                                            cohort.setArea(i + oag * 10 + 1, 0);
                                            cohort.setArea(i + young * 10 + 1, areaTmp_oag + areaTmp_young);
                                            biomass = cohort.getBm() * plot.BEF(cohort.getBm());
                                        }
                                    }
                                } else if (ageSize[oag] > 0 && ageSize[oag - 1] > 0) {
                                    for (int i = 0; i < ageSize[oag]; ++i) {
                                        double areaTmp_oag = cohort.getArea(i + oag * 10 + 1);
                                        double areaTmp_young = cohort.getArea(i + (oag - 1) * 10 + 1);
                                        cohort.setArea(i + oag * 10 + 1, 0);
                                        cohort.setArea(i + (oag - 1) * 10 + 1, areaTmp_oag + areaTmp_young);
                                        biomass = cohort.getBm() * plot.BEF(cohort.getBm());
                                    }
                                }
                            }

                        }

                        double stockingDegree =
                                max(0., plot.CAboveHa * cohort.getArea() / (cohort.getBm() * plot.BEF(cohort.getBm())));
                        cohort.setStockingDegreeMin(stockingDegree * sdMinCoef);
                        cohort.setStockingDegreeMax(stockingDegree * sdMaxCoef);
                        commonThinningForest(plot.x, plot.y) = stockingDegree;
                        for (int i = 0; i < 321; ++i)
                            cohort.setBm(i, stockingDegree * cohort.getBm(i));
                        cohort.setU(321);
                        auto ignored = cohort.aging();
                    }

                    rotation = max(MAIRot, species[species_tmp].getUSdTab(cohort.getBm() / cohort.getArea(), mai_tmp,
                                                                          thinning_tmp) + 1);
                    cohort.setU(rotation);

                    if (plot.oldGrowthForest_thirty > 0)
                        cohort30 = cohort;
                    else
                        cohort30.createNormalForest(1, 0, 1);

                } else if (forFlag || forFlag30) {
                    rotation = max(MAIRot, rotation);

                    cohort.createNormalForest(rotation, forFlag, thinning_tmp);
                    cohort.setStockingDegreeMin(thinning_tmp * sdMinCoef);
                    cohort.setStockingDegreeMax(thinning_tmp * sdMaxCoef);
                    cohort.setU(rotation);

                    if (plot.oldGrowthForest_thirty > 0) {
                        cohort30.createNormalForest(rotation, forFlag30, thinning_tmp);
                        cohort30.setStockingDegreeMin(thinning_tmp * sdMinCoef);
                        cohort30.setStockingDegreeMax(thinning_tmp * sdMaxCoef);
                        cohort30.setU(rotation);
                    } else
                        cohort30.createNormalForest(1, 0, thinning_tmp);
                } else {  // MG: create an existing forest with 0 area for consistency of the singleCell structure
                    cohort.createNormalForest(1, 0, thinning_tmp);
                    cohort30.createNormalForest(1, 0, thinning_tmp);
                }

                commonRotationForest(plot.x, plot.y) = rotation;
                abBiomass0 = cohort.getBm();  // modelled biomass at time 0, tC/ha

            } else {
                cohort.createNormalForest(1, forFlag, 1);
                cohort30.createNormalForest(1, forFlag30, 1);
            }

            if (plot.oldGrowthForest_ten > 0) {
                double biomassRot10 = species[species_tmp].getU(1.3 * plot.CAboveHa, mai_tmp);
                double MAIRot10 = species[species_tmp].getTOpt(mai_tmp, optimMAI);

                biomassRot10 = max(biomassRot10, 0.5 * oldestAge);
                biomassRot10 = max(biomassRot10, 1.5 * MAIRot10);

                cohort10.createNormalForest(biomassRot10, 1., -1.);
                cohort10.setU(biomassRot10);

                double oldest = cohort10.getActiveAge();
                double stockingDegree = 1;
                // 16.09.2021: In natural forest stocking 1 is already maximum
                if (plot.CAboveHa > 0 && cohort10.getArea() > 0)
                    stockingDegree = clamp(
                            1.3 * plot.CAboveHa * cohort10.getArea() / (cohort10.getBm() * plot.BEF(cohort10.getBm())),
                            0.6, 1.);

                cohort10.setStockingDegreeMin(-stockingDegree * sdMinCoef);
                cohort10.setStockingDegreeMax(-stockingDegree * sdMaxCoef);
                commonThinningForest10(plot.x, plot.y) = -stockingDegree;

                int tmp_max = static_cast<int>(ceil(oldest / modTimeStep)) + 1;
                for (int i = 0; i <= tmp_max; ++i)
                    cohort10.setBm(i, stockingDegree * cohort10.getBm(i * modTimeStep));

            } else
                cohort10.createNormalForest(1, 0, -1);

            if (plot.strictProtected > 0) {
                double biomassRotP = species[species_tmp].getU(1.3 * plot.CAboveHa, mai_tmp);
                double MAIRotP = species[species_tmp].getTOpt(mai_tmp, optimMAI);

                biomassRotP = max(biomassRotP, oldestAge);
                biomassRotP = max(biomassRotP, MAIRotP);

                cohort_primary.createNormalForest(biomassRotP, 1, -1);
                cohort_primary.setU(biomassRotP);

                double oldest = cohort_primary.getActiveAge();
                double stockingDegree = 1;
                //16.09.2021: In natural forest stocking 1 is already maximum
                if (plot.CAboveHa > 0 && cohort_primary.getArea() > 0)
                    stockingDegree = clamp(1.3 * plot.CAboveHa * cohort_primary.getArea() /
                                           (cohort_primary.getBm() * plot.BEF(cohort_primary.getBm())), 0.6, 1.);

                cohort_primary.setStockingDegreeMin(-stockingDegree * sdMinCoef);
                cohort_primary.setStockingDegreeMax(-stockingDegree * sdMaxCoef);

                int tmp_max = static_cast<int>(ceil(oldest / modTimeStep)) + 1;
                for (int i = 0; i <= tmp_max; ++i)
                    cohort_primary.setBm(i, stockingDegree * cohort_primary.getBm(i * modTimeStep));

            } else
                cohort_primary.createNormalForest(1, 0, -1);

            // rotation changes
            commonNewCohort_all.emplace_back(&species[species_tmp], &ffsws, &countriesFFIpols.at(plot.country), &ffcov,
                                             &ffcoe, &ffdov, &ffdoe, mai_tmp, 0, rotation, 0, 0, 0, 0,
                                             thinning_tmp * sdMaxCoef, thinning_tmp * sdMinCoef, 30, minRotVal, 1, 0,
                                             1);
            AgeStruct &newCohort = commonNewCohort_all.back();
            newCohort.createNormalForest(rotation, 0, thinning_tmp);

            commonDat_all.emplace_back();
            Dat &singleCell = commonDat_all.back();
            singleCell.rotation = commonRotationForest(plot.x, plot.y);
            singleCell.landAreaHa = plot.landArea * 100;
            singleCell.forestShare = plot.getForestShare();
            singleCell.forestShare0 = singleCell.forestShare;
            singleCell.OForestShare = singleCell.forestShare;
            singleCell.OForestShareU = plot.forest;
            singleCell.OForestShare10 = plot.oldGrowthForest_ten;
            singleCell.OForestShare30 = plot.oldGrowthForest_thirty;
            singleCell.prevOForShare = forestShare;  // MG: Old forest share in the previous reporting year
            singleCell.prevOForShare = singleCell.OForestShare;     // forest share of all old forest one modelling step back in each cell
            singleCell.prevOForShareU = singleCell.OForestShareU;   // forest share of "usual" old forest one modelling step back in each cell
            singleCell.prevOForShare10 = singleCell.OForestShare10; // forest share of 10% policy forest one modelling step back in each cell
            singleCell.prevOForShare30 = singleCell.OForestShare30; // forest share of 30% policy forest one modelling step back in each cell
            singleCell.prevOForShareRP = forestShare;  // MG: Old forest share in the previous reporting year
            singleCell.OBiomassPrev = abBiomass0;
            singleCell.OBiomass0 = abBiomass0;                   // Modelled biomass at time 0, tC/ha
            singleCell.OBiomassPrev10 = cohort10.getBm();   // Biomass of 10% policy forest on a previous step, tC/ha
            singleCell.OBiomassPrev30 = cohort30.getBm();   // Biomass of 30% policy forest on a previous step, tC/ha
            singleCell.OBiomassPrevP = cohort_primary.getBm();      // Biomass of primary forest on a previous step, tC/ha
            singleCell.oForestBm = abBiomass0;
            singleCell.oForestBm10 = singleCell.OBiomassPrev10;
            singleCell.oForestBm30 = singleCell.OBiomassPrev30;
            singleCell.oForestBmP = singleCell.OBiomassPrevP;
            singleCell.rotBiomass = rotation;
            singleCell.SD = thinning_tmp;
            singleCell.species = plot.speciesType;
            singleCell.deforPrev = plot.forLoss;
            singleCell.road = plot.road.data.at(2000);
            singleCell.slashBurn = plot.slashBurn.data.at(2000);
            singleCell.deadwood = plot.forest > 0 ? plot.deadWood : 0;
            singleCell.deadwood10 = plot.oldGrowthForest_ten > 0 ? plot.deadWood : 0;
            singleCell.deadwood30 = plot.oldGrowthForest_thirty > 0 ? plot.deadWood : 0;
            singleCell.deadwoodP = plot.strictProtected > 0 ? plot.deadWood : 0;
            singleCell.forest10 = plot.oldGrowthForest_ten;
            singleCell.forest30 = plot.oldGrowthForest_thirty;

            harvestResiduesCountry[plot.country].emplace_back();
            HarvestResidues &residuesCellTmp = harvestResiduesCountry[plot.country].back();
            residuesCellTmp.simuId = plot.simuID;
            residuesCellTmp.asID = plot.asID;
            residuesCellTmp.country = plot.country;
            residuesCellTmp.fTimber = plot.fTimber.data.at(2000);
            residuesCellTmp.costsSuit1 = plot.residuesUseCosts;
            residuesCellTmp.costsSuit2 = plot.residuesUseCosts + 10;
            residuesCellTmp.costsSuit3 = plot.residuesUseCosts;
            residuesCellTmp.costsSuit4_notTaken = plot.residuesUseCosts * 10;
        }
    }

    // MG: 22 July 2022
    // Converts all forest to unused, then converts back to used starting from most productive forest in each country
    // while potential harvest (MAI - harvest losses) satisfies wood demand initial year.
    // Wood and land prices by countries!
    void initZeroProdArea() {
        if (!zeroProdAreaInit) {
            INFO("initZeroProdArea is turned off");
            return;

            auto thinningForestInit = commonThinningForest;
            array<double, numberOfCountries> woodPotHarvest{};

            INFO("Putting data for current cell into container...");
            for (const auto &plot: commonPlots)
                if (plot.protect.data.at(2000) == 0) {
                    double MAI = commonMaiForest(plot.x, plot.y);  // MG: mean annual increment in tC/ha/2000
                    size_t species_tmp = plot.speciesType - 1;

                    double biomassRot = 1;  // MG: rotation time fitted to get certain biomass under certain MAI (w/o thinning)
                    double biomassRotTh = 1;
                    double rotMAI = 1;
                    double rotMaxBm = 1;

                    double Bm = commonCohort_all[plot.asID].getBm();

                    if (plot.CAboveHa > 0 && MAI > 0) {
                        // rotation time to get current biomass (without thinning)
//                        biomassRot = species[species_tmp].getU(Bm, MAI);  overwritten
                        rotMAI = species[species_tmp].getTOpt(MAI, optimMAI);
                        rotMaxBm = species[species_tmp].getTOpt(MAI, optimMaxBm);
                        // rotation time to get current biomass (with thinning)
                        biomassRotTh = species[species_tmp].getUSdTab(Bm, MAI, commonThinningForest(plot.x, plot.y));
                    }

                    biomassRot = max(rotMaxBm, commonRotationForest(plot.x, plot.y));
                    DIMA decision{1990, plot.NPP, plot.sPopDens, plot.sAgrSuit, plot.priceIndex, coef.priceIndexE,
                                  plot.R, coef.priceC, coef.plantingCostsR, coef.priceLandMinR, coef.priceLandMaxR,
                                  coef.maxRotInter, coef.minRotInter, coef.decRateL, coef.decRateS, plot.fracLongProd,
                                  coef.baseline, plot.fTimber, coef.priceTimberMaxR, coef.priceTimberMinR,
                                  coef.fCUptake, plot.GDP, coef.harvLoos,
                                  commonOForestShGrid(plot.x, plot.y) -
                                  plot.strictProtected,  // forestShare0 - forest available for wood supply initially
                                  woodPriceScenarios.at(s_bauScenario).at(plot.country), rotMAI,
                                  MAI * plot.fTimber.data.at(2000) * (1 - coef.harvLoos)};  // harvMAI

                    double thinning = -1;
                    double rotation = 1;

                    if (commonThinningForest(plot.x, plot.y) > 0) {
                        commonThinningForest(plot.x, plot.y) = thinning;
                        commonThinningForest30(plot.x, plot.y) = thinning;
                        commonRotationType(plot.x, plot.y) = 10;

                        commonCohort_all[plot.asID].setStockingDegree(thinning);
                        commonNewCohort_all[plot.asID].setStockingDegree(thinning);
                        commonCohort30_all[plot.asID].setStockingDegree(thinning);

                        // defIncome = 0 => decision.agrVal() + defIncome = decision.agrVal()
                        rotation = max(biomassRot, rotMAI) + 1;

                        if (MAI > MAI_CountryUprotect[plot.country - 1]) {
                            if (decision.forValNC() * hurdle_opt[plot.country - 1] > decision.agrVal()) {
                                commonManagedForest(plot.x, plot.y) = 0;
                                commonRotationType(plot.x, plot.y) = 1;
                            } else {
                                commonManagedForest(plot.x, plot.y) = -1;
                                commonRotationType(plot.x, plot.y) = 10;
                            }
                        } else {
                            if (decision.forValNC() * hurdle_opt[plot.country - 1] > decision.agrVal()) {
                                commonManagedForest(plot.x, plot.y) = -1;
                                commonRotationType(plot.x, plot.y) = 10;
                            } else {
                                commonManagedForest(plot.x, plot.y) = -2;
                                commonRotationType(plot.x, plot.y) = 10;
                            }
                        }

                        commonRotationForest(plot.x, plot.y) = rotation;
                        commonCohort_all[plot.asID].setU(rotation);
                        commonNewCohort_all[plot.asID].setU(rotation);
                        commonCohort30_all[plot.asID].setU(rotation);
                    }
                }

            for (const auto &plot: commonPlots)
                if (plot.protect.data.at(2000) == 0 && thinningForestInit(plot.x, plot.y) > 0) {
                    double MAI = commonMaiForest(plot.x, plot.y);  // MG: mean annual increment in tC/ha/2000
                    size_t species_tmp = plot.speciesType - 1;

                    double rotMAI = 0;
                    double rotMaxBm = 0;
                    double rotMaxBmTh = 0;
                    double biomassRotTh2 = 0;  // MG: rotation time fitted to get certain biomass under certain MAI (with thinning = 2)

                    double stockingDegree = thinningForestInit(plot.x, plot.y);
                    double Bm = commonCohort_all[plot.asID].getBm();
                    double rotation = 0;

                    if (plot.CAboveHa > 0 && MAI > 0) {
                        // rotation time to get current biomass (with thinning)
                        biomassRotTh2 = species[species_tmp].getUSdTab(Bm, MAI, stockingDegree);
                        rotMAI = species[species_tmp].getTOptSdTab(MAI, stockingDegree, optimMAI);
                        rotMaxBmTh = species[species_tmp].getTOptSdTab(MAI, stockingDegree, optimMaxBm);
                    } else if (MAI > 0) {
                        rotMAI = species[species_tmp].getTOpt(MAI, optimMAI);
                        rotMaxBm = species[species_tmp].getTOpt(MAI, optimMaxBm);
                    }

                    if (woodPriceScenarios.at(s_bauScenario).at(plot.country)(coef.bYear) >
                        woodPotHarvest[plot.country - 1]) {
                        if (commonManagedForest(plot.x, plot.y) == 0) {
                            rotation = rotMAI + 1;
                            commonManagedForest(plot.x, plot.y) = 3;
                            commonRotationType(plot.x, plot.y) = 1;
                        } else if (commonManagedForest(plot.x, plot.y) == -1) {
                            rotation = min(rotMAI + 1, rotMaxBmTh);
                            commonManagedForest(plot.x, plot.y) = 2;
                            commonRotationType(plot.x, plot.y) = 2;
                        } else if (commonManagedForest(plot.x, plot.y) == -2) {
                            commonManagedForest(plot.x, plot.y) = 1;
                            commonRotationType(plot.x, plot.y) = 3;
                            rotation = clamp(biomassRotTh2 + 1, rotMAI, rotMaxBmTh);
                        }

                        double harvMAI = MAI * plot.fTimber(coef.bYear) * (1 - coef.harvLoos);
                        // area of forest available for wood supply
                        double forestArea0 = plot.landArea * 100 * (plot.forest + plot.oldGrowthForest_thirty);
                        woodPotHarvest[plot.country - 1] += harvMAI * forestArea0;

                        commonRotationForest(plot.x, plot.y) = rotation;
                        commonCohort_all[plot.asID].setU(rotation);

                        commonThinningForest(plot.x, plot.y) = stockingDegree;
                        commonCohort_all[plot.asID].setStockingDegree(stockingDegree);

                        commonNewCohort_all[plot.asID].setU(rotation);
                        commonNewCohort_all[plot.asID].setStockingDegree(stockingDegree);

                        commonCohort30_all[plot.asID].setU(rotation);
                        commonThinningForest30(plot.x, plot.y) = stockingDegree;
                        commonCohort30_all[plot.asID].setStockingDegree(stockingDegree);
                    }
                }
        }
    }

    void Init() {
        future<void> settings_future = async(launch::async, [] {
            Log::Init("settings");
            settings.readSettings("settings_Europe_dw_v02.ini");
        });

        idCountryGLOBIOM = setIdCountryGLOBIOM();
        countryGLOBIOMId = setCountryGLOBIOMId();
        settings_future.get();

        future<void> coef_future = async(launch::async, [] {
            Log::Init("coef");
            coef.readCoef(settings.coefPath);
        });

        future<void> NUTS2_future = async(launch::async, [] {
            Log::Init("NUTS2");
            nuts2id = readNUTS2();
        });

        future<void> plots_future = async(launch::async, [] {
            Log::Init("plots");
            regions = regionsToConsider();
            countriesList = countriesToConsider();
            rawPlots = readPlots();
            commonPlots = filterPlots(rawPlots);
            plotsSimuID = initPlotsSimuID(rawPlots);
            plotsXY_SimuID = initPlotsXY_SimuID(rawPlots);
        });

        coef_future.get();

        future<void> globiom_datamaps_future = async(launch::async, [] {
            Log::Init("globiom_datamaps");
            readGlobiom();      // created dicts
            readDatamaps();     // adds bau scenario to dicts
            convertUnitsDatamaps();
        });

        future<void> CO2_price_future = async(launch::async, [] {
            Log::Init("CO2_price");
            CO2PriceScenarios = readCO2price();
            correctAndConvertCO2Prices(CO2PriceScenarios);
        });

        plots_future.get();
        NUTS2_future.get();

        future<void> MAI_future = async(launch::async, [] {
            Log::Init("MAI");
            correctMAI(commonPlots);
            MAI_CountryUprotect = calculateAverageMAI(commonPlots);
            maiClimateShiftersScenarios = readMAIClimate();
            scaleMAIClimate2020(maiClimateShiftersScenarios);
        });

        future<void> globiom_land_future = async(launch::async, [] {
            Log::Init("globiom_land");
            readGlobiomLandCalibrate();
            readGlobiomLand();
        });

        future<void> globiom_land_country_future = async(launch::async, [] {
            Log::Init("globiom_land_country");
            readGlobiomLandCountryCalibrate_calcCountryLandArea();
            readGlobiomLandCountry();
        });

        future<void> disturbances_future = async(launch::async, [] {
            Log::Init("disturbances");
            readDisturbances();
            add2020Disturbances();
            scaleDisturbances2020();
        });

        future<void> disturbances_extreme_future = async(launch::async, [] {
            Log::Init("disturbances_extreme");
            readDisturbancesExtreme();
        });

        future<void> biomass_bau_future = async(launch::async, [] {
            Log::Init("biomass_bau");
            if (fmPol && binFilesOnDisk) {
                const auto &[scenario, bau_vec] = findAndReadBau("biomass_bau");
                biomassBauScenarios[scenario] = bau_vec;
            } else
                INFO("biomass_bau reading is turned off");
        });

        future<void> NPV_bau_future = async(launch::async, [] {
            Log::Init("NPV_bau");
            if (fmPol && binFilesOnDisk) {
                const auto &[scenario, bau_vec] = findAndReadBau("NPVbau");
                NPVBauScenarios[scenario] = bau_vec;
            } else
                INFO("NPV_bau reading is turned off");
        });

        years = yearsToConsider(1990, 2070);
        countryRegList = countryRegionsToConsider();
        countriesFmcpol = countriesFmcpolToConsider();
        countriesWoodProdStat = setCountriesWoodProdStat();
        countriesFmEmission_unfccc = setCountriesFmEmission_unfccc();
        calcAvgFM_sink_stat();
        species = defineSpecies();
        setupFMP();
        setCountryData(countriesList);
        stumpHarvCountrySpecies = setCountrySpecies();
        nuts2grid.fillFromNUTS(nuts2id);
        correctNUTS2Data(commonPlots);
        countriesFFIpols = initCountriesFFIpols(commonPlots);
        setAsIds(commonPlots);

        CO2_price_future.get();
        globiom_land_country_future.get();
        globiom_datamaps_future.get();
        disturbances_future.get();
        disturbances_extreme_future.get();
        globiom_land_future.get();
        MAI_future.get();
        biomass_bau_future.get();
        NPV_bau_future.get();

        // start calculations
        initGlobiomLandAndManagedForestGlobal();
        initLoop();
        initZeroProdArea();

//        printData();
    }

}

#endif
