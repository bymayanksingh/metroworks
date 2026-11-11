#include "metroworks/fare_calculator.hpp"
#include <algorithm>
#include <limits>
#include <cmath>

namespace metroworks {

FareBreakdown FareCalculator::calculateFromDistance(DistanceKm distanceKm) {
    if (distanceKm <= 0.0) {
        return {0, 0, 0};
    }

    int standard = 64;
    int sunday = 54;

    if (distanceKm <= 2.0) {
        standard = 11;
        sunday = 11;
    } else if (distanceKm <= 5.0) {
        standard = 21;
        sunday = 11;
    } else if (distanceKm <= 12.0) {
        standard = 32;
        sunday = 21;
    } else if (distanceKm <= 21.0) {
        standard = 43;
        sunday = 32;
    } else if (distanceKm <= 32.0) {
        standard = 54;
        sunday = 43;
    } else {
        standard = 64;
        sunday = 54;
    }

    // Smart Card receives flat 10% discount rounded to nearest rupee
    int smartCard = static_cast<int>(std::round(standard * 0.90));

    return {standard, smartCard, sunday};
}

void FareCalculator::setMatrix(std::vector<std::vector<int>> matrix) {
    fareMatrix_ = std::move(matrix);
}

std::optional<CurrencyInr> FareCalculator::getFare(StationId sourceId, StationId destId) const {
    if (fareMatrix_.empty()) {
        return std::nullopt;
    }

    if (sourceId < 1 || destId < 1) {
        return std::nullopt;
    }

    size_t r = static_cast<size_t>(sourceId - 1);
    size_t c = static_cast<size_t>(destId - 1);

    if (r >= fareMatrix_.size()) {
        return std::nullopt;
    }

    if (c >= fareMatrix_[r].size()) {
        return std::nullopt;
    }

    return fareMatrix_[r][c];
}

std::optional<CurrencyInr> FareCalculator::getFare(const std::vector<StationId>& sourceIds,
                                                  const std::vector<StationId>& destIds) const {
    if (sourceIds.empty() || destIds.empty()) {
        return std::nullopt;
    }

    int minFare = std::numeric_limits<int>::max();
    bool found = false;

    for (StationId sId : sourceIds) {
        for (StationId dId : destIds) {
            auto fareOpt = getFare(sId, dId);
            if (fareOpt.has_value()) {
                minFare = std::min(minFare, *fareOpt);
                found = true;
            }
        }
    }

    if (found) {
        return minFare;
    }
    return std::nullopt;
}

bool FareCalculator::isLoaded() const {
    return !fareMatrix_.empty();
}

size_t FareCalculator::rowCount() const {
    return fareMatrix_.size();
}

size_t FareCalculator::colCount() const {
    return fareMatrix_.empty() ? 0 : fareMatrix_[0].size();
}

} // namespace metroworks

