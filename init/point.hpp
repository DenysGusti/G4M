#ifndef G4M_EUROPE_DG_POINT_HPP
#define G4M_EUROPE_DG_POINT_HPP

#include <string>
#include <format>

using namespace std;

namespace g4m::init {
    struct Point {
        size_t x = 0;
        size_t y = 0;

        Point() = default;

        Point(const size_t x_, const size_t y_) : x{x_}, y{y_} {}

        [[nodiscard]] string str() const {
            return format("({}, {})", x, y);
        }
    };
}

template<>
struct std::formatter<g4m::init::Point> : formatter<string> {
    auto format(const g4m::init::Point &obj, format_context &ctx) const {
        return formatter<string>::format(obj.str(), ctx);
    }
};

#endif
