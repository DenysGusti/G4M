#ifndef G4M_EUROPE_DG_RESULT_FILES_HPP
#define G4M_EUROPE_DG_RESULT_FILES_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = filesystem;

namespace g4m::GLOBIOM_scenarios_data {
    class ResultFiles {
    public:
        static constexpr size_t initialBufferSize = 10'000'000;

        vector<vector<double>> biomassBau;
        vector<vector<double>> profitBau;

        fs::path outputPath;

        fs::path detailsFile;
        fs::path cellInfoFile;
        fs::path deadwoodTestFile;
        fs::path bioclimaDetailsFile;
        fs::path residueExtractDetailsFile;
        fs::path harvestDetailsFile;
        fs::path NPVFile;

        string detailsBuffer;
        string cellInfoBuffer;
        string deadwoodTestBuffer;
        string bioclimaDetailsBuffer;
        string residueExtractDetailsBuffer;
        string harvestDetailsBuffer;

        ResultFiles(const string_view outputFolder, const string_view local_suffix) noexcept:
                outputPath{format("{}/{}", outputFolder, local_suffix)},
                detailsFile{outputPath / "details.csv"},
                cellInfoFile{outputPath / "cellInfo.csv"},
                deadwoodTestFile{outputPath / "deadwoodTest.csv"},
                bioclimaDetailsFile{outputPath / "bioclimaDetails.csv"},
                residueExtractDetailsFile{outputPath / "residueExtractDetails.csv"},
                harvestDetailsFile{outputPath / "harvestDetails.csv"},
                NPVFile{outputPath / "NPV.csv"} {
            if (!fs::exists(outputPath))
                fs::create_directories(outputPath);

            biomassBau.reserve(256);

            detailsBuffer.reserve(initialBufferSize);
            cellInfoBuffer.reserve(initialBufferSize);
            deadwoodTestBuffer.reserve(initialBufferSize);
            bioclimaDetailsBuffer.reserve(initialBufferSize);
            residueExtractDetailsBuffer.reserve(initialBufferSize);
            harvestDetailsBuffer.reserve(initialBufferSize);
        }

        void addHeadersToBuffers(const string_view cellCSVHeader) noexcept {
            detailsBuffer +=
                    "asID,simuID,country,year,OForestShare,AForestShare,CAb,abFM,abFMNew,MAI_M3Ha,rotMAI,"
                    "harvestThM3HaOld,harvestFctM3HaOld,harvestThM3HaNew,harvestFctM3HaNew,harvTotCur,harvMAI,"
                    "netCAI_m3ha,grossCAI_m3ha,rotBiomass,forNPV_CH,fVal,aVal,rotationTimeCurr,rotationTimeCurrNew,"
                    "timberPrice,thinningForest,thinningForestNew,rotMAI_gTOpt,realAreaO,defShare,defArea_virtFor,BEF,"
                    "BEF_prev,nai_m3ha,nai_thm3,fellings_thm3,fellings_nai_share,fellings_nai_m3,delta_bmstO_m3,"
                    "balanceO,lost_bm_m3,bm_stem_tC,bm_stem_prev_tC,bm_stem_tCha,bm_stem_prev_tCha,oForShare,"
                    "oForSharePrev,landAreaHa,FM_sink_ab_mtCO2year,FM_sink_bm_mtCO2year,trotMaxBm,oldestAge,"
                    "oldestAgeNew,lostW_fc_thm3,lostW_th_thm3,em_fm_check,globiomReserved,maxAff,landBalance\n";
            cellInfoBuffer += format("asID,simuID,country,year,{}\n", cellCSVHeader);
            deadwoodTestBuffer +=
                    "asID,simuID,country,year,forest_share_type,managed,extractedResidues,extractedCleaned,"
                    "RESIDUESUSESHARE,deadwoodBranches_fc,deadwoodBranches_th,branches_harvtrees_tot,"
                    "branches_deadtrees_multifunction,deadwood_fc_mng,deadwood_fc_unmng,stem_deadtrees_multifunction,"
                    "deadwood_mort,deadwoodBranches_mort,deadwoodStump_mort,deadwoodPoolIn,litterBranches_fc,"
                    "litterBranches_th,branches_harvtrees_lit,branches_deadtrees_multifunction_lit,litter_mort,"
                    "litterBranches_mort,litterStump_mort,litter_hRes_mng,litter_hRes_unmng,"
                    "litter_hRes_unmng_harv_litterin,litter_hRes_unmng_ded_litterin,litterPoolIn\n";
            bioclimaDetailsBuffer +=
                    "x,y,simuID,country,nuts2,year,land_area_ha,species,protected,area_forest_U_ha,"
                    "area_forest_O10_ha,area_forest_O30_ha,area_P_ha,area_forest_N_ha,SD_U,SD_O10,SD_O30,"
                    "rotation_mai,rotation_max_bm,rotation_U,rotation_O10,rotation_O30,rotation_P,cai_U_m3,"
                    "cai_O10_m3,cai_O30_m3,cai_P_m3,cai_N_m3,biomass_U_tCha,biomass_O10_tCha,biomass_O30_tCha,"
                    "biomass_P_tCha,biomass_N_tCha,harvest_U_m3year,harvest_O10_m3year,harvest_O30_m3year,"
                    "harvest_N_m3year,fellings_U_m3year,fellings_O10_m3year,fellings_O30_m3year,fellings_N_m3year,"
                    "deadwood_U_tCha,deadwood_O10_tCha,deadwood_O30_tCha,deadwood_P_tCha,deadwood_N_tCha,"
                    "deadwood_in_U_tChaYear,deadwood_in_O10_tChaYear,deadwood_in_O30_tChaYear,deadwood_in_P_tChaYear,"
                    "deadwood_in_N_tChaYear,deadwood_em_U_tChaYear,deadwood_em_O10_tChaYear,deadwood_em_O30_tChaYear,"
                    "deadwood_em_P_tChaYear,deadwood_em_N_tChaYear,litter_U_tCha,litter_O10_tCha,litter_O30_tCha,"
                    "litter_P_tCha,litter_N_tCha,litter_U_in_tChaYear,litter_O10_in_tChaYear,litter_O30_in_tChaYear,"
                    "litter_P_in_tChaYear,litter_N_in_tChaYear,litter_em_U_tChaYear,litter_em_O10_tChaYear,"
                    "litter_em_O30_tChaYear,litter_em_P_tChaYear,litter_em_N_tChaYear,set_aside_O10,set_aside_O30,"
                    "oldest_age_U,oldest_age_O10,oldest_age_O30,oldest_age_P,oldest_age_N\n";
            residueExtractDetailsBuffer +=
                    "simuID,country,year,usedForest,U_ForestArea_kha,residuesBrHl_Int_m3ha,residuesStumps_Int_m3ha,"
                    "residuesBrHl_nonInt_m3ha,residues_notTakenDead_m3ha,residuesBrHl_Int_m3,residuesStumps_Int_m3,"
                    "residuesBrHl_nonInt_m3,residues_notTakenDead_m3,em_SustBrHl_Int_MtCO2year,em_Stumps_Int_MtCO2year,"
                    "em_BrHl_nonInt_MtCO2year,em_Sust4_MtCO2year,costsSuitBrHl_Int_USDm3,costsStumps_Int_USDm3,"
                    "costsBrHl_nonInt_USDm3,costs_notTakenDead_USDm3,costsTotal_USDm3,usedBrHl_Int,usedStumps,"
                    "usedBrHl_nonInt,used_notTakenDead\n";
            harvestDetailsBuffer +=
                    "x,y,simuid,countryid,nuts2,year,SD,rotation,area_forest_old_ha,area_forest_new_ha,cai_m3ha,"
                    "harvest_total_m3hayear,harvest_fc_m3hayear,harvest_th_m3hayear,biom_fm_tcha,biom_af_tcha,"
                    "harvres_fcO_m3halyear,harvres_thO_m3halyear,harvres_fcN_m3halyear,harvres_thN_m3halyear,"
                    "harvest_gs_fco_m3hayear,harvest_gs_fcn_m3hayear,harvest_dbh_fco_cm,harvest_dbh_fcn_cm,"
                    "harvest_h_fco_m,harvest_h_fcn_m,harvest_dbh_tho_cm,harvest_dbh_thn_cm,harvest_h_tho_m,"
                    "harvest_h_thn_m,species,harvest_fco_m3halyear,harvest_tho_m3halyear,harvest_fcn_m3halyear,"
                    "harvest_thn_m3halyear,harvest_fco_m3year,harvest_tho_m3year,harvest_fcn_m3year,harvest_thn_m3year,"
                    "harvest_conifer_fco_m3year,harvest_conifer_tho_m3year,harvest_conifer_fcn_m3year,"
                    "harvest_conifer_thn_m3year,harvest_broadleave_fco_m3year,harvest_broadleave_tho_m3year,"
                    "harvest_broadleave_fcn_m3year,harvest_broadleave_thn_m3year,harvest_sc_m3hayear,"
                    "harvres_scO_m3halyear,harvres_scN_m3halyear,harvest_gs_fco_m3hayear,harvest_gs_fcn_m3hayear,"
                    "harvest_dbh_fc_sco_cm,harvest_dbh_fc_scn_cm,harvest_h_fc_sco_m,harvest_h_fc_scn_m,"
                    "harvest_dbh_th_sco_cm,harvest_dbh_th_scn_cm,harvest_h_th_sco_m,harvest_h_th_scn_m,"
                    "harvest_sco_m3halyear,harvest_scn_m3halyear,harvest_sco_m3year,harvest_scn_m3year,"
                    "harvest_conifer_sco_m3year,harvest_conifer_scn_m3year,harvest_broadleave_sco_m3year,"
                    "harvest_broadleave_scn_m3year,rotation_mai,rotation_mxbm,deadwood_old_tCha,deadwood_new_tCha,"
                    "litter_old_tCha,litter_new_tCha,deadwood_old_in_tChaYear,deadwood_new_in_tChaYear,"
                    "litter_old_in_tChaYear,litter_new_in_tChaYear,deadwood_old_em_tChaYear,deadwood_new_em_tChaYear,"
                    "litter_old_em_tChaYear,litter_new_em_tChaYear\n";
        }

        /*
        a  ,  b  _  U  ,  c  ,  d  _  N  ,  e
        0  1  2  3  4  5  6  7  8  9 10 11 12
              fd _              ld _
        ___c1___|_______c2________|
           1           2     3           4
        fc _              lc _
        0        1        2        3        4
                |_________s_________|
        grouping
        0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
        0  4  8 12 16  1  5  9 13 17  2  6 10 14 18  3  7 11 15 19
        */
        void sortCellInfo() {
            auto rows = cellInfoBuffer | rv::split('\n') | ranges::to<vector<string> >();
            rows.pop_back();    // empty line
            string &header = rows.front();

            ptrdiff_t first_data = header.find("_U");
            ptrdiff_t last_data = header.rfind("_N");

            size_t first_comma = ranges::count(header | rv::take(first_data), ',');
            size_t last_comma =
                    first_comma + ranges::count(header | rv::drop(first_data) | rv::take(last_data - first_data), ',');

            constexpr size_t number_of_cell_forests = 5;
            const size_t param_size = (last_comma - first_comma + 1) / number_of_cell_forests;

            if ((last_comma - first_comma + 1) % number_of_cell_forests != 0) {
                FATAL("(last_comma - first_comma + 1) % number_of_cell_forests != 0");
                throw runtime_error{"bad csv file!"};
            }

            for (auto &row: rows) {
                auto cells = row | rv::split(',') | ranges::to<vector<string> >();
                auto cell_forest_data = span{cells}.subspan(first_comma, last_comma - first_comma + 1);

                vector<string> data{cell_forest_data.begin(), cell_forest_data.end()};
                // group by variable name
                for (size_t i = 0; i < cell_forest_data.size(); ++i)
                    cell_forest_data[i] = data[i / number_of_cell_forests + i % number_of_cell_forests * param_size];

                row = cells | rv::join_with(',') | ranges::to<string>();
            }

            string header_row = rows.front();
            rows.erase(rows.begin());
            ranges::sort(rows);
            cellInfoBuffer = header_row + '\n' + (rows | rv::join_with('\n') | ranges::to<string>());
        }

        void saveFiles() const {
            {
                ofstream of{detailsFile};
                of << detailsBuffer;
                INFO("{} bytes written to {}", detailsBuffer.length(), detailsFile.string());
            }
            {
                ofstream of{cellInfoFile};
                of << cellInfoBuffer;
                INFO("{} bytes written to {}", cellInfoBuffer.length(), cellInfoFile.string());
            }
            {
                ofstream of{deadwoodTestFile};
                of << deadwoodTestBuffer;
                INFO("{} bytes written to {}", deadwoodTestBuffer.length(), deadwoodTestFile.string());
            }
            {
                ofstream of{bioclimaDetailsFile};
                of << bioclimaDetailsBuffer;
                INFO("{} bytes written to {}", bioclimaDetailsBuffer.length(), bioclimaDetailsFile.string());
            }
            {
                ofstream of{residueExtractDetailsFile};
                of << residueExtractDetailsBuffer;
                INFO("{} bytes written to {}", residueExtractDetailsBuffer.length(),
                     residueExtractDetailsFile.string());
            }
            size_t maxBufferSize = max({detailsBuffer.length(), cellInfoBuffer.length(), deadwoodTestBuffer.length(),
                                        bioclimaDetailsBuffer.length(), residueExtractDetailsBuffer.length(),
                                        harvestDetailsBuffer.length()});
            if (initialBufferSize < maxBufferSize)
                WARN("Increase initialBufferSize! initialBufferSize = {}, maxBufferSize = {}",
                     initialBufferSize, maxBufferSize);
        }
    };
}

#endif
