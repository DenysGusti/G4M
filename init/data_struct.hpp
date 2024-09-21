#ifndef G4M_EUROPE_DG_DATASTRUCT_HPP
#define G4M_EUROPE_DG_DATASTRUCT_HPP

#include <cmath>
#include <string>
#include <span>
#include <optional>

#include "../misc/concrete/ipol.hpp"
#include "../settings/constants.hpp"
#include "species.hpp"
#include "vegetation_type.hpp"

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

        Species speciesType = Species::NoTree;

        int8_t mngmType = 0;
        VegetationType potVeg = VegetationType::NoType;

        bool managedFlag = false;
        bool managed_UNFCCC = false;  // Flag: true: the cell is counted as managed land for the UNFCCC reporting
        bool protect = false;

        double landArea = 0;
        double forest = 0;
        double forLoss = 0;
        double agrSuit = 0;
        double sAgrSuit = 0;
        double CAboveHa = 0;
        double CBelowHa = 0;
        double CDeadHa = 0;
        double CLitterHa = 0;
        double SOCHa = 0;   // soil organic carbon per ha
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
        double grLnd_protect = 0;   // Natural vegetation and heathland protected under Nature2000 or to be protected! EU27 only!
        double corruption = 0;
        double fTimber = 0;
        double priceIndex = 0;
        double slashBurn = 0;       // share of burnt wood at deforestation
        double fracLongProd = 0;
        double decHerb = 0;
        double decWood = 0;
        double decSOC = 0;
        double harvestCosts = 0;

        Ipol<double> NPP;
        Ipol<double> popDens;
        Ipol<double> sPopDens; // MG: added because it's used in forest_calculations
        Ipol<double> R;
        Ipol<double> GDP;
        Ipol<double> builtUp;
        Ipol<double> crop;
        Ipol<double> MAIE;
        Ipol<double> MAIN;
        Ipol<double> road;
        Ipol<double> GLOBIOM_reserved;
        Ipol<double> afforMax;

        DataStruct() = default;

        DataStruct(const span<pair<string, optional<uint16_t> > > header,
                   const span<const double> line_cells) {
            buildFromCSVLine(header, line_cells);
        }

        [[nodiscard]] pair<double, double> getInputCoordinates() const noexcept {
            return {(x + 0.5) * gridStep - 180, (y + 0.5) * gridStep - 90};
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
                    else if (name == "PROTECT")
                        protect = static_cast<bool>(cell);
                    else if (name == "MNGMTYPE")
                        mngmType = static_cast<int8_t>(cell);
                    else if (name == "POTVEG")
                        potVeg = static_cast<VegetationType>(cell);
                    else if (name == "SPECIESTYPE")
                        speciesType = static_cast<Species>(cell);
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
                    else if (name == "CORRUPTION")
                        corruption = cell;
                    else if (name == "FTIMBER")
                        fTimber = cell;
                    else if (name == "PRICEINDEX")
                        priceIndex = cell;
                    else if (name == "FRACLONGPROD")
                        fracLongProd = cell;
                    else if (name == "SLASHBURN")
                        slashBurn = cell;
                    else if (name == "DECHERB")
                        decHerb = cell;
                    else if (name == "DECWOOD")
                        decWood = cell;
                    else if (name == "DECSOC")
                        decSOC = cell;
                    else if (name == "HARVESTCOSTS")
                        harvestCosts = cell;
                        // Ipol vars with default key 2000
                    else if (name == "R")
                        R.data[2000] = cell;
                    else if (name == "MAIE")
                        MAIE.data[2000] = cell;
                    else if (name == "MAIN")
                        MAIN.data[2000] = cell;
                    else if (name == "NPP")
                        NPP.data[2000] = cell;
                    else if (name == "ROAD")
                        road.data[2000] = cell;
                    else if (name == "AFFORMAX")
                        afforMax.data[2000] = cell;
                    else
                        WARN("Unused DataStruct parameter: {}", name);
                }
            }
        }

        [[nodiscard]] string str() const noexcept {
            string format_integral = format(
                    "x = {}\ny = {}\nsimuID = {}\ncountry = {}\nIIASA_region = {}\npolesReg = {}\ncountryRegMix = {}\n"
                    "speciesType = {}\nmngmType = {}\npotVeg = {}\nmanagedFlag = {}\nmanaged_UNFCCC = {}\n"
                    "protect = {}\n",
                    x, y, simuID, country, int{IIASA_region}, int{polesReg}, int{countryRegMix},
                    speciesName.at(speciesType), int{mngmType}, vegetationTypeName.at(potVeg), managedFlag,
                    managed_UNFCCC, protect);
            string format_floating_point = format(
                    "landArea = {}\nforest = {}\nforLoss = {}\nagrSuit = {}\nsAgrSuit = {}\nCAboveHa = {}\n"
                    "CBelowHa = {}\nCDeadHa = {}\nCLitterHa = {}\nSOCHa = {}\nmanagedShare = {}\n"
                    "residuesUseShare = {}\nresiduesUseCosts = {}\ndeadWood = {}\noldGrowthForest_ten = {}\n"
                    "oldGrowthForest_thirty = {}\nstrictProtected = {}\nforestAll = {}\nforest_correction = {}\n"
                    "GL_correction = {}\nnatLnd_correction = {}\ngrLnd_protect = {}\ncorruption = {}\nfTimber = {}\n"
                    "priceIndex = {}\nfracLongProd = {}\nslashBurn = {}\ndecHerb = {}\ndecWood = {}\ndecSOC = {}\n"
                    "harvestCosts = {}\n",
                    landArea, forest, forLoss, agrSuit, sAgrSuit, CAboveHa, CBelowHa, CDeadHa, CLitterHa, SOCHa,
                    managedShare, residuesUseShare, residuesUseCosts, deadWood, oldGrowthForest_ten,
                    oldGrowthForest_thirty, strictProtected, forestAll, forest_correction, GL_correction,
                    natLnd_correction, grLnd_protect, corruption, fTimber, priceIndex, fracLongProd, slashBurn, decHerb,
                    decWood, decSOC, harvestCosts);
            string format_ipol = format(
                    "NPP:\n{}\npopDens:\n{}\nsPopDens\n{}\nR:\n{}\nGDP:\n{}\nbuiltUp:\n{}\ncrop:\n{}\nMAIE:\n{}\n"
                    "MAIN:\n{}\nroad:\n{}\nGLOBIOM_reserved:\n{}\nafforMax:\n{}\n",
                    NPP.str(), popDens.str(), sPopDens.str(), R.str(), GDP.str(), builtUp.str(),
                    crop.str(), MAIE.str(), MAIN.str(), road.str(), GLOBIOM_reserved.str(), afforMax.str());
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
            oldGrowthForest_ten -= strictProtected; // Now forest10 is just additional to the primary forest to be protected
        }

        [[nodiscard]] optional<double> initForestArea(double dfor) noexcept {
            if (forest >= dfor) {
                forest -= dfor;
                return nullopt;
            } else if (forest + oldGrowthForest_thirty >= dfor) {
                dfor -= forest;
                forest = 0;
                oldGrowthForest_thirty -= dfor;
                return nullopt;
            } else if (forest + oldGrowthForest_thirty + oldGrowthForest_ten >= dfor) {
                dfor -= forest + oldGrowthForest_thirty;
                forest = 0;
                oldGrowthForest_thirty = 0;
                oldGrowthForest_ten -= dfor;
                return nullopt;
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

        [[nodiscard]] double getMaxForestShare(const uint16_t year) const {
            return 1 - GLOBIOM_reserved(year);
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

        const double tC_m3 = 4 changed to fTimber
        */
        [[nodiscard]] double BEF(const double growingStockC) const {
            double growingStock = growingStockC * fTimber;

            if (growingStock <= 0)
                return 1;

            switch (speciesType) {
                case Species::Fir:
                    return clamp(1.069 + 1.919 * pow(growingStock, -0.524), 1.1, 3.5);
                case Species::Spruce:
                    return clamp(1.204 + 0.903 * exp(-0.009 * growingStock), 1.1, 4.);
                case Species::Pine:
                case Species::PinusHalepensis:
                    return clamp(0.949 + 3.791 * pow(growingStock, -0.501), 1.1, 6.);
                case Species::Birch:
                    return clamp(1.105 + 9.793 / growingStock, 1.1, 1.6);
                case Species::Beech:
                    return clamp(1.197 + 0.386 * exp(-0.009 * growingStock), 1.1, 3.5);
                case Species::Oak:
                    return clamp(1.202 + 0.422 * exp(-0.013 * growingStock), 1.1, 3.5);
                case Species::Larch:
                    return clamp(1.023 + 2.058 * pow(growingStock, -0.508), 1.1, 3.5);
                default:
                    ERROR("Unknown speciesType: {}", static_cast<int>(speciesType));
                    return nan("");
            }
        }

        /*
        Functions for estimation of biomass of stump+coarse roots from dbh
        Mykola Gusti 27 July 2017
        For each cell we estimate amount of wood in stumps of the felled trees based on the diameter and dbh of the felled trees and species specific allometric equations. Since the allometric equations provide the stump weight per tree, we estimate the number of felled trees by estimating the weight (or volume in case of fir and larch) of one tree and dividing the felled volume by the volume of one tree.
        Fore estimating the stumps weight, the following references were used:
        For Fir, Beech and Oak (Drexhage and Colin, 2001)
        For Birch (Repola, 2008).
        For Spruce (Eq.N269), Pine, and Larch (pine equation is applied) (Eq.N474) from (Zianis et al., 2005).

        For estimating the stem weight the following allometric equations were applied:
        Beech (Eq.N 282), Birch (Eq.N72), Fir (volume, Eq.N.7), Larch (volume, Eq.N.66), Oak (Eq. N. 597), Pine (Eq.N.491) and Spruce (Eq.N.310) from (Zianis et al., 2005).

        References
        Drexhage M., Colin F. (2001) Estimating root system biomass from breast-height diameters. Forestry, Vol.74, N.5, pp.491-497
        Repola J. (2008) Biomass equations for birch in Finland. Silva Fennica 42 (4): 605-624
        Zianis, D., Muukkonen, P., M�kip��, R. & Mencuccini, M. 2005. Biomass and stem volume equations for tree species in Europe. Silva Fennica Monographs 4. 63 p.

        dbh - diameter of stem at breast height, cm
        h - height of tree, m
        felledTreesStem - weight of felled trees, tC
        stump + coarse root biomass is in tC/tree
        original functions are multiplied by (0.5 * 1.0e-3) to convert to carbon and ton

        fTimber changed to fTimber
        */
        [[nodiscard]] double DBHHToStump(const double dbh, const double h, double felledTreesStem) const {
            if (dbh <= 0 || h <= 0 || felledTreesStem <= 0 || fTimber <= 0)
                return 0;

            double str = 0;
            switch (speciesType) {
                case Species::Fir: {
                    // stump + coarse roots tC per tree
                    double strOneTree = pow(dbh, 2.33) * pow(10, -1.55) * 5e-4;
                    // volume of stem of one tree
                    double V = pow(dbh, 1.942) * pow(h, 0.9836) * 4.52e-5;
                    // weight of one tree stem
                    double treeStem = V / fTimber;
                    // number of felled trees per cell if felledTreesStem is per cell
                    double treesNumber = felledTreesStem / treeStem;
                    // stump + roots tC per felled trees
                    str = strOneTree * treesNumber;
                }
                    break;

                case Species::Spruce: {
                    // stump + coarse roots tC per tree
                    double strOneTree = exp(-2.4447 + 10.5381 * dbh / (dbh + 14)) * 5e-4;
                    // weight of one tree stem
                    double treeStem = pow(dbh, 1.5953) * pow(h, 0.9336) * 0.0558 * 5e-4;
                    // number of felled trees per cell if felledTreesStem is per cell
                    double treesNumber = felledTreesStem / treeStem;
                    // stump + roots tC per felled trees
                    str = strOneTree * treesNumber;
                }
                    break;

                case Species::Pine:
                case Species::PinusHalepensis: {
                    // stump + coarse roots tC per tree
                    double strOneTree = exp(-3.3913 + 11.1106 * dbh / (dbh + 12)) * 5e-4;
                    // weight of one tree stem
                    double treeStem = exp(-2.6768 + 7.5939 * dbh / (dbh + 13) + 0.0151 * h) * pow(h, 0.8799) * 5e-4;
                    // number of felled trees per cell if felledTreesStem is per cell
                    double treesNumber = felledTreesStem / treeStem;
                    // stump + roots tC per felled trees
                    str = strOneTree * treesNumber;
                }
                    break;

                case Species::Birch: {
                    double tmp_num = 2 + 1.25 * dbh;
                    // stump
                    double stump = exp(-3.574 + 11.304 * tmp_num / (tmp_num + 26)) * 5e-4;
                    // roots > 1cm d
                    double roots = exp(-3.223 + 6.497 * tmp_num / (tmp_num + 26)) * 5e-4;
                    // stump + coarse roots tC per tree
                    double strOneTree = stump + 0.8 * roots;
                    // weight of one tree stem
                    double treeStem = exp(-3.5686 + 8.2827 * dbh / (dbh + 7) + 0.0393 * h) * pow(h, 0.5772) * 5e-4;
                    // number of felled trees per cell if felledTreesStem is per cell
                    double treesNumber = felledTreesStem / treeStem;
                    // stump + roots tC per felled trees
                    str = strOneTree * treesNumber;
                }
                    break;

                case Species::Beech: {
                    // stump + coarse roots tC per tree
                    double strOneTree = pow(dbh, 2.54) * pow(10, -1.66) * 5e-4;
                    // weight of one tree stem
                    double treeStem = exp(-2.2052 + 7.4361 * dbh / (dbh + 14) + 0.0186 * h) * pow(h, 0.7595) * 5e-4;
                    // number of felled trees per cell if felledTreesStem is per cell
                    double treesNumber = felledTreesStem / treeStem;
                    // stump + roots tC per felled trees
                    str = strOneTree * treesNumber;
                }
                    break;

                case Species::Oak: {
                    // stump + coarse roots tC per tree
                    double strOneTree = pow(dbh, 2.19) * pow(10, -1.05) * 5e-4;
                    // weight of one tree stem
                    double treeStem = pow(dbh, 2.157) * pow(10, -1.088 + 0.039 * h) * 5e-4;
                    // number of felled trees per cell if felledTreesStem is per cell
                    double treesNumber = felledTreesStem / treeStem;
                    // stump + roots tC per felled trees
                    str = strOneTree * treesNumber;
                }
                    break;

                case Species::Larch: {
                    // stump + roots tC per tree
                    double strOneTree = exp(-3.3913 + 11.1106 * dbh / (dbh + 12)) * 0.5 * 1.0e-3;
                    // volume of stem of one tree
                    double V = -0.0011638 * h - 0.03088 +
                               dbh * (dbh * (-4.8614e-5 - 3.8178e-6 * dbh + 4.0597e-5 * h) + 0.004676261);
                    // weight of one tree stem
                    double treeStem = V / fTimber;
                    // number of felled trees per cell if felledTreesStem is per cell
                    double treesNumber = felledTreesStem / treeStem;
                    // stump + roots tC per felled trees
                    str = strOneTree * treesNumber;
                }
                    break;

                default:
                    ERROR("Unknown speciesType: {}", static_cast<int>(speciesType));
                    str = nan("");
            }

            return str;
        }

        [[nodiscard]] double afforestationSoilInputCoef() const noexcept {
            switch (potVeg) {
                case VegetationType::TemperateNeedleleafEvergreenForest:
                case VegetationType::BorealEvergreenForest:
                    return 0.04;
                case VegetationType::MixedForest:
                    return 0.2;
                default:
                    return 0.35;
            }
        }

        // below-ground phytomass of planted forest = 18% for tropical, 22% for temperate and 25% for boreal of above-ground phytomass
        [[nodiscard]] double coefBL() const noexcept {
            switch (potVeg) {
                case VegetationType::TropicalEvergreenForest:
                case VegetationType::TropicalDeciduousForest:
                    return 0.18;
                    // commented because value is default
//                case VegetationType::TemperateBroadleafEvergreenForest:
//                case VegetationType::TemperateNeedleleafEvergreenForest:
//                case VegetationType::TemperateDeciduousForest:
//                case VegetationType::MixedForest:
//                    return 0.22;
                case VegetationType::BorealEvergreenForest:
                case VegetationType::BorealDeciduousForest:
                    return 0.25;
                default:
                    return 0.22;
            }
        }
    };
}

#endif
