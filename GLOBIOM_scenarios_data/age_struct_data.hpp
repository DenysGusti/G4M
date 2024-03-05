#ifndef G4M_EUROPE_DG_AGE_STRUCT_DATA_CONTROLLER_HPP
#define G4M_EUROPE_DG_AGE_STRUCT_DATA_CONTROLLER_HPP

#include <unordered_map>
#include <vector>

#include "../settings/dicts/dicts.hpp"
#include "../helper/check_file.hpp"
#include "../log.hpp"

using namespace std;
using namespace g4m::Dicts;

namespace g4m::GLOBIOM_scenarios_data {
    class AgeStructData {
    public:
        unordered_map<uint8_t, vector<double> > ageStructData;

        void readAgeStructData() {
            INFO("> Reading the age struct data...");
            ifstream fp = checkFile(fileNames.at("ageStruct"));

            string line;
            getline(fp, line);

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
        }

        void normalizeAgeStructData() {
            for (auto &[country, ageShares]: ageStructData) {
                double sum = ranges::fold_left(ageShares, 0., plus{});
                if (sum <= 0)
                    ageShares.assign(ageShares.size(), 1 / static_cast<double>(ageShares.size()));
                else {
                    double norm_coef = 1 / sum;
                    for (auto &el: ageShares)
                        el *= norm_coef;
                }
            }
        }
    };
}

#endif
