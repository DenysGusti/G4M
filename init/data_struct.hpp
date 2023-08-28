#ifndef G4M_EUROPE_DG_DATASTRUCT_HPP
#define G4M_EUROPE_DG_DATASTRUCT_HPP

#include <cmath>
#include <string>
#include <span>
#include <optional>

#include "../misc/concrete/ipol.hpp"
#include "../constants.hpp"

using namespace g4m::misc::concrete;

namespace g4m::init {

    struct DataStruct {
        size_t x = 0;
        size_t y = 0;
        size_t simuID = 0;

        uint8_t country = 0;
        uint8_t IIASA_region = 0;
        uint8_t polesReg = 0;
        uint8_t countryRegMix = 0;
        uint8_t speciesType = 0;

        int8_t mngmType = 0;

        bool managedFlag = false;
        bool managed_UNFCCC = false;  // Flag: true: the cell is counted as managed land for the UNFCCC reporting

        double landArea = 0;
        double forest = 0;
        double forLoss = 0;
        double agrSuit = 0;
        double sAgrSuit = 0;
        double CAboveHa = 0;
        double CBelowHa = 0;
        double CDeadHa = 0;
        double CLitterHa = 0;
        double SOCHa = 0;
        double managedShare = 0;
        double residuesUseShare = 0;
        double residuesUseCosts = 0;
        double deadWood = 0;
        double oldGrowthForest_ten = 0;
        double oldGrowthForest_thirty = 0;
        double strictProtected = 0;
        double forestAll = 0;
        double forest_correction = 0;
        double GL_correction = 0;
        double natLnd_correction = 0;
        double grLnd_protect = 0;  // Natural vegetation and heathland protected under Nature2000 or to be protected! EU27 only!

        Ipol<double> potVeg;
        Ipol<double> protect;
        Ipol<double> NPP;
        Ipol<double> popDens;
        Ipol<double> sPopDens; // MG: added because it's used in forest_calculations
        Ipol<double> priceIndex;
        Ipol<double> R;
        Ipol<double> GDP;
        Ipol<double> builtUp;
        Ipol<double> crop;
        Ipol<double> fracLongProd;
        Ipol<double> corruption;
        Ipol<double> slashBurn;
        Ipol<double> decHerb;
        Ipol<double> decWood;
        Ipol<double> decSOC;
        Ipol<double> fTimber;
        Ipol<double> MAIE;
        Ipol<double> MAIN;
        Ipol<double> road;
        Ipol<double> GLOBIOM_reserved;
        Ipol<double> afforMax;
        Ipol<double> harvestCosts;

        DataStruct() = default;

        DataStruct(const span<pair<string, optional<uint16_t> > > header,
                   const span<const double> line_cells) {
            buildFromCSVLine(header, line_cells);
        }

        void buildFromCSVLine(const span<pair<string, optional<uint16_t> > > header,
                              const span<const double> line_cells) {
            for (const auto &[col_name_opt_year, cell]: rv::zip(header, line_cells)) {
                const auto &[name, pYyear] = col_name_opt_year;
                if (pYyear) {
                    // Ipol vars
                    if (name == "BUILTUP")
                        builtUp.data[*pYyear] = cell;
                    else if (name == "CROP")
                        crop.data[*pYyear] = cell;
                    else if (name == "POPDENS")
                        popDens.data[*pYyear] = cell;
                    else if (name == "SPOPDENS")
                        sPopDens.data[*pYyear] = cell;
                    else if (name == "GDP")
                        GDP.data[*pYyear] = cell;
                    else if (name == "GLOBIOM_RESERVED")
                        GLOBIOM_reserved.data[*pYyear] = cell;
                    else
                        WARN("Unused DataStruct parameter: {} | {}", name, *pYyear);
                } else {
                    // integral vars
                    if (name == "X")
                        x = lround((cell + 180) / gridStep - 0.5);
                    else if (name == "Y")
                        y = lround((cell + 90) / gridStep - 0.5);
                    else if (name == "SIMUID")
                        simuID = static_cast<size_t>(cell);
                    else if (name == "COUNTRY")
                        country = static_cast<uint8_t>(cell);
                    else if (name == "IIASA_REGION")
                        IIASA_region = static_cast<uint8_t>(cell);
                    else if (name == "POLESREG")
                        polesReg = static_cast<uint8_t>(cell);
                    else if (name == "COUNTRYREGMIX")
                        countryRegMix = static_cast<uint8_t>(cell);
                    else if (name == "MANAGEDFLAG")
                        managedFlag = static_cast<uint8_t>(cell);
                    else if (name == "MANAGED_UNFCCC")
                        managed_UNFCCC = static_cast<bool>(cell);
                    else if (name == "MNGMTYPE")
                        mngmType = static_cast<int8_t>(cell);
                    else if (name == "SPECIESTYPE")
                        speciesType = static_cast<uint8_t>(cell);
                        // floating point vars
                    else if (name == "LANDAREA")
                        landArea = cell;
                    else if (name == "FOREST")
                        forest = cell;
                    else if (name == "FORLOSS")
                        forLoss = cell;
                    else if (name == "AGRSUIT")
                        agrSuit = cell;
                    else if (name == "SAGRSUIT")
                        sAgrSuit = cell;
                    else if (name == "CABOVEHA")
                        CAboveHa = cell;
                    else if (name == "CBELOWHA")
                        CBelowHa = cell;
                    else if (name == "CDEADHA")
                        CDeadHa = cell;
                    else if (name == "CLITTERHA")
                        CLitterHa = cell;
                    else if (name == "SOCHA")
                        SOCHa = cell;
                    else if (name == "MANAGEDSHARE")
                        managedShare = cell;
                    else if (name == "RESIDUESUSESHARE")
                        residuesUseShare = cell;
                    else if (name == "RESIDUESUSECOSTS")
                        residuesUseCosts = cell;
                    else if (name == "DEADWOOD")
                        deadWood = cell;
                    else if (name == "OLDGROWTHFOREST_TEN")
                        oldGrowthForest_ten = cell;
                    else if (name == "OLDGROWTHFOREST_THIRTY")
                        oldGrowthForest_thirty = cell;
                    else if (name == "STRICTPROTECTED")
                        strictProtected = cell;
                    else if (name == "FORESTALL")
                        forestAll = cell;
                    else if (name == "FOREST_CORRECTION")
                        forest_correction = cell;
                    else if (name == "GL_CORRECTION")
                        GL_correction = cell;
                    else if (name == "NATLND_CORRECTION")
                        natLnd_correction = cell;
                    else if (name == "GRLND_PROTECT")
                        grLnd_protect = cell;
                        // Ipol vars with default key 2000
                    else if (name == "CORRUPTION")
                        corruption.data[2000] = cell;
                    else if (name == "R")
                        R.data[2000] = cell;
                    else if (name == "PRICEINDEX")
                        priceIndex.data[2000] = cell;
                    else if (name == "FTIMBER")
                        fTimber.data[2000] = cell;
                    else if (name == "POTVEG")
                        potVeg.data[2000] = cell;
                    else if (name == "PROTECT")
                        protect.data[2000] = cell;
                    else if (name == "MAIE")
                        MAIE.data[2000] = cell;
                    else if (name == "MAIN")
                        MAIN.data[2000] = cell;
                    else if (name == "NPP")
                        NPP.data[2000] = cell;
                    else if (name == "ROAD")
                        road.data[2000] = cell;
                    else if (name == "FRACLONGPROD")
                        fracLongProd.data[2000] = cell;
                    else if (name == "SLASHBURN")
                        slashBurn.data[2000] = cell;
                    else if (name == "DECHERB")
                        decHerb.data[2000] = cell;
                    else if (name == "DECWOOD")
                        decWood.data[2000] = cell;
                    else if (name == "DECSOC")
                        decSOC.data[2000] = cell;
                    else if (name == "HARVESTCOSTS")
                        harvestCosts.data[2000] = cell;
                    else if (name == "AFFORMAX")
                        afforMax.data[2000] = cell;
                    else
                        WARN("Unused DataStruct parameter: {}", name);
                }
            }
        }

        [[nodiscard]] string str() const noexcept {
            string format_integral = format(
                    "x = {}\ny = {}\nsimuID = {}\ncountry = {}\nIIASA_region = {}\npolesReg = {}\ncountryRegMix = {}\nspeciesType = {}\nmngmType = {}\nmanagedFlag = {}\nmanaged_UNFCCC = {}\n",
                    x, y, simuID, country, IIASA_region, polesReg, countryRegMix, speciesType, mngmType, managedFlag,
                    managed_UNFCCC);
            string format_floating_point = format(
                    "landArea = {}\nforest = {}\nforLoss = {}\nagrSuit = {}\nsAgrSuit = {}\nCAboveHa = {}\nCBelowHa = {}\nCDeadHa = {}\nCLitterHa = {}\nSOCHa = {}\nmanagedShare = {}\nresiduesUseShare = {}\nresiduesUseCosts = {}\ndeadWood = {}\noldGrowthForest_ten = {}\noldGrowthForest_thirty = {}\nstrictProtected = {}\nforestAll = {}\nforest_correction = {}\nGL_correction = {}\nnatLnd_correction = {}\ngrLnd_protect = {}\n",
                    landArea, forest, forLoss, agrSuit, sAgrSuit, CAboveHa, CBelowHa, CDeadHa, CLitterHa, SOCHa,
                    managedShare, residuesUseShare, residuesUseCosts, deadWood, oldGrowthForest_ten,
                    oldGrowthForest_thirty, strictProtected, forestAll, forest_correction, GL_correction,
                    natLnd_correction, grLnd_protect);
            string format_ipol = format(
                    "potVeg:\n{}\nprotect:\n{}\nNPP:\n{}\npopDens:\n{}\nsPopDens\n{}\npriceIndex:\n{}\nR:\n{}\nGDP:\n{}\nbuiltUp:\n{}\ncrop:\n{}\nfracLongProd:\n{}\ncorruption:\n{}\nslashBurn:\n{}\ndecHerb:\n{}\ndecWood:\n{}\ndecSOC:\n{}\nfTimber:\n{}\nMAIE:\n{}\nMAIN:\n{}\nroad:\n{}\nGLOBIOM_reserved:\n{}\nafforMax:\n{}\nharvestCosts:\n{}\n",
                    potVeg.str(), protect.str(), NPP.str(), popDens.str(), sPopDens.str(), priceIndex.str(), R.str(),
                    GDP.str(), builtUp.str(), crop.str(), fracLongProd.str(), corruption.str(), slashBurn.str(),
                    decHerb.str(), decWood.str(), decSOC.str(), fTimber.str(), MAIE.str(), MAIN.str(), road.str(),
                    GLOBIOM_reserved.str(), afforMax.str(), harvestCosts.str());
            return format_integral + format_floating_point + format_ipol;
        }

        friend ostream &operator<<(ostream &os, const DataStruct &obj) {
            os << obj.str();
            return os;
        }
    };
}

#endif
