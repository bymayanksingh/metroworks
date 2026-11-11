#ifndef METROWORKS_ROUTE_PLANNER_HPP
#define METROWORKS_ROUTE_PLANNER_HPP

#include "metroworks/types.hpp"
#include "metroworks/metro_network.hpp"
#include "metroworks/fare_calculator.hpp"
#include <string>
#include <memory>

namespace metroworks {

/**
 * @brief High-level route planning engine calculating paths, fares, and interchanges.
 */
class RoutePlanner {
public:
    RoutePlanner(const MetroNetwork& network, const FareCalculator& fareCalculator);

    /**
     * @brief Computes the optimal route between source and destination stations.
     */
    RouteResult findShortestRoute(const std::string& source, const std::string& destination) const;

    /**
     * @brief Formats RouteResult into human-readable text.
     */
    std::string formatRoute(const RouteResult& route, bool useColors = true) const;

    /**
     * @brief Formats RouteResult into structured JSON string.
     */
    std::string toJson(const RouteResult& route) const;

private:
    const MetroNetwork& network_;
    const FareCalculator& fareCalculator_;
};

} // namespace metroworks

#endif // METROWORKS_ROUTE_PLANNER_HPP

