#ifndef G4M_EUROPE_DG_ENUM_HASHER_HPP
#define G4M_EUROPE_DG_ENUM_HASHER_HPP

namespace g4m::helper {

    struct EnumHasher {
        template<typename T>
        size_t operator()(const T t) const {
            return static_cast<size_t>(t);
        }
    };
}

#endif
