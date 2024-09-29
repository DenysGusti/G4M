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
    class Settings {
    public:
        string coefPath;
        string inputPath;
        string outputPath;

        unordered_set<string, StringHash, equal_to<> > parametersTable;
        unordered_set<string, StringHash, equal_to<> > parametersTableReg;
        unordered_set<string, StringHash, equal_to<> > parametersMap;

        bool produceTabs{};
        bool produceMaps{};

        array<bool, 3> tabs{};
        array<bool, 3> maps{};

        unordered_map<string, string, StringHash, equal_to<> > fileNames;

        string bauScenario; // is used as a key in dictionaries
        vector<array<string, 4> > scenarios;

        void readSettings() {
            readPaths();
            readParameters();
            readFileNames();
            readScenarios();
        }

    private:
        filesystem::path ini_folder = "ini";

        void readPaths() {
            const filesystem::path file_path = ini_folder / "paths.ini";
            ifstream file{file_path};
            if (!file.is_open()) {
                FATAL("Cannot read {} in directory {}", file_path.string(), fs::current_path().string());
                throw runtime_error{"Cannot read paths file"};
            }

            string line;

            for (int line_num = 0; file;) {
                getline(file, line);

                if (!line.empty() && line.back() == '\r')   // remove carriage return for linux
                    line.pop_back();

                if (line.empty() || line.front() == '#')
                    continue;

                switch (line_num) {
                    case 0:
                        coefPath = line;
                        break;
                    case 1:
                        inputPath = line;
                        break;
                    case 2:
                        outputPath = line;
                        break;
                    default:
                        WARN("Unexpected line of paths file");
                }
                ++line_num;
            }
        }

        void readParameters() {
            const filesystem::path file_path = ini_folder / "parameters.ini";
            ifstream file{file_path};
            if (!file.is_open()) {
                FATAL("Cannot read {} in directory {}", file_path.string(), fs::current_path().string());
                throw runtime_error{"Cannot read parameters file"};
            }

            string line;

            auto fillSet = [&](unordered_set<string, StringHash, equal_to<> > &s_set) -> void {
                s_set.reserve(200);
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

                if (!line.empty() && line.back() == '\r')   // remove carriage return for linux
                    line.pop_back();

                if (line.empty() || line.front() == '#')
                    continue;

                switch (line_num) {
                    case 0:
                        fillSet(parametersMap);
                        break;
                    case 1:
                        fillSet(parametersTable);
                        break;
                    case 2:
                        fillSet(parametersTableReg);
                        break;
                    case 3:
                        produceTabs = (line == "1");
                        TRACE("produceTabs = {}", produceTabs);
                        break;
                    case 4:
                        tabs[0] = (line == "1");
                        TRACE("Tabs[0] = {}", tabs[0]);
                        break;
                    case 5:
                        tabs[1] = (line == "1");
                        TRACE("Tabs[1] = {}", tabs[1]);
                        break;
                    case 6:
                        tabs[2] = (line == "1");
                        TRACE("Tabs[2] = {}", tabs[2]);
                        break;
                    case 7:
                        produceMaps = (line == "1");
                        TRACE("produceMaps = {}", produceMaps);
                        break;
                    case 8:
                        maps[0] = (line == "1");
                        TRACE("Maps[0] = {}", maps[0]);
                        break;
                    case 9:
                        maps[1] = (line == "1");
                        TRACE("Maps[1] = {}", maps[1]);
                        break;
                    case 10:
                        maps[2] = (line == "1");
                        TRACE("Maps[2] = {}", maps[2]);
                        break;
                    default:
                        WARN("Unexpected line of parameters file");
                }
                ++line_num;
            }
        }

        void readFileNames() {
            const filesystem::path file_path = ini_folder / "file_names.ini";
            ifstream file{file_path};
            if (!file.is_open()) {
                FATAL("Cannot read {} in directory {}", file_path.string(), fs::current_path().string());
                throw runtime_error{"Cannot read file names file"};
            }

            fileNames.reserve(32);

            string line, file_key;

            for (int line_num = 0; file;) {
                getline(file, line);

                if (!line.empty() && line.back() == '\r')   // remove carriage return for linux
                    line.pop_back();

                if (line.empty() || line.front() == '#')
                    continue;

                switch (line_num % 2) {
                    case 0:
                        file_key = line;
                        break;
                    case 1:
                        fileNames.emplace(file_key, line);
                        break;
                }
                ++line_num;
            }
        }

        void readScenarios() {
            const filesystem::path file_path = ini_folder / "scenarios.ini";
            ifstream file{file_path};
            if (!file.is_open()) {
                FATAL("Cannot read {} in directory {}", file_path.string(), fs::current_path().string());
                throw runtime_error{"Cannot read scenarios file"};
            }

            string line;

            for (int line_num = 0; file;) {
                getline(file, line);

                if (!line.empty() && line.back() == '\r')   // remove carriage return for linux
                    line.pop_back();

                if (line.empty() || line.front() == '#')
                    continue;

                if (line_num == 0)
                    bauScenario = line;
                else {
                    auto tokens = line | rv::transform(::toupper) | rv::split(' ') | ranges::to<vector<string> >();
                    if (tokens.size() == 4) {
                        scenarios.emplace_back();
                        ranges::copy(tokens, scenarios.back().begin());
                    } else
                        ERROR("Scenarios file contains wrong number of arguments!");
                }
                ++line_num;
            }
        }
    };
}

#endif
