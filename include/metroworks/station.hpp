#ifndef METROWORKS_STATION_HPP
#define METROWORKS_STATION_HPP

#include "metroworks/types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace metroworks {

struct StationRecord {
    StationId id{0};
    std::string name;
};

/**
 * @brief Catalog of Delhi Metro stations with case-insensitive and alias-aware lookups.
 */
class StationCatalog {
public:
    StationCatalog() = default;

    /**
     * @brief Adds a station record.
     */
    void addStation(StationId id, const std::string& name);

    /**
     * @brief Adds an alias mapping (e.g. "Noida City Center" -> "Noida City Centre").
     */
    void addAlias(const std::string& alias, const std::string& targetCanonicalName);

    /**
     * @brief Checks if a station exists (case-insensitive, alias-aware).
     */
    bool hasStation(const std::string& name) const;

    /**
     * @brief Resolves input to canonical station name if found.
     */
    std::optional<std::string> resolveCanonicalName(const std::string& input) const;

    /**
     * @brief Returns station IDs associated with a name (interchange stations may have multiple IDs).
     */
    std::vector<StationId> getStationIds(const std::string& name) const;

    /**
     * @brief Returns the primary station ID for fare lookup.
     */
    std::optional<StationId> getPrimaryId(const std::string& name) const;

    /**
     * @brief Returns all unique canonical station names sorted alphabetically.
     */
    std::vector<std::string> getAllCanonicalNames() const;

    /**
     * @brief Suggests closest matching station names for typo tolerance.
     */
    std::vector<std::string> suggestStations(const std::string& input, size_t maxSuggestions = 3) const;

    /**
     * @brief Total number of registered unique stations.
     */
    size_t size() const;

    /**
     * @brief Clears the catalog.
     */
    void clear();

private:
    std::unordered_map<StationId, std::string> idToName_;
    // Normalized lowercase name/alias -> Canonical name
    std::unordered_map<std::string, std::string> normalizedToCanonical_;
    // Normalized lowercase canonical name -> list of StationIds
    std::unordered_map<std::string, std::vector<StationId>> canonicalToIds_;
    std::vector<std::string> canonicalNames_;
};

} // namespace metroworks

#endif // METROWORKS_STATION_HPP

