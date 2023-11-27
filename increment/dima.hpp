#ifndef G4M_EUROPE_DG_DIMA_HPP
#define G4M_EUROPE_DG_DIMA_HPP

#include <cmath>
#include <numeric>
#include <numbers>

#include "../constants.hpp"
#include "../misc/concrete/ipol.hpp"

using namespace std;
using namespace std::numbers;
using namespace g4m::misc::concrete;
using namespace g4m::Constants;

namespace g4m::increment {
    // Calculating costs for planting and managing forest stands for estimating land use change decisions.
    // Based on DIMA LUC model (Kindermann et al. 2006, 2006; Rokityanskiy et al. 2007)
    class DIMA {
    public:
        // Value of Forestry during multiple rotation (Eq.4)
        // Changed to multiple years!!!!
        [[nodiscard]] double forVal() const noexcept {
            return calcNpvSum() * forestValueOne() * modTimeStep;
        }

        // Value of Forestry multiple rotation No Carbon Price
        // Changed to multiple years!!!!
        [[nodiscard]] double forValNC() const noexcept {
            return calcNpvSum() * forestValueOneNC() * modTimeStep;
            // Georg's definition (like in Kindermann et al. 2007)
            // return forestValueOneNC() / (1 - pow(1 +r(year), -rotInter())));
        }

        // Net present Value of Agriculture (Eq.5)
        [[nodiscard]] double agrVal() const noexcept {
            double priceLevel = priceLandMin0 * priceIndex(year) / priceIndex0;
            //Importance of Population density
            // double popImp = (log(priceLandMax0(year)) - log(priceLandMin0(year))) / (2 * log(10));
            double popImp = log(priceLandMax0 / priceLandMin0) * 0.5 * log10e;
            //Importance of the Suitable for Agriculture
            double agrImp = popImp;
            return priceLevel * pow(sPopDens(year), popImp) * pow(sAgrSuit, agrImp);
        }

        // Value of amenity
        [[nodiscard]] double amenVal() const noexcept {
            double priceLevel = priceLandMin0 * priceIndex(year) / priceIndex0;
            // Importance of Population density
            // double popImp = (log(priceLandMax0(year)) - log(priceLandMin0(year))) / (2 * log(10));
            double popImp = log(priceLandMax0 / priceLandMin0) * 0.5 * log10e;
            //Importance of GDP
            double gdpImp = popImp;
            return priceLevel * pow(sPopDens(year), popImp) * pow(gdp(year), gdpImp);
        }

        // Get the minimum carbon price where Forest = Argic (Eq.6)
        // MG: changed for external price correction
        [[nodiscard]] double minPriceC() const noexcept {
            double r_year = r(year);
            return (agrVal() * (1 - pow(1 + r_year, (-rotInter()))) + plantingCosts() -
                    (priceTimber() - priceHarvest()) * woodHarvestVol() * pow(1 + r_year, (-rotInter()))) /
                   (CUptake() * (1 - baseline) * (((1 - pow(1 + r_year, (-rotInter()))) / r_year) -
                                                  rotInter() * (1 - beta()) * pow(1 + r_year, (-rotInter()))));
        }

        void setYear(const int year_) noexcept {
            year = year_;
        }

        void setForest(const double x) noexcept {
            forest = clamp(x, 0., 1.);
        }

        // MG: Value of Forestry during multiple rotation with External Timber price
        // Changed to multiple years!!!!
        [[nodiscard]] double forValExt() const noexcept {
            return calcNpvSum() * forestValueOneExt() * modTimeStep;
        }

        // MG: Value of Forestry during multiple rotation with combined (G4M + External) Timber price
        // Changed to multiple years!!!!
        [[nodiscard]] double forValComb() const noexcept {
            return calcNpvSum() * forestValueOneComb() * modTimeStep;
        }

        // MG: Value of Forestry multiple rotation No Carbon Price using External wood price
        // Changed to multiple years!!!!
        [[nodiscard]] double forValNCExt() const noexcept {
            return calcNpvSum() * forestValueOneNCExt() * modTimeStep;
            // Georg's definition (like in Kindermann et al. 2007)
            // return forestValueOneNCExt() / (1 - pow(1 + r(year), -rotInter()));
        }

        // MG: Value of Forestry during multiple rotation with combined (G4M + External) Timber price
        // Changed to multiple years!!!!
        // No carbon price
        [[nodiscard]] double forValNCComb() const noexcept {
            return calcNpvSum() * forestValueOneNCComb() * modTimeStep;
        }

        // MG: Attention: agrVal changed! Only 2000 value is estimated here
        // MG: Net present Value of Agriculture in the year 2000
        [[nodiscard]] double agrVal2000() const noexcept {
            double priceLevel = priceLandMin0 * priceIndex(2000) / priceIndex0;
            // Importance of Population density
            double popImp = log(priceLandMax0 / priceLandMin0) * 0.5 * log10e;
            // Importance of the Suitable for Agriculture
            double agrImp = popImp;
            return priceLevel * pow(sPopDens(2000), popImp) * pow(sAgrSuit, agrImp);

        }

        // Costs to plant 1 ha of forest
        // Maybe these costs do not occur on the second rotation interval because of natural regeneration coppice forests
        // return plantingCosts0(year) * priceIndex(year) / priceIndex0(year);
        [[nodiscard]] double plantingCosts() const noexcept {
            double plantRate = clamp((vIncr() - 3) / 6, 0., 1.);
            return plantRate * plantingCosts0 * priceIndex(year) / priceIndex0;
        }

        DIMA(const uint16_t aYear,
             const Ipol<double> &aNpp,              // npp [kg-C/m2/year]
             const Ipol<double> &aSPopDens,         // Standardised (1-10) population density
             const double aSAgrSuit,         // Standardised (1-10) agricultural suitability
             const Ipol<double> &aPriceIndex,       // Price index
             const double aPriceIndex0,      // Price index ref Country
             const Ipol<double> &aR,                // discount rate
             const double aPriceC,           // carbon price [$/tC]
             const double aPlantingCosts0,   // Planting costs ref Country [$/ha]
             const double aPriceLandMin0,    // Minimum Land price in ref country [$/ha]
             const double aPriceLandMax0,    // Maximum Land price in ref country [$/ha]
             const double aMaxRotInter,      // Maximal rotation interval [years]
             const double aMinRotInter,      // Minimal rotation interval [years]
             const double aDecLongProd,      // Decay rate for long-lived prod
             const double aDecShortProd,     // Decay rate for short-lived pro
             const Ipol<double> &aFracLongProd,     // Fraction of carbon stored in long-term products
             const double aBaseline,         // Fraction of carbon subtracted due to baseline considerations
             const Ipol<double> &aFTimber,          // Commercial timber volume per ton of carbon [m3/tC]
             const double aPriceTimberMax0,  // Maximal timber price in reference country [$/m3]
             const double aPriceTimberMin0,  // Minimal timber price in reference country {$/m3]
             const double aFCUptake,         // Factor of carbon uptake from npp
             const Ipol<double> &aGdp,              // Gross domestic production
             const double aHarvLoos,         // Harvesting losses
             const double aForest,                  // Share of land area used by forest [0-1]
             const Ipol<double> &aWoodPriceCorr,       // MG: Added for wood price correction (+ in year 2000 when price of carbon = 0)
             const double aRotInterM,               // MG: Added rotation interval estimated from Georg's Forest Management Tool
             const double aHarvWood                 // MG: Added harvestable wood from Georg's Forest Management Tool
        ) : year{aYear}, npp{aNpp}, sPopDens{aSPopDens}, sAgrSuit{aSAgrSuit}, priceIndex{aPriceIndex},
            priceIndex0{aPriceIndex0}, r{aR}, priceC{aPriceC}, plantingCosts0{aPlantingCosts0},
            priceLandMin0{aPriceLandMin0}, priceLandMax0{aPriceLandMax0}, decLongProd{aDecLongProd},
            decShortProd{aDecShortProd}, fracLongProd{aFracLongProd}, baseline{aBaseline}, fTimber{aFTimber},
            priceTimberMax0{aPriceTimberMax0}, priceTimberMin0{aPriceTimberMin0}, fCUptake{aFCUptake}, gdp{aGdp},
            forest{aForest}, woodPriceCorr{aWoodPriceCorr}, woodPriceCorr0{aWoodPriceCorr(2000)}, rotInterM{aRotInterM},
            harvWood{aHarvWood} {
        }

        // Timber price internal
        [[nodiscard]] double priceTimber() const noexcept {
            double sfor = (1 - forest) * 9 + 1;
            double c4 = priceTimberMax0 - priceTimberMin0 / 99;
            double c3 = priceTimberMin0 - c4;
            return (c3 + c4 * sPopDens(year) * sfor) * priceIndex(year) / priceIndex0;
        }

        // MG: Timber price external
        // MG: use internal G4M wood price
        // MG: Changed to external SawnLogsPrice
        [[nodiscard]] double priceTimberExt() const noexcept {
            double sfor = (1 - forest) * 9 + 1;
            double c4 = (priceTimberMax0 - priceTimberMin0) / 99;
            double c3 = priceTimberMin0 - c4;
            return (c3 + c4 * sPopDens(2000) * sfor) * priceIndex(2000) / priceIndex0 * woodPriceCorr(year) /
                   woodPriceCorr0;
        }

        // MG: Combined timber price (G4M+external)
        // MG: use internal G4M wood price
        // MG: Changed to external SawnLogsPrice
        [[nodiscard]] double priceTimberComb() const noexcept {
            double sfor = (1 - forest) * 9 + 1;
            double c4 = (priceTimberMax0 - priceTimberMin0) / 99;
            double c3 = priceTimberMin0 - c4;
            return ((c3 + c4 * sPopDens(year) * sfor) * priceIndex(year) / priceIndex0 * woodPriceCorr(year) /
                    woodPriceCorr0);
        }

        // Rotation interval of a Forest in Years
        // MG: Use Georg's Optimal Rotation Time
        [[nodiscard]] double rotInter() const noexcept {
            return rotInterM;
        }

        // Harvest volume of the timber during 1 rotation period
        // MG: Georg's harvestable wood
        // Disturbances can also be mentioned
        [[nodiscard]] double woodHarvestVol() const noexcept {
            return harvWood * rotInter();
        }

        // mean annual carbon uptake (t-C/ha/year)
        // This value should depend on the NPP which is reduced by a factor
        // This factor should not be the same all over the world
        // Tropical takes not so much NPP because of:
        // high rotation of leaves and little branches
        // insects eat the leaves
        // kg/m^2 -> t/ha
        [[nodiscard]] double CUptake() const noexcept {
            return npp(year) * 10 * fCUptake;
        }

        // Harvestable wood-volume increment (m3/ha/year)
        [[nodiscard]] double vIncr() const noexcept {
            return CUptake() * fTimber(year);
        }

    private:
        uint16_t year = 0;
        double forest = 0;              // Share of land area used by forest [0-1]
        double woodPriceCorr0 = 0;      // MG: Added for wood price correction in year 2000
        double rotInterM = 0;           // MG: Added rotation interval estimated from Georg's Forest Management Tool
        double harvWood = 0;            // MG: Added harvestable wood estimated from Georg's Forest Management Tool

        double plantingCosts0;          // Costs to Plant 1ha of forests in ref country
        Ipol<double> r;                 // Discount rate
        double sAgrSuit;                // Standardised Agricultural Suitability
        Ipol<double> sPopDens;          // Standardised Population Density (1-10)
        Ipol<double> npp;               // netto primary production
        Ipol<double> priceIndex;        // Price Index
        double priceIndex0;             // Price Index of reference country
        double priceC;                  // Carbon Price
        double priceLandMin0;           // Minimum Land price in ref country
        double priceLandMax0;           // Maximum Land price in ref country
        double decLongProd;             // Decay rate for long-lived products
        double decShortProd;            // Decay rate for short-lived products
        Ipol<double> fracLongProd;      // Fraction of carbon stored in long-term products
        double baseline;                // Fraction of carbon subtracted due to baseline considerations
        Ipol<double> fTimber;           // Commercial timber volume per ton of carbon (m3/tC)
        double priceTimberMax0;         // Maximal timber price in reference country
        double priceTimberMin0;         // Minimal timber price in reference country
        double fCUptake;                // Factor of carbon uptake from npp
        Ipol<double> gdp;               // Gross domestic production
        Ipol<double> woodPriceCorr;     // MG: Added for wood price correction

        // Fraction of carbon costs during harvest
        // Depends on fraction of short and long term products
        [[nodiscard]] double beta() const noexcept {
            return 1 - decLongProd / (decLongProd + r(year)) * fracLongProd(year) -
                   decShortProd / (decShortProd + r(year)) * (1 - fracLongProd(year));
        }

        // Carbon benefit (Eq. 3)
        [[nodiscard]] double cBenefit() const noexcept {
            return priceC * CUptake() * (1 - baseline) * (((1 - pow(1 + r(year), -rotInter())) / r(year)) -
                                                          rotInter() * (1 - beta()) *
                                                          pow(1 + r(year), -rotInter()));
        }

        // Price to harvest the timber
        // Beside harvesting costs also thinning costs, branch-removal,... can be considered
        // currently made static
        [[nodiscard]] static double priceHarvest() noexcept {
            return 0;  // priceTimber() * 0
        }

        // Value of Forestry during one rotation (Eq.1)
        // Changed to 1 year!!!!
        // MG:  changed for external price correction
        [[nodiscard]] double forestValueOne() const noexcept {
            return (-plantingCosts() + (priceTimber() - priceHarvest()) * woodHarvestVol() + cBenefit()) / rotInter();
        }

        // MG: Value of Forestry during one rotation External
        // Changed to 1 year!!!!
        //MG:  changed for external price correction
        [[nodiscard]] double forestValueOneExt() const noexcept {
            return (-plantingCosts() + (priceTimberExt() - priceHarvest()) * woodHarvestVol() + cBenefit()) /
                   rotInter();
        }

        //MG: Value of Forestry during one rotation, combination of G4M + External
        // Changed to 1 year!!!!
        // MG:  changed for external price correction
        [[nodiscard]] double forestValueOneComb() const noexcept {
            return (-plantingCosts() + (priceTimberComb() - priceHarvest()) * woodHarvestVol() + cBenefit()) /
                   rotInter();
        }

        // Value of Forestry one rotation NoCarbonPrice
        // Changed to one year!!!
        // MG:  changed for external price correction
        // MG: I deleted * pow(1 + r(year), -rotInter()) to make it similar to forestValueOne
        [[nodiscard]] double forestValueOneNC() const noexcept {
            return (-plantingCosts() + (priceTimber() - priceHarvest()) * woodHarvestVol()) / rotInter();
        }

        // MG: Value of Forestry one rotation NoCarbonPrice using External wood price
        // Changed to one year!!!
        // MG:  changed for external price correction
        // MG: I deleted * pow(1 + r(year), -rotInter()) to make it similar to forestValueOne
        [[nodiscard]] double forestValueOneNCExt() const noexcept {
            return ((-plantingCosts() + (priceTimberExt() - priceHarvest()) * woodHarvestVol()) / rotInter());
        }

        // MG: Value of Forestry during one rotation, combination of G4M + External
        // Changed to 1 year!!!! No Carbon Price
        // MG:  changed for external price correction
        [[nodiscard]] double forestValueOneNCComb() const noexcept {
            return (-plantingCosts() + (priceTimberComb() - priceHarvest()) * woodHarvestVol()) / rotInter();
        }

        // calculate a sum of the series of a discounted value
        // TODO j += modTimeStep is necessary?
        [[nodiscard]] double calcNpvSum() const noexcept {
//            double currF = 0;
//            double r_year = r(year);
//            npvSum = 0;
//            for (int j = 0; currF > 0.00001 && j < 400; j += modTimeStep) {
//                currF = pow(1 + r_year, -j);
//                npvSum += currF;
//            }
            return 1 + 1 / r(year);  // sum of infinite geometric series, b_0 = 1, q = 1 / (1 + r(year))
        }
    };
}

#endif
