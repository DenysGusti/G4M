#ifndef G4M_EUROPE_DG_PLOTS_CONTROLLER_HPP
#define G4M_EUROPE_DG_PLOTS_CONTROLLER_HPP

#include <unordered_set>

#include "../settings/arrays/arrays.hpp"
#include "../settings/dicts/dicts.hpp"
#include "../init/data_struct.hpp"
#include "../helper/check_file.hpp"
#include "../log.hpp"

using namespace std;
using namespace g4m::init;
using namespace g4m::Dicts;
using namespace g4m::Arrays;

namespace g4m::GLOBIOM_scenarios_data {
    class Plots {
    public:
        vector<DataStruct> rawPlots;  // raw structure with data plots[<elNum>].<variable>[year]
        vector<DataStruct> filteredPlots;  // structure with data plots[<elNum>].<variable>[year]

        // for quick plots search
        unordered_set<uint32_t> plotsSimuID;
        map<pair<uint32_t, uint32_t>, uint32_t> plotsXY_SimuID;  // <[x, y], simuID>

        void readPlots() {
            INFO("> Reading the rest of input data...");
            ifstream fp = checkFile(settings.fileNames.at("dat"));
            string line;
            getline(fp, line);

            auto get_HeaderName_YearFromHeaderColumn = [](const string &s) -> pair<string, optional<uint16_t> > {
                size_t num_pos = s.find_first_of("012345789");
                if (num_pos == string::npos)
                    return {s, nullopt};
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
                                 rv::transform([&](const auto &cell) {  // sub-range
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

        void filterPlots() {
            filteredPlots.reserve(rawPlots.size());

            for (const auto &plot: rawPlots)
                if (regions.contains(plot.polesReg) && countriesList.contains(plot.country))
                    filteredPlots.push_back(plot);
        }

        void initPlotsSimuID() {
            plotsSimuID.reserve(rawPlots.size());

            for (const auto &plot: rawPlots)
                plotsSimuID.insert(plot.simuID);
        }

        void initPlotsXY_SimuID() {
            for (const auto &plot: rawPlots)
                plotsXY_SimuID[{plot.x, plot.y}] = plot.simuID;
        }

        void setAsIds() {
            for (auto &&[i, plot]: filteredPlots | rv::enumerate)
                plot.asID = i;
        }

        void correctMAI() {
            for (auto &plot: filteredPlots)
                // Test only some regions and some countries
                if (!plot.protect) {  // if there is no lerp, why not simple map then?
                    // forest with specified age structure
                    plot.MAIE.data[2000] *= maiCoefficients[plot.country];
                    plot.MAIN.data[2000] *= maiCoefficients[plot.country];
                }
        }

        [[nodiscard]] array<double, numberOfCountries> calculateAverageMAI() {
            INFO("calculating average MAI");
            array<double, numberOfCountries> fun_MAI_CountryUProtect{};
            array<double, numberOfCountries> forestAreaCountry{};
            for (const auto &plot: filteredPlots)
                if (!plot.protect) {
                    double forestArea0 = plot.landArea * 100 * clamp(plot.forest, 0., 1.);
                    if (forestArea0 > 0) {
                        // Max mean annual increment (tC/ha) of Existing forest (with uniform age structure and managed with rotation length maximizing MAI)
                        fun_MAI_CountryUProtect[plot.country - 1] += plot.MAIE.data.at(2000) * forestArea0;
                        forestAreaCountry[plot.country - 1] += forestArea0;
                    }
                }

            for (auto &&[MAI, area]: rv::zip(fun_MAI_CountryUProtect, forestAreaCountry))
                if (area > 0)
                    MAI /= area;

            for (size_t i = 0; i < fun_MAI_CountryUProtect.size(); ++i)
                if (fun_MAI_CountryUProtect[i] > 0)
                    DEBUG("fun_MAI_CountryUProtect[{}] = {}", i, fun_MAI_CountryUProtect[i]);

            INFO("calculated average MAI");
            return fun_MAI_CountryUProtect;
        }

        void printPlots() {
            TRACE("Plots:");
            for (size_t i = 0; const auto &plot: filteredPlots)
                TRACE("plots[{}]:\n{}", i++, plot.str());
        }
    };
}

#endif
