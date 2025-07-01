#ifndef G4M_EUROPE_DG_IPOLM_HPP
#define G4M_EUROPE_DG_IPOLM_HPP

#include <vector>
#include <ranges>
#include <map>
#include <format>
#include <string>

#include "../abstract/vipol.hpp"
#include "../abstract/iipolm.hpp"
#include "../../log.hpp"

using namespace std;
using namespace g4m::misc::abstract;
namespace rv = ranges::views;

namespace g4m::misc::concrete {
    // Multidimensional interpolation
    template<floating_point T>
    class IpolM : public VIpol<T>, public IIpolM<T> {
    public:
        map<vector<T>, T> data;

        IpolM() = default;

        // add x to all
        IpolM &operator+=(const T x) override {
            for (auto &value: data | rv::values)
                value += x;
            return *this;
        }

        // multiply all by x
        IpolM &operator*=(const T x) override {
            for (auto &value: data | rv::values)
                value *= x;
            return *this;
        }

        [[nodiscard]] string str() const override {
            string s = "IpolM data:\n";
            s.reserve(s.length() + 64 * data.size());
            for (const auto &[key, value]: data) {
                for (const auto el: key)
                    s += format("{:>2} ", el);
                s += format(":\t{}\n", value);
            }
            return s;
        }

        // returns a minimal key
        vector<T> minKey() const {
            return minOrMaxKey(true);
        };

        // returns a maximal key
        vector<T> maxKey() const {
            return minOrMaxKey(false);
        };

        T ip(const span<const T> vec) const override {
            const size_t regions = 1 << vec.size(); // 2 ^ i.size(), but better
            // values of near points & shortest distance in region
            vector<pair<vector<T>, T> > y_dist(regions, {{}, -1});

            T d = 0; // distance
            size_t pos = 0; // memory where to save the minimum distance
            size_t mul = 0; // multiplier to get right pos
            for (T tmp = 0; const auto &[key, value]: data) {
                d = 0;
                pos = 0;
                mul = 1;
                for (size_t i = 0; i < key.size(); ++i) {
                    tmp = key[i] - vec[i];

                    if (tmp > 0)
                        pos += mul;

                    if (pos >= regions) {
                        ERROR("out of range problem: pos = {}, regions = {}", pos, regions);
                        return 0;
                    }

                    d += abs(tmp); // d += abs(tmp) - manhattan distance; d += tmp * tmp - geometric interpolation
                    mul <<= 1; // mul *= 2
                }
                if (auto &[y, dist] = y_dist[pos]; d <= dist || dist < 0) {
                    if (dist != d) {
                        y.clear();
                        dist = d;
                    }
                    y.push_back(value);
                }
            }

            T ip = 0, distSum = 0;
            int64_t n = 0;
            for (const auto &[y, dist]: y_dist)
                if (dist > 0 && n == 0)
                    for (const auto y_el: y) {
                        ip += y_el / dist;
                        distSum += 1 / dist;
                    }
                else if (dist == 0) {
                    if (n == 0)
                        ip = 0;
                    for (const auto y_el: y) {
                        ip += y_el;
                        ++n;
                    }
                }

            if (n > 0)
                ip /= n;
            else if (distSum > 0)
                ip /= distSum;
            else
                ip = 0; // numeric_limits<Value>::quiet_NaN();
            return ip;
        }

    protected:
        // returns a minimal or maximal key
        vector<T> minOrMaxKey(const bool min_flag) const {
            if (data.empty())
                return {}; // returns empty vector!

            // copy assignment of the first vector
            vector<T> ret = data.begin()->first; // every vector must be the same size!

            if (min_flag)
                for (size_t i = 0; i < ret.size(); ++i)
                    for (const auto &key: data | rv::keys)
                        ret[i] = min(key[i], ret[i]);
            else
                for (size_t i = 0; i < ret.size(); ++i)
                    for (const auto &key: data | rv::keys)
                        ret[i] = max(key[i], ret[i]);

            return ret;
        }
    };
}

#endif
