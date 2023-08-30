#ifndef G4M_EUROPE_DG_APPLICATION_HPP
#define G4M_EUROPE_DG_APPLICATION_HPP

#include <vector>
#include <span>
#include <string>

using namespace std;

namespace g4m::application::abstract {

    class Application {
    public:
        static vector <string> CreateArgsFromArgcArgv(const int argc, const char *const argv[]) noexcept {
            vector<string> arguments;
            arguments.reserve(argc);
            for (size_t i = 0; i < argc; ++i)
                arguments.emplace_back(argv[i]);
            return arguments;
        }

        Application() = default;

        explicit Application(const span<const string> &args_) : args{args_.begin(), args_.end()} {}

        virtual void Run() = 0;

        virtual ~Application() = default;

    protected:
        vector <string> args;
    };

}

#endif
