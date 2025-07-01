#ifndef G4M_EUROPE_DG_FFIPOL_HPP
#define G4M_EUROPE_DG_FFIPOL_HPP

#include <vector>
#include <format>
#include <string>

#include "../abstract/vipol.hpp"
#include "../abstract/iipol.hpp"
#include "ipol.hpp"

using namespace std;
using namespace g4m::misc::abstract;

namespace g4m::misc::concrete {

    template<floating_point T>
    class FFIpol : public VIpol<T>, public IIpol<T> {
    public:
        vector<T> data;

        FFIpol() = default;

        explicit FFIpol(const Ipol<T> &ipol, const T zoom_ = 1, const T add = 0.5) : zoom{zoom_}, intercept{
                zoom_ * ipol.minKey() + add} {
            const size_t n = 1 + zoom * (ceil(ipol.maxKey()) - floor(ipol.minKey()));
            data.assign(n, 0);
            for (size_t i = 0; i < data.size(); ++i)
                data[i] = ipol((intercept + i) / zoom - add);
        };

        void overwrite(const Ipol<T> &ipol, const T zoom_ = 1, const T add = 0.5) {
            zoom = zoom_;
            intercept = zoom_ * ipol.minKey() + add;
            const size_t n = 1 + zoom * (ceil(ipol.maxKey()) - floor(ipol.minKey()));
            data.assign(n, 0);
            for (size_t i = 0; i < data.size(); ++i)
                data[i] = ipol((intercept + i) / zoom - add);
        }

        // add x to all
        FFIpol &operator+=(const T x) override {
            for (auto &value: data)
                value += x;
            return *this;
        }

        // multiply all by x
        FFIpol &operator*=(const T x) override {
            for (auto &value: data)
                value *= x;
            return *this;
        }

        [[nodiscard]] string str() const override {
            string s = "FFIpol data:\n";
            s.reserve(s.length() + 16 * data.size());
            for (const auto el: data)
                s += format("{}\n", el);
            return s;
        }

        T ip(const T i) const override {
            if (data.empty())
                return 0;
            if (data.size() == 1)
                return data.front();

            const T zi = i * zoom + intercept;
            if (zi < 0)
                return data.front();
            if (zi >= data.size() - 1)
                return data.back();

            const size_t i0 = zi;
            return data[i0];
        }

    protected:
        T intercept = 0, zoom = 1;
    };
}

#endif
