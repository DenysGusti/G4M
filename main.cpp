#include "log.hpp"
#include "start_data/start_data_project_independent.hpp"
#include "application/abstract/application.hpp"
#include "application/concrete/forest_GUI_Europe_param_dw_5_3.hpp"
#include "thread_pool.hpp"
#include "init/data_grid.hpp"

using namespace std;
using namespace g4m;
using namespace g4m::diagnostics;
using namespace g4m::application::abstract;
using namespace g4m::application::concrete;

int main(int argc, char *argv[]) {
    try {
        Timer timer{"main"};
        Log::Init();
        StartData::Init();

        // 1 scenario
        Forest_GUI_Europe_param_dw_5_3 app{
                Application::ConvertToUppercase(Application::CreateArgsFromArgcArgv(argc, argv))};
        app.Run();
//        LOG_DEBUG("Dynamic allocation: {} bytes", s_AllocationMetrics.CurrentUsage());

        // multiple scenarios
//        g4m::ThreadPool pool;
//        for (int i = 0; i < 100; ++i)
//            pool.enqueue([&] {
//                try {
//                    Forest_GUI_Europe_param_dw_5_3 app{
//                            Application::ConvertToUppercase(Application::CreateArgsFromArgcArgv(argc, argv))};
//                    app.Run();
//                } catch (const exception &e) {
//                    cerr << e.what() << endl;
//                }
//            });

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
    return 0;
}