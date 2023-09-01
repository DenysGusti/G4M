#ifndef G4M_EUROPE_DG_START_DATA_PROJECT_INDEPENDENT_HPP
#define G4M_EUROPE_DG_START_DATA_PROJECT_INDEPENDENT_HPP

#include "start_data.hpp"
#include "start_data_project_dependent.hpp"

namespace g4m::StartData {

    void setIdCountryGLOBIOM() noexcept {
        idCountryGLOBIOM.reserve(256);
        idCountryGLOBIOM[6] = "Albania";
        idCountryGLOBIOM[63] = "Algeria";
        idCountryGLOBIOM[3] = "Angola";
        idCountryGLOBIOM[10] = "Argentina";
        idCountryGLOBIOM[11] = "Armenia";
        idCountryGLOBIOM[16] = "Australia";
        idCountryGLOBIOM[17] = "Austria";
        idCountryGLOBIOM[18] = "Azerbaijan";
        idCountryGLOBIOM[26] = "Bahamas";
        idCountryGLOBIOM[25] = "Bahrain";
        idCountryGLOBIOM[23] = "Bangladesh";
        idCountryGLOBIOM[28] = "Belarus";
        idCountryGLOBIOM[20] = "Belgium";
        idCountryGLOBIOM[29] = "Belize";
        idCountryGLOBIOM[21] = "Benin";
        idCountryGLOBIOM[35] = "Bhutan";
        idCountryGLOBIOM[31] = "Bolivia";
        idCountryGLOBIOM[27] = "BosniaHerzg";
        idCountryGLOBIOM[37] = "Botswana";
        idCountryGLOBIOM[32] = "Brazil";
        idCountryGLOBIOM[34] = "BruneiDarsm";
        idCountryGLOBIOM[24] = "Bulgaria";
        idCountryGLOBIOM[22] = "BurkinaFaso";
        idCountryGLOBIOM[19] = "Burundi";
        idCountryGLOBIOM[115] = "Cambodia";
        idCountryGLOBIOM[45] = "Cameroon";
        idCountryGLOBIOM[39] = "Canada";
        idCountryGLOBIOM[51] = "CapeVerde";
        idCountryGLOBIOM[38] = "CentAfrRep";
        idCountryGLOBIOM[209] = "Chad";
        idCountryGLOBIOM[42] = "Chile";
        idCountryGLOBIOM[43] = "China";
        idCountryGLOBIOM[49] = "Colombia";
        idCountryGLOBIOM[50] = "Comoros";
        idCountryGLOBIOM[46] = "CongoDemR";
        idCountryGLOBIOM[47] = "CongoRep";
        idCountryGLOBIOM[52] = "CostaRica";
        idCountryGLOBIOM[44] = "CotedIvoire";
        idCountryGLOBIOM[97] = "Croatia";
        idCountryGLOBIOM[53] = "Cuba";
        idCountryGLOBIOM[56] = "Cyprus";
        idCountryGLOBIOM[57] = "CzechRep";
        idCountryGLOBIOM[61] = "Denmark";
        idCountryGLOBIOM[59] = "Djibouti";
        idCountryGLOBIOM[62] = "DominicanRp";
        idCountryGLOBIOM[64] = "Ecuador";
        idCountryGLOBIOM[65] = "Egypt";
        idCountryGLOBIOM[195] = "ElSalvador";
        idCountryGLOBIOM[86] = "EqGuinea";
        idCountryGLOBIOM[66] = "Eritrea";
        idCountryGLOBIOM[69] = "Estonia";
        idCountryGLOBIOM[70] = "Ethiopia";
        idCountryGLOBIOM[73] = "FalklandIs";
        idCountryGLOBIOM[72] = "FijiIslands";
        idCountryGLOBIOM[71] = "Finland";
        idCountryGLOBIOM[74] = "France";
        idCountryGLOBIOM[91] = "FrGuiana";
        idCountryGLOBIOM[180] = "FrPolynesia";
        idCountryGLOBIOM[77] = "Gabon";
        idCountryGLOBIOM[84] = "Gambia";
        idCountryGLOBIOM[79] = "Georgia";
        idCountryGLOBIOM[58] = "Germany";
        idCountryGLOBIOM[80] = "Ghana";
        idCountryGLOBIOM[87] = "Greece";
        idCountryGLOBIOM[89] = "Greenland";
        idCountryGLOBIOM[83] = "Guadeloupe";
        idCountryGLOBIOM[90] = "Guatemala";
        idCountryGLOBIOM[82] = "Guinea";
        idCountryGLOBIOM[85] = "GuineaBissau";
        idCountryGLOBIOM[93] = "Guyana";
        idCountryGLOBIOM[98] = "Haiti";
        idCountryGLOBIOM[96] = "Honduras";
        idCountryGLOBIOM[99] = "Hungary";
        idCountryGLOBIOM[106] = "Iceland";
        idCountryGLOBIOM[101] = "India";
        idCountryGLOBIOM[100] = "Indonesia";
        idCountryGLOBIOM[104] = "Iran";
        idCountryGLOBIOM[105] = "Iraq";
        idCountryGLOBIOM[103] = "Ireland";
        idCountryGLOBIOM[107] = "Israel";
        idCountryGLOBIOM[108] = "Italy";
        idCountryGLOBIOM[109] = "Jamaica";
        idCountryGLOBIOM[111] = "Japan";
        idCountryGLOBIOM[110] = "Jordan";
        idCountryGLOBIOM[112] = "Kazakhstan";
        idCountryGLOBIOM[113] = "Kenya";
        idCountryGLOBIOM[176] = "KoreaDPRp";
        idCountryGLOBIOM[118] = "KoreaRep";
        idCountryGLOBIOM[119] = "Kuwait";
        idCountryGLOBIOM[114] = "Kyrgyzstan";
        idCountryGLOBIOM[120] = "Laos";
        idCountryGLOBIOM[130] = "Latvia";
        idCountryGLOBIOM[121] = "Lebanon";
        idCountryGLOBIOM[127] = "Lesotho";
        idCountryGLOBIOM[122] = "Liberia";
        idCountryGLOBIOM[123] = "Libya";
        idCountryGLOBIOM[128] = "Lithuania";
        idCountryGLOBIOM[129] = "Luxembourg";
        idCountryGLOBIOM[139] = "Macedonia";
        idCountryGLOBIOM[135] = "Madagascar";
        idCountryGLOBIOM[151] = "Malawi";
        idCountryGLOBIOM[152] = "Malaysia";
        idCountryGLOBIOM[140] = "Mali";
        idCountryGLOBIOM[141] = "Malta";
        idCountryGLOBIOM[147] = "Mauritania";
        idCountryGLOBIOM[150] = "Mauritius";
        idCountryGLOBIOM[137] = "Mexico";
        idCountryGLOBIOM[134] = "MoldovaRep";
        idCountryGLOBIOM[144] = "Mongolia";
        idCountryGLOBIOM[132] = "Morocco";
        idCountryGLOBIOM[146] = "Mozambique";
        idCountryGLOBIOM[142] = "Myanmar";
        idCountryGLOBIOM[154] = "Namibia";
        idCountryGLOBIOM[163] = "Nepal";
        idCountryGLOBIOM[161] = "Netherlands";
        idCountryGLOBIOM[155] = "NewCaledonia";
        idCountryGLOBIOM[165] = "NewZealand";
        idCountryGLOBIOM[159] = "Nicaragua";
        idCountryGLOBIOM[156] = "Niger";
        idCountryGLOBIOM[158] = "Nigeria";
        idCountryGLOBIOM[162] = "Norway";
        idCountryGLOBIOM[166] = "Oman";
        idCountryGLOBIOM[167] = "Pakistan";
        idCountryGLOBIOM[172] = "Palestin";
        idCountryGLOBIOM[168] = "Panama";
        idCountryGLOBIOM[173] = "PapuaNGuin";
        idCountryGLOBIOM[178] = "Paraguay";
        idCountryGLOBIOM[170] = "Peru";
        idCountryGLOBIOM[171] = "Philippines";
        idCountryGLOBIOM[174] = "Poland";
        idCountryGLOBIOM[177] = "Portugal";
        idCountryGLOBIOM[175] = "PuertoRico";
        idCountryGLOBIOM[181] = "Qatar";
        idCountryGLOBIOM[182] = "Reunion";
        idCountryGLOBIOM[183] = "Romania";
        idCountryGLOBIOM[184] = "RussianFed";
        idCountryGLOBIOM[185] = "Rwanda";
        idCountryGLOBIOM[237] = "Samoa";
        idCountryGLOBIOM[186] = "SaudiArabia";
        idCountryGLOBIOM[188] = "Senegal";
        idCountryGLOBIOM[199] = "Serbia-Monte";
        idCountryGLOBIOM[194] = "SierraLeone";
        idCountryGLOBIOM[189] = "Singapore";
        idCountryGLOBIOM[202] = "Slovakia";
        idCountryGLOBIOM[203] = "Slovenia";
        idCountryGLOBIOM[193] = "SolomonIs";
        idCountryGLOBIOM[197] = "Somalia";
        idCountryGLOBIOM[239] = "SouthAfrica";
        idCountryGLOBIOM[68] = "Spain";
        idCountryGLOBIOM[126] = "SriLanka";
        idCountryGLOBIOM[187] = "Sudan";
        idCountryGLOBIOM[201] = "Suriname";
        idCountryGLOBIOM[205] = "Swaziland";
        idCountryGLOBIOM[204] = "Sweden";
        idCountryGLOBIOM[41] = "Switzerland";
        idCountryGLOBIOM[207] = "Syria";
        idCountryGLOBIOM[212] = "Tajikistan";
        idCountryGLOBIOM[222] = "Tanzania";
        idCountryGLOBIOM[211] = "Thailand";
        idCountryGLOBIOM[215] = "TimorLeste";
        idCountryGLOBIOM[210] = "Togo";
        idCountryGLOBIOM[217] = "TrinidadTob";
        idCountryGLOBIOM[218] = "Tunisia";
        idCountryGLOBIOM[219] = "Turkey";
        idCountryGLOBIOM[214] = "Turkmenistan";
        idCountryGLOBIOM[223] = "Uganda";
        idCountryGLOBIOM[78] = "UK";
        idCountryGLOBIOM[224] = "Ukraine";
        idCountryGLOBIOM[9] = "UntdArabEm";
        idCountryGLOBIOM[226] = "Uruguay";
        idCountryGLOBIOM[227] = "USA";
        idCountryGLOBIOM[228] = "Uzbekistan";
        idCountryGLOBIOM[235] = "Vanuatu";
        idCountryGLOBIOM[231] = "Venezuela";
        idCountryGLOBIOM[234] = "VietNam";
        idCountryGLOBIOM[67] = "WestSahara";
        idCountryGLOBIOM[238] = "Yemen";
        idCountryGLOBIOM[240] = "Zambia";
        idCountryGLOBIOM[241] = "Zimbabwe";
    }

    void setCountryGLOBIOMId() noexcept {
        countryGLOBIOMId.reserve(256);
        countryGLOBIOMId["Albania"] = 6;
        countryGLOBIOMId["Algeria"] = 63;
        countryGLOBIOMId["Angola"] = 3;
        countryGLOBIOMId["Argentina"] = 10;
        countryGLOBIOMId["Armenia"] = 11;
        countryGLOBIOMId["Australia"] = 16;
        countryGLOBIOMId["Austria"] = 17;
        countryGLOBIOMId["Azerbaijan"] = 18;
        countryGLOBIOMId["Bahamas"] = 26;
        countryGLOBIOMId["Bahrain"] = 25;
        countryGLOBIOMId["Bangladesh"] = 23;
        countryGLOBIOMId["Belarus"] = 28;
        countryGLOBIOMId["Belgium"] = 20;
        countryGLOBIOMId["Belize"] = 29;
        countryGLOBIOMId["Benin"] = 21;
        countryGLOBIOMId["Bhutan"] = 35;
        countryGLOBIOMId["Bolivia"] = 31;
        countryGLOBIOMId["BosniaHerzg"] = 27;
        countryGLOBIOMId["Botswana"] = 37;
        countryGLOBIOMId["Brazil"] = 32;
        countryGLOBIOMId["BruneiDarsm"] = 34;
        countryGLOBIOMId["Bulgaria"] = 24;
        countryGLOBIOMId["BurkinaFaso"] = 22;
        countryGLOBIOMId["Burundi"] = 19;
        countryGLOBIOMId["Cambodia"] = 115;
        countryGLOBIOMId["Cameroon"] = 45;
        countryGLOBIOMId["Canada"] = 39;
        countryGLOBIOMId["CapeVerde"] = 51;
        countryGLOBIOMId["CentAfrRep"] = 38;
        countryGLOBIOMId["Chad"] = 209;
        countryGLOBIOMId["Chile"] = 42;
        countryGLOBIOMId["China"] = 43;
        countryGLOBIOMId["Colombia"] = 49;
        countryGLOBIOMId["Comoros"] = 50;
        countryGLOBIOMId["CongoDemR"] = 46;
        countryGLOBIOMId["CongoRep"] = 47;
        countryGLOBIOMId["CostaRica"] = 52;
        countryGLOBIOMId["CotedIvoire"] = 44;
        countryGLOBIOMId["Croatia"] = 97;
        countryGLOBIOMId["Cuba"] = 53;
        countryGLOBIOMId["Cyprus"] = 56;
        countryGLOBIOMId["CzechRep"] = 57;
        countryGLOBIOMId["Denmark"] = 61;
        countryGLOBIOMId["Djibouti"] = 59;
        countryGLOBIOMId["DominicanRp"] = 62;
        countryGLOBIOMId["Ecuador"] = 64;
        countryGLOBIOMId["Egypt"] = 65;
        countryGLOBIOMId["ElSalvador"] = 195;
        countryGLOBIOMId["EqGuinea"] = 86;
        countryGLOBIOMId["Eritrea"] = 66;
        countryGLOBIOMId["Estonia"] = 69;
        countryGLOBIOMId["Ethiopia"] = 70;
        countryGLOBIOMId["FalklandIs"] = 73;
        countryGLOBIOMId["FijiIslands"] = 72;
        countryGLOBIOMId["Finland"] = 71;
        countryGLOBIOMId["France"] = 74;
        countryGLOBIOMId["FrGuiana"] = 91;
        countryGLOBIOMId["FrPolynesia"] = 180;
        countryGLOBIOMId["Gabon"] = 77;
        countryGLOBIOMId["Gambia"] = 84;
        countryGLOBIOMId["Georgia"] = 79;
        countryGLOBIOMId["Germany"] = 58;
        countryGLOBIOMId["Ghana"] = 80;
        countryGLOBIOMId["Greece"] = 87;
        countryGLOBIOMId["Greenland"] = 89;
        countryGLOBIOMId["Guadeloupe"] = 83;
        countryGLOBIOMId["Guatemala"] = 90;
        countryGLOBIOMId["Guinea"] = 82;
        countryGLOBIOMId["GuineaBissau"] = 85;
        countryGLOBIOMId["Guyana"] = 93;
        countryGLOBIOMId["Haiti"] = 98;
        countryGLOBIOMId["Honduras"] = 96;
        countryGLOBIOMId["Hungary"] = 99;
        countryGLOBIOMId["Iceland"] = 106;
        countryGLOBIOMId["India"] = 101;
        countryGLOBIOMId["Indonesia"] = 100;
        countryGLOBIOMId["Iran"] = 104;
        countryGLOBIOMId["Iraq"] = 105;
        countryGLOBIOMId["Ireland"] = 103;
        countryGLOBIOMId["Israel"] = 107;
        countryGLOBIOMId["Italy"] = 108;
        countryGLOBIOMId["Jamaica"] = 109;
        countryGLOBIOMId["Japan"] = 111;
        countryGLOBIOMId["Jordan"] = 110;
        countryGLOBIOMId["Kazakhstan"] = 112;
        countryGLOBIOMId["Kenya"] = 113;
        countryGLOBIOMId["KoreaDPRp"] = 176;
        countryGLOBIOMId["KoreaRep"] = 118;
        countryGLOBIOMId["Kuwait"] = 119;
        countryGLOBIOMId["Kyrgyzstan"] = 114;
        countryGLOBIOMId["Laos"] = 120;
        countryGLOBIOMId["Latvia"] = 130;
        countryGLOBIOMId["Lebanon"] = 121;
        countryGLOBIOMId["Lesotho"] = 127;
        countryGLOBIOMId["Liberia"] = 122;
        countryGLOBIOMId["Libya"] = 123;
        countryGLOBIOMId["Lithuania"] = 128;
        countryGLOBIOMId["Luxembourg"] = 129;
        countryGLOBIOMId["Macedonia"] = 139;
        countryGLOBIOMId["Madagascar"] = 135;
        countryGLOBIOMId["Malawi"] = 151;
        countryGLOBIOMId["Malaysia"] = 152;
        countryGLOBIOMId["Mali"] = 140;
        countryGLOBIOMId["Malta"] = 141;
        countryGLOBIOMId["Mauritania"] = 147;
        countryGLOBIOMId["Mauritius"] = 150;
        countryGLOBIOMId["Mexico"] = 137;
        countryGLOBIOMId["MoldovaRep"] = 134;
        countryGLOBIOMId["Mongolia"] = 144;
        countryGLOBIOMId["Morocco"] = 132;
        countryGLOBIOMId["Mozambique"] = 146;
        countryGLOBIOMId["Myanmar"] = 142;
        countryGLOBIOMId["Namibia"] = 154;
        countryGLOBIOMId["Nepal"] = 163;
        countryGLOBIOMId["Netherlands"] = 161;
        countryGLOBIOMId["NewCaledonia"] = 155;
        countryGLOBIOMId["NewZealand"] = 165;
        countryGLOBIOMId["Nicaragua"] = 159;
        countryGLOBIOMId["Niger"] = 156;
        countryGLOBIOMId["Nigeria"] = 158;
        countryGLOBIOMId["Norway"] = 162;
        countryGLOBIOMId["Oman"] = 166;
        countryGLOBIOMId["Pakistan"] = 167;
        countryGLOBIOMId["Palestin"] = 172;
        countryGLOBIOMId["Panama"] = 168;
        countryGLOBIOMId["PapuaNGuin"] = 173;
        countryGLOBIOMId["Paraguay"] = 178;
        countryGLOBIOMId["Peru"] = 170;
        countryGLOBIOMId["Philippines"] = 171;
        countryGLOBIOMId["Poland"] = 174;
        countryGLOBIOMId["Portugal"] = 177;
        countryGLOBIOMId["PuertoRico"] = 175;
        countryGLOBIOMId["Qatar"] = 181;
        countryGLOBIOMId["Reunion"] = 182;
        countryGLOBIOMId["Romania"] = 183;
        countryGLOBIOMId["RussianFed"] = 184;
        countryGLOBIOMId["Rwanda"] = 185;
        countryGLOBIOMId["Samoa"] = 237;
        countryGLOBIOMId["SaudiArabia"] = 186;
        countryGLOBIOMId["Senegal"] = 188;
        countryGLOBIOMId["Serbia-Monte"] = 199;
        countryGLOBIOMId["SierraLeone"] = 194;
        countryGLOBIOMId["Singapore"] = 189;
        countryGLOBIOMId["Slovakia"] = 202;
        countryGLOBIOMId["Slovenia"] = 203;
        countryGLOBIOMId["SolomonIs"] = 193;
        countryGLOBIOMId["Somalia"] = 197;
        countryGLOBIOMId["SouthAfrica"] = 239;
        countryGLOBIOMId["Spain"] = 68;
        countryGLOBIOMId["SriLanka"] = 126;
        countryGLOBIOMId["Sudan"] = 187;
        countryGLOBIOMId["Suriname"] = 201;
        countryGLOBIOMId["Swaziland"] = 205;
        countryGLOBIOMId["Sweden"] = 204;
        countryGLOBIOMId["Switzerland"] = 41;
        countryGLOBIOMId["Syria"] = 207;
        countryGLOBIOMId["Tajikistan"] = 212;
        countryGLOBIOMId["Tanzania"] = 222;
        countryGLOBIOMId["Thailand"] = 211;
        countryGLOBIOMId["TimorLeste"] = 215;
        countryGLOBIOMId["Togo"] = 210;
        countryGLOBIOMId["TrinidadTob"] = 217;
        countryGLOBIOMId["Tunisia"] = 218;
        countryGLOBIOMId["Turkey"] = 219;
        countryGLOBIOMId["Turkmenistan"] = 214;
        countryGLOBIOMId["Uganda"] = 223;
        countryGLOBIOMId["UK"] = 78;
        countryGLOBIOMId["Ukraine"] = 224;
        countryGLOBIOMId["UntdArabEm"] = 9;
        countryGLOBIOMId["Uruguay"] = 226;
        countryGLOBIOMId["USA"] = 227;
        countryGLOBIOMId["Uzbekistan"] = 228;
        countryGLOBIOMId["Vanuatu"] = 235;
        countryGLOBIOMId["Venezuela"] = 231;
        countryGLOBIOMId["VietNam"] = 234;
        countryGLOBIOMId["WestSahara"] = 67;
        countryGLOBIOMId["Yemen"] = 238;
        countryGLOBIOMId["Zambia"] = 240;
        countryGLOBIOMId["Zimbabwe"] = 241;
    }

    void setCountriesWoodProdStat() noexcept {
        for (size_t i = 0; i < woodProdEUStats.size(); ++i)
            for (size_t j = 0; j < woodProdEUStats[0].size(); ++j)
                // Malta is in countryNwp[18], there are no wood production data for Malta
                countriesWoodProdStat.setVal(countryNwp[i + (i >= 18)], 1990 + j, woodProdEUStats[i][j]);
    }

    void setCountriesfmEmission_unfccc() noexcept {
        for (size_t i = 0; i < fmEmission_unfccc_CRF.size(); ++i)
            for (size_t j = 0; j < fmEmission_unfccc_CRF[0].size(); ++j)
                countriesfmEmission_unfccc.setVal(eu28OrderCode[i], 1990 + j, fmEmission_unfccc_CRF[i][j]);
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

    ifstream checkFile(const string_view fileName) {
        auto filePath = fs::path{settings.inputPath} / fileName;
        ifstream fp{filePath};

        if (!fp.is_open()) {
            FATAL("Cannot read input file: {} !", filePath.string());
            throw runtime_error{format("Cannot read input file: {} !", filePath.string())};
        }

        string line;
        getline(fp, line);

        if (line.empty()) {
            FATAL("Empty input file: {} !", filePath.string());
            throw runtime_error{format("Empty input file: {} !", filePath.string())};
        }

        fp.seekg(0, ios::beg);
        return fp;
    }

    void readInputDet() {
        INFO("> Reading the rest of input data...");
        ifstream fp = checkFile(fileName_dat);
        string line;
        getline(fp, line);

        auto get_HeaderName_YearFromHeaderColumn = [](const string &s) -> pair<string, optional<uint16_t> > {
            size_t num_pos = s.find_first_of("012345789");
            if (num_pos == string::npos)
                return {s, {}};
            return {s.substr(0, num_pos), stoi(s.substr(num_pos, s.length()))};
        };

        auto header_columns = line | rv::transform(::toupper) | rv::split(',') | ranges::to<vector<string> >();
        vector<pair<string, optional<uint16_t> > > header;
        header.reserve(header_columns.size());
        for (const auto &header_column: header_columns)
            header.push_back(get_HeaderName_YearFromHeaderColumn(header_column));

        plots.reserve(3'000);
        vector<double> line_cells;
        while (fp) {
            getline(fp, line);
            if (!line.empty() && line[0] != '#') {
                line_cells = line | rv::split(',') |
                             rv::transform([](const auto &cell) {  // subrange
                                 if (cell.empty()) {
                                     ERROR("!!! CSV line {} empty cell, substituted by 0", plots.size() + 2);
                                     return 0.;
                                 }
                                 return stod(string{cell.begin(), cell.end()});
                             }) | ranges::to<vector<double> >();
                plots.emplace_back(header, line_cells);
            }
        }
        INFO("Successfully read {} lines.", plots.size() + 1);
    }

    datamapType readHistoric(const string_view file_path, const string_view message,
                             const uint16_t firstYear, const uint16_t lastYear) {
        INFO("> Reading the Historic {} 2000-2020...", message);
        ifstream fp = checkFile(file_path);
        string line;
        getline(fp, line);

        auto year_columns = line | rv::split(',') | rv::drop_while(
                [](const auto &s) { return string_view{s}.find_first_of("012345789") == string::npos; }) |
                            rv::transform([](const auto &s) { return stoi(string{s.begin(), s.end()}); }) |
                            ranges::to<vector<uint16_t> >();

        auto getTrimmingOffsets = [&]() -> pair<size_t, size_t> {
            return {distance(year_columns.begin(), ranges::lower_bound(year_columns, firstYear)),
                    distance(year_columns.begin(), prev(ranges::upper_bound(year_columns, lastYear)))};
        };
        const auto [offset_first, offset_last] = getTrimmingOffsets();

        datamapType datamap;
        datamap.reserve(30);
        vector<string> s_row;
        vector<double> d_row;
        while (fp) {
            getline(fp, line);
            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                d_row = s_row | rv::drop(s_row.size() - year_columns.size() + offset_first) |
                        rv::take(offset_last + 1) |
                        rv::transform([&](const auto &cell) {  // subrange
                            if (cell.empty()) {
                                ERROR("!!! CSV line {} empty cell, substituted by 0", datamap.size() + 2);
                                return 0.;
                            }
                            return stod(string{cell.begin(), cell.end()});
                        }) | ranges::to<vector<double> >();
                if (countryGLOBIOMId.contains(s_row[0]))
                    datamap[countryGLOBIOMId[s_row[0]]] = {year_columns, d_row};
                else
                    ERROR("!!! No such country: {}, line: {}", s_row[0], datamap.size() + 2);
            }
        }
        INFO("Successfully read {} lines.", datamap.size() + 1);
        return datamap;
    }

    void readDatamaps() {
        histLandPrice = readHistoric(fileName_lp0, "Land Price", 2000, 2020);
        histWoodPrice = readHistoric(fileName_wp0, "Wood Price", 2000, 2020);
        histWoodDemand = readHistoric(fileName_wd0, "Wood Demand", 1990, 2021);
        histResiduesDemand = readHistoric(fileName_rd0, "Residues Demand", 2000, 2020);
    }

    heterDatamapScenariosType readGlobiomScenarios(const string_view file_path, const string_view message) {
        INFO("> Reading the Globiom Scenarios {}...", message);
        ifstream fp = checkFile(file_path);
        string line;
        getline(fp, line);

        auto year_columns = line | rv::split(',') | rv::drop_while(
                [](const auto &s) { return string_view{s}.find_first_of("012345789") == string::npos; }) |
                            rv::transform([](const auto &s) { return stoi(string{s.begin(), s.end()}); }) |
                            ranges::to<vector<uint16_t> >();

        heterDatamapScenariosType scenariosDatamaps;
        scenariosDatamaps.reserve(3'400);
        vector<string> s_row;
        vector<double> d_row;
        string scenario_name;
        while (fp) {
            getline(fp, line);
            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                d_row = s_row | rv::drop(s_row.size() - year_columns.size()) |
                        rv::transform([&](const auto &cell) {  // subrange
                            if (cell.empty()) {
                                ERROR("!!! CSV line {} empty cell, substituted by 0", scenariosDatamaps.size() + 2);
                                return 0.;
                            }
                            return stod(string{cell.begin(), cell.end()});
                        }) | ranges::to<vector<double> >();
                scenario_name = s_row[3] + '_' + s_row[4] + '_' + s_row[5];
                if (countryGLOBIOMId.contains(s_row[0]))
                    scenariosDatamaps[scenario_name][countryGLOBIOMId[s_row[0]]] = {year_columns, d_row};
                else
                    ERROR("!!! No such country: {}, line: {}", s_row[0], scenariosDatamaps.size() + 2);
            }
        }
        INFO("Successfully read {} lines.", scenariosDatamaps.size() + 1);
        return scenariosDatamaps;
    }

    void readGlobiom() {
        landPriceScenarios = readGlobiomScenarios(fileName_lp, "Land Price");
        woodPriceScenarios = readGlobiomScenarios(fileName_wp, "Wood Price");
        woodDemandScenarios = readGlobiomScenarios(fileName_wd, "Wood Demand");
        residuesDemandScenarios = readGlobiomScenarios(fileName_rd, "Residues Demand");
    }

    void convertUnitsDatamaps() noexcept {
        for (auto &[id, ipol]: histWoodDemand)
            for (auto &[key, value]: ipol.data)
                value *= 1'000;
        for (auto &[id, ipol]: histResiduesDemand)
            for (auto &[key, value]: ipol.data)
                value *= 250;
        for (auto &[scenario, datamap]: woodDemandScenarios)
            for (auto &[id, ipol]: datamap)
                for (auto &[key, value]: ipol.data)
                    value *= 1'000;
        for (auto &[scenario, datamap]: residuesDemandScenarios)
            for (auto &[id, ipol]: datamap)
                for (auto &[key, value]: ipol.data)
                    value *= 250;
    }

    void readGlobiomLandCountryCalibrate_calcCountryLandArea() {
        if (fileName_gl_country_0.empty()) {
            WARN("No GLOBIOM LC country data for 2000-2020!!!!");
            return;
        }

        INFO("> Reading the GLOBIOM land country data for 2000-2020...");
        ifstream fp = checkFile(fileName_gl_country_0);
        string line;
        getline(fp, line);

        size_t first_data_column = 5;  // Forest,Arable,Natural,Wetland,Blocked
        auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                      ranges::to<vector<string> >();

        auto s_bauScenario = string{bauScenario};
        globiomAfforMaxCountryScenarios[s_bauScenario].reserve(250);
        globiomLandCountryScenarios[s_bauScenario].reserve(250);

        vector<string> s_row;
        vector<double> d_row;

        uint32_t line_num = 2;
        for (; fp; ++line_num) {
            getline(fp, line);

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                uint16_t year = stoi(s_row[first_data_column - 1]);

                if (year <= 2020) {
                    d_row = s_row | rv::drop(first_data_column) |
                            rv::transform([&](const auto &cell) {  // subrange
                                if (cell.empty()) {
                                    ERROR("!!! CSV line {} empty cell, substituted by 0", line_num);
                                    return 0.;
                                }
                                return stod(string{cell.begin(), cell.end()});
                            }) | ranges::to<vector<double> >();

                    if (countryGLOBIOMId.contains(s_row[0])) {
                        uint8_t id = countryGLOBIOMId[s_row[0]];
                        double gl_tmp = 0;
                        double gl_tot = 0;

                        for (const auto &[type, cell]: rv::zip(header, d_row)) {
                            if (year == 2000 && type == "FOREST")
                                gl_tot = cell;
                            else if (type == "NATURAL") {
                                globiomAfforMaxCountryScenarios[s_bauScenario][id].data[year] = cell;
                                if (year == 2000)
                                    gl_tot += cell;
                            } else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                gl_tmp += cell;
                        }

                        globiomLandCountryScenarios[s_bauScenario][id].data[year] = gl_tmp;
                        if (year == 2000)
                            countryLandArea[id] = gl_tot + gl_tmp;
                    } else
                        ERROR("!!! No such country: {}, line: {}", s_row[0], line_num);
                }
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

    void readGlobiomLandCountry() {
        if (fileName_gl_country.empty()) {
            WARN("No GLOBIOM LC country data!!!!");
            return;
        }

        INFO("> Reading the GLOBIOM land country data...");
        ifstream fp = checkFile(fileName_gl_country);
        string line;
        getline(fp, line);

        const size_t first_data_column = 5;  // Forest,Arable,Natural,Wetland,Blocked
        auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                      ranges::to<vector<string> >();

        globiomAfforMaxCountryScenarios.reserve(27'500);
        globiomLandCountryScenarios.reserve(27'500);

        vector<string> s_row;
        vector<double> d_row;
        string scenario_name;
        uint32_t line_num = 2;
        for (; fp; ++line_num) {
            getline(fp, line);

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                uint16_t year = stoi(s_row[first_data_column - 1]);

                if (year > 2020 && year <= coef.eYear) {
                    d_row = s_row | rv::drop(first_data_column) |
                            rv::transform([&](const auto &cell) {  // subrange
                                if (cell.empty()) {
                                    ERROR("!!! CSV line {} empty cell, substituted by 0", line_num);
                                    return 0.;
                                }
                                return stod(string{cell.begin(), cell.end()});
                            }) | ranges::to<vector<double> >();

                    if (countryGLOBIOMId.contains(s_row[0])) {
                        uint8_t id = countryGLOBIOMId[s_row[0]];
                        scenario_name = s_row[1] + '_' + s_row[2] + '_' + s_row[3];
                        double gl_tmp = 0;
                        for (const auto &[type, cell]: rv::zip(header, d_row)) {
                            if (type == "NATURAL") {
                                globiomAfforMaxCountryScenarios[scenario_name][id].data[year] = cell;
                            } else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                gl_tmp += cell;
                        }
                        globiomLandCountryScenarios[scenario_name][id].data[year] = gl_tmp;
                    } else
                        ERROR("!!! No such country: {}, line: {}", s_row[0], line_num);
                }
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

    void readCO2price() {
        INFO("> Reading the CO2 prices...");
        ifstream fp = checkFile(fileName_co2p);
        string line;
        getline(fp, line);

        auto year_columns = line | rv::split(',') | rv::drop_while(
                [](const auto &s) { return string_view{s}.find_first_of("012345789") == string::npos; }) |
                            rv::transform([](const auto &s) { return stoi(string{s.begin(), s.end()}); }) |
                            ranges::to<vector<uint16_t> >();
        size_t idx_ge_refYear = distance(year_columns.begin(), ranges::lower_bound(year_columns, refYear));
        CO2PriceScenarios.reserve(3'400);
        vector<string> s_row;
        vector<double> d_row;
        string scenario_name;
        while (fp) {
            getline(fp, line);
            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                d_row = s_row | rv::drop(s_row.size() - year_columns.size()) |
                        rv::transform([&](const auto &cell) {  // subrange
                            if (cell.empty()) {
                                ERROR("!!! CSV line {} empty cell, substituted by 0", CO2PriceScenarios.size() + 2);
                                return 0.;
                            }
                            return stod(string{cell.begin(), cell.end()});
                        }) | ranges::to<vector<double> >();
                scenario_name = s_row[3] + '_' + s_row[4] + '_' + s_row[5];

                if (countryGLOBIOMId.contains(s_row[0])) {
                    CO2PriceScenarios[scenario_name][countryGLOBIOMId[s_row[0]]] = {year_columns, d_row};
                    CO2PriceScenarios[scenario_name][countryGLOBIOMId[s_row[0]]].data[refYear] = d_row[idx_ge_refYear];
                } else
                    ERROR("!!! No such country: {}, line: {}", s_row[0], CO2PriceScenarios.size() + 2);
            }
        }
        INFO("Successfully read {} lines.", CO2PriceScenarios.size() + 1);
    }

    void readNUTS2() {
        INFO("> Reading the NUTS2...");
        ifstream fp = checkFile(fileName_nuts2);
        string line;
        getline(fp, line);

        vector<string> s_row;
        while (fp) {
            getline(fp, line);
            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                uint32_t x = lround((stod(s_row[0]) + 180) / gridStep - 0.5);
                uint32_t y = lround((stod(s_row[1]) + 90) / gridStep - 0.5);
                nuts2id[{x, y}] = s_row[2];
            }
        }
        INFO("Successfully read {} lines.", nuts2id.size() + 1);
    }

    void printPlots() noexcept {
        TRACE("Plots:");
        for (size_t i = 0; const auto &plot: plots)
            TRACE("plots[{}]:\n{}", i++, plot.str());
    }

    void printDatamap(const datamapType &datamap, const string_view message) noexcept {
        TRACE("{}", message);
        for (const auto &[id, ipol]: datamap)
            TRACE("{}\n{}", idCountryGLOBIOM[id], ipol.str());
    }

    void printDatamapScenarios(const heterDatamapScenariosType &datamapScenarios, const string_view message) noexcept {
        TRACE("{}", message);
        for (const auto &[scenario, datamap]: datamapScenarios) {
            TRACE("{}", scenario);
            for (const auto &[id, ipol]: datamap)
                TRACE("{}\n{}", idCountryGLOBIOM[id], ipol.str());
        }
    }

    void printCountryLandArea() noexcept {
        for (uint16_t i = 0; const auto area: countryLandArea)
            TRACE("{} area = {}", idCountryGLOBIOM[i++], area);
    }

    void printNuts2Id() noexcept {
        for (const auto &[cords, NUTS2]: nuts2id)
            TRACE("x = {}, y = {}, NUTS2 = {}", cords.first, cords.second, NUTS2);
    }

    void printSimuId(const simuIdType &simuIdDatamap, const string_view message) noexcept {
        TRACE("{}", message);
        for (const auto &[id, ipol]: simuIdDatamap)
            TRACE("{}\n{}", id, ipol.str());
    }

    void printSimuIdScenarios(const heterSimuIdScenariosType &simuIdScenarios, const string_view message) noexcept {
        TRACE("{}", message);
        for (const auto &[scenario, simuIdDatamap]: simuIdScenarios) {
            TRACE("{}", scenario);
            for (const auto &[simu_id, ipol]: simuIdDatamap)
                TRACE("{}\n{}", simu_id, ipol.str());
        }
    }

    void printData() noexcept {
//        printPlots();
//
//        printDatamap(histLandPrice, "Historic Land Price");
//        printDatamap(histWoodPrice, "Historic Wood Price");
//        printDatamap(histWoodDemand, "Historic Wood Demand");
//        printDatamap(histResiduesDemand, "Historic Residues Demand");
//
//        printDatamapScenarios(landPriceScenarios, "Globiom scenarios Land Price");
//        printDatamapScenarios(woodPriceScenarios, "Globiom scenarios Wood Price");
//        printDatamapScenarios(woodDemandScenarios, "Globiom scenarios Wood Demand");
//        printDatamapScenarios(residuesDemandScenarios, "Globiom scenarios Residues Demand");
//        printDatamapScenarios(globiomAfforMaxCountryScenarios, "globiomAfforMaxCountryScenarios");
//        printDatamapScenarios(globiomLandCountryScenarios, "globiomLandCountryScenarios");
//        printDatamapScenarios(CO2PriceScenarios, "CO2PriceScenarios");
//
//        printCountryLandArea();
//        printNuts2Id();
//
//        printSimuIdScenarios(maiClimateShifters, "maiClimateShifters");
//
//        printSimuId(disturbWind, "disturbWind");
//        printSimuId(disturbFire, "disturbFire");
//        printSimuId(disturbBiotic, "disturbBiotic");
        printSimuId(disturbWindExtreme, "disturbWindExtreme");
        printSimuId(disturbFireExtreme, "disturbFireExtreme");
        printSimuId(disturbBioticExtreme, "disturbBioticExtreme");
    }

    void correctNUTS2Data() noexcept {
        for (const auto &plot: plots)
            // Test only some regions and some countries
            if (regions.contains(plot.polesReg) && countriesList.contains(plot.country) &&
                plot.protect.data.at(2000) == 0) {  // if there is no lerp, why not simple map then?
                // locate the struct with asID == asID within the country
                string_view countryISO = countryOrderISO2[countryCodeOrder[plot.country - 1]];

                if (auto it_nuts2 = nuts2id.find({plot.x, plot.y}); it_nuts2 != nuts2id.end()) {
                    auto &[coords, NUTS2] = *it_nuts2;
                    if (countryISO == "GB")
                        countryISO = "UK";
                    if (NUTS2.substr(0, 2) != countryISO) {
                        nuts2grid.setNeighNum(2, 2);
                        auto neighbours = nuts2grid.getNeighValues(plot.x, plot.y);
                        auto it_nearbyCountry = ranges::find_if(neighbours, [countryISO](const string_view el) {
                            return el.substr(0, 2) == countryISO;
                        });
                        if (it_nearbyCountry != neighbours.end()) {
                            DEBUG("x = {}, y = {}, NUTS2 = {}, *it_nearbyCountry = {}, countryISO = {}",
                                  plot.x, plot.y, NUTS2, *it_nearbyCountry, countryISO);
                            NUTS2 = *it_nearbyCountry;
                        } else
                            ERROR("!no x = {}, y = {}, NUTS2 = {}, countryISO = {}", plot.x, plot.y, NUTS2, countryISO);
                    }
                } else {
                    nuts2grid.setNeighNum(2, 2);
                    auto neighbours = nuts2grid.getNeighValues(plot.x, plot.y);
                    auto it_nearbyCountry = ranges::find_if(neighbours, [countryISO](const string_view el) {
                        return el.substr(0, 2) == countryISO;
                    });
                    if (it_nearbyCountry != neighbours.end()) {
                        DEBUG("x = {}, y = {}, *it_nearbyCountry = {}, countryISO = {}",
                              plot.x, plot.y, *it_nearbyCountry, countryISO);
                        nuts2id[{plot.x, plot.y}] = *it_nearbyCountry;
                    } else
                        ERROR("!no x = {}, y = {}, countryISO = {}", plot.x, plot.y, countryISO);
                }
            }
    }

    // Setup forest increment table
    void defineSpecies() {
        species.reserve(8);
        // fir [0]
        species.emplace_back(
                array{-0.4562, -0.7403, -1.0772, 1.4803, 0.6713, 300., -0.2151, -0.9929, 0.5, 0.2, -0.7642, 0.3156,
                      -0.4, 0.4468, 0.1425, 0., 0., 0., 0.25, -1., -2., -0.5, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8,
                      0.002, 2., 0.01, 0.5, 24.82935, 0.60708, -0.0212, 2.41308, -0.48246, 16.11, 17.78, -0.01436,
                      0.37424, -1.52401, 2.282, 1.2718, -0.7707, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // spruce [1]
        species.emplace_back(
                array{0., -0.9082, -0.2728, 0.6483, 209.6889, 300., 1.8536, 0.4811, 0., 0.9, -0.143, -0.5915, -0.4,
                      0.4507, 0.3713, 0., 0., 0., 0.1, 1., -2., -0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2.,
                      0.01, 0.5, 22.58866, 0.6168, -0.02102, 2.4176, -0.35816, 16.11, 17.78, -0.01436, 0.37424,
                      -1.52401, 2.282, 1.2718, -0.7707, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // pine [2]
        species.emplace_back(array{-0.3835, -0.2416, -1.7576, 1.1638,                 // k
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
        // pinus Halepensis [3]
        species.emplace_back(
                array{-0.3, -0.306, -2.052, 1.673, 150., 130., 1.898, -1.141, 0.92, 0.07, -4.25, 6.168, -0.4, 0.93237,
                      -0.00468, 0., 0., 0., 0.25, -1., -2., -0.5, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2., 0.01,
                      0.5, 26.59488, 0.62839, -0.02023, 1.05953, -0.03489, 18.7252, 46.3833, -0.2643, 14.1425, -0.6368,
                      0.8948, 0.000422233, -4.9625, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // birch [4]
        species.emplace_back(
                array{0., -0.7422, -0.54, 0.5719, 136.958, 100., 0.2972, -0.7543, 0., 0.9, -0.953, -0.9236, -0.4, 1.052,
                      0.108, 0., 0., 0., 0.1, 1., -2.5, -0.4, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2., 0.01, 0.5,
                      23.23597, 0.44554, -0.02485, 1.36973, -0.42941, 13.61, 10.69, -0.02688, 0.24196, -0.7015, 1.337,
                      0.0708, -2.1515, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // beech [5]
        species.emplace_back(
                array{0., -0.5998, -0.2467, 0.7674, 245.5516, 100., 2.6345, -0.8978, 0.69135, 0., 0., 0., -0.03177, 0.,
                      0., 0., 0., 0., -0.875, 1., 0., 0., 0.5, 1.5, 300., 0.01, 0.5, 0.5, 0.8, 0.001, 2., 0.01, 0.5,
                      21.29459, 0.48722, -0.01967, 1.81479, -0.29145, 30.707, 7.008, -0.01051, -0.19793, 0.2977, 1.423,
                      1.025, -16.85, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // oak [6]
        species.emplace_back(
                array{0., -0.6, -0.4419, 0.3179, 16.6688, 300., -0.6066, -1.1243, 0.7, 0.3, -0.4339, 0.5288, -0.4,
                      2.01561, -0.07354, 0., 0., 0., 0.1, 1., -3.5, 0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2.,
                      0.01, 0.5, 21.26281, 0.51987, -0.01901, 1.34081, -0.10979, -7.511, 41.689, -0.02201, 0.58055,
                      1.72465, 3.6757, 1.7544, 0.3264, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
        // larch [7]
        species.emplace_back(
                array{0., -0.388, -0.01226, 0.85934, 195.3724, 600., 0.9883, 1.0784, 0., 0.9, -2.1347, -0.3437, -0.4,
                      1.3238, 0.4061, 0., 0., 0., 0.1, 1., -2.5, -0.3, 0.5, 1.5, 150., 0.01, 0.5, 0.5, 0.8, 0.002, 2.,
                      0.01, 0.5, 23.63487, 0.50281, -0.01557, 1.16199, -0.18673, 25.196, 9.118, -0.01376, 0.64637,
                      -0.79909, 1.0817, 0.1667, -0.9408, 1.6, 0.95}, 5, 0.25, 600, 1, 0.25, 1.5, 0.25, 1);
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

        ffsws = FFIpol<double>{sws};
        ffhlv = FFIpol<double>{hlv};
        ffhle = FFIpol<double>{hle};
        ffsdMinH = FFIpol<double>{sdMinH};
        ffsdMaxH = FFIpol<double>{sdMaxH};
    }

    void correctMAI() noexcept {
        for (auto &plot: plots)
            // Test only some regions and some countries
            if (regions.contains(plot.polesReg) && countriesList.contains(plot.country) &&
                plot.protect.data.at(2000) == 0) {  // if there is no lerp, why not simple map then?
                // forest with specified age structure
                plot.MAIE.data[2000] *= maiCoefficients[plot.country];
                plot.MAIN.data[2000] *= maiCoefficients[plot.country];
            }
    }

    void calculateAverageMAI() noexcept {
        INFO("calculating average MAI");
        array<double, numberOfCountries> forestAreaCountry{};
        for (const auto &plot: plots)
            // Test only some regions and some countries
            if (regions.contains(plot.polesReg) && countriesList.contains(plot.country) &&
                plot.protect.data.at(2000) == 0) {
                double forestArea0 = plot.landArea * 100 * clamp(plot.forest, 0., 1.);
                if (forestArea0 > 0) {
                    // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
                    MAI_CountryUprotect[plot.country - 1] += plot.MAIE.data.at(2000) * forestArea0;
                    forestAreaCountry[plot.country - 1] += forestArea0;
                }
            }

        for (auto &&[MAI, area]: rv::zip(MAI_CountryUprotect, forestAreaCountry))
            if (area > 0)
                MAI /= area;

        for (size_t i = 0; i < MAI_CountryUprotect.size(); ++i)
            if (MAI_CountryUprotect[i] > 0)
                DEBUG("MAI_CountryUprotect[{}] = {}", i, MAI_CountryUprotect[i]);

        INFO("calculated average MAI");
    }

    void initPlotsSimuID() noexcept {
        for (const auto &plot: plots)
            plotsSimuID[{plot.x, plot.y}] = plot.simuID;
    }

    void readMAIClimate() {
        if (!MAIClimateShift) {
            INFO("MAIClimateShift is turned off");
            return;
        }

        if (fileName_maic.empty()) {
            WARN("No MAI climate data!!!!");
            return;
        }

        INFO("> Reading the MAI climate data...");
        ifstream fp = checkFile(fileName_maic);
        string line;
        getline(fp, line);

        // ...1,ClimaScen,ANYRCP,lon,lat,Year,G4MOutput,value
        maiClimateShifters.reserve(96);  // 2'373'408 / 24'723 = 96

        vector<string> s_row;
        string scenario_name;
        uint32_t line_num = 2;

        for (; fp; ++line_num) {
            getline(fp, line);

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();

                if (s_row[6] == "mai")
                    if (uint16_t year = stoi(s_row[5]); year <= coef.eYear) {

                        uint32_t x = lround((stod(s_row[3]) + 180) / gridStep - 0.5);
                        uint32_t y = lround((stod(s_row[4]) + 90) / gridStep - 0.5);

                        if (auto g4mId = plotsSimuID.find({x, y}); g4mId != plotsSimuID.end()) {
                            // TODO to test uppercase and lowercase
                            // scenario_name = s_row[2] + '_' + s_row[1] | rv::transform(::tolower) | ranges::to<string>();
                            scenario_name = s_row[2] + '_' + s_row[1];
                            double value = stod(s_row[7]);
                            maiClimateShifters[scenario_name][g4mId->second].data[year] = value;
                        }
                    }
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

    // Scaling the MAI climate shifters to the 2020 value (i.e., MAIShifter_year = MAIShifter_year/MAIShifter_2000, so the 2000 value = 1);
    void scaleMAIClimate2020() noexcept {
        if (!scaleMAIClimate) {
            INFO("scaleMAIClimate is turned off");
            return;
        }

        INFO("Scaling MAI climate shifters to the 2020 value!");
        for (auto &[scenario, MAI]: maiClimateShifters)
            for (auto &[simu_id, ipol]: MAI) {
                double reciprocal_value_2020 = 1 / ipol.data[2020];
                ipol *= reciprocal_value_2020;
            }
    }

    void applyMAIClimateShifters() noexcept {
        if (!MAIClimateShift) {
            INFO("MAIClimateShift is turned off");
            return;
        }

        for (auto &plot: plots)
            // Test only some regions and some countries
            if (regions.contains(plot.polesReg) && countriesList.contains(plot.country)) {
                double maie = plot.MAIE.data[2000];
                double main = plot.MAIN.data[2000];
                double npp = plot.NPP.data[2000];

                plot.decHerb.data[2020] = plot.decHerb.data[2000];
                plot.decWood.data[2020] = plot.decWood.data[2000];
                plot.decSOC.data[2020] = plot.decSOC.data[2000];

                for (const auto &[scenario, MAI]: maiClimateShifters)
                    if (MAI.contains(plot.simuID))
                        for (const auto [year, value]: MAI.at(plot.simuID).data) {
                            plot.MAIE.data[year] = maie * value;
                            plot.MAIN.data[year] = main * value;
                            plot.NPP.data[year] = npp * value;
                        }
            }
    }

    void readDisturbances() {
        if (!disturbanceTrend) {
            INFO("disturbanceTrend is turned off");
            return;
        }

        if (fileName_disturbance.empty()) {
            WARN("No disturbance projection data!!!!");
            return;
        }

        INFO("> Reading the disturbance data...");
        ifstream fp = checkFile(fileName_disturbance);
        string line;
        getline(fp, line);

        // "","lon","lat","Year","Agent","value"
        disturbWind.reserve(22'000);
        disturbFire.reserve(22'000);
        disturbBiotic.reserve(22'000);

        vector<string> s_row;
        uint32_t line_num = 2;

        for (; fp; ++line_num) {
            getline(fp, line);
            erase(line, '"');  // trimming double quotes

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();

                if (uint16_t year = stoi(s_row[3]); year <= coef.eYear) {
                    uint32_t x = lround((stod(s_row[1]) + 180) / gridStep - 0.5);
                    uint32_t y = lround((stod(s_row[2]) + 90) / gridStep - 0.5);

                    if (auto g4mId = plotsSimuID.find({x, y}); g4mId != plotsSimuID.end()) {
                        double value = stod(s_row[5]);

                        if (s_row[4] == "wind")
                            disturbWind[g4mId->second].data[year] = value;
                        else if (s_row[4] == "fire")
                            disturbFire[g4mId->second].data[year] = value;
                        else if (s_row[4] == "biotic")
                            disturbBiotic[g4mId->second].data[year] = value;
                        else
                            WARN("Unknown agent type: {}, line: {}", s_row[4], line_num);
                    }
                }
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

    void add2020Disturbances() noexcept {
        for (auto &[id, ipol]: disturbWind)
            ipol.data[2020] = ipol.data[2030] / 1.025;
        for (auto &[id, ipol]: disturbFire)
            ipol.data[2020] = ipol.data[2030] / 1.05;
        for (auto &[id, ipol]: disturbBiotic)
            ipol.data[2020] = ipol.data[2030] / 1.05;
    }

    void scaleDisturbance(simuIdType &disturbance, const uint16_t scaleYear) {
        for (auto &[id, ipol]: disturbance)
            ipol += -ipol.data[scaleYear];
    }

    void scaleDisturbances2020() noexcept {
        if (!scaleDisturbance2020) {
            INFO("scaleDisturbance2020 is turned off");
            return;
        }

        const uint16_t scaleYear = 2020;
        scaleDisturbance(disturbWind, scaleYear);
        scaleDisturbance(disturbFire, scaleYear);
        scaleDisturbance(disturbBiotic, scaleYear);

        INFO("Disturbances are scaled to the {} value!", scaleYear);
    }

    void readDisturbancesExtreme() {
        if (!disturbanceExtreme) {
            INFO("disturbanceExtreme is turned off");
            return;
        }

        if (fileName_disturbanceExtreme.empty()) {
            WARN("No extreme disturbance projection data!!!!");
            return;
        }

        INFO("> Reading the extreme disturbance data ...");
        ifstream fp = checkFile(fileName_disturbanceExtreme);
        string line;
        getline(fp, line);

        // "","lon","lat","Year","Agent","value"
        disturbWind.reserve(25'000);
        disturbFire.reserve(25'000);
        disturbBiotic.reserve(25'000);

        vector<string> s_row;
        uint32_t line_num = 2;

        for (; fp; ++line_num) {
            getline(fp, line);
            erase(line, '"');  // trimming double quotes

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();

                if (uint16_t year = stoi(s_row[3]); year <= coef.eYear) {
                    uint32_t x = lround((stod(s_row[1]) + 180) / gridStep - 0.5);
                    uint32_t y = lround((stod(s_row[2]) + 90) / gridStep - 0.5);

                    if (auto g4mId = plotsSimuID.find({x, y}); g4mId != plotsSimuID.end()) {
                        double value = stod(s_row[5]);

                        if (s_row[4] == "wind")
                            disturbWindExtreme[g4mId->second].data[year] = value;
                        else if (s_row[4] == "fire")
                            disturbFireExtreme[g4mId->second].data[year] = value;
                        else if (s_row[4] == "biotic")
                            disturbBioticExtreme[g4mId->second].data[year] = value;
                        else
                            WARN("Unknown agent type: {}, line: {}", s_row[4], line_num);
                    }
                }
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

// Lists of countries, regions and mixed to be considered
    void Init() {
        settings.readSettings("settings_Europe_dw_v02.ini");
        coef.readCoef(settings.coeffPath);
        yearsToConsider(1990, 2070);
        regionsToConsider();
        countryRegionsToConsider();
        countriesFmcpolToConsider();
        countriesToConsider();
        setCountryData();
        setCountrySpecies();
        setIdCountryGLOBIOM();
        setCountryGLOBIOMId();
        setCountriesWoodProdStat();
        setCountriesfmEmission_unfccc();
        calcAvgFM_sink_stat();
        readInputDet();
        readDatamaps();
        readGlobiom();
        convertUnitsDatamaps();
        readGlobiomLandCountryCalibrate_calcCountryLandArea();
        readGlobiomLandCountry();
        readCO2price();
        readNUTS2();
        nuts2grid.fillFromNUTS(nuts2id);
        correctNUTS2Data();
        defineSpecies();
        setupFMP();
        correctMAI();
        calculateAverageMAI();
        initPlotsSimuID();
        readMAIClimate();
        scaleMAIClimate2020();
        applyMAIClimateShifters();
        readDisturbances();
        add2020Disturbances();
        scaleDisturbances2020();
        readDisturbancesExtreme();
        printData();
    }
}

#endif
