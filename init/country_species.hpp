#ifndef G4M_EUROPE_DG_COUNTRY_SPECIES_HPP
#define G4M_EUROPE_DG_COUNTRY_SPECIES_HPP

namespace g4m::init {

    struct CountrySpecies {
        int country = 0;
        int species = 0;

        CountrySpecies() = default;

        CountrySpecies(const int country_, const int species_) : country{country_}, species{species_} {}
    };
}

#endif
