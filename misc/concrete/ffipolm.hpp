#ifndef G4M_EUROPE_DG_FFIPOLM_HPP
#define G4M_EUROPE_DG_FFIPOLM_HPP

#include <cmath>
#include <vector>
#include <format>
#include <string>
#include <functional>
#include <numeric>
#include <algorithm>

#include "../abstract/vipol.hpp"
#include "../abstract/iipolm.hpp"
#include "ipolm.hpp"

using namespace std;
using namespace g4m::misc::abstract;

namespace g4m::misc::concrete {

    template<floating_point T>
    class FFIpolM : public VIpol<T>, public IIpolM<T> {
    public:
        vector<T> data;

        explicit FFIpolM(const span<const size_t> n_) : n{n_.begin(), n_.end()}, dim{n_.size()} {
            intercept.assign(dim, 0);
            zoom.assign(dim, 1);
            data.assign(reduce(n.cbegin(), n.cend(), size_t{1}, multiplies<>{}), 0);
        }

        FFIpolM(const IpolM<T> &t, const span<const T> zoom_, const T add = 0.5) : zoom{zoom_.begin(), zoom_.end()} {
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

            vector<T> key(dim, 0);
            fillMap(t, key, 0, 0, 1, add);
        }

        explicit FFIpolM(const IpolM<T> &t, const T add = 0.5) : FFIpolM(t, vector<T>(t.minKey().size(), 1), add) {}

        bool insert(const span<const size_t> indices, const T value) {
            if (indices.size() != dim) {
                ERROR("indices size ({}) doesn't equal dim ({})", indices.size(), dim);
                throw invalid_argument{"indices size doesn't equal dim"};
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

        [[nodiscard]] vector<size_t> getN() const noexcept {
            return n;
        }

        // add x to all
        FFIpolM &operator+=(const T x) noexcept override {
            for (auto &value: data)
                value += x;
            return *this;
        }

        // multiply all by x
        FFIpolM &operator*=(const T x) noexcept override {
            for (auto &value: data)
                value *= x;
            return *this;
        }

        [[nodiscard]] string str() const noexcept override {
            string s = "FFIpolM data:\n";
            s.reserve(s.length() + 16 * data.size());
            for (const auto el: data)
                s += format("{}\n", el);
            return s;
        }

        T ip(const span<const T> i) const noexcept override {
            if (i.size() != dim)
                return 0;

            size_t idx = 0;
            size_t mul = 1;
            size_t k = 0;
            for (size_t j = 0; j < dim; ++j) {
                k = clamp(i[j] * zoom[j] + intercept[j], T{0}, static_cast<T>(n[j] - 1));
                idx += k * mul;
                mul *= n[j];
            }
            return data[idx];
        }

    protected:
        size_t dim = 0;
        vector<size_t> n;  // Size of array and how many dimensions does the index have
        vector<T> intercept; // If data range does not start from 0
        vector<T> zoom;


        void fillMap(const IpolM<T> &t, const span<T> key, const size_t idx_, const size_t aDim, const size_t mul,
                     const T add) {
            for (size_t i = 0; i < n[aDim]; ++i) {
                key[aDim] = (intercept[aDim] + i) / zoom[aDim] + add;
                if (aDim + 1 < dim) {
                    fillMap(t, key, idx_ + i * mul, aDim + 1, mul * n[aDim], add);
                } else {
                    data[idx_ + i * mul] = t(key);
                }
            }
        };
    };
}

#endif
