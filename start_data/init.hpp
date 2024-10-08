#ifndef G4M_EUROPE_DG_INIT_HPP
#define G4M_EUROPE_DG_INIT_HPP

#include "start_data_functions.hpp"

namespace g4m::StartData {
    // Initialisation of input datasets
    void Init() {
        future<void> settings_future = async(launch::async, [] {
            Log::Init("settings");
            settings.readSettings();
        });

        settings_future.get();

        future<void> coef_future = async(launch::async, [] {
            Log::Init("coef");
            coef.readCoef(settings.coefPath);
        });

        future<void> NUTS2_future = async(launch::async, [] {
            Log::Init("NUTS2");
            nuts2.readNUTS2();
        });

        future<void> plots_future = async(launch::async, [] {
            Log::Init("plots");
            plots.readPlots();
            plots.filterPlots();
            plots.initPlotsSimuID();
            plots.initPlotsXY_SimuID();
        });

        coef_future.get();

        future<void> globiom_datamaps_future = async(launch::async, [] {
            Log::Init("globiom_datamaps");
            datamapScenarios.readGLOBIOM_and_datamaps();
            datamapScenarios.convertUnitsDatamaps();
        });

        future<void> CO2_price_future = async(launch::async, [] {
            Log::Init("CO2_price");
            datamapScenarios.readCO2price();
            datamapScenarios.correctAndConvertCO2Prices();
        });

        plots_future.get();
        NUTS2_future.get();

        future<void> MAI_future = async(launch::async, [] {
            if constexpr (!debugWithoutBigFiles) {
                Log::Init("MAI");
                simuIdScenarios.readMAIClimate(plots.plotsXY_SimuID);
                if constexpr (!scaleMAIClimate)
                    simuIdScenarios.scaleMAIClimate2020();
                else
                    INFO("scaleMAIClimate is turned off");
            }
        });

        future<void> globiom_land_future = async(launch::async, [] {
            if constexpr (!debugWithoutBigFiles) {
                Log::Init("globiom_land");
                simuIdScenarios.readGlobiomLandCalibrate(plots.plotsSimuID);
                simuIdScenarios.readGlobiomLand(plots.plotsSimuID);
            }
        });

        future<void> globiom_land_country_future = async(launch::async, [] {
            if constexpr (!debugWithoutBigFiles) {
                Log::Init("globiom_land_country");
                countryLandArea = datamapScenarios.readGlobiomLandCountryCalibrate_calcCountryLandArea();
                datamapScenarios.readGlobiomLandCountry();
            }
        });

        future<void> disturbances_future = async(launch::async, [] {
            if constexpr (!debugWithoutBigFiles) {
                Log::Init("disturbances");
                simuIdScenarios.readDisturbances(plots.plotsXY_SimuID);
                simuIdScenarios.add2020Disturbances();
                if constexpr (scaleDisturbance2020)
                    simuIdScenarios.scaleDisturbances2020();
                else
                    INFO("scaleDisturbance2020 is turned off");
            }
        });

        future<void> disturbances_extreme_future = async(launch::async, [] {
            if constexpr (!debugWithoutBigFiles) {
                Log::Init("disturbances_extreme");
                simuIdScenarios.readDisturbancesExtreme(plots.plotsXY_SimuID);
            }
        });

        future<void> biomass_bau_future = async(launch::async, [] {
            Log::Init("biomass_bau");
            if constexpr (fmPol && binFilesOnDisk)
                bauScenarios.readBiomassBau();
            else
                INFO("biomass_bau reading is turned off");
        });

        future<void> age_struct_data_future = async(launch::async, [] {
            Log::Init("age_struct_data");
            asd.readAgeStructData();
            asd.normalizeAgeStructData();
        });

        plots.correctMAI();
        MAI_CountryUprotect = plots.calculateAverageMAI();
//        countriesWoodProdStat = setCountriesWoodProdStat();
        countriesFmEmission_unfccc = setCountriesFmEmission_unfccc();
        calcAvgFM_sink_stat();
        fmp.initCountriesHLVE(plots.filteredPlots);
        fmp.setupFMP();
        nuts2.fillFromNUTS();
        nuts2.correctNUTS2Data(plots.filteredPlots);
        plots.setAsIds();

        CO2_price_future.get();
        globiom_land_country_future.get();
        globiom_datamaps_future.get();
        disturbances_future.get();
        disturbances_extreme_future.get();
        globiom_land_future.get();
        MAI_future.get();
        biomass_bau_future.get();
        age_struct_data_future.get();

        // start calculations
        initGlobiomLandAndManagedForest();
        initLoop();
        if constexpr (zeroProdAreaInit)
            initZeroProdArea();
        else
            INFO("initZeroProdArea is turned off");

//        plots.printPlots();
//        nuts2.printNuts2Id();
//        datamapScenarios.printData();
//        simuIdScenarios.printData();
//        for (const auto &[i, area]: countryLandArea | rv::enumerate)
//            TRACE("{} area = {}", idCountryGLOBIOM.at(i), area);
    }
}

#endif
