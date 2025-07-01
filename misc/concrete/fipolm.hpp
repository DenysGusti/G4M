#ifndef G4M_EUROPE_DG_FIPOLM_HPP
#define G4M_EUROPE_DG_FIPOLM_HPP

#include <cmath>
#include <vector>
#include <format>
#include <string>
#include <functional>
#include <numeric>

#include "../abstract/vipol.hpp"
#include "../abstract/iipolm.hpp"
#include "ipolm.hpp"

using namespace std;
using namespace g4m::misc::abstract;

namespace g4m::misc::concrete {

    template<floating_point T>
    class FIpolM : public VIpol<T>, public IIpolM<T> {
    public:
        vector<T> data;

        explicit FIpolM(const span<const size_t> n_) : n{n_.begin(), n_.end()}, dim{n_.size()} {
            intercept.assign(dim, 0);
            zoom.assign(dim, 1);
            data.assign(reduce(n.cbegin(), n.cend(), size_t{1}, multiplies<>{}), 0);
        }

        FIpolM(const IpolM<T> &t, const span<const T> zoom_) : zoom{zoom_} {
            vector<T> idxMin = t.minKey();
            vector<T> idxMax = t.maxKey();

            dim = idxMin.size();

            intercept.reserve(dim);
            for (size_t i = 0; i < dim; ++i)
                intercept.push_back(zoom[i] * idxMin[i]);

            n.reserve(dim);
            for (size_t i = 0; i < dim; ++i)
                n.push_back(1 + zoom[i] * (ceil(idxMax[i]) - floor(idxMin[i])));

            data.assign(reduce(n.cbegin(), n.cend(), size_t{1}, multiplies<>{}), 0);

            fillMap(t, vector<T>(dim));
        }

        explicit FIpolM(const IpolM<T> &t) : FIpolM(t, vector<T>{t.minKey().size(), 1}) {}

        bool insert(const span<const T> indices, const T value) {
            if (indices.size() > dim) {
                throw invalid_argument{"indices size is bigger than dim"};
            }
            for (size_t i = 0; i < indices.size(); ++i)
                if (indices[i] >= n[i])
                    return false;

            size_t index = 0;
            size_t mul = 1;
            for (size_t i = 0; i < indices.size(); ++i) {
                index += indices[i] * mul;
                mul *= n[i];
            }
            data[index] = value;
            return true;
        }

        [[nodiscard]] vector<size_t> getN() const {
            return n;
        }

        // add x to all
        FIpolM &operator+=(const T x) override {
            for (auto &value: data)
                value += x;
            return *this;
        }

        // multiply all by x
        FIpolM &operator*=(const T x) override {
            for (auto &value: data)
                value *= x;
            return *this;
        }

        [[nodiscard]] string str() const override {
            string s = "FIpolM data:\n";
            s.reserve(s.length() + 16 * data.size());
            for (const auto el: data)
                s += format("{}\n", el);
            return s;
        }

        T ip(const span<const T> i) const override {
            if (i.size() != dim)
                return 0;

            T k_j = clamp(i[0] * zoom[0] + intercept[0], T{0}, static_cast<T>(n[0] - 1));  // TODO to check sign

            size_t sur = 1 << n.size();  // n Surrounding points 2 ^ dim
            vector<size_t> idx(sur, string::npos);  // Index for surrounding points
            vector<T> dist(sur);  // Distance of surrounding points

            idx[0] = floor(k_j);
            idx[1] = ceil(k_j);
            dist[0] = abs(k_j - idx[0]); // Manhattan distance
            dist[1] = abs(k_j - idx[1]);

            size_t mul = n[0];  // Array size in dimension 0

            size_t t = 0, uc = 0, uf = 0;
            T dc = 0, df = 0;
            for (size_t j = 1; j < dim; ++j) {
                k_j = clamp(i[j] * zoom[j] + intercept[j], T{0}, static_cast<T>(n[j] - 1));  // TODO to check sign
                t = 1 << j;  // 2^n points used in this dim
                uc = ceil(k_j) * mul; // Index where of grid point
                uf = floor(k_j) * mul;
                dc = abs(k_j - ceil(k_j)); // Manhattan distance
                df = abs(k_j - floor(k_j));
                for (size_t k = 0; k < t; ++k) {
                    idx[k + t] = idx[k] + uc;
                    idx[k] += uf;
                    dist[k + t] = dist[k] + dc;
                    dist[k] += df;
                }
                mul *= n[j];
            }

            T sdist = 0;
            T sval = 0;
            for (size_t j = 0; j < sur; ++j)
                if (idx[j] >= 0) {
                    if (dist[j] > 0) {
                        sval += data[idx[j]] / dist[j];
                        sdist += 1 / dist[j];
                    } else {
                        sval = data[idx[j]];
                        sdist = 1;
                        break;
                    }
                }
            return sdist > 0 ? sval / sdist : 0;
        }

    protected:
        size_t dim = 0;
        vector<size_t> n;  // Size of array and how many dimensions does the index have
        vector<T> intercept; // If data range does not start from 0
        vector<T> zoom;

        void fillMap(const IpolM<T> &t, const span<T> key, const size_t idx_ = 0, const size_t aDim = 0,
                     const size_t mul = 1) {
            for (size_t i = 0; i < n[aDim]; ++i) {
                key[aDim] = (intercept[aDim] + i) / zoom[aDim];
                if (aDim + 1 < dim) {
                    fillMap(t, key, idx_ + i * mul, aDim + 1, mul * n[aDim]);
                } else {
                    data[idx_ + i * mul] = t(key);
                }
            }
        };
    };
}

#endif
