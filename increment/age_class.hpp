#ifndef G4M_EUROPE_DG_AGE_CLASS_HPP
#define G4M_EUROPE_DG_AGE_CLASS_HPP

namespace g4m::increment {

    struct AgeClass {
        double area = 0;    // Forest stand area (single age class)
        double bm = 0;      // Biomass (stem), tC/ha
        double d = 0;       // diameter, cm
        double h = 0;       // height, m

        enum class Mode : uint8_t {
            Area,
            Bm,
            D,
            H
        };

        double &operator()(const Mode type) {
            switch (type) {
                case Mode::Area:
                    return area;
                case Mode::Bm:
                    return bm;
                case Mode::D:
                    return d;
                case Mode::H:
                    return h;
            }
        }

        [[nodiscard]] double operator()(const Mode type) const {
            switch (type) {
                case Mode::Area:
                    return area;
                case Mode::Bm:
                    return bm;
                case Mode::D:
                    return d;
                case Mode::H:
                    return h;
                default:
                    return numeric_limits<double>::quiet_NaN();
            }
        }

        // area * bm
        [[nodiscard]] double getBiomass_tC() const {
            return area * bm;
        }

        [[nodiscard]] AgeClass getWeightedAgeClass(const AgeClass &other) const {
            double reciprocalTotalArea = 1 / (area + other.area);
            return AgeClass{
                    .area = area + other.area,
                    .bm = (getBiomass_tC() + other.getBiomass_tC()) * reciprocalTotalArea,
                    .d = (d * area + other.d * other.area) * reciprocalTotalArea,
                    .h = (h * area + other.h * other.area) * reciprocalTotalArea
            };
        }

        [[nodiscard]] string str() const {
            return format("area = {}, bm = {}, d = {}, h = {}", area, bm, d, h);
        }

        friend ostream &operator<<(ostream &os, const AgeClass &obj) {
            os << obj.str();
            return os;
        }
    };
}

template<>
struct std::formatter<g4m::increment::AgeClass> : formatter<string> {
    auto format(const g4m::increment::AgeClass &obj, format_context &ctx) const {
        return formatter<string>::format(obj.str(), ctx);
    }
};

#endif
