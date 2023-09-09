#ifndef G4M_EUROPE_DG_READING_HPP
#define G4M_EUROPE_DG_READING_HPP

#include "../constants.hpp"
#include "../arrays/arrays.hpp"
#include "../start_data/start_data.hpp"
#include "../log.hpp"

using namespace std;
using namespace g4m::StartData;

namespace g4m::DataIO::reading {

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

    void readPlots() {
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

        rawPlots.reserve(3'000);
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

                rawPlots.emplace_back(header, line_cells);
            }
        }
        INFO("Successfully read {} lines.", line_num);
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
                    datamap[countryGLOBIOMId[s_row[0]]] = {trimmed_year_columns, d_row};
                else
                    ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
            }
        }
        INFO("Successfully read {} lines.", line_num);
        return datamap;
    }

    void readDatamaps() {
        landPriceScenarios[s_bauScenario] = readHistoric(fileName_lp0, "Land Price", 2000, 2020);
        woodPriceScenarios[s_bauScenario] = readHistoric(fileName_wp0, "Wood Price", 2000, 2020);
        woodDemandScenarios[s_bauScenario] = readHistoric(fileName_wd0, "Wood Demand", 1990, 2021);
        residuesDemandScenarios[s_bauScenario] = readHistoric(fileName_rd0, "Residues Demand", 2000, 2020);
    }

    heterDatamapScenariosType readGlobiomScenarios(const string_view file_path, const string_view message,
                                                   const uint16_t firstYear, const uint16_t lastYear) {
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

        heterDatamapScenariosType scenariosDatamaps;
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
                        s_row[3] + '_' + s_row[4] + '_' + s_row[5] | rv::transform(::toupper) | ranges::to<string>();
                if (countryGLOBIOMId.contains(s_row[0]))
                    scenariosDatamaps[scenario_name][countryGLOBIOMId[s_row[0]]] = {trimmed_year_columns, d_row};
                else
                    ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
            }
        }
        INFO("Successfully read {} lines.", line_num);
        return scenariosDatamaps;
    }

    void readGlobiom() {
        landPriceScenarios = readGlobiomScenarios(fileName_lp, "Land Price", 2030, coef.eYear);
        woodPriceScenarios = readGlobiomScenarios(fileName_wp, "Wood Price", 2030, coef.eYear);
        woodDemandScenarios = readGlobiomScenarios(fileName_wd, "Wood Demand", 2030, coef.eYear);
        residuesDemandScenarios = readGlobiomScenarios(fileName_rd, "Residues Demand", 2030, coef.eYear);

//        years
//        cout << landPriceScenarios.begin()->second.begin()->second.data.begin()->first << '\n';
//        cout << landPriceScenarios.begin()->second.begin()->second.data.rbegin()->first << '\n';
    }

    void readGlobiomLandCalibrate() {
        if (fileName_gl_0.empty()) {
            WARN("No GLOBIOM LC data for 2000-2020!!!!");
            return;
        }

        INFO("> Reading the GLOBIOM LC data for 2000-2020...");
        ifstream fp = checkFile(fileName_gl_0);
        string line;
        getline(fp, line);

        size_t first_data_column = 5;
        auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                      ranges::to<vector<string> >();

        globiomAfforMaxScenarios[s_bauScenario].reserve(3'100);
        globiomLandScenarios[s_bauScenario].reserve(3'100);

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
                                globiomAfforMaxScenarios[s_bauScenario][simuId].data[year] = cell;
                            else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                gl_tmp += cell;
                        }
                        globiomLandScenarios[s_bauScenario][simuId].data[year] = gl_tmp;
                    }
                } else
                    DEBUG("Plots don't contain simuId = {}", simuId);
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

    void readGlobiomLand() {
        if (fileName_gl.empty()) {
            WARN("No GLOBIOM LC data!!!!");
            return;
        }

        INFO("> Reading the GLOBIOM LC data...");
        ifstream fp = checkFile(fileName_gl);
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

        globiomAfforMaxCountryScenarios[s_bauScenario].reserve(250);
        globiomLandCountryScenarios[s_bauScenario].reserve(250);

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
                        ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
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
                        uint8_t id = countryGLOBIOMId[s_row[0]];
                        scenario_name = s_row[1] + '_' + s_row[2] + '_' + s_row[3] |
                                        rv::transform(::toupper) | ranges::to<string>();
                        double gl_tmp = 0;
                        for (const auto &[type, cell]: rv::zip(header, d_row)) {
                            if (type == "NATURAL") {
                                globiomAfforMaxCountryScenarios[scenario_name][id].data[year] = cell;
                            } else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                gl_tmp += cell;
                        }
                        globiomLandCountryScenarios[scenario_name][id].data[year] = gl_tmp;
                    } else
                        ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
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
                        s_row[3] + '_' + s_row[4] + '_' + s_row[5] | rv::transform(::toupper) | ranges::to<string>();

                if (countryGLOBIOMId.contains(s_row[0])) {
                    CO2PriceScenarios[scenario_name][countryGLOBIOMId[s_row[0]]] = {year_columns, d_row};
                    CO2PriceScenarios[scenario_name][countryGLOBIOMId[s_row[0]]].data[refYear] = d_row[idx_ge_refYear];
                } else
                    ERROR("!!! No such country: {}, line: {}", s_row[0], line_num + 1);
            }
        }
        INFO("Successfully read {} lines.", line_num);
    }

    void readNUTS2() {
        INFO("> Reading the NUTS2...");
        ifstream fp = checkFile(fileName_nuts2);
        string line;
        getline(fp, line);

        uint32_t line_num = 1;
        for (vector<string> s_row; !fp.eof(); ++line_num) {
            getline(fp, line);
            if (!line.empty() && line[0] != '#') {
                s_row = line | rv::split(',') | ranges::to<vector<string> >();
                uint32_t x = lround((stod(s_row[0]) + 180) / gridStep - 0.5);
                uint32_t y = lround((stod(s_row[1]) + 90) / gridStep - 0.5);
                nuts2id[{x, y}] = s_row[2];
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

    void readMAIClimate() {
        if (fileName_maic.empty()) {
            WARN("No MAI climate data!!!!");
            return;
        }

        INFO("> Reading the MAI climate data...");
        ifstream fp = checkFile(fileName_maic);
        string line;
        getline(fp, line);

        // ...1,ClimaScen,ANYRCP,lon,lat,Year,G4MOutput,value
        maiClimateShiftersScenarios.reserve(96);  // 2'373'408 / 24'723 = 96

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
                            maiClimateShiftersScenarios[scenario_name][g4mId->second].data[year] = value;
                        } else
                            DEBUG("Plots don't contain (x, y) = ({}, {}), line {}", x, y, line_num + 1);
                    }
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

    void readDisturbances() {
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
                            disturbWind[g4mId->second].data[year] = value;
                        else if (s_row[4] == "fire")
                            disturbFire[g4mId->second].data[year] = value;
                        else if (s_row[4] == "biotic")
                            disturbBiotic[g4mId->second].data[year] = value;
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
                            disturbWindExtreme[g4mId->second].data[year] = value;
                        else if (s_row[4] == "fire")
                            disturbFireExtreme[g4mId->second].data[year] = value;
                        else if (s_row[4] == "biotic")
                            disturbBioticExtreme[g4mId->second].data[year] = value;
                        else
                            DEBUG("Unknown agent type: {}, line: {}", s_row[4], line_num + 1);
                    } else
                        DEBUG("Plots don't contain (x, y) = ({}, {}), line {}", x, y, line_num);
                }
            }
        }

        INFO("Successfully read {} lines.", line_num);
    }

    unordered_map<uint8_t, vector<double> > readAgeStructData() {
        INFO("> Reading the age struct data...");
        ifstream fp = checkFile(fileName_ageStruct);

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
}

#endif
