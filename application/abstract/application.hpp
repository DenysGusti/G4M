#ifndef G4M_EUROPE_DG_APPLICATION_HPP
#define G4M_EUROPE_DG_APPLICATION_HPP

#include <vector>
#include <span>
#include <string>

using namespace std;

namespace g4m::application::abstract {

    class Application {
    public:
        Application() = default;

        explicit Application(const span<const string_view> &args_) : args{args_.begin(), args_.end()} {}

        Application(const int argc, const char *const argv[]) {
            args.reserve(argc);
            for (size_t i = 0; i < argc; ++i)
                args.emplace_back(argv[i]);
        };

        virtual void Run() = 0;

        virtual ~Application() = default;

    protected:
        vector<string> args;
    };

}

#endif
