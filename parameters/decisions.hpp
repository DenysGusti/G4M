#ifndef G4M_EUROPE_DG_DECISIONS_HPP
#define G4M_EUROPE_DG_DECISIONS_HPP

namespace g4m::parameters {
    // do thinning (Vornutzung) (depending on d and removed volume per hectare) and final fellings (Endnutzung)
    // (depending on d and stocking volume per hectare) in relation to standing timber (Vorratsfestmeter)
    struct Decisions {
        double diameterDovMin = 0;
        double stemBiomassDovMin = 0;
        double thinShareDovMin = 0;
        double diameterDoeMin = 0;
        double stemBiomassDoeMin = 0;

        // do thinning (Vornutzung)
        [[nodiscard]] bool DOV(const double diameter, const double stemBiomass, const double thinShare) const noexcept {
            return diameter >= diameterDovMin && stemBiomass >= stemBiomassDovMin && thinShare >= thinShareDovMin;
        }

        // do final fellings (Endnutzung)
        [[nodiscard]] bool DOE(const double diameter, const double stemBiomass) const noexcept {
            return diameter >= diameterDoeMin || stemBiomass >= stemBiomassDoeMin;
        }
    };
}

#endif
