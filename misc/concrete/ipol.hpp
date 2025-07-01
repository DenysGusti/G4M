#ifndef G4M_EUROPE_DG_IPOL_HPP
#define G4M_EUROPE_DG_IPOL_HPP

#include <vector>
#include <ranges>
#include <map>
#include <algorithm>
#include <format>
#include <string>

#include "../abstract/vipol.hpp"
#include "../abstract/iipol.hpp"

using namespace std;
using namespace g4m::misc::abstract;
namespace rv = ranges::views;

namespace g4m::misc::concrete {

    template<floating_point T>
    class Ipol : public VIpol<T>, public IIpol<T> {
    public:
        // access: [myObject].data.[method]
        // other methods: https://en.cppreference.com/w/cpp/container/map
        map <T, T> data;

        Ipol() = default;

        Ipol(const span<const uint16_t> year_columns, const span<const double> d_row) {
            buildFromCSVLine(year_columns, d_row);
        }

        void buildFromCSVLine(const span<const uint16_t> year_columns, const span<const double> d_row) {
            for (const auto &[year, cell]: rv::zip(year_columns, d_row))
                data[year] = cell;
        }

        // add x to all
        Ipol &operator+=(const T x) override {
            for (auto &value: data | rv::values)
                value += x;
            return *this;
        }

        // multiply all by x
        Ipol &operator*=(const T x) override {
            for (auto &value: data | rv::values)
                value *= x;
            return *this;
        }

        [[nodiscard]] string str() const override {
            string s = "Ipol data:\n";
            s.reserve(s.length() + 32 * data.size());
            for (const auto &[key, value]: data)
                s += format("{}: {}\n", key, value);
            return s;
        }

        // find min key
        T minKey() const {
            return minOrMaxKey(true);
        }

        // find max key
        T maxKey() const {
            return minOrMaxKey(false);
        }

        // find min value
        T minValue() const {
            return minOrMaxValue(true);
        }

        // find max value
        T maxValue() const {
            return minOrMaxValue(false);
        }

        // find min value <= x
        T minValueNotGreater(const T x) const {
            return minOrMaxValueLessOrNotGreater(x, true, false);
        }

        // find min value < x
        T minValueLess(const T x) const {
            return minOrMaxValueLessOrNotGreater(x, true, true);
        }

        // find max value <= x
        T maxValueNotGreater(const T x) const {
            return minOrMaxValueLessOrNotGreater(x, false, false);
        }

        // find max value < x
        T maxValueLess(const T x) const {
            return minOrMaxValueLessOrNotGreater(x, false, true);
        }

        // find min value >= x
        T minValueNotLess(const T x) const {
            return minOrMaxValueGreaterOrNotLess(x, true, false);
        }

        // find min value > x
        T minValueGreater(const T x) const {
            return minOrMaxValueGreaterOrNotLess(x, true, true);
        }

        // find max value >= x
        T maxValueNotLess(const T x) const {
            return minOrMaxValueGreaterOrNotLess(x, false, false);
        }

        // find max value > x
        T maxValueGreater(const T x) const {
            return minOrMaxValueGreaterOrNotLess(x, false, true);
        }

        // find min value in [x, y]
        T minValueRangeNotStrict(const T x, const T y) const {
            return minOrMaxValueRange(x, y, true, false);
        }

        // find min value in (x, y)
        T minValueRangeStrict(const T x, const T y) const {
            return minOrMaxValueRange(x, y, true, true);
        }

        // find max value in [x, y]
        T maxValueRangeNotStrict(const T x, const T y) const {
            return minOrMaxValueRange(x, y, false, false);
        }

        // find max value in (x, y)
        T maxValueRangeStrict(const T x, const T y) const {
            return minOrMaxValueRange(x, y, false, true);
        }

        // Returns true if the map is filled in with at least one non-zero value
        [[nodiscard]] bool nonZero() const {
            return ranges::any_of(data | rv::values, [](const auto x) -> bool { return x != 0; });
        }

        // interpolate i (better for pointers, default ())
        [[nodiscard]] T ip(const T i) const override {
            if (data.empty())
                return 0;

            const auto itUp = data.lower_bound(i);

            if (itUp == data.end())
                return data.rbegin()->second;

            if (i == itUp->first)
                return itUp->second;

            if (itUp == data.begin())
                return data.begin()->second;

            const auto itLo = prev(itUp);

            const auto [i0, v0] = *itLo;
            const auto [i1, v1] = *itUp;

            return lerp(v0, v1, (i - i0) / (i1 - i0));  // interpolate intermediate value
        }

    protected:
        // find min or max key
        T minOrMaxKey(const bool min_flag) const {
            if (data.empty())
                return 0;

            return min_flag ? data.begin()->first : data.rbegin()->first;
        }

        // find min or max value
        T minOrMaxValue(const bool min_flag) const {
            if (data.empty())
                return 0;

            const auto values = data | rv::values;

            return min_flag ? ranges::min(values) : ranges::max(values);
        }

        // find min or max value < or <= x
        T minOrMaxValueLessOrNotGreater(const T x, const bool min_flag, const bool strict) const {
            auto it = strict ? data.lower_bound(x) : data.upper_bound(x);

            if (it != data.begin())
                --it;

            if (strict ? it->first >= x : it->first > x)
                return 0;

            const auto subRange = ranges::subrange(data.begin(), ++it) | rv::values;

            return min_flag ? ranges::min(subRange) : ranges::max(subRange);
        }

        // find min or max value > or >= x
        T minOrMaxValueGreaterOrNotLess(const T x, const bool min_flag, const bool strict) const {
            auto it = strict ? data.upper_bound(x) : data.lower_bound(x);

            if (it == data.end())
                return 0;

            const auto subRange = ranges::subrange(it, data.end()) | rv::values;

            return (min_flag ? ranges::min(subRange) : ranges::max(subRange));
        }

        // find min or max value in (x, y) or [x, y]
        T minOrMaxValueRange(const T x, const T y, const bool min_flag, const bool strict) const {
            auto itA = strict ? data.upper_bound(x) : data.lower_bound(x);
            auto itB = strict ? data.lower_bound(y) : data.upper_bound(y);

            if (itB != data.begin())
                --itB;

            if (itA == data.end() || strict ? itB->first >= y : itB->first > y)
                return 0;

            const auto subRange = ranges::subrange(itA, ++itB) | rv::values;

            if (!subRange)
                return 0;

            return min_flag ? ranges::min(subRange) : ranges::max(subRange);
        }
    };
}

#endif