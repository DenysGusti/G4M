#ifndef G4M_EUROPE_DG_DATAMAP_SCENARIOS_HPP
#define G4M_EUROPE_DG_DATAMAP_SCENARIOS_HPP

#include <string>
#include <print>
#include <ranges>

#include "../../log.hpp"
#include "../../helper/check_file.hpp"
#include "../../settings/dicts/dicts.hpp"
#include "../../misc/concrete/ipol.hpp"
#include "../../settings/constants.hpp"

using namespace std;
using namespace g4m::helper;
using namespace g4m::Dicts;
using namespace g4m::Constants;

namespace rv = ranges::views;

namespace g4m::GLOBIOM_scenarios_data {
    using datamapType = unordered_map<uint8_t, Ipol<double> >;
    using datamapScenariosType = unordered_map<string, datamapType, StringHash, equal_to<> >;

    class DatamapScenarios {
    public:
        // static functions

        [[nodiscard]] static datamapType
        readHistoric(const string_view file_path, const string_view message, const uint16_t firstYear,
                     const uint16_t lastYear) {
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
            auto trimmed_year_columns = span{year_columns}.subspan(offset_first, offset_last - offset_first + 1);

            datamapType datamap;
            datamap.reserve(30);

            vector<double> d_row;
            uint32_t line_num = 1;
            for (vector<string> s_row; !fp.eof(); ++line_num) {
                getline(fp, line);

                if (!line.empty() && line[0] != '#') {
                    s_row = line | rv::split(',') | ranges::to<vector<string> >();
                    d_row = s_row | rv::drop(s_row.size() - year_columns.size() + offset_first) |
                            rv::take(trimmed_year_columns.size()) |
                            rv::transform([&](const auto &cell) {  // subrange
                                if (cell.empty()) {
                                    ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                    return 0.;
                                }
                                return stod(string{cell.begin(), cell.end()});
                            }) | ranges::to<vector<double> >();
                    if (countryGLOBIOMId.contains(s_row[0]))
                        datamap[countryGLOBIOMId.at(s_row[0])] = {trimmed_year_columns, d_row};
                    else
                        ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
                }
            }
            INFO("Successfully read {} lines.", line_num);
            return datamap;
        }

        [[nodiscard]] static datamapScenariosType
        readGlobiomScenarios(const string_view file_path, const string_view message, const uint16_t firstYear,
                             const uint16_t lastYear) {
            INFO("> Reading the Globiom Scenarios {}...", message);
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
            auto trimmed_year_columns = span{year_columns}.subspan(offset_first, offset_last - offset_first + 1);

            datamapScenariosType scenariosDatamaps;
            scenariosDatamaps.reserve(3'400);

            vector<double> d_row;
            string scenario_name;
            uint32_t line_num = 1;
            for (vector<string> s_row; !fp.eof(); ++line_num) {
                getline(fp, line);

                if (!line.empty() && line[0] != '#') {
                    s_row = line | rv::split(',') | ranges::to<vector<string> >();
                    d_row = s_row | rv::drop(s_row.size() - year_columns.size() + offset_first) |
                            rv::take(trimmed_year_columns.size()) |
                            rv::transform([&](const auto &cell) {  // subrange
                                if (cell.empty()) {
                                    ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                    return 0.;
                                }
                                return stod(string{cell.begin(), cell.end()});
                            }) | ranges::to<vector<double> >();
                    scenario_name =
                            s_row[3] + '_' + s_row[4] + '_' + s_row[5] | rv::transform(::toupper) |
                            ranges::to<string>();
                    if (countryGLOBIOMId.contains(s_row[0]))
                        scenariosDatamaps[scenario_name][countryGLOBIOMId.at(s_row[0])] = {trimmed_year_columns, d_row};
                    else
                        ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
                }
            }
            INFO("Successfully read {} lines.", line_num);
            return scenariosDatamaps;
        }

        static void
        printDatamapScenarios(const datamapScenariosType &datamapScenarios, const string_view message) {
            TRACE("{}", message);
            for (const auto &[scenario, datamap]: datamapScenarios) {
                TRACE("{}", scenario);
                for (const auto &[id, ipol]: datamap)
                    TRACE("{}\n{}", idCountryGLOBIOM.at(id), ipol.str());
            }
        }

        datamapScenariosType landPriceScenarios;       // datamap for land price corrections for current price scenario (GLOBIOM)
        datamapScenariosType woodPriceScenarios;       // datamap for wood price corrections for current price scenario (GLOBIOM)
        datamapScenariosType woodDemandScenarios;      // datamap for wood demand (GLOBIOM)
        datamapScenariosType residuesDemandScenarios;  // datamap for residues demand (GLOBIOM)

        datamapScenariosType CO2PriceScenarios;

        datamapScenariosType GLOBIOM_AfforMaxCountryScenarios;  // Country maximum allowed afforestation estimated from GLOBIOM natural land (kha)
        datamapScenariosType GLOBIOM_LandCountryScenarios;      // Country GLOBIOM land (kha) reserved for croplands, pastures and short rotation plantations (also wetlands and infrastructure)

        // created dicts and adds bau scenario to dicts
        // order is important!!!
        void readGLOBIOM_and_datamaps() {
            readGLOBIOM();
            readDatamaps();
        }

        void readCO2price() {
            INFO("> Reading the CO2 prices...");
            ifstream fp = checkFile(settings.fileNames.at("co2p"));
            string line;
            getline(fp, line);

            auto year_columns = line | rv::split(',') | rv::drop_while(
                    [](const auto &s) { return string_view{s}.find_first_of("012345789") == string::npos; }) |
                                rv::transform([](const auto &s) { return stoi(string{s.begin(), s.end()}); }) |
                                ranges::to<vector<uint16_t> >();
            size_t idx_ge_refYear = distance(year_columns.begin(), ranges::lower_bound(year_columns, refYear));

            CO2PriceScenarios.reserve(3'400);

            vector<double> d_row;
            string scenario_name;
            uint32_t line_num = 1;
            for (vector<string> s_row; !fp.eof(); ++line_num) {
                getline(fp, line);
                if (!line.empty() && line[0] != '#') {
                    s_row = line | rv::split(',') | ranges::to<vector<string> >();
                    d_row = s_row | rv::drop(s_row.size() - year_columns.size()) |
                            rv::transform([&](const auto &cell) {  // subrange
                                if (cell.empty()) {
                                    ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                    return 0.;
                                }
                                return stod(string{cell.begin(), cell.end()});
                            }) | ranges::to<vector<double> >();
                    scenario_name =
                            s_row[3] + '_' + s_row[4] + '_' + s_row[5] | rv::transform(::toupper) |
                            ranges::to<string>();

                    if (countryGLOBIOMId.contains(s_row[0])) {
                        CO2PriceScenarios[scenario_name][countryGLOBIOMId.at(s_row[0])] = {year_columns, d_row};
                        CO2PriceScenarios[scenario_name][countryGLOBIOMId.at(
                                s_row[0])].data[refYear] = d_row[idx_ge_refYear];
                    } else
                        ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
                }
            }
            INFO("Successfully read {} lines.", line_num);
        }

        [[nodiscard]] array<double, numberOfCountries> readGlobiomLandCountryCalibrate_calcCountryLandArea() {
            if (!settings.fileNames.contains("gl_country_0") || settings.fileNames.at("gl_country_0").empty()) {
                WARN("No GLOBIOM LC country data for 2000-2020!!!!");
                return {};
            }

            INFO("> Reading the GLOBIOM land country data for 2000-2020...");
            ifstream fp = checkFile(settings.fileNames.at("gl_country_0"));
            string line;
            getline(fp, line);

            size_t first_data_column = 5;  // Forest,Arable,Natural,Wetland,Blocked
            auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                          ranges::to<vector<string> >();

            GLOBIOM_AfforMaxCountryScenarios[settings.bauScenario].reserve(250);
            GLOBIOM_LandCountryScenarios[settings.bauScenario].reserve(250);

            array<double, numberOfCountries> countryLandArea{};

            vector<double> d_row;

            uint32_t line_num = 1;
            for (vector<string> s_row; !fp.eof(); ++line_num) {
                getline(fp, line);

                if (!line.empty() && line[0] != '#') {
                    s_row = line | rv::split(',') | ranges::to<vector<string> >();
                    uint16_t year = stoi(s_row[first_data_column - 1]);

                    if (year <= 2020) {
                        d_row = s_row | rv::drop(first_data_column) |
                                rv::transform([&](const auto &cell) {  // subrange
                                    if (cell.empty()) {
                                        ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                        return 0.;
                                    }
                                    return stod(string{cell.begin(), cell.end()});
                                }) | ranges::to<vector<double> >();

                        if (countryGLOBIOMId.contains(s_row[0])) {
                            uint8_t id = countryGLOBIOMId.at(s_row[0]);
                            double gl_tmp = 0;
                            double gl_tot = 0;

                            for (const auto &[type, cell]: rv::zip(header, d_row)) {
                                if (year == 2000 && type == "FOREST")
                                    gl_tot = cell;
                                else if (type == "NATURAL") {
                                    GLOBIOM_AfforMaxCountryScenarios[settings.bauScenario][id].data[year] = cell;
                                    if (year == 2000)
                                        gl_tot += cell;
                                } else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                    gl_tmp += cell;
                            }

                            GLOBIOM_LandCountryScenarios[settings.bauScenario][id].data[year] = gl_tmp;
                            if (year == 2000)
                                countryLandArea[id] = gl_tot + gl_tmp;
                        } else
                            ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
                    }
                }
            }
            INFO("Successfully read {} lines.", line_num);
            return countryLandArea;
        }

        void readGlobiomLandCountry() {
            if (!settings.fileNames.contains("gl_country") || settings.fileNames.at("gl_country").empty()) {
                WARN("No GLOBIOM LC country data!!!!");
                return;
            }

            INFO("> Reading the GLOBIOM land country data...");
            ifstream fp = checkFile(settings.fileNames.at("gl_country"));
            string line;
            getline(fp, line);

            const size_t first_data_column = 5;  // Forest,Arable,Natural,Wetland,Blocked
            auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                          ranges::to<vector<string> >();

            GLOBIOM_AfforMaxCountryScenarios.reserve(27'500);
            GLOBIOM_LandCountryScenarios.reserve(27'500);

            vector<double> d_row;
            string scenario_name;
            uint32_t line_num = 1;
            for (vector<string> s_row; !fp.eof(); ++line_num) {
                getline(fp, line);

                if (!line.empty() && line[0] != '#') {
                    s_row = line | rv::split(',') | ranges::to<vector<string> >();
                    uint16_t year = stoi(s_row[first_data_column - 1]);

                    if (year > 2020 && year <= coef.eYear) {
                        d_row = s_row | rv::drop(first_data_column) |
                                rv::transform([&](const auto &cell) {  // subrange
                                    if (cell.empty()) {
                                        ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                        return 0.;
                                    }
                                    return stod(string{cell.begin(), cell.end()});
                                }) | ranges::to<vector<double> >();

                        if (countryGLOBIOMId.contains(s_row[0])) {
                            uint8_t id = countryGLOBIOMId.at(s_row[0]);
                            scenario_name = s_row[1] + '_' + s_row[2] + '_' + s_row[3] |
                                            rv::transform(::toupper) | ranges::to<string>();
                            double gl_tmp = 0;
                            for (const auto &[type, cell]: rv::zip(header, d_row)) {
                                if (type == "NATURAL") {
                                    GLOBIOM_AfforMaxCountryScenarios[scenario_name][id].data[year] = cell;
                                } else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                    gl_tmp += cell;
                            }
                            GLOBIOM_LandCountryScenarios[scenario_name][id].data[year] = gl_tmp;
                        } else
                            ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
                    }
                }
            }
            INFO("Successfully read {} lines.", line_num);
        }

        void convertUnitsDatamaps() {
            for (auto &[scenario, datamap]: woodDemandScenarios)
                for (auto &[id, ipol]: datamap)
                    for (auto &[key, value]: ipol.data)
                        value *= 1'000;
            for (auto &[scenario, datamap]: residuesDemandScenarios)
                for (auto &[id, ipol]: datamap)
                    for (auto &[key, value]: ipol.data)
                        value *= 250;
        }

        void correctAndConvertCO2Prices() {
            const double coef = deflator * molarRatio;
            for (auto &[scenario, datamap]: CO2PriceScenarios)
                for (auto &[id, ipol]: datamap)
                    for (auto &[year, CO2Price]: ipol.data)
                        CO2Price = CO2Price < 0.011 ? 0 : CO2Price * coef;
        }

        void printData() const {
            printDatamapScenarios(landPriceScenarios, "Globiom scenarios Land Price");
            printDatamapScenarios(woodPriceScenarios, "Globiom scenarios Wood Price");
            printDatamapScenarios(woodDemandScenarios, "Globiom scenarios Wood Demand");
            printDatamapScenarios(residuesDemandScenarios, "Globiom scenarios Residues Demand");
            printDatamapScenarios(GLOBIOM_AfforMaxCountryScenarios, "GLOBIOM Affor Max Country Scenarios");
            printDatamapScenarios(GLOBIOM_LandCountryScenarios, "GLOBIOM Land Country Scenarios");
            printDatamapScenarios(CO2PriceScenarios, "CO2PriceScenarios");
        }

    private:
        // adds bau scenario to dicts
        void readDatamaps() {
            landPriceScenarios[settings.bauScenario] = readHistoric(settings.fileNames.at("lp_0"), "Land Price", 2000,
                                                                    2020);
            woodPriceScenarios[settings.bauScenario] = readHistoric(settings.fileNames.at("wp_0"), "Wood Price", 2000,
                                                                    2020);
            woodDemandScenarios[settings.bauScenario] = readHistoric(settings.fileNames.at("wd_0"), "Wood Demand", 1990,
                                                                     2021);
            residuesDemandScenarios[settings.bauScenario] = readHistoric(settings.fileNames.at("rd_0"),
                                                                         "Residues Demand", 2000, 2020);
        }

        // created dicts
        void readGLOBIOM() {
            landPriceScenarios = readGlobiomScenarios(settings.fileNames.at("lp"), "Land Price", 2030, coef.eYear);
            woodPriceScenarios = readGlobiomScenarios(settings.fileNames.at("wp"), "Wood Price", 2030, coef.eYear);
            woodDemandScenarios = readGlobiomScenarios(settings.fileNames.at("wd"), "Wood Demand", 2030, coef.eYear);
            residuesDemandScenarios = readGlobiomScenarios(settings.fileNames.at("rd"), "Residues Demand", 2030,
                                                           coef.eYear);

            // years
//            println("{}", landPriceScenarios.begin()->second.begin()->second.data.begin()->first);
//            println("{}", landPriceScenarios.begin()->second.begin()->second.data.rbegin()->first);
        }
    };
}

#endif
