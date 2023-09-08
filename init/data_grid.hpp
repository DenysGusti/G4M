#ifndef G4M_EUROPE_DG_DATA_GRID_HPP
#define G4M_EUROPE_DG_DATA_GRID_HPP

#include <string>
#include <format>
#include <vector>
#include <numeric>
#include <iostream>

#include "point.hpp"

#include "../log.hpp"
#include "../diagnostics/heterogeneous_lookup.hpp"

using namespace std;

namespace g4m::init {

    template<class T>
    class DataGrid {
    public:
        constexpr static int NODATA_VALUE = -9999;
        constexpr static string_view NODATA_VALUE_STR = "-9999";
        constexpr static char8_t NODATA_VALUE_COUNTRIES = 0;

        explicit DataGrid(const size_t VR = 360, const T &value = {}, const size_t years = 2)
                : DataGrid(VR, value, years, {0, 0}, {VR * 2, VR}) {}

        // topLeft (x, y) = (0, 0)  *____________
        //                          |     Y     |
        //                          |           |
        //                          |           |
        //                          |X          |
        //                          |           |
        //                          |           |
        //                          |___________* bottomRight (x, y) = (720, 360)
        DataGrid(const size_t VR, const T &value, const size_t years, const Point &topLeft_, const Point &bottomRight_)
                : horRes{VR * 2}, verRes{VR}, topLeft{topLeft_}, bottomRight{bottomRight_} {
            initGuard();
            grid.assign(years, vector<vector<T> >
                    (bottomRight.x - topLeft.x, vector<T>(bottomRight.y - topLeft.y, value)));
            gridCountries.assign(bottomRight.x - topLeft.x, vector<uint8_t>(bottomRight.y - topLeft.y, 0));
        }

        void fillFromNUTS(const map<pair<uint32_t, uint32_t>, string> &NUTS2_dict) requires is_same_v<T, string> {
            for (const auto &[coords, NUTS2]: NUTS2_dict)
                grid[0][coords.first - topLeft.x][coords.second - topLeft.y] = NUTS2;
        }

        //  https://en.cppreference.com/w/cpp/compiler_support/23
        //  wait for P2128R6 in MSVC, update to C++23 and change () to []
        T &operator()(const size_t x, const size_t y, const size_t year = 0) {
            accessGridGuard(x, y, year);
            return grid[year][x - topLeft.x][y - topLeft.y];
        }

        //  https://en.cppreference.com/w/cpp/compiler_support/23
        //  wait for P2128R6 in MSVC, update to C++23 and change () to []
        [[nodiscard]] const T &operator()(const size_t x, const size_t y, const size_t year = 0) const {
            accessGridGuard(x, y, year);
            return grid[year][x - topLeft.x][y - topLeft.y];
        }

        // possibly () after P2128R6
        uint8_t &country(const size_t x, const size_t y) {
            accessCountryGuard(x, y);
            return gridCountries[x - topLeft.x][y - topLeft.y];
        }

        // possibly () after P2128R6
        [[nodiscard]] const uint8_t &country(const size_t x, const size_t y) const {
            accessCountryGuard(x, y);
            return gridCountries[x - topLeft.x][y - topLeft.y];
        }

        // ESRI ASCII Grid
        [[nodiscard]] string str(const size_t year = 0) const noexcept {
            double cell_size = 360. / horRes;
            auto x_left = -static_cast<ptrdiff_t>(horRes - topLeft.x) * cell_size * 0.5;
            auto y_left = -static_cast<ptrdiff_t>(verRes - topLeft.y) * cell_size * 0.5;
            string props = format("NCOLS {}\nNROWS {}\nXLLCORNER {}\nYLLCORNER {}\nCELLSIZE {}\nNODATA_VALUE {}\n",
                                  grid[year].size(), grid[year][0].size(), x_left, y_left, cell_size, NODATA_VALUE);
            string data;
            data.reserve((bottomRight.x - topLeft.x) * (bottomRight.y - topLeft.y) * 10);
            for (ptrdiff_t j = ssize(grid[year][0]) - 1; j >= 0; --j) {
                for (size_t i = 0; i < grid[year].size(); ++i)
                    // https://en.cppreference.com/w/cpp/utility/format/formatter#Standard_format_specification
                    data += format("{} ", grid[year][i][j]);
                data += '\n';
            }
            return props + data;
        }

        // print ESRI ASCII Grid to ostream
        friend ostream &operator<<(ostream &os, const DataGrid<T> &obj) {
            os << obj.str();
            return os;
        }

        void update1YearBackward() noexcept {
            ranges::shift_left(grid, 1);
            grid.back().assign(bottomRight.x - topLeft.x, vector<T>(bottomRight.y - topLeft.y, T{}));
        }

        void update1YearForward() noexcept {
            ranges::shift_right(grid, 1);
            grid.front().assign(bottomRight.x - topLeft.x, vector<T>(bottomRight.y - topLeft.y, T{}));
        }

        void resizeGrid(const size_t VR) noexcept requires is_floating_point_v<T> {
            if (VR == 0)
                return;

            const size_t HR = VR * 2;

            restoreOriginalSizeNum();
            restoreOriginalCountriesSize();

            resizeGridAvg(HR, VR);
            resizeCountriesMax(HR, VR);
            adjustPoints(HR, VR);

            horRes = HR;
            verRes = VR;

            shrinkToRectangleSize();
            shrinkToRectangleCountriesSize();
        }

        void resizeGrid(const size_t VR) noexcept requires is_integral_v<T> {
            if (VR == 0)
                return;

            const size_t HR = VR * 2;

            restoreOriginalSizeNum();
            restoreOriginalCountriesSize();

            resizeGridMax(HR, VR);
            resizeCountriesMax(HR, VR);
            adjustPoints(HR, VR);

            horRes = HR;
            verRes = VR;

            shrinkToRectangleSize();
            shrinkToRectangleCountriesSize();
        }

        void resizeGrid(const size_t VR) noexcept requires is_same_v<T, string> {
            if (VR == 0)
                return;

            const size_t HR = VR * 2;

            restoreOriginalSizeStr();
            restoreOriginalCountriesSize();

            resizeGridMax(HR, VR);
            resizeCountriesMax(HR, VR);
            adjustPoints(HR, VR);

            horRes = HR;
            verRes = VR;

            shrinkToRectangleSize();
            shrinkToRectangleCountriesSize();
        }

        // sets number of neighbour cells to be considered
        void setNeighNum(const size_t hor_neigh, const size_t ver_neigh) noexcept {
            horNeigh = hor_neigh;
            verNeigh = ver_neigh;
        }

        // returns maximum value of all neighbours for the year (current = 0, previous = 1, ...)
        T getMaxNeigh(const size_t x, const size_t y, const size_t year = 0) const requires is_arithmetic_v<T> {
            accessGridGuard(x, y, year);

            T max_value = NODATA_VALUE;

            auto x_left = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(x - topLeft.x) - static_cast<ptrdiff_t>(horNeigh));
            auto x_right = min(x - topLeft.x + horNeigh, grid[year].size() - 1);
            auto y_top = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(y - topLeft.y) - static_cast<ptrdiff_t>(verNeigh));
            auto y_bottom = min(y - topLeft.y + verNeigh, grid[year][x - topLeft.x].size() - 1);

            for (size_t i = x_left; i <= x_right; ++i)
                for (size_t j = y_top; j <= y_bottom; ++j)
                    if ((i != x - topLeft.x || j != y - topLeft.y) &&
                        grid[year][i][j] != NODATA_VALUE)  // not this cell
                        max_value = max(max_value, grid[year][i][j]);

            return max_value;
        }

        // returns minimum value of all neighbours for the year (current = 0, previous = 1, ...)
        T getMinNeigh(const size_t x, const size_t y, const size_t year = 0) const requires is_arithmetic_v<T> {
            accessGridGuard(x, y, year);

            T min_value = NODATA_VALUE;

            auto x_left = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(x - topLeft.x) - static_cast<ptrdiff_t>(horNeigh));
            auto x_right = min(x - topLeft.x + horNeigh, grid[year].size() - 1);
            auto y_top = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(y - topLeft.y) - static_cast<ptrdiff_t>(verNeigh));
            auto y_bottom = min(y - topLeft.y + verNeigh, grid[year][x - topLeft.x].size() - 1);

            for (size_t i = x_left; i <= x_right; ++i)
                for (size_t j = y_top; j <= y_bottom; ++j)
                    if ((i != x - topLeft.x || j != y - topLeft.y) &&
                        grid[year][i][j] != NODATA_VALUE)  // not this cell
                        min_value = min(min_value, grid[year][i][j]);

            return min_value;
        }

        // returns maximum value of all neighbours within a country for the year (current = 0, previous = 1, ...)
        T getMaxCountry(const size_t x, const size_t y, const size_t year = 0) const requires is_arithmetic_v<T> {
            accessGridGuard(x, y, year);

            auto country_code = gridCountries[x - topLeft.x][y - topLeft.y];
            T max_value = NODATA_VALUE;

            auto x_left = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(x - topLeft.x) - static_cast<ptrdiff_t>(horNeigh));
            auto x_right = min(x - topLeft.x + horNeigh, grid[year].size() - 1);
            auto y_top = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(y - topLeft.y) - static_cast<ptrdiff_t>(verNeigh));
            auto y_bottom = min(y - topLeft.y + verNeigh, grid[year][x - topLeft.x].size() - 1);

            for (size_t i = x_left; i <= x_right; ++i)
                for (size_t j = y_top; j <= y_bottom; ++j)
                    if ((i != x - topLeft.x || j != y - topLeft.y) && gridCountries[i][j] == country_code &&
                        grid[year][i][j] != NODATA_VALUE)
                        max_value = max(max_value, grid[year][i][j]);

            return max_value;
        }

        // returns minimum value of all neighbours within a country for the year (current = 0, previous = 1, ...)
        T getMinCountry(const size_t x, const size_t y, const size_t year = 0) const requires is_arithmetic_v<T> {
            accessGridGuard(x, y, year);

            auto country_code = gridCountries[x - topLeft.x][y - topLeft.y];
            T min_value = NODATA_VALUE;

            auto x_left = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(x - topLeft.x) - static_cast<ptrdiff_t>(horNeigh));
            auto x_right = min(x - topLeft.x + horNeigh, grid[year].size() - 1);
            auto y_top = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(y - topLeft.y) - static_cast<ptrdiff_t>(verNeigh));
            auto y_bottom = min(y - topLeft.y + verNeigh, grid[year][x - topLeft.x].size() - 1);

            for (size_t i = x_left; i <= x_right; ++i)
                for (size_t j = y_top; j <= y_bottom; ++j)
                    if ((i != x - topLeft.x || j != y - topLeft.y) && gridCountries[i][j] == country_code &&
                        grid[year][i][j] != NODATA_VALUE)
                        min_value = min(min_value, grid[year][i][j]);

            return min_value;
        }

        // returns average value for the year (current = 0, previous = 1, ...)
        T getAvg(const size_t x, const size_t y, const size_t year = 0) const requires is_floating_point_v<T> {
            accessGridGuard(x, y, year);

            T avg = 0;

            auto x_left = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(x - topLeft.x) - static_cast<ptrdiff_t>(horNeigh));
            auto x_right = min(x - topLeft.x + horNeigh, grid[year].size() - 1);
            auto y_top = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(y - topLeft.y) - static_cast<ptrdiff_t>(verNeigh));
            auto y_bottom = min(y - topLeft.y + verNeigh, grid[year][x - topLeft.x].size() - 1);

            bool only_no_data = true;
            for (size_t i = x_left; i <= x_right; ++i)
                for (size_t j = y_top; j <= y_bottom; ++j)
                    if (grid[year][i][j] != NODATA_VALUE) {
                        avg += grid[year][i][j];
                        only_no_data = false;
                    }

            return only_no_data ? NODATA_VALUE : avg / ((x_right - x_left + 1) * (y_bottom - y_top + 1));
        }

        // returns neighbour values for cell [x][y][year] including cell
        vector<T> getNeighValues(const size_t x, const size_t y, const size_t year = 0) const {
            accessGridGuard(x, y, year);

            vector<T> neighValues;

            auto x_left = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(x - topLeft.x) - static_cast<ptrdiff_t>(horNeigh));
            auto x_right = min(x - topLeft.x + horNeigh, grid[year].size() - 1);
            auto y_top = max(ptrdiff_t{0}, static_cast<ptrdiff_t>(y - topLeft.y) - static_cast<ptrdiff_t>(verNeigh));
            auto y_bottom = min(y - topLeft.y + verNeigh, grid[year][x - topLeft.x].size() - 1);

            neighValues.reserve((x_right - x_left + 1) * (y_bottom - y_top + 1));

            for (size_t i = x_left; i <= x_right; ++i)
                for (size_t j = y_top; j <= y_bottom; ++j)
                    neighValues.push_back(grid[year][i][j]);

            return neighValues;
        }

    private:
        size_t horRes = 720;
        size_t verRes = 360;

        Point topLeft;
        Point bottomRight;

        size_t horNeigh = 1;
        size_t verNeigh = 1;

        vector<vector<vector<T> > > grid;  // newer year on the front

        vector<vector<uint8_t> > gridCountries;  // uint8_t (unsigned char) grid that is used for storing country codes

        void initGuard() const {
            if ((bottomRight.y - topLeft.y) * 2 != bottomRight.x - topLeft.x) {
                FATAL("rectangle must be 2:1 - ({} - {}) * 2 != {} - {}",
                      bottomRight.y, topLeft.y, bottomRight.x, topLeft.x);
                throw invalid_argument{"rectangle must be 2:1"};
            }

            if (bottomRight.x > horRes || bottomRight.y > verRes) {
                FATAL("bottom right point must be inside grid: {} > {} || {} > {}",
                      bottomRight.x, horRes, bottomRight.y, verRes);
                throw invalid_argument{"bottom right point must be inside grid"};
            }

            if (topLeft.x > bottomRight.x || topLeft.y > bottomRight.y) {
                FATAL("top left and bottom right point are placed wrong: {} > {} || {} > {}",
                      topLeft.x, bottomRight.x, topLeft.y, bottomRight.y);
                throw invalid_argument{"top left and bottom right point are placed wrong"};
            }
        }

        void accessGridGuard(const size_t x, const size_t y, const size_t year) const {
            if (year >= grid.size() || x - topLeft.x >= grid[year].size() ||
                y - topLeft.y >= grid[year][x - topLeft.x].size()) {
                FATAL("invalid grid index: {} >= {} || {} - {} >= {} || {} - {} >= {}",
                      year, grid.size(), x, topLeft.x, grid[year].size(), y, topLeft.y,
                      grid[year][x - topLeft.x].size());
                throw out_of_range{"invalid grid index"};
            }
        }

        void accessCountryGuard(const size_t x, const size_t y) const {
            if (x - topLeft.x >= gridCountries.size() || y - topLeft.y >= gridCountries[x - topLeft.x].size()) {
                FATAL("invalid gridCountries index: {} - {} >= {} || {} - {} >= {}", x, topLeft.x, gridCountries.size(),
                      y, topLeft.y, gridCountries[x - topLeft.x].size());
                throw out_of_range{"invalid grid index"};
            }
        }

        void adjustPoints(const size_t HR, const size_t VR) noexcept {
            auto d_horRes = static_cast<double>(horRes);
            auto d_verRes = static_cast<double>(verRes);
            auto d_HR = static_cast<double>(HR);
            auto d_VR = static_cast<double>(VR);
            double ratio = d_HR / d_horRes;

            topLeft.x = ceil((topLeft.x - d_horRes * 0.5) * ratio + d_HR * 0.5);
            topLeft.y = ceil((topLeft.y - d_verRes * 0.5) * ratio + d_VR * 0.5);
            bottomRight.x = ceil((bottomRight.x - d_horRes * 0.5) * ratio + d_HR * 0.5);
            bottomRight.y = ceil((bottomRight.y - d_verRes * 0.5) * ratio + d_VR * 0.5);

            DEBUG("{} {}", topLeft.str(), bottomRight.str());
        }

        void restoreOriginalSizeNum() noexcept {
            for (auto &layer: grid) {
                layer.insert(layer.begin(), topLeft.x, vector<T>(layer[0].size(), NODATA_VALUE));
                layer.insert(layer.end(), horRes - bottomRight.x, vector<T>(layer[0].size(), NODATA_VALUE));
            }
            for (auto &layer: grid)
                for (auto &row: layer) {
                    row.insert(row.begin(), topLeft.y, NODATA_VALUE);
                    row.insert(row.end(), verRes - bottomRight.y, NODATA_VALUE);
                }
        }

        void restoreOriginalSizeStr() noexcept {
            auto s_NODATA_VALUE_STR = string{NODATA_VALUE_STR};
            for (auto &layer: grid) {
                layer.insert(layer.begin(), topLeft.x, vector<string>(layer[0].size(), s_NODATA_VALUE_STR));
                layer.insert(layer.end(), horRes - bottomRight.x,
                             vector<string>(layer[0].size(), s_NODATA_VALUE_STR));
            }
            for (auto &layer: grid)
                for (auto &row: layer) {
                    row.insert(row.begin(), topLeft.y, s_NODATA_VALUE_STR);
                    row.insert(row.end(), verRes - bottomRight.y, s_NODATA_VALUE_STR);
                }
        }

        void shrinkToRectangleSize() noexcept {
            for (auto &layer: grid) {
                layer.erase(layer.begin(), next(layer.begin(), topLeft.x));
                layer.erase(prev(layer.end(), horRes - bottomRight.x), layer.end());
            }
            for (auto &layer: grid)
                for (auto &row: layer) {
                    row.erase(row.begin(), next(row.begin(), topLeft.y));
                    row.erase(prev(row.end(), verRes - bottomRight.y), row.end());
                }
        }

        void restoreOriginalCountriesSize() noexcept {
            gridCountries.insert(gridCountries.begin(), topLeft.x,
                                 vector<uint8_t>(gridCountries[0].size(), NODATA_VALUE_COUNTRIES));
            gridCountries.insert(gridCountries.end(), horRes - bottomRight.x,
                                 vector<uint8_t>(gridCountries[0].size(), NODATA_VALUE_COUNTRIES));
            for (auto &row: gridCountries) {
                row.insert(row.begin(), topLeft.y, NODATA_VALUE_COUNTRIES);
                row.insert(row.end(), verRes - bottomRight.y, NODATA_VALUE_COUNTRIES);
            }
        }

        void shrinkToRectangleCountriesSize() noexcept {
            gridCountries.erase(gridCountries.begin(), next(gridCountries.begin(), topLeft.x));
            gridCountries.erase(prev(gridCountries.end(), horRes - bottomRight.x), gridCountries.end());
            for (auto &row: gridCountries) {
                row.erase(row.begin(), next(row.begin(), topLeft.y));
                row.erase(prev(row.end(), verRes - bottomRight.y), row.end());
            }
        }

        void resizeGridAvg(const size_t HR, const size_t VR) noexcept {
            size_t intermediateHR = lcm(horRes, HR);
            size_t intermediateRatio = intermediateHR / horRes;
            size_t intermediateRatioSquared = intermediateRatio * intermediateRatio;
            size_t finalRatio = intermediateHR / HR;

            vector<vector<vector<T> > > intermediateGrid;

            if (intermediateHR <= horRes)
                intermediateGrid = grid;
            else {
                intermediateGrid.assign(grid.size(),
                                        vector<vector<T> >(intermediateHR, vector<T>(intermediateHR / 2)));
                for (size_t year = 0; year < grid.size(); ++year)
                    for (size_t i = 0; i < grid[year].size(); ++i)
                        for (size_t j = 0; j < grid[year][0].size(); ++j)
                            // 1 cell
                            for (size_t a = 0; a < intermediateRatio; ++a)
                                for (size_t b = 0; b < intermediateRatio; ++b)
                                    intermediateGrid[year][i * intermediateRatio + a][j * intermediateRatio + b] =
                                            grid[year][i][j] == NODATA_VALUE ? NODATA_VALUE :
                                            grid[year][i][j] / intermediateRatioSquared;
            }

            if (intermediateHR == HR)
                grid = intermediateGrid;
            else {
                grid.assign(intermediateGrid.size(), vector<vector<T> >(HR, vector<T>(VR)));
                for (size_t year = 0; year < intermediateGrid.size(); ++year)
                    for (size_t i = 0; i < intermediateGrid[year].size(); i += finalRatio)
                        for (size_t j = 0; j < intermediateGrid[year][0].size(); j += finalRatio) {
                            // 1 cell
                            T total = 0;
                            bool only_no_data = true;
                            for (size_t a = 0; a < finalRatio; ++a)
                                for (size_t b = 0; b < finalRatio; ++b)
                                    if (intermediateGrid[year][i + a][j + b] != NODATA_VALUE) {
                                        total += intermediateGrid[year][i + a][j + b];
                                        only_no_data = false;
                                    }
                            grid[year][i / finalRatio][j / finalRatio] = only_no_data ? NODATA_VALUE : total;
                        }
            }
        }

        void resizeGridMax(const size_t HR, const size_t VR) noexcept {
            size_t intermediateHR = lcm(horRes, HR);
            size_t intermediateRatio = intermediateHR / horRes;
            size_t finalRatio = intermediateHR / HR;

            vector<vector<vector<T> > > intermediateGrid;

            if (intermediateHR <= horRes)
                intermediateGrid = grid;
            else {
                intermediateGrid.assign(grid.size(),
                                        vector<vector<T> >(intermediateHR, vector<T>(intermediateHR / 2)));
                for (size_t year = 0; year < grid.size(); ++year)
                    for (size_t i = 0; i < grid[year].size(); ++i)
                        for (size_t j = 0; j < grid[year][0].size(); ++j)
                            // 1 cell
                            for (size_t a = 0; a < intermediateRatio; ++a)
                                for (size_t b = 0; b < intermediateRatio; ++b)
                                    intermediateGrid[year][i * intermediateRatio + a][j * intermediateRatio + b] =
                                            grid[year][i][j];
            }

            if (intermediateHR == HR)
                grid = intermediateGrid;
            else {
                grid.assign(intermediateGrid.size(), vector<vector<T> >(HR, vector<T>(VR)));
                unordered_map<T, uint32_t> countDict;
                for (size_t year = 0; year < intermediateGrid.size(); ++year)
                    for (size_t i = 0; i < intermediateGrid[year].size(); i += finalRatio)
                        for (size_t j = 0; j < intermediateGrid[year][0].size(); j += finalRatio) {
                            // 1 cell
                            for (size_t a = 0; a < finalRatio; ++a)
                                for (size_t b = 0; b < finalRatio; ++b)
                                    ++countDict[intermediateGrid[year][i + a][j + b]];
                            grid[year][i / finalRatio][j / finalRatio] =
                                    ranges::max_element(countDict, [](const auto &lhs, const auto &rhs) {
                                        return lhs.second < rhs.second;
                                    })->first;
                            countDict.clear();
                        }
            }
        }

        void resizeCountriesMax(const size_t HR, const size_t VR) noexcept {
            size_t intermediateHR = lcm(horRes, HR);
            size_t intermediateRatio = intermediateHR / horRes;
            size_t finalRatio = intermediateHR / HR;

            vector<vector<uint8_t >> intermediateCountries;

            if (intermediateHR <= horRes)
                intermediateCountries = gridCountries;
            else {
                intermediateCountries.assign(intermediateHR, vector<uint8_t>(intermediateHR / 2));
                for (size_t i = 0; i < gridCountries.size(); ++i)
                    for (size_t j = 0; j < gridCountries[0].size(); ++j)
                        // 1 cell
                        for (size_t a = 0; a < intermediateRatio; ++a)
                            for (size_t b = 0; b < intermediateRatio; ++b)
                                intermediateCountries[i * intermediateRatio + a][j * intermediateRatio + b] =
                                        gridCountries[i][j];
            }

            if (intermediateHR == HR)
                gridCountries = intermediateCountries;
            else {
                gridCountries.assign(HR, vector<uint8_t>(VR));
                unordered_map<uint8_t, uint32_t> countDict;
                for (size_t i = 0; i < intermediateCountries.size(); i += finalRatio)
                    for (size_t j = 0; j < intermediateCountries[0].size(); j += finalRatio) {
                        // 1 cell
                        for (size_t a = 0; a < finalRatio; ++a)
                            for (size_t b = 0; b < finalRatio; ++b)
                                ++countDict[intermediateCountries[i + a][j + b]];
                        gridCountries[i / finalRatio][j / finalRatio] =
                                ranges::max_element(countDict, [](const auto &lhs, const auto &rhs) {
                                    return lhs.second < rhs.second;
                                })->first;
                        countDict.clear();
                    }
            }
        }
    };
}

#endif
