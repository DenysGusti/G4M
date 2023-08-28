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

//        Forest_GUI_Europe_param_dw_5_3 app{argc, argv};
//        app.Run();

        // app in thread
        g4m::ThreadPool pool;
        for (size_t i = 0; i < 100; ++i)
            pool.enqueue([&] {
                try {
                    Forest_GUI_Europe_param_dw_5_3 app{argc, argv};
                    app.Run();
                } catch (const exception &e) {
                    cerr << e.what() << endl;
                }
            });
    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
    return 0;
}