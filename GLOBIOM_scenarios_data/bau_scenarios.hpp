#ifndef G4M_EUROPE_DG_BAU_SCENARIOS_CONTROLLER_HPP
#define G4M_EUROPE_DG_BAU_SCENARIOS_CONTROLLER_HPP

#include <unordered_map>
#include <vector>

#include "../settings/constants.hpp"
#include "../settings/dicts/dicts.hpp"
#include "../helper/check_file.hpp"
#include "../log.hpp"

using namespace std;
using namespace g4m::Constants;
using namespace g4m::Dicts;

namespace fs = filesystem;
namespace rv = ranges::views;

namespace g4m::GLOBIOM_scenarios_data {
    class BauScenarios {
    public:
        [[nodiscard]] static vector<vector<double> > readBau(const string_view file_path, const string_view message) {
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

        [[nodiscard]] static fs::path locateBauFile(const string_view bauName) {
            for (const auto &dir_entry: fs::directory_iterator{settings.inputPath}) {
                auto file_path = dir_entry.path();
                if (file_path.string().contains(bauName))
                    return file_path;
            }

            FATAL("file with bauName = {} is not found in {}", bauName, settings.inputPath);
            throw runtime_error{"Missing bau file"};
        }

        unordered_map<string, vector<vector<double> >, StringHash, equal_to<> > biomassBauScenarios;
        unordered_map<string, vector<vector<double> >, StringHash, equal_to<> > NPVBauScenarios;

        void readBiomassBau() {
            string bauName = "biomass_bau" + suffix;
            const auto file_path = locateBauFile(bauName);
            string scenario =
                    file_path.stem().string().substr(bauName.size()) | rv::transform(::toupper) | ranges::to<string>();
            const auto bau_vec = readBau(file_path.filename().string(), "biomass_bau");
            biomassBauScenarios[scenario] = bau_vec;
        }

        void readNPVBau() {
            string bauName = "NPVbau" + suffix;
            const auto file_path = locateBauFile(bauName);
            string scenario =
                    file_path.stem().string().substr(bauName.size()) | rv::transform(::toupper) | ranges::to<string>();
            const auto bau_vec = readBau(file_path.filename().string(), "NPVbau");
            biomassBauScenarios[scenario] = bau_vec;
        }
    };
}

#endif
