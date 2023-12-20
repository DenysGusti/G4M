#ifndef G4M_EUROPE_DG_READING_HPP
#define G4M_EUROPE_DG_READING_HPP

#include "../constants.hpp"
#include "../arrays/arrays.hpp"
#include "../start_data/start_data.hpp"
#include "../dicts/dicts.hpp"
#include "../log.hpp"
#include "../helper/check_file.hpp"

using namespace std;
using namespace g4m::StartData;
using namespace g4m::Dicts;

namespace g4m::DataIO::reading {

    [[nodiscard]] vector<DataStruct> readPlots() {
        INFO("> Reading the rest of input data...");
        ifstream fp = checkFile(fileNames.at("dat"));
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

        vector<DataStruct> plots;
        plots.reserve(3'000);
        uint32_t line_num = 1;
        for (vector<double> line_cells; !fp.eof(); ++line_num) {
            getline(fp, line);

            if (!line.empty() && line[0] != '#') {
                line_cells = line | rv::split(',') |
                             rv::transform([&](const auto &cell) {  // subrange
                                 if (cell.empty()) {
                                     ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                     return 0.;
                                 }
                                 return stod(string{cell.begin(), cell.end()});
                             }) | ranges::to<vector<double> >();

                plots.emplace_back(header, line_cells);
            }
        }
        INFO("Successfully read {} lines.", line_num);
        return plots;
    }

    void readGlobiomLandCalibrate() {
        if (fileNames.at("gl_0").empty()) {
            WARN("No GLOBIOM LC data for 2000-2020!!!!");
            return;
        }

        INFO("> Reading the GLOBIOM LC data for 2000-2020...");
        ifstream fp = checkFile(fileNames.at("gl_0"));
        string line;
        getline(fp, line);

        size_t first_data_column = 5;
        auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                      ranges::to<vector<string> >();

        globiomAfforMaxScenarios[bauScenario].reserve(3'100);
        globiomLandScenarios[bauScenario].reserve(3'100);

        vector<double> d_row;

        uint32_t line_num = 1;
        for (vector<string> s_row; !fp.eof(); ++line_num) {
            getline(fp, line);

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                uint32_t simuId = stoi(s_row[0]);

                if (plotsSimuID.contains(simuId)) {
                    uint16_t year = stoi(s_row[first_data_column - 1]);

                    if (year <= 2020) [[likely]] {
                        d_row = s_row | rv::take(s_row.size() - 1) | rv::drop(first_data_column) |
                                rv::transform([&](const auto &cell) {  // subrange
                                    if (cell.empty()) {
                                        ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                        return 0.;
                                    }
                                    return stod(string{cell.begin(), cell.end()});
                                }) | ranges::to<vector<double> >();

                        double gl_tmp = 0;
                        for (const auto &[type, cell]: rv::zip(header, d_row)) {
                            if (type == "NATURAL")
                                globiomAfforMaxScenarios[bauScenario][simuId].data[year] = cell;
                            else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                gl_tmp += cell;
                        }
                        globiomLandScenarios[bauScenario][simuId].data[year] = gl_tmp;
                    }
                } else
                    DEBUG("Plots don't contain simuId = {}", simuId);
            }
        }
        INFO("Successfully read {} lines.", line_num);
    }

    void readGlobiomLand() {
        if (fileNames.at("gl").empty()) {
            WARN("No GLOBIOM LC data!!!!");
            return;
        }

        INFO("> Reading the GLOBIOM LC data...");
        ifstream fp = checkFile(fileNames.at("gl"));
        string line;
        getline(fp, line);

        const size_t first_data_column = 5;
        auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                      ranges::to<vector<string> >();

        globiomAfforMaxScenarios.reserve(27'500);
        globiomLandScenarios.reserve(27'500);

        vector<double> d_row;
        string scenario_name;
        uint32_t line_num = 1;
        for (vector<string> s_row; !fp.eof(); ++line_num) {
            getline(fp, line);

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                uint32_t simuId = stoi(s_row[0]);

                if (plotsSimuID.contains(simuId)) {
                    uint16_t year = stoi(s_row[first_data_column - 1]);

                    if (year > 2020 && year <= coef.eYear) {
                        d_row = s_row | rv::take(s_row.size() - 1) | rv::drop(first_data_column) |
                                rv::transform([&](const auto &cell) {  // subrange
                                    if (cell.empty()) {
                                        ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                        return 0.;
                                    }
                                    return stod(string{cell.begin(), cell.end()});
                                }) | ranges::to<vector<double> >();

                        scenario_name = s_row[1] + '_' + s_row[2] + '_' + s_row[3] |
                                        rv::transform(::toupper) | ranges::to<string>();
                        double gl_tmp = 0;
                        for (const auto &[type, cell]: rv::zip(header, d_row)) {
                            if (type == "NATURAL") {
                                globiomAfforMaxScenarios[scenario_name][simuId].data[year] = cell;
                            } else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                gl_tmp += cell;
                        }
                        globiomLandScenarios[scenario_name][simuId].data[year] = gl_tmp;
                    }
                } else
                    DEBUG("Plots don't contain simuId = {}", simuId);
            }
        }
        INFO("Successfully read {} lines.", line_num);
    }

    [[nodiscard]] map <pair<uint32_t, uint32_t>, string> readNUTS2() {
        INFO("> Reading the NUTS2...");
        ifstream fp = checkFile(fileNames.at("nuts2"));
        string line;
        getline(fp, line);

        map<pair<uint32_t, uint32_t>, string> nuts2_id;
        uint32_t line_num = 1;
        for (vector<string> s_row; !fp.eof(); ++line_num) {
            getline(fp, line);
            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                uint32_t x = lround((stod(s_row[0]) + 180) / gridStep - 0.5);
                uint32_t y = lround((stod(s_row[1]) + 90) / gridStep - 0.5);
                nuts2_id[{x, y}] = s_row[2];
            }
        }
        INFO("Successfully read {} lines.", line_num);
        return nuts2_id;
    }

    [[nodiscard]] heterSimuIdScenariosType readMAIClimate() {
        if (fileNames.at("maic").empty()) {
            WARN("No MAI climate data!!!!");
            return {};
        }

        INFO("> Reading the MAI climate data...");
        ifstream fp = checkFile(fileNames.at("maic"));
        string line;
        getline(fp, line);

        // ...1,ClimaScen,ANYRCP,lon,lat,Year,G4MOutput,value
        heterSimuIdScenariosType simuIdScenarios;
        simuIdScenarios.reserve(96);  // 2'373'408 / 24'723 = 96

        string scenario_name;
        uint32_t line_num = 1;
        for (vector<string> s_row; !fp.eof(); ++line_num) {
            getline(fp, line);

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();

                if (s_row[6] == "mai")
                    if (uint16_t year = stoi(s_row[5]); year <= coef.eYear) {

                        uint32_t x = lround((stod(s_row[3]) + 180) / gridStep - 0.5);
                        uint32_t y = lround((stod(s_row[4]) + 90) / gridStep - 0.5);

                        if (auto g4mId = plotsXY_SimuID.find({x, y}); g4mId != plotsXY_SimuID.end()) {
                            scenario_name = s_row[2] + '_' + s_row[1] | rv::transform(::toupper) | ranges::to<string>();
                            double value = stod(s_row[7]);
                            simuIdScenarios[scenario_name][g4mId->second].data[year] = value;
                        } else
                            DEBUG("Plots don't contain (x, y) = ({}, {}), line {}", x, y, line_num + 1);
                    }
            }
        }
        INFO("Successfully read {} lines.", line_num);
        return simuIdScenarios;
    }

    void readDisturbances() {
        if (fileNames.at("disturbance").empty()) {
            WARN("No disturbance projection data!!!!");
            return;
        }

        INFO("> Reading the disturbance data...");
        ifstream fp = checkFile(fileNames.at("disturbance"));
        string line;
        getline(fp, line);

        // "","lon","lat","Year","Agent","value"
        commonDisturbWind.reserve(22'000);
        commonDisturbFire.reserve(22'000);
        commonDisturbBiotic.reserve(22'000);

        uint32_t line_num = 1;

        for (vector<string> s_row; !fp.eof(); ++line_num) {
            getline(fp, line);
            erase(line, '"');  // trimming double quotes

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();

                if (uint16_t year = stoi(s_row[3]); year <= coef.eYear) {
                    uint32_t x = lround((stod(s_row[1]) + 180) / gridStep - 0.5);
                    uint32_t y = lround((stod(s_row[2]) + 90) / gridStep - 0.5);

                    if (auto g4mId = plotsXY_SimuID.find({x, y}); g4mId != plotsXY_SimuID.end()) {
                        double value = stod(s_row[5]);

                        if (s_row[4] == "wind")
                            commonDisturbWind[g4mId->second].data[year] = value;
                        else if (s_row[4] == "fire")
                            commonDisturbFire[g4mId->second].data[year] = value;
                        else if (s_row[4] == "biotic")
                            commonDisturbBiotic[g4mId->second].data[year] = value;
                        else
                            DEBUG("Unknown agent type: {}, line: {}", s_row[4], line_num + 1);
                    } else
                        DEBUG("Plots don't contain (x, y) = ({}, {}), line {}", x, y, line_num + 1);
                }
            }
        }
        INFO("Successfully read {} lines.", line_num);
    }

    void readDisturbancesExtreme() {
        if (fileNames.at("disturbanceExtreme").empty()) {
            WARN("No extreme disturbance projection data!!!!");
            return;
        }

        INFO("> Reading the extreme disturbance data ...");
        ifstream fp = checkFile(fileNames.at("disturbanceExtreme"));
        string line;
        getline(fp, line);

        // "","lon","lat","Year","Agent","value"
        commonDisturbWindExtreme.reserve(25'000);
        commonDisturbFireExtreme.reserve(25'000);
        commonDisturbBioticExtreme.reserve(25'000);

        uint32_t line_num = 1;

        for (vector<string> s_row; !fp.eof(); ++line_num) {
            getline(fp, line);
            erase(line, '"');  // trimming double quotes

            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();

                if (uint16_t year = stoi(s_row[3]); year <= coef.eYear) {
                    uint32_t x = lround((stod(s_row[1]) + 180) / gridStep - 0.5);
                    uint32_t y = lround((stod(s_row[2]) + 90) / gridStep - 0.5);

                    if (auto g4mId = plotsXY_SimuID.find({x, y}); g4mId != plotsXY_SimuID.end()) {
                        double value = stod(s_row[5]);

                        if (s_row[4] == "wind")
                            commonDisturbWindExtreme[g4mId->second].data[year] = value;
                        else if (s_row[4] == "fire")
                            commonDisturbFireExtreme[g4mId->second].data[year] = value;
                        else if (s_row[4] == "biotic")
                            commonDisturbBioticExtreme[g4mId->second].data[year] = value;
                        else
                            DEBUG("Unknown agent type: {}, line: {}", s_row[4], line_num + 1);
                    } else
                        DEBUG("Plots don't contain (x, y) = ({}, {}), line {}", x, y, line_num);
                }
            }
        }
        INFO("Successfully read {} lines.", line_num);
    }

    [[nodiscard]] unordered_map<uint8_t, vector<double> > readAgeStructData() {
        INFO("> Reading the age struct data...");
        ifstream fp = checkFile(fileNames.at("ageStruct"));

        string line;
        getline(fp, line);

        unordered_map<uint8_t, vector<double> > ageStructData;
        ageStructData.reserve(36);

        uint32_t line_num = 1;

        for (vector<double> line_cells; !fp.eof(); ++line_num) {
            getline(fp, line);

            if (!line.empty() && line[0] != '#') {
                line_cells = line | rv::split(',') |
                             rv::transform([&](const auto &cell) {  // subrange
                                 if (cell.empty()) {
                                     ERROR("!!! CSV line {} empty cell, substituted by 0", line_num + 1);
                                     return 0.;
                                 }
                                 return stod(string{cell.begin(), cell.end()});
                             }) | ranges::to<vector<double> >();
                ageStructData[static_cast<uint8_t>(line_cells[0])] = {line_cells.begin() + 1, line_cells.end()};
            }
        }
        INFO("Successfully read {} lines.", line_num);
        return ageStructData;
    }

    [[nodiscard]] vector<vector<double> > readBau(const string_view file_path, const string_view message) {
        INFO("> Reading bau data {}...", message);
        ifstream fp = checkFile(file_path, true);

        int size = 0;
        fp.read(reinterpret_cast<char *>(&size), sizeof(int));
        int dimensionsNum = 0;
        fp.read(reinterpret_cast<char *>(&dimensionsNum), sizeof(int));

        vector<vector<double> > bauData(size, vector<double>(dimensionsNum));

        for (auto &row: bauData)
            fp.read(reinterpret_cast<char *>(row.data()), static_cast<streamsize>(sizeof(double) * dimensionsNum));

        return bauData;
    }
}

#endif
