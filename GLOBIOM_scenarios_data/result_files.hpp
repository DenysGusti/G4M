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
        fs::path harvestDetailsFile;
        fs::path residueExtractDetailsFile;
        fs::path bioclimaDetailsFile;
        fs::path NPVFile;

        string detailsBuffer;
        string cellInfoBuffer;
        string harvestDetailsBuffer;
        string residueExtractDetailsBuffer;
        string bioclimaDetailsBuffer;
        string deadwoodTestBuffer;

        ResultFiles(const string_view outputFolder, const string_view local_suffix) noexcept:
                outputPath{format("{}/{}", outputFolder, local_suffix)},
                detailsFile{outputPath / "details.csv"},
                cellInfoFile{outputPath / "cellInfo.csv"},
                deadwoodTestFile{outputPath / "deadwoodTest.csv"},
                harvestDetailsFile{outputPath / "harvestDetails.csv"},
                residueExtractDetailsFile{outputPath / "residueExtractDetails.csv"},
                bioclimaDetailsFile{outputPath / "bioclimaDetails.csv"},
                NPVFile{outputPath / "NPV.csv"} {
            if (!fs::exists(outputPath))
                fs::create_directories(outputPath);

            biomassBau.reserve(256);

            detailsBuffer.reserve(initialBufferSize);
            cellInfoBuffer.reserve(initialBufferSize);
            deadwoodTestBuffer.reserve(initialBufferSize);
            harvestDetailsBuffer.reserve(initialBufferSize);
            residueExtractDetailsBuffer.reserve(initialBufferSize);
            bioclimaDetailsBuffer.reserve(initialBufferSize);
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
                    "litter_old_tCha,litter_new_tCha,deadwood_old_in_tChayear,deadwood_new_in_tChayear,"
                    "litter_old_in_tChayear,litter_new_in_tChayear,deadwood_old_em_tChayear,deadwood_new_em_tChayear,"
                    "litter_old_em_tChayear,litter_new_em_tChayear\n";
            residueExtractDetailsBuffer +=
                    "simuid,country,year,usedForest,OforestArea_kha,residuesBrHl_Int_m3ha,residuesStumps_Int_m3ha,"
                    "residuesBrHl_nonInt_m3ha,residues_notTakenDead_m3ha,residuesBrHl_Int_m3,residuesStumps_Int_m3,"
                    "residuesBrHl_nonInt_m3,residues_notTakenDead_m3,em_SustBrHl_Int_MtCO2year,em_Stumps_Int_MtCO2year,"
                    "em_BrHl_nonInt_MtCO2year,em_Sust4_MtCO2year,costsSuitBrHl_Int_USDm3,costsStumps_Int_USDm3,"
                    "costsBrHl_nonInt_USDm3,costs_notTakenDead_USDm3,costsTotal_USDm3,usedBrHl_Int,usedStumps,"
                    "usedBrHl_nonInt,used_notTakenDead\n";
            bioclimaDetailsBuffer +=
                    "x,y,simuid,countryid,nuts2,year,landarea_ha,species,protected,area_forest_old_ha,"
                    "area_forest_new_ha,area_forest10_ha,area_forest30_ha,area_forest_primary_ha,SD,SD10,SD30,"
                    "rotation_mai,rotation_mxbm,rotation,rotation10,rotation30,rotation_primary,cai_m3,cai_new_m3,"
                    "cai10_m3,cai30_m3,cai_primary_m3,biom_fm_tcha,biom_af_tcha,biom_fm10_tcha,biom_fm30_tcha,"
                    "biom_fm_primary_tcha,harvest_o_m3year,harvest_n_m3year,harvest10_m3year,harvest30_m3year,"
                    "fellings_o_m3year,fellings_n_m3year,fellings10_m3year,fellings30_m3year,deadwood_old_tCha,"
                    "deadwood_new_tCha,deadwood10_tCha,deadwood30_tCha,deadwood_primary_tCha,deadwood_old_in_tChayear,"
                    "deadwood_new_in_tChayear,deadwood10_in_tChayear,deadwood30_in_tChayear,"
                    "deadwood_primary_in_tChayear,deadwood_old_em_tChayear,deadwood_new_em_tChayear,"
                    "deadwood10_em_tChayear,deadwood30_em_tChayear,deadwood_primary_em_tChayear,litter_old_tCha,"
                    "litter_new_tCha,litter10_tCha,litter30_tCha,litter_primary_tCha,litter_old_in_tChayear,"
                    "litter_new_in_tChayear,litter10_in_tChayear,litter30_in_tChayear,litter_primary_in_tChayear,"
                    "litter_old_em_tChayear,litter_new_em_tChayear,litter10_em_tChayear,litter30_em_tChayear,"
                    "litter_primary_em_tChayear,setaside10,setaside30,oldest_age_o,oldest_age_n,oldest_age10,"
                    "oldest_age30,oldest_age_primary\n";
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
            size_t maxBufferSize = max({detailsBuffer.length(), cellInfoBuffer.length(), harvestDetailsBuffer.length(),
                                        residueExtractDetailsBuffer.length(), bioclimaDetailsBuffer.length(),
                                        deadwoodTestBuffer.length()});
            if (initialBufferSize < maxBufferSize)
                WARN("Increase initialBufferSize! initialBufferSize = {}, maxBufferSize = {}",
                     initialBufferSize, maxBufferSize);
        }
    };
}

#endif
