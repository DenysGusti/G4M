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
    try {
        Timer timer{"main"};
        Log::Init();
//        increment::CanopyLayer::Test_createNormalForest();
        StartData::Init();

        // 1 scenario
        {
            Forest_GUI_Europe_param_dw_5_3 app{settings.scenarios.front()};
            app.Run();
        }
//        s_AllocationMetrics.printMemoryUsage();

        for (auto &[suffix0, signal]: signalZeroCToMainScenarios)
            signal.acquire();

        // multiple scenarios
//        g4m::ThreadPool pool;
//        for (int i = 0; i < 100; ++i)
//            pool.enqueue([&] {
//                try {
//                    Forest_GUI_Europe_param_dw_5_3 app{
//                            Application::ConvertToUppercase(Application::CreateArgsFromArgcArgv(argc, argv))};
//                    app.Run();
//                } catch (const exception &e) {
//                    FATAL("{}", e.what());
//                    cerr << e.what() << endl;
//                }
//            });

    } catch (const exception &e) {
        FATAL("{}", e.what());
        cerr << e.what() << endl;
    }
    return 0;
}