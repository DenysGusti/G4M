#ifndef G4M_EUROPE_DG_READING_HPP
#define G4M_EUROPE_DG_READING_HPP

#include "../settings/constants.hpp"
#include "../settings/arrays/arrays.hpp"
#include "../start_data/start_data.hpp"
#include "../settings/dicts/dicts.hpp"
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
                             rv::transform([&](const auto &cell) {  // sub-range
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
