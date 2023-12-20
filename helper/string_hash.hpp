#ifndef G4M_EUROPE_DG_STRING_HASH_HPP
#define G4M_EUROPE_DG_STRING_HASH_HPP

#include <string>

using namespace std;

namespace g4m::helper {
//  https://www.cppstories.com/2021/heterogeneous-access-cpp20/
    struct StringHash {
        using is_transparent = void;

        [[nodiscard]] size_t operator()(const char *txt) const {
            return hash<string_view>{}(txt);
        }

        [[nodiscard]] size_t operator()(const string_view txt) const {
            return hash<string_view>{}(txt);
        }

        [[nodiscard]] size_t operator()(const string &txt) const {
            return hash<string>{}(txt);
        }
    };
}

#endif
