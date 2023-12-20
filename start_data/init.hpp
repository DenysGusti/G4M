#ifndef G4M_EUROPE_DG_INIT_HPP
#define G4M_EUROPE_DG_INIT_HPP

#include "start_data_functions.hpp"

namespace g4m::StartData {
    // Initialisation of input datasets
    void Init() {
        future<void> settings_future = async(launch::async, [] {
            Log::Init("settings");
            settings.readSettings("settings_Europe_dw_v02.ini");
        });

        settings_future.get();

        future<void> coef_future = async(launch::async, [] {
            Log::Init("coef");
            coef.readCoef(settings.coefPath);
        });

        future<void> NUTS2_future = async(launch::async, [] {
            Log::Init("NUTS2");
            nuts2id = readNUTS2();
        });

        future<void> plots_future = async(launch::async, [] {
            Log::Init("plots");
            rawPlots = readPlots();
            commonPlots = filterPlots(rawPlots);
            plotsSimuID = initPlotsSimuID(rawPlots);
            plotsXY_SimuID = initPlotsXY_SimuID(rawPlots);
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
            Log::Init("MAI");
            correctMAI(commonPlots);
            MAI_CountryUprotect = calculateAverageMAI(commonPlots);
            maiClimateShiftersScenarios = readMAIClimate();
            scaleMAIClimate2020(maiClimateShiftersScenarios);
        });

        future<void> globiom_land_future = async(launch::async, [] {
            Log::Init("globiom_land");
            readGlobiomLandCalibrate();
            readGlobiomLand();
        });

        future<void> globiom_land_country_future = async(launch::async, [] {
            Log::Init("globiom_land_country");
            countryLandArea = datamapScenarios.readGlobiomLandCountryCalibrate_calcCountryLandArea();
            datamapScenarios.readGlobiomLandCountry();
        });

        future<void> disturbances_future = async(launch::async, [] {
            Log::Init("disturbances");
            readDisturbances();
            add2020Disturbances();
            scaleDisturbances2020();
        });

        future<void> disturbances_extreme_future = async(launch::async, [] {
            Log::Init("disturbances_extreme");
            readDisturbancesExtreme();
        });

        future<void> biomass_bau_future = async(launch::async, [] {
            Log::Init("biomass_bau");
            if (fmPol && binFilesOnDisk) {
                const auto &[scenario, bau_vec] = findAndReadBau("biomass_bau");
                biomassBauScenarios[scenario] = bau_vec;
            } else
                INFO("biomass_bau reading is turned off");
        });

        future<void> NPV_bau_future = async(launch::async, [] {
            Log::Init("NPV_bau");
            if (fmPol && binFilesOnDisk) {
                const auto &[scenario, bau_vec] = findAndReadBau("NPVbau");
                NPVBauScenarios[scenario] = bau_vec;
            } else
                INFO("NPV_bau reading is turned off");
        });

        countriesWoodProdStat = setCountriesWoodProdStat();
        countriesFmEmission_unfccc = setCountriesFmEmission_unfccc();
        calcAvgFM_sink_stat();
        setupFMP();
        setCountryData(countriesList);
        nuts2grid.fillFromNUTS(nuts2id);
        correctNUTS2Data(commonPlots);
        countriesFFIpols = initCountriesFFIpols(commonPlots);
        setAsIds(commonPlots);

        CO2_price_future.get();
        globiom_land_country_future.get();
        globiom_datamaps_future.get();
        disturbances_future.get();
        disturbances_extreme_future.get();
        globiom_land_future.get();
        MAI_future.get();
        biomass_bau_future.get();
        NPV_bau_future.get();

        // start calculations
        initGlobiomLandAndManagedForest();
        initLoop();
        initZeroProdArea();

//        printData();
    }
}

#endif
