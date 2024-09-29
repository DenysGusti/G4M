#ifndef G4M_EUROPE_DG_SIMU_ID_SCENARIOS_HPP
#define G4M_EUROPE_DG_SIMU_ID_SCENARIOS_HPP

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
    using simuIdType = unordered_map<uint32_t, Ipol<double> >;
    using simuIdScenariosType = unordered_map<string, simuIdType, StringHash, equal_to<> >;

    class SimuIdScenarios {
    public:
        // static functions

        static void scaleDisturbance(simuIdType &disturbance, const uint16_t scaleYear) {
            for (auto &[id, ipol]: disturbance)
                ipol += -ipol.data.at(scaleYear);
        }

        static void printSimuId(const simuIdType &simuIdDatamap, const string_view message) noexcept {
            TRACE("{}", message);
            for (const auto &[id, ipol]: simuIdDatamap)
                TRACE("{}\n{}", id, ipol.str());
        }

        static void
        printSimuIdScenarios(const simuIdScenariosType &simuIdScenarios, const string_view message) noexcept {
            TRACE("{}", message);
            for (const auto &[scenario, simuIdDatamap]: simuIdScenarios) {
                TRACE("{}", scenario);
                for (const auto &[simu_id, ipol]: simuIdDatamap)
                    TRACE("{}\n{}", simu_id, ipol.str());
            }
        }

        simuIdScenariosType GLOBIOM_AfforMaxScenarios;         // Maximum allowed afforestation estimated from GLOBIOM natural land
        simuIdScenariosType GLOBIOM_LandScenarios;      // Country GLOBIOM land (kha) reserved for croplands, pastures and short rotation plantations (also wetlands and infrastructure)

        simuIdScenariosType maiClimateShiftersScenarios;  // MAI climate shifters

        simuIdType disturbWind;     // wood damage due to windfalls, m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023
        simuIdType disturbFire;     // wood damage due to biotic agents (mostly bark beetle), m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023
        simuIdType disturbBiotic;   // wood damage due to fire, m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023

        simuIdType disturbWindExtreme;      // wood damage due to windfalls, m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023
        simuIdType disturbBioticExtreme;    // wood damage due to biotic agents (mostly bark beetle), m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023
        simuIdType disturbFireExtreme;      // wood damage due to fire, m3/(ha year); calculated by Andrey Lessa for the LULUCF2040 T2, 28.06.2023

        // plotsXY_SimuID: <[x, y], simuID> for quick plots search
        void readMAIClimate(const map<pair<uint32_t, uint32_t>, uint32_t> &plotsXY_SimuID) {
            if (!settings.fileNames.contains("maic") || settings.fileNames.at("maic").empty()) {
                WARN("No MAI climate data!!!!");
                return;
            }

            INFO("> Reading the MAI climate data...");
            ifstream fp = checkFile(settings.fileNames.at("maic"));
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
                                scenario_name =
                                        s_row[2] + '_' + s_row[1] | rv::transform(::toupper) | ranges::to<string>();
                                double value = stod(s_row[7]);
                                maiClimateShiftersScenarios[scenario_name][g4mId->second].data[year] = value;
                            } else
                                DEBUG("Plots don't contain (x, y) = ({}, {}), line {}", x, y, line_num + 1);
                        }
                }
            }
            INFO("Successfully read {} lines.", line_num);
        }

        // plotsSimuID for quick plots search
        void readGlobiomLandCalibrate(const unordered_set<uint32_t> &plotsSimuID) {
            if (!settings.fileNames.contains("gl_0") || settings.fileNames.at("gl_0").empty()) {
                WARN("No GLOBIOM LC data for 2000-2020!!!!");
                return;
            }

            INFO("> Reading the GLOBIOM LC data for 2000-2020...");
            ifstream fp = checkFile(settings.fileNames.at("gl_0"));
            string line;
            getline(fp, line);

            size_t first_data_column = 5;
            auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                          ranges::to<vector<string> >();

            GLOBIOM_AfforMaxScenarios[settings.bauScenario].reserve(3'100);
            GLOBIOM_LandScenarios[settings.bauScenario].reserve(3'100);

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
                                    GLOBIOM_AfforMaxScenarios[settings.bauScenario][simuId].data[year] = cell;
                                else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                    gl_tmp += cell;
                            }
                            GLOBIOM_LandScenarios[settings.bauScenario][simuId].data[year] = gl_tmp;
                        }
                    } else {
//                        DEBUG("Plots don't contain simuId = {}", simuId);
                    }
                }
            }
            INFO("Successfully read {} lines.", line_num);
        }

        // plotsSimuID for quick plots search
        void readGlobiomLand(const unordered_set<uint32_t> &plotsSimuID) {
            if (!settings.fileNames.contains("gl") || settings.fileNames.at("gl").empty()) {
                WARN("No GLOBIOM LC data!!!!");
                return;
            }

            INFO("> Reading the GLOBIOM LC data...");
            ifstream fp = checkFile(settings.fileNames.at("gl"));
            string line;
            getline(fp, line);

            const size_t first_data_column = 5;
            auto header = line | rv::transform(::toupper) | rv::split(',') | rv::drop(first_data_column) |
                          ranges::to<vector<string> >();

            GLOBIOM_AfforMaxScenarios.reserve(27'500);
            GLOBIOM_LandScenarios.reserve(27'500);

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
                                    GLOBIOM_AfforMaxScenarios[scenario_name][simuId].data[year] = cell;
                                } else if (type == "ARABLE" || type == "WETLAND" || type == "BLOCKED")
                                    gl_tmp += cell;
                            }
                            GLOBIOM_LandScenarios[scenario_name][simuId].data[year] = gl_tmp;
                        }
                    } else
                        DEBUG("Plots don't contain simuId = {}", simuId);
                }
            }
            INFO("Successfully read {} lines.", line_num);
        }

        // plotsXY_SimuID: <[x, y], simuID> for quick plots search
        void readDisturbances(const map<pair<uint32_t, uint32_t>, uint32_t> &plotsXY_SimuID) {
            if (!settings.fileNames.contains("disturbance") || settings.fileNames.at("disturbance").empty()) {
                WARN("No disturbance projection data!!!!");
                return;
            }

            INFO("> Reading the disturbance data...");
            ifstream fp = checkFile(settings.fileNames.at("disturbance"));
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

        // plotsXY_SimuID: <[x, y], simuID> for quick plots search
        void readDisturbancesExtreme(const map<pair<uint32_t, uint32_t>, uint32_t> &plotsXY_SimuID) {
            if (!settings.fileNames.contains("disturbanceExtreme") ||
                settings.fileNames.at("disturbanceExtreme").empty()) {
                WARN("No extreme disturbance projection data!!!!");
                return;
            }

            INFO("> Reading the extreme disturbance data ...");
            ifstream fp = checkFile(settings.fileNames.at("disturbanceExtreme"));
            string line;
            getline(fp, line);

            // "","lon","lat","Year","Agent","value"
            disturbWindExtreme.reserve(25'000);
            disturbFireExtreme.reserve(25'000);
            disturbBioticExtreme.reserve(25'000);

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

        // Scaling the MAI climate shifters to the 2020 value (i.e., MAIShifter_year = MAIShifter_year/MAIShifter_2000, so the 2000 value = 1);
        void scaleMAIClimate2020() {
            INFO("Scaling MAI climate shifters to the 2020 value!");
            for (auto &[scenario, MAI]: maiClimateShiftersScenarios)
                for (auto &[simu_id, ipol]: MAI) {
                    double reciprocal_value_2020 = 1 / ipol.data.at(2020);
                    ipol *= reciprocal_value_2020;
                }
        }

        void add2020Disturbances() {
            for (auto &[id, ipol]: disturbWind)
                ipol.data[2020] = ipol.data.at(2030) / 1.025;
            for (auto &[id, ipol]: disturbFire)
                ipol.data[2020] = ipol.data.at(2030) / 1.05;
            for (auto &[id, ipol]: disturbBiotic)
                ipol.data[2020] = ipol.data.at(2030) / 1.05;
        }

        void scaleDisturbances2020() {
            const uint16_t scaleYear = 2020;
            scaleDisturbance(disturbWind, scaleYear);
            scaleDisturbance(disturbFire, scaleYear);
            scaleDisturbance(disturbBiotic, scaleYear);

            INFO("Disturbances are scaled to the {} value!", scaleYear);
        }

        void printData() const noexcept {
            printSimuIdScenarios(GLOBIOM_LandScenarios, "GLOBIOM Land Scenarios");
            printSimuIdScenarios(GLOBIOM_AfforMaxScenarios, "GLOBIOM Affor Max Scenarios");
            printSimuIdScenarios(maiClimateShiftersScenarios, "MAI Climate Shifters Scenarios");

            printSimuId(disturbWind, "disturb Wind");
            printSimuId(disturbFire, "disturb Fire");
            printSimuId(disturbBiotic, "disturb Biotic");
            printSimuId(disturbWindExtreme, "disturb Wind Extreme");
            printSimuId(disturbFireExtreme, "disturb Fire Extreme");
            printSimuId(disturbBioticExtreme, "disturb Biotic Extreme");
        }
    };
}

#endif
