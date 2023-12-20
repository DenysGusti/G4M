#ifndef G4M_EUROPE_DG_CHECK_FILE_HPP
#define G4M_EUROPE_DG_CHECK_FILE_HPP

#include <string>
#include <fstream>
#include <filesystem>

#include "../start_data/model_settings.hpp"
#include "../log.hpp"

using namespace std;
using namespace g4m::StartData;

namespace fs = filesystem;

namespace g4m::helper {
    [[nodiscard]] ifstream checkFile(const string_view fileName, const bool binary = false) {

        auto filePath = fs::path{settings.inputPath} / fileName;
        ifstream fp;
        binary ? fp.open(filePath, ios::binary) : fp.open(filePath);

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
}

#endif
