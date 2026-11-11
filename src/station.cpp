#include "metroworks/station.hpp"
#include "metroworks/utils.hpp"
#include <algorithm>

namespace metroworks {

void StationCatalog::addStation(StationId id, const std::string& name) {
    std::string cleanName = utils::trim(name);
    if (cleanName.empty()) {
        return;
    }

    idToName_[id] = cleanName;
    std::string norm = utils::toLowerCase(cleanName);

    // If canonical name not added yet
    if (normalizedToCanonical_.find(norm) == normalizedToCanonical_.end()) {
        normalizedToCanonical_[norm] = cleanName;
        canonicalNames_.push_back(cleanName);
    }

    canonicalToIds_[norm].push_back(id);
}

void StationCatalog::addAlias(const std::string& alias, const std::string& targetCanonicalName) {
    std::string cleanAlias = utils::trim(alias);
    std::string cleanTarget = utils::trim(targetCanonicalName);
    if (cleanAlias.empty() || cleanTarget.empty()) {
        return;
    }

    std::string normAlias = utils::toLowerCase(cleanAlias);
    std::string normTarget = utils::toLowerCase(cleanTarget);

    auto it = normalizedToCanonical_.find(normTarget);
    if (it != normalizedToCanonical_.end()) {
        normalizedToCanonical_[normAlias] = it->second;
    } else {
        // Target doesn't exist yet, store cleanTarget as canonical
        normalizedToCanonical_[normAlias] = cleanTarget;
        normalizedToCanonical_[normTarget] = cleanTarget;
        canonicalNames_.push_back(cleanTarget);
    }
}

bool StationCatalog::hasStation(const std::string& name) const {
    std::string norm = utils::toLowerCase(utils::trim(name));
    return normalizedToCanonical_.find(norm) != normalizedToCanonical_.end();
}

std::optional<std::string> StationCatalog::resolveCanonicalName(const std::string& input) const {
    std::string norm = utils::toLowerCase(utils::trim(input));
    auto it = normalizedToCanonical_.find(norm);
    if (it != normalizedToCanonical_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<StationId> StationCatalog::getStationIds(const std::string& name) const {
    std::string norm = utils::toLowerCase(utils::trim(name));
    auto canonIt = normalizedToCanonical_.find(norm);
    if (canonIt != normalizedToCanonical_.end()) {
        std::string canonNorm = utils::toLowerCase(canonIt->second);
        auto idIt = canonicalToIds_.find(canonNorm);
        if (idIt != canonicalToIds_.end()) {
            return idIt->second;
        }
    }
    return {};
}

std::optional<StationId> StationCatalog::getPrimaryId(const std::string& name) const {
    auto ids = getStationIds(name);
    if (!ids.empty()) {
        return ids.front();
    }
    return std::nullopt;
}

std::vector<std::string> StationCatalog::getAllCanonicalNames() const {
    std::vector<std::string> sorted = canonicalNames_;
    std::sort(sorted.begin(), sorted.end());
    return sorted;
}

std::vector<std::string> StationCatalog::suggestStations(const std::string& input, size_t maxSuggestions) const {
    return utils::findClosestMatches(input, canonicalNames_, maxSuggestions);
}

size_t StationCatalog::size() const {
    return canonicalNames_.size();
}

void StationCatalog::clear() {
    idToName_.clear();
    normalizedToCanonical_.clear();
    canonicalToIds_.clear();
    canonicalNames_.clear();
}

} // namespace metroworks

