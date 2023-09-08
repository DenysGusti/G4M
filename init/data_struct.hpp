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
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t simuID = 0;
        size_t asID = 0;

        uint8_t country = 0;
        uint8_t IIASA_region = 0;
        uint8_t polesReg = 0;
        uint8_t countryRegMix = 0;

        // 1 - fir
        // 2 - spruce
        // 3 - pine
        // 4 - pinus Halepensis
        // 5 - birch
        // 6 - beech
        // 7 - oak
        // 8 - larch
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
        double strictProtected = 0;  // PRIMARYFOREST
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
                        simuID = static_cast<uint32_t>(cell);
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

        void initForestArrange() noexcept {
            if (strictProtected > 0) {
                oldGrowthForest_ten = max(oldGrowthForest_ten, strictProtected);
                oldGrowthForest_thirty = max(oldGrowthForest_thirty, strictProtected);
            }
            oldGrowthForest_thirty = max(oldGrowthForest_thirty, oldGrowthForest_ten);

            if (oldGrowthForest_thirty > forest) {
                oldGrowthForest_thirty = forest;
                oldGrowthForest_ten = min(oldGrowthForest_ten, oldGrowthForest_thirty);
                strictProtected = min(strictProtected, oldGrowthForest_ten);
            }
        }

        void forestsArrangement() noexcept {
            forest = max(0., forest - oldGrowthForest_thirty);  // Now it's just a "usual" old forest
            oldGrowthForest_thirty -= oldGrowthForest_ten;  // forest10 includes primary forest
            oldGrowthForest_ten -= forest; // Now forest10 is just additional to the primary forest to be protected
        }

        [[nodiscard]] optional<double> initForestArea(double dfor) noexcept {
            if (forest >= dfor) {
                forest -= dfor;
                return {};
            } else if (forest + oldGrowthForest_thirty >= dfor) {
                dfor -= forest;
                forest = 0;
                oldGrowthForest_thirty -= dfor;
                return {};
            } else if (forest + oldGrowthForest_thirty + oldGrowthForest_ten >= dfor) {
                dfor -= forest + oldGrowthForest_thirty;
                forest = 0;
                oldGrowthForest_thirty = 0;
                oldGrowthForest_ten -= dfor;
                return {};
            } else {
                dfor -= forest + oldGrowthForest_thirty + oldGrowthForest_ten;
                forest = 0;
                oldGrowthForest_thirty = 0;
                oldGrowthForest_ten = 0;
                GLOBIOM_reserved.data[2000] -= dfor;
                return {dfor};  // postponed dfor subtraction
            }
        }

        [[nodiscard]] pair<double, double> getGeographicCoordinates() const noexcept {
            return {(x + 0.5) * gridStep - 180, (y + 0.5) * gridStep - 90};
        }

        // all old forest in the cell
        [[nodiscard]] double getForestShare() const noexcept {
            return forest + oldGrowthForest_ten + oldGrowthForest_thirty + strictProtected;
        }

        [[nodiscard]] double getMaxAffor() const {
            return 1 - GLOBIOM_reserved.data.at(2000);
        }

        /*
        Biomass Expansion Factor (BEF) functions
        Mykola Gusti, 11 April 2013
        The functions are from
        Teobaldelli M, Somogyi Z., Migliavacca M., Usoltsev V. (2009)
        Generalized functions of biomass expansion factors for conifers and broadleaved by stand age, growing stock and site index
        Forest Ecology and Management, N257, 1004-1013
        BEF functions are used to estimate total above-ground biomass (including leaves) [m3/ha] from growing stock data [m3/ha]
        Brown S. (1997) Estimating Biomass and Biomass Change of Tropical Forests: a Primer. (FAO Forestry Paper - 134, FAO 1997)

        const double tC_m3 = 4 changed to fTimber.data.at(2000)
        */
        [[nodiscard]] double BEF(const double growingStockC) const {
            double growingStock = growingStockC * fTimber.data.at(2000);
            double bef = 1;
            if (growingStock > 0) {
                switch (speciesType) {
                    case 1:
                        bef = clamp(1.069 + 1.919 * pow(growingStock, -0.524), 1.1, 3.5);
                        break;
                    case 2:
                        bef = clamp(1.204 + 0.903 * exp(-0.009 * growingStock), 1.1, 4.);
                        break;
                    case 3:
                    case 4:
                        bef = clamp(0.949 + 3.791 * pow(growingStock, -0.501), 1.1, 6.);
                        break;
                    case 5:
                        bef = clamp(1.105 + 9.793 / growingStock, 1.1, 1.6);
                        break;
                    case 6:
                        bef = clamp(1.197 + 0.386 * exp(-0.009 * growingStock), 1.1, 3.5);
                        break;
                    case 7:
                        bef = clamp(1.202 + 0.422 * exp(-0.013 * growingStock), 1.1, 3.5);
                        break;
                    case 8:
                        bef = clamp(1.023 + 2.058 * pow(growingStock, -0.508), 1.1, 3.5);
                        break;
                    default:
                        ERROR("Unknown speciesType: {}", speciesType);
                }
            }
            return bef;
        }
    };
}

#endif
