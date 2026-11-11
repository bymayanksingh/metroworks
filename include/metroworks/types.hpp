#ifndef METROWORKS_TYPES_HPP
#define METROWORKS_TYPES_HPP

#include <string>
#include <vector>
#include <optional>
#include <iostream>

namespace metroworks {

using StationId = int;
using DistanceKm = double;
using CurrencyInr = int;

/**
 * @brief Represents a Delhi Metro line (e.g. Red Line, Yellow Line).
 */
struct LineInfo {
    std::string id;         // e.g. "red", "yellow", "blue", "green", "violet"
    std::string name;       // e.g. "Red Line", "Yellow Line"
    std::string colorHex;   // e.g. "#E53935"
    std::string dotColor;   // Graphviz color name (e.g. "red", "yellow", "blue")
};

/**
 * @brief Represents a physical track connection between two stations.
 */
struct NetworkEdge {
    std::string fromStation;
    std::string toStation;
    DistanceKm distanceKm{0.0};
    std::string lineId;     // e.g. "red", "blue"
};

/**
 * @brief Represents a step along a traversed route.
 */
struct RouteStep {
    std::string fromStation;
    std::string toStation;
    DistanceKm distanceKm{0.0};
    std::string lineId;
    std::string lineName;
};

/**
 * @brief Represents a line transfer at an interchange station.
 */
struct Interchange {
    std::string station;
    std::string fromLineId;
    std::string fromLineName;
    std::string toLineId;
    std::string toLineName;
};

/**
 * @brief Comprehensive details of a computed route between two stations.
 */
struct RouteResult {
    bool found{false};
    std::string source;
    std::string destination;
    std::vector<std::string> path;              // Sequence of station names
    std::vector<RouteStep> steps;              // Inter-station segments
    std::vector<Interchange> interchanges;      // Transfers required
    DistanceKm totalDistanceKm{0.0};
    int totalStops{0};
    int estimatedTimeMinutes{0};
    std::optional<CurrencyInr> fare;
    std::optional<CurrencyInr> smartCardFare;
    std::optional<CurrencyInr> sundayFare;
    std::string errorMessage;
};

} // namespace metroworks

#endif // METROWORKS_TYPES_HPP

