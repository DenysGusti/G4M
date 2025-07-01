#ifndef G4M_EUROPE_DG_FM_RESULT_HPP
#define G4M_EUROPE_DG_FM_RESULT_HPP

namespace g4m::increment {

    struct FMResult {
        double area = 0;            // Area where management was done
        double sawnWood = 0;        // Sawn-wood [tC/Ha]
        double restWood = 0;        // Rest-wood [tC/Ha]
        double harvestCosts = 0;    // Costs [Costs/Ha]
        double biomass = 0;         // Total cut biomass including harvesting losses [tC/Ha]
        // For final harvest the values are per hectare, for thinning they are total
        double H = 0;               // MG: average height of trees in age classes that are thinned, clear-cut or slash-logged
        double DBH = 0;             // MG: average DBH of trees in age classes that are thinned, clear-cut or slash-logged
        double netInc = 0;          // MG:  net annual increment averaged over all age classes, tC/(ha*year)
        double grossInc = 0;        // MG:  gross annual increment averaged over all age classes, tC/(ha*year)
        double deadwood = 0;        // Average stem deadwood, trees of Dbh > 10cm, tC/ha
        double litter = 0;          // Average stem litter, trees of Dbh <= 10cm, tC/ha
        double mortDeadwoodH = 0;   // MG: average height of trees in age classes that die and are not thinned, DBH > 10cm
        double mortDeadwoodDBH = 0; // MG: average diameter of trees in age classes that die and are not thinned, DBH > 10cm
        double mortLitterH = 0;     // MG: average height of trees in age classes that die and are not thinned, DBH <= 10cm
        double mortLitterDBH = 0;   // MG: average diameter of trees in age classes that die and are not thinned, DBH <= 10cm
        double thinnedWeight = 0;   // MG: area-weighted thinned wood (no harvest losses)
        double deadwoodWeight = 0;  // MG: area & bm-weighted deadwood
        double litterWeight = 0;    // MG: area & bm-weighted litter

        // sawnWood + restWood
        [[nodiscard]] double getWood() const {
            return sawnWood + restWood;
        }

        // (sawnWood + restWood) * area
        [[nodiscard]] double getTotalSalvageWoodRemoval() const {
            return (sawnWood + restWood) * area;
        }

        // bm * area
        [[nodiscard]] double getHarvestedBiomassForest() const {
            return biomass * area;
        }

        // H * area
        [[nodiscard]] double getHArea() const {
            return H * area;
        }

        // DBH * area
        [[nodiscard]] double getDBHArea() const {
            return DBH * area;
        }
    };
}

#endif
