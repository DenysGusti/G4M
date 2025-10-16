#include "log.hpp"
#include "start_data/init.hpp"
#include "application/forest_GUI_Europe_param_dw_5_3.hpp"
#include "thread_pool.hpp"
#include "init/data_grid.hpp"

using namespace std;
using namespace g4m;
using namespace g4m::diagnostics;
using namespace g4m::application;

int main(int argc, char *argv[]) {
    vector<string> args{argv, argv + argc};

    try {
        Timer timer{"main"};
        Log::Init();
        StartData::Init();

        // 1 scenario
//        {
//            Log::Init(settings.scenarios.front() | rv::join_with('_') | ranges::to<string>());
//            Forest_GUI_Europe_param_dw_5_3 app{settings.scenarios.front()};
//            app.Run();
//        }
//        s_AllocationMetrics.printMemoryUsage();

        for (auto &[suffix0, signal]: signalZeroCToMainScenarios)
            signal.acquire();

        // multiple scenarios
        g4m::ThreadPool pool;
        for (const auto& scenario: settings.scenarios)
            pool.enqueue([&] {
                try {
                    Log::Init(scenario | rv::join_with('_') | ranges::to<string>());
                    Forest_GUI_Europe_param_dw_5_3 app{scenario};
                    app.Run();
                } catch (const exception &e) {
                    println(cerr, "{}", e.what());
                    FATAL("{}", e.what());
                }
            });

    } catch (const exception &e) {
        println(cerr, "{}", e.what());
        FATAL("{}", e.what());
    }
    return 0;
}