#ifndef G4M_EUROPE_DG_ARRAYS_PROJECT_DEPENDENT_HPP
#define G4M_EUROPE_DG_ARRAYS_PROJECT_DEPENDENT_HPP

#include <string>
#include <array>

#include "../constants.hpp"

using namespace std;

using namespace g4m::Constants;

namespace g4m::Arrays {
    // country specific harvest losses NEW Country Codes!!!! Shifted by -1 to start array from 0
    // Source: Forest Resources of Europe; CIS; North America; Australia; Japan and New Zealand: Main Report 2000
    // Estimated by Hannes Bottcher??
    consteval array<double, numberOfCountries> initCountryLosses() noexcept {
        array<double, numberOfCountries> countryLosses{};
        countryLosses.fill(0.15);
        countryLosses[1] = 0.15;
        countryLosses[2] = 0.15;
        countryLosses[5] = 0.15;
        countryLosses[8] = 0.15;
        countryLosses[9] = 0.15;
        countryLosses[10] = 0.15;
        countryLosses[15] = 0.15;
        countryLosses[16] = 0.13; // Austria
        countryLosses[17] = 0.15;
        countryLosses[18] = 0.15;
        countryLosses[19] = 0.05;    //0.15	; Belgium
        countryLosses[20] = 0.15;
        countryLosses[21] = 0.15;
        countryLosses[22] = 0.15;
        countryLosses[23] = 0.16;    //0.32	; Bulgaria
        countryLosses[25] = 0.15;
        countryLosses[26] = 0.15;
        countryLosses[27] = 0.15;
        countryLosses[28] = 0.15;
        countryLosses[30] = 0.15;
        countryLosses[31] = 0.15;
        countryLosses[32] = 0.15;
        countryLosses[33] = 0.15;
        countryLosses[34] = 0.15;
        countryLosses[36] = 0.15;
        countryLosses[37] = 0.15;
        countryLosses[38] = 0.15;
        countryLosses[40] = 0.14;
        countryLosses[41] = 0.15;
        countryLosses[42] = 0.15;
        countryLosses[43] = 0.15;
        countryLosses[44] = 0.15;
        countryLosses[45] = 0.15;
        countryLosses[46] = 0.15;
        countryLosses[48] = 0.15;
        countryLosses[49] = 0.15;
        countryLosses[51] = 0.15;
        countryLosses[52] = 0.15;
        countryLosses[55] = 0.15;
        countryLosses[56] = 0.09;    //0.2	; Czech
        countryLosses[57] = 0.2;
        countryLosses[59] = 0.15;
        countryLosses[60] = 0.15;
        countryLosses[61] = 0.15;
        countryLosses[62] = 0.15;
        countryLosses[63] = 0.15;
        countryLosses[65] = 0.15;
        countryLosses[67] = 0.04;    //0.15	;//Spain
        countryLosses[68] = 0.1;    //0.15	;//Estonia
        countryLosses[69] = 0.15;
        countryLosses[70] = 0.09;
        countryLosses[71] = 0.15;
        countryLosses[73] = 0.05;    //0.21	; France
        countryLosses[76] = 0.15;
        countryLosses[76] = 0.15;
        countryLosses[77] = 0.14;
        countryLosses[78] = 0.15;
        countryLosses[79] = 0.15;
        countryLosses[81] = 0.15;
        countryLosses[81] = 0.15;
        countryLosses[86] = 0.15;
        countryLosses[87] = 0.15;
        countryLosses[89] = 0.15;
        countryLosses[92] = 0.15;
        countryLosses[95] = 0.15;
        countryLosses[96] = 0.07;
        countryLosses[97] = 0.15;
        countryLosses[98] = 0.12;
        countryLosses[99] = 0.15;
        countryLosses[100] = 0.15;
        countryLosses[102] = 0.06;    //0.15	; Ireland 10.02.2023 national value
        countryLosses[103] = 0.15;
        countryLosses[104] = 0.15;
        countryLosses[106] = 0.15;
        countryLosses[107] = 0.04;
        countryLosses[108] = 0.15;
        countryLosses[110] = 0.15;
        countryLosses[111] = 0.15;
        countryLosses[112] = 0.15;
        countryLosses[113] = 0.15;
        countryLosses[114] = 0.15;
        countryLosses[117] = 0.15;
        countryLosses[117] = 0.15;
        countryLosses[118] = 0.15;
        countryLosses[119] = 0.15;
        countryLosses[120] = 0.15;
        countryLosses[121] = 0.15;
        countryLosses[122] = 0.15;
        countryLosses[123] = 0.15;
        countryLosses[125] = 0.15;
        countryLosses[126] = 0.15;
        countryLosses[127] = 0.15;
        countryLosses[128] = 0.15;
        countryLosses[129] = 0.11;    //0.17	; Latvia
        countryLosses[131] = 0.15;
        countryLosses[133] = 0.15;
        countryLosses[134] = 0.15;
        countryLosses[136] = 0.15;
        countryLosses[138] = 0.15;
        countryLosses[139] = 0.15;
        countryLosses[143] = 0.15;
        countryLosses[145] = 0.15;
        countryLosses[145] = 0.15;
        countryLosses[148] = 0.15;
        countryLosses[150] = 0.15;
        countryLosses[151] = 0.15;
        countryLosses[153] = 0.15;
        countryLosses[157] = 0.15;
        countryLosses[158] = 0.15;
        countryLosses[160] = 0.14;    //0.15	; Netherlands
        countryLosses[161] = 0.13;    //0.06	;//Norway // according to MCPFE2015 data in 2010 (removals/fellings)
        countryLosses[162] = 0.15;
        countryLosses[164] = 0.15;
        countryLosses[165] = 0.15;
        countryLosses[166] = 0.15;
        countryLosses[167] = 0.15;
        countryLosses[169] = 0.15;
        countryLosses[170] = 0.15;
        countryLosses[172] = 0.15;
        countryLosses[173] = 0.12;    //0.17	; Poland
        countryLosses[176] = 0.02;
        countryLosses[177] = 0.15;
        countryLosses[182] = 0.02;    //0.12;	//0.15	; Romania
        countryLosses[183] = 0.15;
        countryLosses[184] = 0.15;
        countryLosses[184] = 0.15;
        countryLosses[185] = 0.15;
        countryLosses[186] = 0.15;
        countryLosses[187] = 0.15;
        countryLosses[188] = 0.15;
        countryLosses[192] = 0.15;
        countryLosses[193] = 0.15;
        countryLosses[194] = 0.15;
        countryLosses[194] = 0.15;
        countryLosses[196] = 0.15;
        countryLosses[198] = 0.15;
        countryLosses[199] = 0.15;
        countryLosses[200] = 0.15;
        countryLosses[201] = 0.03;    //0.14;	//0.24	; Slovakia
        countryLosses[202] = 0.15;//Slovenia
        countryLosses[203] = 0.07;//Sweden
        countryLosses[204] = 0.15;
        countryLosses[205] = 0.15;
        countryLosses[206] = 0.15;
        countryLosses[208] = 0.15;
        countryLosses[209] = 0.15;
        countryLosses[209] = 0.15;
        countryLosses[210] = 0.15;
        countryLosses[211] = 0.15;
        countryLosses[211] = 0.15;
        countryLosses[216] = 0.15;
        countryLosses[217] = 0.15;
        countryLosses[218] = 0.07;
        countryLosses[222] = 0.15;
        countryLosses[223] = 0.15;
        countryLosses[225] = 0.15;
        countryLosses[226] = 0.14;
        countryLosses[227] = 0.15;
        countryLosses[229] = 0.15;
        countryLosses[230] = 0.15;
        countryLosses[233] = 0.15;
        countryLosses[234] = 0.15;
        countryLosses[237] = 0.15;
        countryLosses[238] = 0.15;
        countryLosses[239] = 0.15;
        countryLosses[240] = 0.15;
        return countryLosses;
    }

    // weighted average share of collected wood out of available (0.1 close to nature, 0.3 low intensity and 1 multifunction) from Nabuurs et al. 2019
    // based on Figure S4 from Nabuurs et al. 2019 https://doi.org/10.1038/s41893-019-0374-3
    consteval array<double, numberOfCountries> initCleanedWoodUse() noexcept {
        array<double, numberOfCountries> cleanedWoodUse{};
        cleanedWoodUse.fill(0.7);
        cleanedWoodUse[6] = 0.73;    //	Albania
        cleanedWoodUse[17] = 0.66;    //	Austria
        cleanedWoodUse[20] = 0.63;    //	Belgium
        cleanedWoodUse[27] = 0.88;    //	bosnia and herzegovina
        cleanedWoodUse[24] = 0.70;    //	Bulgaria
        cleanedWoodUse[97] = 0.68;    //	Croatia
        cleanedWoodUse[57] = 0.50;    //	Czech
        cleanedWoodUse[61] = 0.80;    //	Denmark
        cleanedWoodUse[69] = 0.90;    //	Estonia
        cleanedWoodUse[71] = 0.96;    //	Finland
        cleanedWoodUse[74] = 0.70;    //	France
        cleanedWoodUse[58] = 0.56;    //	Germany
        cleanedWoodUse[87] = 0.25;    //	Greece
        cleanedWoodUse[99] = 0.74;    //	Hungary
        cleanedWoodUse[103] = 0.55;// 0.90;	//	Ireland // 03.01.2021 adjusting
        cleanedWoodUse[108] = 0.60;    //	Italy
        cleanedWoodUse[130] = 0.87;    //	Latvia
        cleanedWoodUse[128] = 0.80;    //	Lithuania
        cleanedWoodUse[129] = 0.28;    //	Luxembourg
        cleanedWoodUse[161] = 0.63;    //	Netherlands
        cleanedWoodUse[162] = 0.90;    //	Norway
        cleanedWoodUse[174] = 0.74;    //	Poland
        cleanedWoodUse[177] = 0.73;    //	Portugal
        cleanedWoodUse[183] = 0.78;    //	Romania
        cleanedWoodUse[143] = 0.82;    //	Montenegro
        cleanedWoodUse[199] = 0.82;    //	Serbia (and Montenegro)
        cleanedWoodUse[202] = 0.65;    //	Slovakia
        cleanedWoodUse[203] = 0.26;    //0.65;	//	Slovenia //15.03.2023 corrected to make harvest match the demand
        cleanedWoodUse[68] = 0.63;    //	Spain
        cleanedWoodUse[204] = 0.97;    //	Sweden
        cleanedWoodUse[41] = 0.71;    //	Switzerland
        cleanedWoodUse[78] = 0.63;    //	UK
        return cleanedWoodUse;
    }

    consteval array<double, numberOfCountries> initCountryLossesD() noexcept {
        array<double, numberOfCountries> countryLossesD{};
        countryLossesD.fill(9);
        countryLossesD[16] = 14;    //13;	//12;	//11;	//9;//Austria
        countryLossesD[19] = 4;    //7; //Belgium
        countryLossesD[23] = 8;    //7;	//6;	//5;	//4;	//2;	//4;	//7; //Bulgaria
        countryLossesD[96] = 6.2;    //6;	//5;	//4; //Croatia
        countryLossesD[56] = 13;    //12;	//11;	//10;	//9;	//7;	//9;	//16; //Czech
        countryLossesD[60] = 6; //Denmark
        countryLossesD[68] = 12; //Estonia
        countryLossesD[70] = 1.5;    //1.3;	//1.2;	//1.1;	//1; //Finland
        countryLossesD[73] = 4;    //3;	//4;	//9;	//13; //France
        countryLossesD[57] = 15;    //14;	//13;	//11; //Germany
        countryLossesD[86] = 2;    //3; //Greece
        countryLossesD[98] = 7;    //6;	//5;	//4;	//5;	//6; //Hungary
        countryLossesD[102] = 4;    //6; //Ireland
        countryLossesD[107] = 1;    //2;	//4; //Italy
        countryLossesD[129] = 9;    //7; //Latvia
        countryLossesD[127] = 12;    //11;	//10;	//9; //Lithuania
        countryLossesD[160] = 12;    //11;	//9;	//8;	//7;	//5;	//7; //Netherlands
        countryLossesD[161] = 1; //Norway
        countryLossesD[173] = 13;    //12;	//11;	//10;	//9;	//11;	//12; //Poland
        countryLossesD[176] = 3;    //2; //Portugal
        countryLossesD[182] = 1.3;    //1.5;	//2; //Romania
        countryLossesD[201] = 1.5;    //2;	//3;	//5;	//7;	//13; //Slovakia
        countryLossesD[202] = 2.3;    //2; //Slovenia
        countryLossesD[67] = 0.5;    //0.8;	//1;	//1.5; //2//Spain
        countryLossesD[203] = 1;    //1.5;	//2; //Sweden
        countryLossesD[77] = 5;    //7; //UK
        return countryLossesD;
    }


    // Forest management CO2 sink (only CO2, only biomass) reported by the countries, GgCO2
    // Net stock changes biomass FL-FL (from CRF table 5.A), average for 1990-2000, GgCO2/year ?
    // New country code!!! shifted by -1
    consteval array<double, numberOfCountries> initFM_sink_stat() noexcept {
        array<double, numberOfCountries> FM_sink_stat{};
        FM_sink_stat[15] = 39242;
        FM_sink_stat[16] = 15001;
        FM_sink_stat[19] = 33870;
        FM_sink_stat[23] = 2175;
        FM_sink_stat[27] = 13607;
        FM_sink_stat[38] = 48346;
        FM_sink_stat[96] = 8707;
        FM_sink_stat[56] = 8731;
        FM_sink_stat[60] = 939;
        FM_sink_stat[68] = 6424;
        FM_sink_stat[70] = 25384;
        FM_sink_stat[73] = 52784;
        FM_sink_stat[57] = 62429;
        FM_sink_stat[86] = 1788;
        FM_sink_stat[98] = 4032;
        FM_sink_stat[105] = 3;
        FM_sink_stat[102] = 604;
        FM_sink_stat[107] = 24658;
        FM_sink_stat[110] = 80579;
        FM_sink_stat[129] = 22899;
        FM_sink_stat[124] = 19;
        FM_sink_stat[127] = 13654;
        FM_sink_stat[128] = 296;
        FM_sink_stat[132] = 0;
        FM_sink_stat[160] = 2420;
        FM_sink_stat[164] = 23387;
        FM_sink_stat[161] = 9402;
        FM_sink_stat[173] = 24987;
        FM_sink_stat[176] = 3630;
        FM_sink_stat[182] = 38417;
        FM_sink_stat[183] = 235445;
        FM_sink_stat[201] = 2000;
        FM_sink_stat[202] = 9522;
        FM_sink_stat[67] = 39167;
        FM_sink_stat[203] = 37819;
        FM_sink_stat[40] = 2420;
        FM_sink_stat[218] = 44338;
        FM_sink_stat[223] = 58968;
        FM_sink_stat[77] = 7652;
        FM_sink_stat[226] = 434294;
        return FM_sink_stat;
    }


    // deforestation soil emissions, tC/ha year, derived from countries; UNFCCC CRF tables 2023 submission; 2005-2020 average;
    consteval array<double, numberOfCountries> initDefor_sl_em_unfccc_tCha() noexcept {
        array<double, numberOfCountries> defor_sl_em_unfccc_tCha{};
        defor_sl_em_unfccc_tCha[17] = 0.808519236;
        defor_sl_em_unfccc_tCha[20] = 1.635669517;
        defor_sl_em_unfccc_tCha[24] = 2.352859349;
        defor_sl_em_unfccc_tCha[56] = 0.347765091;
        defor_sl_em_unfccc_tCha[57] = 0.218951586;
        defor_sl_em_unfccc_tCha[58] = 1.188810861;
        defor_sl_em_unfccc_tCha[61] = 0.354670609;
        defor_sl_em_unfccc_tCha[68] = 0.688946825;
        defor_sl_em_unfccc_tCha[69] = 1.292809639;
        defor_sl_em_unfccc_tCha[71] = 1.23883855;
        defor_sl_em_unfccc_tCha[74] = 0.704664056;
        defor_sl_em_unfccc_tCha[78] = 2.494532687;
        defor_sl_em_unfccc_tCha[87] = 2.664474352;
        defor_sl_em_unfccc_tCha[97] = 1.409282453;
        defor_sl_em_unfccc_tCha[99] = 0.416593886;
        defor_sl_em_unfccc_tCha[103] = 0.960396164;
        defor_sl_em_unfccc_tCha[108] = 7.609471107;
        defor_sl_em_unfccc_tCha[128] = 6.032312528;
        defor_sl_em_unfccc_tCha[129] = 1.350930143;
        defor_sl_em_unfccc_tCha[130] = 1.332441087;
        defor_sl_em_unfccc_tCha[141] = 0;
        defor_sl_em_unfccc_tCha[161] = 0.150666602;
        defor_sl_em_unfccc_tCha[162] = 1.025590905;
        defor_sl_em_unfccc_tCha[174] = 2.324730298;
        defor_sl_em_unfccc_tCha[177] = 0.849843121;
        defor_sl_em_unfccc_tCha[183] = 1.933699428;
        defor_sl_em_unfccc_tCha[202] = 1.295828307;
        defor_sl_em_unfccc_tCha[203] = 1.424908031;
        defor_sl_em_unfccc_tCha[204] = 0.649515664;
        return defor_sl_em_unfccc_tCha;
    }

    // deforestation soil emissions, MtCO2/year, derived from countries; UNFCCC CRF tables 2023 submission; 2005-2020 average;
    consteval array<double, numberOfCountries> initDefor_sl_em_unfccc_MtCO2() noexcept {
        array<double, numberOfCountries> defor_sl_em_unfccc_MtCO2{};
        defor_sl_em_unfccc_MtCO2[17] = 0.199383443;
        defor_sl_em_unfccc_MtCO2[20] = 0.114487618;
        defor_sl_em_unfccc_MtCO2[24] = 0.029531443;
        defor_sl_em_unfccc_MtCO2[56] = 0.000145119;
        defor_sl_em_unfccc_MtCO2[57] = 0.010628726;
        defor_sl_em_unfccc_MtCO2[58] = 0.32331076;
        defor_sl_em_unfccc_MtCO2[61] = 0.010422006;
        defor_sl_em_unfccc_MtCO2[68] = 0.62505598;
        defor_sl_em_unfccc_MtCO2[69] = 0.090563174;
        defor_sl_em_unfccc_MtCO2[71] = 1.413505958;
        defor_sl_em_unfccc_MtCO2[74] = 2.095581545;
        defor_sl_em_unfccc_MtCO2[78] = 0.506861958;
        defor_sl_em_unfccc_MtCO2[87] = 0.039075189;
        defor_sl_em_unfccc_MtCO2[97] = 0.025230643;
        defor_sl_em_unfccc_MtCO2[99] = 0.034561426;
        defor_sl_em_unfccc_MtCO2[103] = 0.053604293;
        defor_sl_em_unfccc_MtCO2[108] = 1.107948738;
        defor_sl_em_unfccc_MtCO2[128] = 0.085561505;
        defor_sl_em_unfccc_MtCO2[129] = 0.005072563;
        defor_sl_em_unfccc_MtCO2[130] = 0.246104892;
        defor_sl_em_unfccc_MtCO2[141] = 1.03743E-05;
        defor_sl_em_unfccc_MtCO2[161] = 0.02109033;
        defor_sl_em_unfccc_MtCO2[162] = 0.440967418;
        defor_sl_em_unfccc_MtCO2[174] = 0.133933093;
        defor_sl_em_unfccc_MtCO2[177] = 0.67867849;
        defor_sl_em_unfccc_MtCO2[183] = 0.375520803;
        defor_sl_em_unfccc_MtCO2[202] = 0.034571268;
        defor_sl_em_unfccc_MtCO2[203] = 0.09479031;
        defor_sl_em_unfccc_MtCO2[204] = 0.642802537;
        return defor_sl_em_unfccc_MtCO2;
    }

    // deforestation biomass emissions, MtCO2/year, derived from countries; UNFCCC CRF tables 2023 submission; 2005-2020 average;
    consteval array<double, numberOfCountries> initDefor_bm_em_unfccc_MtCO2() noexcept {
        array<double, numberOfCountries> defor_bm_em_unfccc_MtCO2{};
        defor_bm_em_unfccc_MtCO2[17] = 0.71889397;
        defor_bm_em_unfccc_MtCO2[20] = 0.450227937;
        defor_bm_em_unfccc_MtCO2[24] = 0.076895551;
        defor_bm_em_unfccc_MtCO2[56] = 0.000987683;
        defor_bm_em_unfccc_MtCO2[57] = 0.242955873;
        defor_bm_em_unfccc_MtCO2[58] = 0.167516716;
        defor_bm_em_unfccc_MtCO2[61] = 0.063765338;
        defor_bm_em_unfccc_MtCO2[68] = 0.885982015;
        defor_bm_em_unfccc_MtCO2[69] = 0.386232677;
        defor_bm_em_unfccc_MtCO2[71] = 1.964094344;
        defor_bm_em_unfccc_MtCO2[74] = 10.93596794;
        defor_bm_em_unfccc_MtCO2[78] = 0.604123468;
        defor_bm_em_unfccc_MtCO2[87] = 0.008434182;
        defor_bm_em_unfccc_MtCO2[97] = 0.015159056;
        defor_bm_em_unfccc_MtCO2[99] = 0.099465268;
        defor_bm_em_unfccc_MtCO2[103] = 0.238519505;
        defor_bm_em_unfccc_MtCO2[108] = 0.727254527;
        defor_bm_em_unfccc_MtCO2[128] = 0;
        defor_bm_em_unfccc_MtCO2[129] = 0.014310222;
        defor_bm_em_unfccc_MtCO2[130] = 0.173853898;
        defor_bm_em_unfccc_MtCO2[141] = 9.96777E-06;
        defor_bm_em_unfccc_MtCO2[161] = 0.602289468;
        defor_bm_em_unfccc_MtCO2[162] = 0.793093524;
        defor_bm_em_unfccc_MtCO2[174] = 0.387656331;
        defor_bm_em_unfccc_MtCO2[177] = 0.477626684;
        defor_bm_em_unfccc_MtCO2[183] = 0.782714118;
        defor_bm_em_unfccc_MtCO2[202] = 0.090430433;
        defor_bm_em_unfccc_MtCO2[203] = 0.226644967;
        defor_bm_em_unfccc_MtCO2[204] = 1.743829086;
        return defor_bm_em_unfccc_MtCO2;
    }

    // deforestation DOM emissions, MtCO2/year, derived from countries; UNFCCC CRF tables 2023 submission; 2005-2020 average;
    consteval array<double, numberOfCountries> initDefor_dom_em_unfccc_MtCO2() noexcept {
        array<double, numberOfCountries> defor_dom_em_unfccc_MtCO2{};
        defor_dom_em_unfccc_MtCO2[17] = 0.326656659;
        defor_dom_em_unfccc_MtCO2[20] = 0.036260351;
        defor_dom_em_unfccc_MtCO2[24] = 0.017980171;
        defor_dom_em_unfccc_MtCO2[56] = 0.00327102;
        defor_dom_em_unfccc_MtCO2[57] = 0.016958656;
        defor_dom_em_unfccc_MtCO2[58] = 0.350897794;
        defor_dom_em_unfccc_MtCO2[61] = 0.031797636;
        defor_dom_em_unfccc_MtCO2[68] = 0.104759919;
        defor_dom_em_unfccc_MtCO2[69] = 0.064795254;
        defor_dom_em_unfccc_MtCO2[71] = 0.028147854;
        defor_dom_em_unfccc_MtCO2[74] = 1.837897427;
        defor_dom_em_unfccc_MtCO2[78] = 0.161918975;
        defor_dom_em_unfccc_MtCO2[87] = 0.003392675;
        defor_dom_em_unfccc_MtCO2[97] = 0.007260402;
        defor_dom_em_unfccc_MtCO2[99] = 0.047398907;
        defor_dom_em_unfccc_MtCO2[103] = 0.052934671;
        defor_dom_em_unfccc_MtCO2[108] = 0.056507643;
        defor_dom_em_unfccc_MtCO2[128] = 0.010660583;
        defor_dom_em_unfccc_MtCO2[129] = 0.003255544;
        defor_dom_em_unfccc_MtCO2[130] = 0.268158973;
        defor_dom_em_unfccc_MtCO2[141] = -4.4855E-05;
        defor_dom_em_unfccc_MtCO2[161] = 0.238202906;
        defor_dom_em_unfccc_MtCO2[162] = 1.607352551;
        defor_dom_em_unfccc_MtCO2[174] = 0.006717771;
        defor_dom_em_unfccc_MtCO2[177] = 0.07904;
        defor_dom_em_unfccc_MtCO2[183] = 0.048398901;
        defor_dom_em_unfccc_MtCO2[202] = 0.012487773;
        defor_dom_em_unfccc_MtCO2[203] = 0.041353003;
        defor_dom_em_unfccc_MtCO2[204] = 0.471894093;
        return defor_dom_em_unfccc_MtCO2;
    }

    consteval array<double, numberOfCountries> initHarvestedWoodUse() noexcept {
        array<double, numberOfCountries> harvestedWoodUse{};
        harvestedWoodUse.fill(1);
        return harvestedWoodUse;
    }

    consteval array<array<double, 21>, 29> initWoodProdEUStat() noexcept {
        array<array<double, 21>, 29> woodProdEUStats = {
                {{19000.000, 13900.000, 14800.000, 14800.000, 17400.000, 16700.000, 18100.000, 17800.000, 17000.000,
                  17000.000, 16000.000, 16300.000, 17900.000, 20600.000, 19900.000, 19900.000, 23100.000, 25800.000,
                  25800.000, 25800.000, 25800.000},
                 {3227.000, 3227.000, 3227.000, 3227.000, 3227.000, 3227.000, 3227.000, 3227.000, 3227.000,
                  3731.000,
                  3731.000, 3731.000, 3731.000},
                 {4671.561, 4170.347, 4054.101, 4054.152, 3063.606, 3243.239, 3657.743, 3471.820, 3690.779,
                  6493.736,
                  6919.243, 5252.628, 6464.316},
                 {2278.432, 2278.432, 2278.432, 2809.144, 3227.121, 2983.701, 2912.695, 3494.873, 3893.556,
                  3993.904,
                  4203.138, 3971.277, 4171.523, 4408.098, 4400.090, 4602.299, 5097.843, 4822.110, 5119.863,
                  4859.822,
                  5130.143},
                 {70.634, 60.509, 50.734, 59.709, 52.322, 53.982, 50.736, 46.117, 39.777, 40.991, 23.154, 20.609,
                  17.381, 13.497, 11.332, 10.900, 8.392, 22.131, 22.320, 11.152, 10.084},
                 {14569.000, 11749.000, 10766.000, 11374.000, 13075.000, 13744.000, 13766.000, 14778.000, 15343.000,
                  15590.000, 15854.000, 15779.000, 15984.000, 16663.000, 17154.000, 17074.000, 19465.000, 20381.000,
                  20381.000, 20381.000, 20381.000},
                 {2553.261, 2614.917, 2523.001, 2583.249, 2583.779, 2583.779, 2583.779, 2408.769, 1764.820,
                  1766.912,
                  3330.389, 1824.112, 1634.831, 1838.633, 1713.754, 3339.859, 2661.475, 2894.563, 3147.436,
                  3178.471,
                  3016.248},
                 {2436.394, 2436.394, 2436.394, 2768.165, 4034.276, 4216.479, 4433.669, 6117.874, 6867.894,
                  7598.993,
                  10089.759, 11550.562, 11890.482, 11890.740, 7705.024, 6231.435, 6122.950, 5101.744, 5510.539,
                  6122.859, 8165.207},
                 {48870.000, 39368.000, 45145.000, 47700.000, 55026.000, 56713.000, 52593.000, 58838.000, 60969.000,
                  60938.000, 61500.000, 59363.000, 60271.000, 61142.000, 61163.000, 58684.000, 56935.000, 63854.000,
                  59137.000, 59137.000, 59137.000},
                 {68128.000, 68822.000, 68688.000, 64551.000, 65625.000, 65773.000, 62623.000, 62903.000, 62314.000,
                  61729.000, 70522.000, 64320.000, 59300.000, 56980.000, 57269.000, 56832.000, 57622.000, 58786.000,
                  58786.000, 58786.000, 58786.000},
                 {45646.000, 45646.000, 49480.000, 51314.000, 53530.000, 54535.000, 54988.000, 57971.000, 58608.000,
                  58426.000, 74989.000, 58816.000, 60711.000, 65762.000, 73925.000, 82947.000, 87117.000, 96123.000,
                  96123.000, 96123.000, 96123.000},
                 {1171.255, 1171.255, 1171.255, 1171.255, 1171.255, 1171.255, 1171.255, 1171.255, 1171.255,
                  1477.058,
                  1297.613, 1264.113, 1155.180, 1293.215, 1270.735, 1352.242, 1307.311, 1319.932, 1313.938,
                  1313.938,
                  1313.938},
                 {6957.200, 6957.200, 6957.200, 6957.200, 6957.200, 6957.200, 6957.200, 6957.200, 6957.200,
                  6957.200,
                  6957.200, 6957.200, 6957.200, 6957.200, 6957.200, 6992.400, 6992.400, 6992.400, 6992.400,
                  6992.400,
                  6992.400},
                 {1787.000, 1837.000, 2156.000, 2003.000, 2220.000, 2424.000, 2520.000, 2398.000, 2493.000,
                  2842.000,
                  2940.000, 2700.000, 2911.000, 2951.000, 2818.000, 2913.000, 2938.000, 2981.000, 2981.000,
                  2981.000,
                  2981.000},
                 {12720.000, 12720.000, 12720.000, 12720.000, 12720.000, 12720.000, 12720.000, 12720.000, 12720.000,
                  12720.000, 12720.000, 12321.572, 12321.572, 12321.572, 12321.572, 12321.572, 12321.572, 12321.572,
                  12321.572, 12321.572, 12321.572},
                 {5000.000, 4400.000, 4000.000, 4800.000, 5700.000, 6900.000, 6800.000, 8900.000, 10000.000,
                  10800.000,
                  11000.000, 11200.000, 12200.000, 11700.000, 11500.000, 11300.000, 9810.000, 10010.000, 10010.000,
                  10010.000, 10010.000},
                 {3571.329, 3571.329, 3571.329, 5088.700, 4511.611, 6731.758, 6255.069, 5837.285, 5549.180,
                  5601.395,
                  6247.449, 6476.043, 6941.689, 7120.431, 6945.112, 6858.647, 6670.154, 7030.802, 6360.345,
                  6204.690,
                  8059.133},
                 {295.370, 295.370, 295.370, 295.370, 295.370, 295.370, 295.370, 295.370, 295.370, 295.370, 295.370,
                  306.695, 292.429, 292.444, 315.475, 282.859, 304.278, 331.706, 402.966, 311.681, 313.001},
                 {1606.871, 1272.259, 1419.308, 1219.359, 1183.327, 1251.066, 1079.194, 1256.787, 1159.150,
                  1183.185,
                  1177.593, 980.202, 950.885, 1184.657, 1163.507, 1258.892, 1255.259, 1160.166, 1268.107, 1154.053,
                  1209.814},
                 {19893.646, 19242.283, 21222.188, 21003.151, 21220.199, 22993.956, 22930.311, 24563.877, 26124.358,
                  27438.693, 29409.053, 28287.873, 30679.801, 34878.342, 37017.112, 36122.998, 36618.481, 40599.738,
                  38736.662, 39145.166, 40081.907},
                 {14688.538, 14116.413, 13437.575, 13351.737, 12869.899, 12310.060, 11840.296, 11853.532, 11311.767,
                  11840.003, 11457.239, 11784.474, 11482.710, 12605.649, 14120.222, 13911.796, 14018.194, 14030.592,
                  14030.592, 14030.592, 14030.592},
                 {14374.842, 14804.972, 14211.133, 10103.810, 13605.076, 13885.212, 13962.289, 15411.895, 13266.473,
                  14468.799, 14985.830, 14163.632, 17248.121, 17577.476, 18020.212, 16523.815, 15936.743, 17461.126,
                  15566.297, 14305.622, 14938.092},
                 {5956.102, 5956.102, 5956.102, 5956.102, 6033.992, 6034.780, 6187.240, 5588.674, 6260.687,
                  6579.762,
                  7000.969, 6581.585, 6571.484, 7217.422, 8217.590, 10531.164, 8920.657, 9216.737, 10492.734,
                  10282.718,
                  10868.548},
                 {1901.266, 1901.266, 1901.266, 1205.825, 2203.435, 2117.887, 2255.495, 2507.659, 2421.064,
                  2347.385,
                  2558.427, 2562.198, 2591.282, 2938.151, 2891.993, 3096.940, 3605.473, 3266.042, 3391.734,
                  3327.197,
                  3345.390},
                 {17654.888, 17222.446, 15808.214, 15635.761, 17372.168, 18253.481, 17756.051, 17756.051, 16893.788,
                  16821.516, 16251.337, 17171.084, 17975.836, 18279.672, 18489.862, 17626.643, 17857.084, 16521.374,
                  19368.390, 15915.872, 17829.388},
                 {63900.000, 61700.000, 63800.000, 64600.000, 66700.000, 75900.000, 68100.000, 72000.000, 73200.000,
                  70100.000, 75800.000, 75800.000, 79500.000, 80400.000, 84300.000, 118000.000, 77600.000,
                  94600.000,
                  94600.000, 94600.000, 94600.000},
                 {7120.000, 7470.000, 7820.000, 8170.000, 8520.000, 8008.000, 7928.000, 8292.500, 8224.600,
                  8683.000,
                  8827.000, 8886.400, 9015.200, 9712.900, 9889.400, 9926.500, 9800.100, 10357.600, 10357.600,
                  10357.600,
                  10357.600},
                 {13431.000, 12817.000, 11516.000, 11034.000, 9936.000, 10278.000, 9572.000, 9484.000, 9286.000,
                  9573.000, 9268.000, 10223.000, 9831.000, 9430.000, 9980.000, 10985.000, 11129.000, 11892.000,
                  11731.000, 10095.000, 11867.000},
                 {7145.783, 5204.456, 5146.468, 4982.010, 5287.524, 5367.803, 4597.520, 5076.637, 4841.483,
                  5355.495, 10421.800, 6392.458, 5147.527, 5780.524, 5794.343, 5970.327, 6443.826, 6238.490,
                  5595.862,
                  5319.554, 5587.340}
                }
        };
        return woodProdEUStats;
    }

    // 24 July 2023
    // Historic data on Annual Net biomass emissions, in Mt CO2 for 1990-2021
    //Query results for Category: 4.A.1 - Forest Land remaining Forest Land -
    //Norway is added at the end
    // Switzerland is added at the end;
    // (2023 submission data)
    consteval array<array<double, 32>, 30> init_fmEmission_unfccc_CRF() noexcept {
        array<array<double, 32>, 30> fmEmission_unfccc_CRF = {
                {{-10.071165, -15.276020, -11.682347, -11.535506, -10.481797, -12.915401, -8.270782, -17.375838,
                  -16.495205, -18.494454, -15.986411, -17.967113, -11.694230, -3.017019, -7.529196, -8.348273,
                  -2.406566, -1.973858, -1.655185, -8.096555, -4.836262, -5.363129, -7.320900, -2.229310, -8.711953,
                  -1.965234, -7.928518, -3.255518, 1.452741, 1.892524, -5.570995, -2.999953},    //	AUT,Austria
                 {-1.903339, -1.900639, -1.897939, -1.895262, -1.892586, -1.889909, -1.887232, -1.884555, -1.881878,
                  -1.879223, -1.876935, -1.875969, -1.751894, -1.749705, -1.747551, -1.745431, -1.743345, -1.741294,
                  -1.739277, -1.737331, -1.734283, -1.731270, -1.727740, -1.550943, -1.511298, -1.404804, -1.634932,
                  -1.655564, -1.682414, -1.657537, -1.696622, -1.698789},    //	BEL,Belgium
                 {-13.267733, -13.441246, -13.637336, -13.857944, -14.077947, -14.297776, -13.593221, -13.841650,
                  -14.089957, -14.299757, -14.445684, -11.517904, -11.589333, -11.660469, -11.712119, -11.775503,
                  -9.762270, -9.787721, -9.783105, -9.780531, -9.734881, -6.050968, -6.019505, -5.987273, -5.955649,
                  -5.839322, -7.162464, -7.139123, -7.116685, -7.093228, -7.070613, -7.081711},    //	BGR,Bulgaria
                 {-6.444758, -8.133306, -8.458527, -8.716736, -8.524819, -8.933053, -8.733071, -8.257676, -8.301344,
                  -8.478754, -8.271644, -8.344891, -8.504616, -8.286406, -8.149695, -8.293846, -8.195201, -7.689952,
                  -7.547977, -7.899570, -7.693810, -6.684449, -6.492894, -6.513346, -6.206705, -5.811124, -5.566394,
                  -5.371838, -5.258764, -5.503818, -5.960956, -5.623552},    //	HRV,Croatia
                 {0.001552, -0.018155, -0.023212, -0.011683, -0.023433, -0.018360, -0.021967, -0.029733, -0.029877,
                  -0.031823, 0.004613, -0.046483, -0.078966, -0.091493, -0.092609, -0.098536, -0.099641, -0.041602,
                  -0.102805, -0.135543, -0.125019, -0.162491, -0.156377, -0.164939, -0.165879, -0.162087, -0.068358,
                  -0.172322, -0.167486, -0.161300, -0.161887, -0.103979},    //	CYP,Cyprus
                 {-5.109317, -9.131779, -10.655323, -10.366830, -7.984999, -7.654104, -7.720979, -6.726943,
                  -6.995087,
                  -6.619795, -6.643831, -6.710112, -6.597991, -5.682866, -5.325840, -5.649455, -2.628364, -1.377623,
                  -4.763193, -6.058569, -4.409401, -6.012700, -6.269721, -6.046850, -5.973728, -5.054344, -3.486956,
                  -1.191699, 6.760669, 15.711936, 20.382626, 14.291112},    //	CZE,Czech
                 {-0.244371, -0.245201, -0.246020, -0.246838, -0.247657, -1.032443, -1.032285, -1.032127, -1.031968,
                  -1.031812, -1.101948, -0.927421, -0.918212, -0.910305, -0.903696, -0.705174, -0.707317, -0.709980,
                  -1.257901, -1.085619, -1.059804, -1.820401, -2.033096, -1.677545, -1.990714, -1.950942, -1.036135,
                  -0.530822, -0.193619, -0.300136, -0.313192, -1.284376},    //	DNK,Denmark
                 {-4.025421, -4.025349, -4.025138, -4.024787, -4.024281, -4.023564, -4.022706, -4.021851, -4.050812,
                  -4.019851, -4.018907, -4.017894, -4.016738, -5.723350, -3.833865, -1.975846, -3.230603, -3.227785,
                  -4.508927, -4.236347, -4.921851, -3.999290, -2.957674, -2.782723, -0.359618, -1.320896, -1.142852,
                  -0.512259, 1.177389, 0.000000, 1.394528, 1.822198},    //	EST,Estonia
                 {-26.404151, -41.320994, -34.442764, -31.950211, -22.970255, -21.560818, -29.782823, -22.911768,
                  -21.165837, -22.309984, -22.404162, -27.052791, -27.978647, -28.980952, -30.772845, -35.105033,
                  -38.866315, -30.107711, -37.813622, -54.763071, -39.683596, -39.140138, -40.611670, -32.777536,
                  -32.489802, -27.942578, -24.137659, -20.594530, -11.971436, -19.145273, -25.105399,
                  -14.413678},    //	FIN,Finland
                 {-24.587782, -24.555310, -24.728686, -29.623483, -30.172820, -32.281011, -38.972404, -38.719602,
                  -39.713310, 23.260477, -30.546213, -43.220492, -52.760635, -57.150086, -57.387492, -57.962619,
                  -58.925759, -61.002111, -62.251772, -36.186256, -54.836179, -54.517354, -56.011713, -64.239657,
                  -52.743530, -49.081704, -39.626634, -28.972737, -23.798370, -21.575415, -23.923800,
                  -19.591483},    //	FRA,France
                 {-5.874597, -67.348708, -72.781428, -72.487809, -63.748515, -57.117070, -60.534916, -58.758406,
                  -57.657708, -59.700773, -37.512149, -57.289994, -19.206213, -20.463517, -19.460172, -18.711568,
                  -17.123993, -12.797189, -43.447968, -49.618581, -44.261880, -42.839328, -42.907369, -46.694575,
                  -45.782533, -44.781226, -47.635160, -46.608372, -39.561909, -36.217852, -29.666293,
                  -27.984510},    //	DEU,Germany
                 {-1.288878, -1.332524, -1.724892, -1.783153, -1.486883, -1.660461, -1.644141, -1.372177, -1.576387,
                  -1.521072, -1.521700, -1.775117, -1.778016, -1.808326, -1.809659, -2.192902, -2.176224, -2.057940,
                  -2.057940, -2.057940, -2.057940, -2.057940, -2.057940, -2.057940, -2.068242, -2.078349, -2.092958,
                  -2.092251, -2.091544, -2.194140, -2.199781, -2.346133},    //	GRC,Greece
                 {-2.918290, -2.223563, -2.763844, -5.245582, -5.627934, -5.384102, -1.420430, -1.727937, -2.664069,
                  -0.361866, 0.620904, -0.914385, -0.089182, -2.352309, -1.269530, -3.543962, -1.249849, -1.417294,
                  -2.895528, -1.921987, -1.892284, -1.749678, -2.529254, -1.939157, -2.929148, -3.758345, -2.940688,
                  -3.343272, -2.815441, -3.510398, -5.331872, -5.389781},    //	HUN,Hungary
                 {-3.145625, -4.076588, -3.199317, -3.599167, -3.222173, -3.039425, -2.791345, -3.385373, -3.614929,
                  -3.099613, -2.318971, -2.437040, -2.022952, -2.582522, -3.313046, -2.859520, -3.605897, -3.366472,
                  -4.235996, -4.155106, -3.801143, -4.045644, -4.248889, -4.399261, -4.200764, -4.119907, -4.446035,
                  -2.351494, -2.745907, -2.192151, -0.396659, 0.386952},    //	IRL,Ireland
                 {-13.919433, -24.456891, -23.167404, -13.997437, -22.933323, -25.432398, -24.857823, -18.307750,
                  -16.927874, -21.390864, -20.407145, -24.964043, -27.656574, -22.501820, -25.870119, -26.426555,
                  -26.713837, -14.209166, -24.423595, -26.136299, -28.715290, -26.149496, -22.571223, -31.020593,
                  -31.956129, -32.659176, -31.950515, -19.132760, -33.383187, -29.300512, -24.689017,
                  -23.329960},    //	ITA,Italy
                 {-17.804089, -18.835377, -19.476073, -18.216061, -19.796447, -17.835505, -18.036004, -14.359532,
                  -12.470764, -7.252509, -11.749697, -12.857865, -11.493295, -10.774388, -7.608033, -6.807492,
                  -9.216212, -8.635965, -10.500229, -3.849234, -0.399191, -1.719768, -3.223313, -2.781773, 1.837488,
                  -1.364737, -2.243458, -4.073757, -3.627124, -4.976927, -2.797685, -1.645385},    //	LVA,Latvia
                 {-6.892193, -6.892193, -6.458764, -6.475425, -5.860738, -3.095514, 1.560080, -0.470771, -8.111950,
                  -7.794926, -9.077827, -6.177164, -4.824142, -3.525108, -3.168532, -2.644235, -2.356515, -3.611223,
                  -5.433813, -6.902049, -8.455810, -9.373181, -9.066831, -8.919420, -8.136242, -7.251922, -6.357637,
                  -6.143284, -5.759131, -5.549922, -5.189199, -4.676437},    //	LTU,Lithuania
                 {0.020577, -0.047183, -0.453028, -0.577058, -0.427486, -0.496477, -0.529176, -0.611279, -0.499416,
                  -0.575828, -0.584522, -0.562501, -0.587578, -0.572023, -0.601154, -0.562624, -0.489216, -0.402766,
                  -0.395694, -0.404585, -0.167804, -0.252818, -0.326590, -0.510245, -0.423069, -0.367324, -0.442718,
                  -0.380092, -0.244952, -0.367792, -0.442749, -0.597490},    //	LUX,Luxembourg
                 {-0.000013, -0.000013, -0.000012, -0.000012, -0.000011, -0.000011, -0.000010, -0.000010, -0.000009,
                  -0.000009, -0.000008, -0.000008, -0.000008, -0.000008, -0.000008, -0.000007, -0.000007, -0.000007,
                  -0.000007, -0.000007, -0.000007, -0.000007, -0.000006, -0.000006, -0.000006, -0.000006, -0.000006,
                  -0.000006, -0.000006, -0.000006, -0.000005, -0.000005},    //	MLT,Malta
                 {-1.521422, -1.551831, -1.582807, -1.614293, -1.646240, -1.678595, -1.711307, -1.744323, -1.777584,
                  -1.811046, -1.817923, -1.824318, -1.830232, -1.493754, -1.504459, -1.515003, -1.525382, -1.535599,
                  -1.545648, -1.554593, -1.558244, -1.560409, -1.561039, -1.501440, -1.498722, -1.494529, -1.488803,
                  -1.488596, -1.487418, -1.485241, -1.490534, -1.382587},    //	NLD,Netherlands
                 {-31.810617, -24.321242, -3.232951, -10.179872, -9.407495, -20.086619, -37.669137, -36.813819,
                  -41.686408, -37.674459, -33.906598, -25.436803, -34.215668, -35.793701, -46.108614, -45.235995,
                  -38.380479, -31.505559, -31.324180, -29.593111, -27.867456, -34.681870, -34.268819, -36.202467,
                  -28.908519, -24.929439, -31.194104, -31.240771, -31.580698, -12.197905, -13.072894,
                  -13.947882},    //	POL,Poland
                 {-0.750572, -2.389204, -3.938938, -4.504758, -5.090901, -6.811477, -6.216396, -6.105775, -7.117467,
                  -6.280449, -2.699177, -6.712889, -7.584051, -5.344995, -2.816896, -2.664544, 0.558674, 0.183805,
                  -3.105316, -5.686754, -4.872524, -0.316186, -0.434608, -0.311553, -0.941678, -0.401646, 0.856801,
                  2.447240, 0.679618, 0.117241, -0.524735, -3.628388},    //	PRT,Portugal
                 {-23.432858, -24.770705, -25.778560, -26.952410, -27.682042, -27.041013, -26.237149, -26.734600,
                  -28.851426, -27.817391, -27.400493, -28.388894, -25.816545, -25.540258, -24.912225, -26.597161,
                  -26.560522, -25.463132, -25.885433, -26.122230, -25.715944, -24.035666, -23.354650, -23.194020,
                  -24.548383, -24.207823, -25.120656, -23.900090, -22.681598, -23.238908, -22.764959,
                  -22.291472},    //	ROU,Romania
                 {-5.514414, -6.916047, -7.735164, -7.731187, -6.825655, -6.419885, -6.398320, -5.960697, -6.797170,
                  -6.561223, -6.260685, -6.272969, -6.347976, -5.840679, -5.022598, -0.691700, -3.783884, -3.521044,
                  -2.340020, -2.933459, -1.955368, -2.558164, -4.653538, -5.166051, -2.694333, -3.058971, -2.784880,
                  -2.727100, -2.010664, -3.028682, -5.704576, -5.448073},    //	SVK,Slovakia
                 {-4.239606, -4.244282, -4.373144, -4.377957, -4.382769, -4.387581, -4.392951, -4.601828, -4.598666,
                  -4.595483, -4.592277, -4.589051, -5.600231, -5.618051, -5.635887, -5.653742, -5.671613, -5.692771,
                  -5.917581, -5.924518, -5.931454, -6.023484, -6.115514, -3.708992, 2.326817, 2.361497, 2.395920,
                  2.428763, 2.461607, -2.651006, -2.687532, -2.724057},    //	SVN,Slovenia
                 {-20.186710, -20.359739, -20.532767, -20.705796, -20.878825, -21.051854, -21.224882, -21.397911,
                  -22.534940, -23.975065, -25.851324, -26.603403, -25.370625, -25.069789, -25.546622, -25.442985,
                  -25.949007, -26.458445, -26.679692, -26.900940, -27.175874, -27.450808, -27.725742, -27.998372,
                  -28.271003, -28.543633, -28.818108, -29.092583, -29.367058, -29.641533, -29.916008,
                  -30.111724},    //	ESP,Spain
                 {-38.093937, -38.637790, -34.963621, -32.051877, -32.084339, -32.754501, -34.693237, -33.568489,
                  -33.948038, -32.933854, -33.461517, -30.829313, -31.473161, -29.096443, -25.188490, -21.876174,
                  -27.553147, -27.254394, -31.242002, -34.331230, -36.620127, -33.566859, -34.086410, -36.119622,
                  -34.397937, -32.849157, -29.870224, -23.411291, -23.470973, -23.557965, -23.812819,
                  -23.877243},    //	SWE,Sweden
                 {-9.584020, -10.174387, -10.530774, -10.804880, -10.489273, -10.585157, -10.812413, -10.622275,
                  -10.857906, -10.556181, -10.584733, -11.070062, -11.154780, -11.017813, -11.334886, -11.538487,
                  -11.696544, -11.545485, -12.381689, -12.273679, -11.941662, -11.396374, -9.865045, -10.248002,
                  -10.143474, -9.701741, -10.154225, -10.259696, -10.304672, -10.089034, -9.994941,
                  -9.704300},    //	GBR,United
                 {-11.217854, -13.048909, -12.810605, -14.403383, -12.151519, -15.811645, -15.713625, -15.415398,
                  -18.249672, -20.499990, -23.003978, -24.373900, -25.059644, -26.310657, -25.213684, -21.803574,
                  -23.824408, -23.896356, -24.755267, -27.634789, -24.770751, -25.257713, -21.475037, -22.284100,
                  -19.107326, -14.097260, -13.068481, -11.726387, -12.326483, -13.984107, -17.575692,
                  -14.654513},    //	NOR,Norway
                 {-0.911117, -3.661034, -3.947278, -4.156100, -3.717106, -3.657192, -3.692048, -2.941704, -1.930418,
                  -2.290569, 5.388154, -0.833358, -2.803852, -1.811705, -1.846534, -1.518405, -0.679013, -0.669096,
                  -1.439681, -2.152282, -1.570812, -1.648659, -2.519303, -2.184913, -1.894256, -2.658540, -2.823773,
                  -2.402624, -1.500982, -2.635497, -2.441969, -1.914697}    //	CHE,Switzerland
                }};
        return fmEmission_unfccc_CRF;
    }
}

#endif
