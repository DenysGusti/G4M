#ifndef G4M_EUROPE_DG_NUTS2_CONTROLLER_HPP
#define G4M_EUROPE_DG_NUTS2_CONTROLLER_HPP

#include <map>

#include "../settings/constants.hpp"
#include "../settings/arrays/arrays.hpp"
#include "../settings/dicts/dicts.hpp"
#include "../init/data_struct.hpp"
#include "../helper/check_file.hpp"
#include "../log.hpp"
#include "../init/data_grid.hpp"

using namespace std;
using namespace g4m::init;
using namespace g4m::Constants;
using namespace g4m::Arrays;
using namespace g4m::Dicts;

namespace g4m::GLOBIOM_scenarios_data {
    class NUTS2 {
    public:
        map<pair<uint32_t, uint32_t>, string> nuts2id; // x, y, nuts2 (pair has no build hash, O(log(n)) look-up)
        DataGrid<string> nuts2grid;

        void readNUTS2() {
            INFO("> Reading the NUTS2...");
            ifstream fp = checkFile(fileNames.at("nuts2"));
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

        void fillFromNUTS() {
            for (const auto &[coords, NUTS2]: nuts2id) {
                const auto [x, y] = coords;
                nuts2grid(x, y) = NUTS2;
            }
        }

        void correctNUTS2Data(const span<const DataStruct> plots) noexcept {
            for (const auto &plot: plots)
                // Test only some regions and some countries
                // !plot.protect
            {
                // locate the struct with asID == asID within the country
                string_view countryISO = countryOrderISO2[countryCodeOrder[plot.country - 1]];
                if (countryISO == "GB")
                    countryISO = "UK";

                const uint32_t MAX_RADIUS = 3;  // 3 for remote islands

                if (auto it_nuts2 = nuts2id.find({plot.x, plot.y}); it_nuts2 != nuts2id.end()) {
                    auto &[coords, NUTS2] = *it_nuts2;

                    if (NUTS2.substr(0, 2) != countryISO)
                        for (uint32_t radius = 1; radius <= MAX_RADIUS; ++radius) {
                            optional<string> opt_neighbour = findNeighbour(countryISO, plot.x, plot.y, radius);
                            if (opt_neighbour) {
                                NUTS2 = *opt_neighbour;
                                DEBUG("x = {}, y = {}, NUTS2 = {}, countryISO = {}, *opt_neighbour = {}, radius = {}",
                                      plot.x, plot.y, NUTS2, countryISO, *opt_neighbour, radius);
                                break;
                            }

                            if (radius == MAX_RADIUS)
                                ERROR("!No x = {}, y = {}, NUTS2 = {}, countryISO = {}",
                                      plot.x, plot.y, NUTS2, countryISO);
                        }

                } else
                    for (uint32_t radius = 1; radius <= MAX_RADIUS; ++radius) {
                        optional<string> opt_neighbour = findNeighbour(countryISO, plot.x, plot.y, radius);
                        if (opt_neighbour) {
                            nuts2id[{plot.x, plot.y}] = *opt_neighbour;
                            DEBUG("x = {}, y = {}, countryISO = {}, *opt_neighbour = {}, radius = {}",
                                  plot.x, plot.y, countryISO, *opt_neighbour, radius);
                            break;
                        }

                        if (radius == MAX_RADIUS)
                            ERROR("!No x = {}, y = {}, countryISO = {}", plot.x, plot.y, countryISO);
                    }
            }
        }

        void printNuts2Id() noexcept {
            for (const auto &[coords, NUTS2]: nuts2id) {
                const auto [x, y] = coords;
                TRACE("x = {}, y = {}, NUTS2 = {}", x, y, NUTS2);
            }
        }

    private:
        [[nodiscard]] optional<string>
        findNeighbour(const string_view countryISO, const uint32_t x, const uint32_t y, const uint32_t radius) {
            nuts2grid.setNeighNum(radius, radius);
            auto neighbours = nuts2grid.getNeighValues(x, y);
            auto it_nearbyCountry = ranges::find_if(neighbours, [countryISO](const string_view el) {
                return el.substr(0, 2) == countryISO;
            });
            if (it_nearbyCountry == neighbours.end())
                return {};
            return *it_nearbyCountry;
        };
    };
}

#endif
