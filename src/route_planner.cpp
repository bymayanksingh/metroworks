#include "metroworks/route_planner.hpp"
#include "metroworks/utils.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace metroworks {

RoutePlanner::RoutePlanner(const MetroNetwork& network, const FareCalculator& fareCalculator)
    : network_(network), fareCalculator_(fareCalculator) {}

RouteResult RoutePlanner::findShortestRoute(const std::string& source,
                                           const std::string& destination) const {
    RouteResult result;
    result.source = utils::trim(source);
    result.destination = utils::trim(destination);

    const auto& catalog = network_.getStationCatalog();

    // 1. Resolve canonical names
    auto canonSrcOpt = catalog.resolveCanonicalName(result.source);
    if (!canonSrcOpt) {
        result.found = false;
        auto suggestions = catalog.suggestStations(result.source, 3);
        std::ostringstream oss;
        oss << "Unknown source station '" << result.source << "'.";
        if (!suggestions.empty()) {
            oss << " Did you mean: ";
            for (size_t i = 0; i < suggestions.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "'" << suggestions[i] << "'";
            }
            oss << "?";
        }
        result.errorMessage = oss.str();
        return result;
    }

    auto canonDstOpt = catalog.resolveCanonicalName(result.destination);
    if (!canonDstOpt) {
        result.found = false;
        auto suggestions = catalog.suggestStations(result.destination, 3);
        std::ostringstream oss;
        oss << "Unknown destination station '" << result.destination << "'.";
        if (!suggestions.empty()) {
            oss << " Did you mean: ";
            for (size_t i = 0; i < suggestions.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "'" << suggestions[i] << "'";
            }
            oss << "?";
        }
        result.errorMessage = oss.str();
        return result;
    }

    std::string canonSrc = *canonSrcOpt;
    std::string canonDst = *canonDstOpt;
    result.source = canonSrc;
    result.destination = canonDst;

    // 2. Handle identical source and destination
    if (canonSrc == canonDst) {
        result.found = true;
        result.path = {canonSrc};
        result.totalDistanceKm = 0.0;
        result.totalStops = 0;
        result.estimatedTimeMinutes = 0;
        result.fare = 0;
        result.smartCardFare = 0;
        result.sundayFare = 0;
        return result;
    }

    // 3. Verify stations exist in graph
    const auto& graph = network_.getGraph();
    if (!graph.hasNode(canonSrc) || !graph.hasNode(canonDst)) {
        result.found = false;
        result.errorMessage = "One or both stations are not connected to the active metro network.";
        return result;
    }

    // 4. Run Dijkstra algorithm
    auto dijkstraResult = graph.dijkstra(canonSrc);
    auto itDist = dijkstraResult.distances.find(canonDst);
    if (itDist == dijkstraResult.distances.end() ||
        std::isinf(itDist->second)) {
        result.found = false;
        result.errorMessage = "No path found between " + canonSrc + " and " + canonDst + ".";
        return result;
    }

    result.path = graph.reconstructPath(canonSrc, canonDst, dijkstraResult.predecessors);
    if (result.path.empty()) {
        result.found = false;
        result.errorMessage = "Could not reconstruct route path.";
        return result;
    }

    result.found = true;
    result.totalDistanceKm = itDist->second;
    result.totalStops = static_cast<int>(result.path.size() - 1);

    // 5. Build step-by-step route segments and detect interchanges
    std::string previousLineId = "";

    for (size_t i = 0; i + 1 < result.path.size(); ++i) {
        const std::string& u = result.path[i];
        const std::string& v = result.path[i + 1];

        auto edgeDistOpt = graph.getEdgeWeight(u, v);
        double dist = edgeDistOpt.value_or(0.0);

        std::string lineId = network_.getLineIdForEdge(u, v).value_or("unknown");
        auto lineInfoOpt = network_.getLineInfo(lineId);
        std::string lineName = lineInfoOpt ? lineInfoOpt->name : "Metro Line";

        result.steps.push_back({u, v, dist, lineId, lineName});

        if (i > 0 && lineId != previousLineId && !previousLineId.empty() && lineId != "unknown") {
            auto prevLineInfo = network_.getLineInfo(previousLineId);
            std::string prevLineName = prevLineInfo ? prevLineInfo->name : previousLineId;
            result.interchanges.push_back({u, previousLineId, prevLineName, lineId, lineName});
        }
        previousLineId = lineId;
    }

    // 6. Estimated transit time: ~2 minutes per hop + ~5 minutes per interchange
    int travelTimeMinutes = static_cast<int>(std::round(result.totalStops * 2.1));
    int transferTimeMinutes = static_cast<int>(result.interchanges.size() * 5);
    result.estimatedTimeMinutes = std::max(2, travelTimeMinutes + transferTimeMinutes);

    // 7. Calculate 2026 official DMRC fare breakdown
    auto breakdown = FareCalculator::calculateFromDistance(result.totalDistanceKm);
    auto srcIds = catalog.getStationIds(canonSrc);
    auto dstIds = catalog.getStationIds(canonDst);
    auto matrixFare = fareCalculator_.getFare(srcIds, dstIds);

    result.fare = matrixFare.has_value() ? *matrixFare : breakdown.standardFare;
    result.smartCardFare = breakdown.smartCardFare;
    result.sundayFare = breakdown.sundayFare;

    return result;
}

std::string RoutePlanner::formatRoute(const RouteResult& route, bool useColors) const {
    std::ostringstream ss;
    using namespace utils::color;

    auto cBold = [useColors](const char* c) { return useColors ? c : ""; };
    auto cReset = [useColors]() { return useColors ? RESET : ""; };

    if (!route.found) {
        ss << cBold(RED) << "Error: " << cReset() << route.errorMessage << "\n";
        return ss.str();
    }

    ss << "\n" << cBold(BOLD) << "========================================================\n"
       << "                   METROWORKS ROUTE                     \n"
       << "========================================================" << cReset() << "\n\n";

    ss << "  " << cBold(CYAN) << "From:        " << cReset() << route.source << "\n"
       << "  " << cBold(CYAN) << "To:          " << cReset() << route.destination << "\n"
       << "  " << cBold(GREEN) << "Distance:    " << cReset() << std::fixed << std::setprecision(1)
       << route.totalDistanceKm << " km\n"
       << "  " << cBold(GREEN) << "Stops:       " << cReset() << route.totalStops << "\n"
       << "  " << cBold(GREEN) << "Interchanges:" << cReset() << " " << route.interchanges.size() << "\n"
       << "  " << cBold(YELLOW) << "Est. Time:   " << cReset() << "~" << route.estimatedTimeMinutes << " mins\n";

    if (route.fare.has_value()) {
        ss << "  " << cBold(YELLOW) << "Fare (2026): " << cReset() << "Rs " << *route.fare;
        if (route.smartCardFare.has_value() && *route.smartCardFare > 0) {
            ss << " (Smart Card: Rs " << *route.smartCardFare << ")";
        }
        if (route.sundayFare.has_value() && *route.sundayFare > 0 && *route.sundayFare != *route.fare) {
            ss << " [Sun/Holiday: Rs " << *route.sundayFare << "]";
        }
        ss << "\n";
    }

    ss << "\n" << cBold(BOLD) << "--- Journey Itinerary ---" << cReset() << "\n";

    if (route.path.size() <= 1) {
        ss << "  You are already at your destination.\n";
        return ss.str();
    }

    // Group steps by line
    size_t stepIdx = 0;
    while (stepIdx < route.steps.size()) {
        std::string currentLine = route.steps[stepIdx].lineName;
        std::string currentLineId = route.steps[stepIdx].lineId;
        std::string startStation = route.steps[stepIdx].fromStation;
        double lineDist = 0.0;
        int lineStops = 0;

        const char* lineColor = WHITE;
        if (currentLineId == "red") lineColor = RED;
        else if (currentLineId == "blue") lineColor = BLUE;
        else if (currentLineId == "green") lineColor = GREEN;
        else if (currentLineId == "violet") lineColor = MAGENTA;
        else if (currentLineId == "yellow") lineColor = YELLOW;
        else if (currentLineId == "orange") lineColor = YELLOW;
        else if (currentLineId == "magenta") lineColor = MAGENTA;
        else if (currentLineId == "pink") lineColor = MAGENTA;
        else if (currentLineId == "grey") lineColor = DIM;

        size_t j = stepIdx;
        while (j < route.steps.size() && route.steps[j].lineId == currentLineId) {
            lineDist += route.steps[j].distanceKm;
            lineStops++;
            j++;
        }

        std::string endStation = route.steps[j - 1].toStation;

        ss << "\n  * " << cBold(lineColor) << "[" << currentLine << "]" << cReset()
           << " Ride from " << cBold(WHITE) << startStation << cReset()
           << " to " << cBold(WHITE) << endStation << cReset() << "\n"
           << "    (" << lineStops << " stops, " << std::fixed << std::setprecision(1)
           << lineDist << " km)\n";

        if (j < route.steps.size()) {
            std::string transferStation = route.steps[j].fromStation;
            std::string nextLine = route.steps[j].lineName;
            ss << "    " << cBold(YELLOW) << "-> Transfer at " << transferStation
               << " to " << nextLine << cReset() << "\n";
        }

        stepIdx = j;
    }

    ss << "\n" << cBold(BOLD) << "--- Station Sequence ---" << cReset() << "\n";
    for (size_t i = 0; i < route.path.size(); ++i) {
        if (i == 0) {
            ss << "  " << cBold(GREEN) << "[START]" << cReset() << " " << route.path[i] << "\n";
        } else if (i + 1 == route.path.size()) {
            ss << "  " << cBold(RED) << "[DEST] " << cReset() << " " << route.path[i] << "\n";
        } else {
            bool isInterchange = false;
            for (const auto& ic : route.interchanges) {
                if (ic.station == route.path[i]) {
                    isInterchange = true;
                    break;
                }
            }
            if (isInterchange) {
                ss << "  " << cBold(YELLOW) << "[XFER] " << cReset() << " " << route.path[i]
                   << cBold(DIM) << " (Change Line)" << cReset() << "\n";
            } else {
                ss << "     |   " << route.path[i] << "\n";
            }
        }
    }

    ss << "\n";
    return ss.str();
}

std::string RoutePlanner::toJson(const RouteResult& route) const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"found\": " << (route.found ? "true" : "false") << ",\n";
    ss << "  \"source\": \"" << utils::escapeJsonString(route.source) << "\",\n";
    ss << "  \"destination\": \"" << utils::escapeJsonString(route.destination) << "\",\n";

    if (!route.found) {
        ss << "  \"error\": \"" << utils::escapeJsonString(route.errorMessage) << "\"\n";
        ss << "}\n";
        return ss.str();
    }

    ss << "  \"totalDistanceKm\": " << std::fixed << std::setprecision(2) << route.totalDistanceKm << ",\n";
    ss << "  \"totalStops\": " << route.totalStops << ",\n";
    ss << "  \"estimatedTimeMinutes\": " << route.estimatedTimeMinutes << ",\n";

    if (route.fare.has_value()) {
        ss << "  \"fareInr\": " << *route.fare << ",\n";
    } else {
        ss << "  \"fareInr\": null,\n";
    }
    if (route.smartCardFare.has_value()) {
        ss << "  \"smartCardFareInr\": " << *route.smartCardFare << ",\n";
    } else {
        ss << "  \"smartCardFareInr\": null,\n";
    }
    if (route.sundayFare.has_value()) {
        ss << "  \"sundayFareInr\": " << *route.sundayFare << ",\n";
    } else {
        ss << "  \"sundayFareInr\": null,\n";
    }

    // Interchanges
    ss << "  \"interchanges\": [\n";
    for (size_t i = 0; i < route.interchanges.size(); ++i) {
        const auto& ic = route.interchanges[i];
        ss << "    {\n"
           << "      \"station\": \"" << utils::escapeJsonString(ic.station) << "\",\n"
           << "      \"fromLine\": \"" << utils::escapeJsonString(ic.fromLineName) << "\",\n"
           << "      \"toLine\": \"" << utils::escapeJsonString(ic.toLineName) << "\"\n"
           << "    }" << (i + 1 < route.interchanges.size() ? "," : "") << "\n";
    }
    ss << "  ],\n";

    // Path
    ss << "  \"path\": [\n";
    for (size_t i = 0; i < route.path.size(); ++i) {
        ss << "    \"" << utils::escapeJsonString(route.path[i]) << "\""
           << (i + 1 < route.path.size() ? "," : "") << "\n";
    }
    ss << "  ]\n";
    ss << "}\n";

    return ss.str();
}

} // namespace metroworks
