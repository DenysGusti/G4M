#ifndef G4M_EUROPE_DG_SETTINGS_HPP
#define G4M_EUROPE_DG_SETTINGS_HPP

#include <string>
#include <cstring>
#include <array>
#include <set>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <ranges>

#include "../log.hpp"

using namespace std;

namespace g4m::init {

    struct Settings {
        string coeffPath;
        string inputPath;
        string outputPath;
        set<string, less<> > parametersTable;
        set<string, less<> > parametersTableReg;
        set<string, less<> > parametersMap;
        bool produceTabs{};
        bool produceMaps{};
        array<bool, 3> tabs{};
        array<bool, 3> maps{};

        Settings() = default;

        explicit Settings(const string_view file_name) {
            readSettings(file_name);
        }

        void readSettings(const string_view file_name) {
            ifstream file{filesystem::path{file_name}};
            if (!file.is_open()) {
                FATAL("Cannot read {}", file_name);
                throw runtime_error{"Cannot read setting file"};
            }

            string line;

            auto fillSet = [&](set<string, less<> > &s_set) -> void {
                stringstream ss{line};
                int num{};
                DEBUG("numSet = {}", num);
                ss >> num;
                for (int i = 0; i < num;) {
                    getline(file, line);
                    if (line.empty() || line[0] == '#')
                        continue;

                    ranges::transform(line, line.begin(), ::tolower);
                    s_set.insert(line);
                    ++i;
                }
            };

            for (int line_num = 0; file;) {
                getline(file, line);
                if (line.empty() || line[0] == '#')
                    continue;

                switch (line_num) {
                    case 0:
                        coeffPath = line;
                        break;
                    case 1:
                        inputPath = line;
                        break;
                    case 2:
                        outputPath = line;
                        break;
                    case 3:
                        fillSet(parametersMap);
                        break;
                    case 4:
                        fillSet(parametersTable);
                        break;
                    case 5:
                        fillSet(parametersTableReg);
                        break;
                    case 6:
                        produceTabs = (line == "1");
                        DEBUG("produceTabs = {}", produceTabs);
                        break;
                    case 7:
                        tabs[0] = (line == "1");
                        DEBUG("Tabs[0] = {}", tabs[0]);
                        break;
                    case 8:
                        tabs[1] = (line == "1");
                        DEBUG("Tabs[1] = {}", tabs[1]);
                        break;
                    case 9:
                        tabs[2] = (line == "1");
                        DEBUG("Tabs[2] = {}", tabs[2]);
                        break;
                    case 10:
                        produceMaps = (line == "1");
                        DEBUG("produceMaps = {}", produceMaps);
                        break;
                    case 11:
                        maps[0] = (line == "1");
                        DEBUG("Maps[0] = {}", maps[0]);
                        break;
                    case 12:
                        maps[1] = (line == "1");
                        DEBUG("Maps[1] = {}", maps[1]);
                        break;
                    case 13:
                        maps[2] = (line == "1");
                        DEBUG("Maps[2] = {}", maps[2]);
                        break;
                    default:
                        TRACE("Unexpected line of settings file");
                }
                ++line_num;
            }
        }
    };
}

#endif
