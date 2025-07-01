#ifndef G4M_EUROPE_DG_COUNTRY_DATA_HPP
#define G4M_EUROPE_DG_COUNTRY_DATA_HPP

#include <cmath>
#include <unordered_set>
#include <array>
#include <span>
#include <string>
#include <filesystem>
#include <fstream>

#include "../misc/concrete/ipol.hpp"
#include "../log.hpp"
#include "../settings/constants.hpp"
#include "../settings/dicts/dicts.hpp"

using namespace std;
using namespace g4m::misc::concrete;
using namespace g4m::Constants;
using namespace g4m::Dicts;

namespace g4m::init {
    // class with statistics by regions
    class CountryData {
    public:
        void reset() {
            for (auto &el: values)
                el.data.clear();
            for (auto &el: count)
                el.data.clear();
        }

        // MG 13.07.2018: added to allow flexible reuse of the containers
        void clearYears(const uint16_t beginYear, const uint16_t endYear, const uint16_t yearStep) {
            for (int i = beginYear; i <= endYear; i += yearStep) {
                for (auto &el: values)
                    el.data.erase(i);
                for (auto &el: count)
                    el.data.erase(i);
            }
        }

        [[nodiscard]] double getVal(const size_t country, const uint16_t year) const {
            return values[country](year);
        }

        void setVal(const size_t country, const uint16_t year, const double value) {
            values[country].data[year] = value;
            count[country].data[year] = 1;
        }

        void inc(const size_t i, const uint16_t year, const double value) {
            values[i].data[year] += value;
            count[i].data[year]++;
        }

        [[nodiscard]] double getIp(const size_t countryIdx, const uint16_t year) const {
            double tmp = values[countryIdx](year);
            return isfinite(tmp) ? tmp : 0;
        }

        [[nodiscard]] double getAvg(const size_t countryIdx, const uint16_t year) const {
            double tmp_1 = values[countryIdx](year);
            double tmp_2 = count[countryIdx](year);
            return isfinite(tmp_1) && tmp_2 > 0 ? tmp_1 / tmp_2 : 0;
        }

        [[nodiscard]] CountryData getTimeAvg(int timePeriodWidth) const {
            return getSmoothAvg(timePeriodWidth, timePeriodWidth);
        }

        [[nodiscard]] CountryData getSmoothAvg(int timePeriodWidth, int timeStep = 1) const {
            if (timePeriodWidth % 2 != 1) {
                ERROR("Please provide odd number for time period! timePeriodWidth = ", timePeriodWidth);
                return {};
            }

            CountryData result;
            for (size_t countryIdx = 0; countryIdx < values.size(); ++countryIdx)
                if (!values[countryIdx].data.empty()) {
                    auto firstYear = static_cast<size_t>(values[countryIdx].minKey());
                    auto lastYear = static_cast<size_t>(values[countryIdx].maxKey());

                    size_t numYears = lastYear - firstYear + 1;

                    vector<double> valuesByYears(numYears);
                    vector<int> countByYears(numYears);

                    for (const auto &[key, value]: values[countryIdx].data)
                        valuesByYears[static_cast<size_t>(key) - firstYear] = value;
                    for (const auto &[key, value]: count[countryIdx].data)
                        countByYears[static_cast<size_t>(key) - firstYear] = static_cast<int>(value);

                    for (size_t yearIdx = 0; yearIdx < numYears; yearIdx += timeStep)
                        result.setVal(countryIdx, firstYear + yearIdx,
                                      computeAvg(valuesByYears, countByYears, yearIdx, timePeriodWidth, numYears));
                }
            return result;
        }

        // print array to file
        void
        printToFile(const string_view fileName, const uint16_t firstYear, const uint16_t lastYear, const uint16_t step,
                    const bool statTypeValues = true) const {
            ofstream f{filesystem::path{fileName}};

            if (!f.is_open()) {
                ERROR("Unable to save to: {}!", fileName);
                return;
            }

            f << "CountryN/Year";
            for (uint16_t j = firstYear; j <= lastYear; j += step)
                f << '\t' << j;
            f << '\n';

            for (const auto country: countriesList) {
                f << country;
                for (int year = firstYear; year <= lastYear; year += step)
                    f << '\t' << (statTypeValues ? values[country](year) : getAvg(country, year));
                f << '\n';
            }

            INFO("Successfully written to: {}", fileName);
        }

        [[nodiscard]] static double
        computeAvg(const span<const double> values_arr, const span<const int> count_arr, const size_t yearIdx,
                   const int timePeriodWidth, const size_t numYears) {
            double sum = 0;
            int num = 0;
            size_t timeHalfPeriodWidth = (timePeriodWidth - 1) / 2;
            for (size_t i = max(size_t{0}, yearIdx - timeHalfPeriodWidth);
                 i < min(numYears - 1, yearIdx + timeHalfPeriodWidth); ++i) {
                num += count_arr[i];
                sum += count_arr[i] == 0 ? 0 : values_arr[i];
            }
            return num != 0 ? sum / num : 0;
        }

    private:
        array<Ipol < double>, numberOfCountries >
        values;
        array<Ipol < double>, numberOfCountries >
        count;
    };

}

#endif
