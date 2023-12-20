#ifndef G4M_EUROPE_DG_DICTS_PROJECT_INDEPENDENT_HPP
#define G4M_EUROPE_DG_DICTS_PROJECT_INDEPENDENT_HPP

#include <unordered_map>
#include <string>
#include <array>
#include <vector>

#include "../helper/string_hash.hpp"
#include "../increment/increment_tab.hpp"

using namespace std;
using namespace g4m::increment;
using namespace g4m::helper;

namespace g4m::Dicts {
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

    [[nodiscard]] unordered_map<string, uint8_t, StringHash, equal_to<> > setCountryGLOBIOMId() noexcept {
        unordered_map<string, uint8_t, StringHash, equal_to<> > fun_countryGLOBIOMId;
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

    // Setup forest increment table
    [[nodiscard]] unordered_map<Species, IncrementTab, EnumHasher> defineSpecies() {
        unordered_map<Species, IncrementTab, EnumHasher> fun_species;
        fun_species.reserve(8);

        // fir [1]
        fun_species.try_emplace(Species::Fir,
                                array{-0.4562, -0.7403, -1.0772, 1.4803, 0.6713, 300., -0.2151, -0.9929, 0.5, 0.2,
                                      -0.7642, 0.3156,
                                      -0.4, 0.4468, 0.1425, 0., 0., 0., 0.25, -1., -2., -0.5, 0.5, 1.5, 150., 0.01, 0.5,
                                      0.5, 0.8,
                                      0.002, 2., 0.01, 0.5, 24.82935, 0.60708, -0.0212, 2.41308, -0.48246, 16.11, 17.78,
                                      -0.01436,
                                      0.37424, -1.52401, 2.282, 1.2718, -0.7707, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5,
                                0.25, 1);
        // spruce [2]
        fun_species.try_emplace(Species::Spruce,
                                array{0., -0.9082, -0.2728, 0.6483, 209.6889, 300., 1.8536, 0.4811, 0., 0.9, -0.143,
                                      -0.5915, -0.4,
                                      0.4507, 0.3713, 0., 0., 0., 0.1, 1., -2., -0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5,
                                      0.8, 0.002, 2.,
                                      0.01, 0.5, 22.58866, 0.6168, -0.02102, 2.4176, -0.35816, 16.11, 17.78, -0.01436,
                                      0.37424,
                                      -1.52401, 2.282, 1.2718, -0.7707, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25,
                                1);
        // pine [3]
        fun_species.try_emplace(Species::Pine,
                                array{-0.3835, -0.2416, -1.7576, 1.1638,                 // k
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
                                      0.95},                                              // iCrit
                                5,       // maiMax
                                0.25,    // maiStep
                                600,     // tMax
                                1,       // tStep
                                0.25,    // sdNatStep
                                1.5,     // sdTabMax
                                0.25,    // sdTabStep
                                1
        );      // timeframe
        // Pinus halepensis [4]
        fun_species.try_emplace(Species::PinusHalepensis,
                                array{-0.3, -0.306, -2.052, 1.673, 150., 130., 1.898, -1.141, 0.92, 0.07, -4.25, 6.168,
                                      -0.4, 0.93237,
                                      -0.00468, 0., 0., 0., 0.25, -1., -2., -0.5, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8,
                                      0.002, 2., 0.01,
                                      0.5, 26.59488, 0.62839, -0.02023, 1.05953, -0.03489, 18.7252, 46.3833, -0.2643,
                                      14.1425, -0.6368,
                                      0.8948, 0.000422233, -4.9625, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // birch / alder / Alnus incana [5]
        fun_species.try_emplace(Species::Birch,
                                array{0., -0.7422, -0.54, 0.5719, 136.958, 100., 0.2972, -0.7543, 0., 0.9, -0.953,
                                      -0.9236, -0.4, 1.052,
                                      0.108, 0., 0., 0., 0.1, 1., -2.5, -0.4, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8,
                                      0.002, 2., 0.01, 0.5,
                                      23.23597, 0.44554, -0.02485, 1.36973, -0.42941, 13.61, 10.69, -0.02688, 0.24196,
                                      -0.7015, 1.337,
                                      0.0708, -2.1515, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // beech [6]
        fun_species.try_emplace(Species::Beech,
                                array{0., -0.5998, -0.2467, 0.7674, 245.5516, 100., 2.6345, -0.8978, 0.69135, 0., 0.,
                                      0., -0.03177, 0.,
                                      0., 0., 0., 0., -0.875, 1., 0., 0., 0.5, 1.5, 300., 0.01, 0.5, 0.5, 0.8, 0.001,
                                      2., 0.01, 0.5,
                                      21.29459, 0.48722, -0.01967, 1.81479, -0.29145, 30.707, 7.008, -0.01051, -0.19793,
                                      0.2977, 1.423,
                                      1.025, -16.85, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // oak [7]
        fun_species.try_emplace(Species::Oak,
                                array{0., -0.6, -0.4419, 0.3179, 16.6688, 300., -0.6066, -1.1243, 0.7, 0.3, -0.4339,
                                      0.5288, -0.4,
                                      2.01561, -0.07354, 0., 0., 0., 0.1, 1., -3.5, 0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5,
                                      0.8, 0.002, 2.,
                                      0.01, 0.5, 21.26281, 0.51987, -0.01901, 1.34081, -0.10979, -7.511, 41.689,
                                      -0.02201, 0.58055,
                                      1.72465, 3.6757, 1.7544, 0.3264, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // larch [8]
        fun_species.try_emplace(Species::Larch,
                                array{0., -0.388, -0.01226, 0.85934, 195.3724, 600., 0.9883, 1.0784, 0., 0.9, -2.1347,
                                      -0.3437, -0.4,
                                      1.3238, 0.4061, 0., 0., 0., 0.1, 1., -2.5, -0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5,
                                      0.8, 0.002, 2.,
                                      0.01, 0.5, 23.63487, 0.50281, -0.01557, 1.16199, -0.18673, 25.196, 9.118,
                                      -0.01376, 0.64637,
                                      -0.79909, 1.0817, 0.1667, -0.9408, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25,
                                1);

        return fun_species;
    }
}

#endif
