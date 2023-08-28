#ifndef G4M_EUROPE_DG_FOREST_GUI_EUROPE_PARAM_DW_5_3_DG_HPP
#define G4M_EUROPE_DG_FOREST_GUI_EUROPE_PARAM_DW_5_3_DG_HPP

#include <iostream>
#include <array>
#include <string>
#include <set>
#include <unordered_set>
#include <random>

#include "../../log.hpp"
#include "../../diagnostics/debugging.hpp"
#include "../../diagnostics/timer.hpp"
#include "../abstract/application.hpp"

#include "../../start_data/start_data.hpp"

#include "../../init/settings.hpp"
#include "../../init/data_struct.hpp"
#include "../../init/coef_struct.hpp"
#include "../../structs/lw_price.hpp"
#include "../../structs/harvest_residues.hpp"
#include "../../structs/dat.hpp"

#include "../../increment/age_struct.hpp"

using namespace std;

using namespace g4m::structs;
using namespace g4m::misc::concrete;
using namespace g4m::increment;
using namespace g4m::diagnostics;

using namespace g4m::application::abstract;

namespace g4m::application::concrete {

    class Forest_GUI_Europe_param_dw_5_3 : public Application {
    public:
        Forest_GUI_Europe_param_dw_5_3(const int argc, const char *const argv[]) : Application{argc, argv} {}

        explicit Forest_GUI_Europe_param_dw_5_3(const span<const string_view> &args_) : Application{args_} {}

        void Run() final {
            Timer timer;
            INFO("Application {}_{}_{}_{} is running", args[1], args[2], args[3], args[4]);

            array<string, 3> c_scenario = {args[1], args[2], args[3]};
            string full_scenario = c_scenario[0] + "_" + c_scenario[1] + "_" + c_scenario[2];
            int inputPriceC = stoi(args[4]);

//            random_device rd;
//            mt19937_64 gen{rd()};
//            uniform_int_distribution<> distrib{0, 1};
//            if (distrib(gen)) {
//                INFO("throw {}", full_scenario);
//                throw runtime_error{"123"};
//            }
//            else
//                INFO("continue {}", full_scenario);

            Ipol<double> wd_supplement;
            CountryData countriesForestArea;

            auto local_suffix = string{suffix} + full_scenario;
            if (inputPriceC == 0)
                local_suffix += "_Pco2_0";

            string full_scenario_gl = full_scenario;

            INFO("Scenario to read in: {}", full_scenario);
            INFO("Scenario to read GL: {}", full_scenario_gl);

            auto suffix0 = string{suffix} + c_scenario[1] + "_" + c_scenario[2];
        }

    protected:
    };
}

#endif
