#include "metroworks/fare_calculator.hpp"
#include <algorithm>
#include <limits>

namespace metroworks {

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

