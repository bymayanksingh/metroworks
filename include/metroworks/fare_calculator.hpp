#ifndef METROWORKS_FARE_CALCULATOR_HPP
#define METROWORKS_FARE_CALCULATOR_HPP

#include "metroworks/types.hpp"
#include <vector>
#include <optional>
#include <string>

namespace metroworks {

struct FareBreakdown {
    CurrencyInr standardFare{0};
    CurrencyInr smartCardFare{0};
    CurrencyInr sundayFare{0};
};

/**
 * @brief Thread-safe fare matrix calculator with bounds checking and 2026 distance slab evaluation.
 */
class FareCalculator {
public:
    FareCalculator() = default;

    /**
     * @brief Computes 2026 official DMRC fare breakdown directly from distance.
     */
    static FareBreakdown calculateFromDistance(DistanceKm distanceKm);

    /**
     * @brief Initializes the fare matrix.
     */
    void setMatrix(std::vector<std::vector<int>> matrix);

    /**
     * @brief Computes the fare between two 1-based station IDs.
     */
    std::optional<CurrencyInr> getFare(StationId sourceId, StationId destId) const;

    /**
     * @brief Computes fare by trying combinations of candidate IDs (useful for interchange stations).
     */
    std::optional<CurrencyInr> getFare(const std::vector<StationId>& sourceIds,
                                      const std::vector<StationId>& destIds) const;

    /**
     * @brief Checks if matrix is loaded.
     */
    bool isLoaded() const;

    /**
     * @brief Number of rows in matrix.
     */
    size_t rowCount() const;

    /**
     * @brief Number of columns in matrix.
     */
    size_t colCount() const;

private:
    std::vector<std::vector<int>> fareMatrix_;
};

} // namespace metroworks

#endif // METROWORKS_FARE_CALCULATOR_HPP

