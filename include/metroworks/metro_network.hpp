#ifndef METROWORKS_METRO_NETWORK_HPP
#define METROWORKS_METRO_NETWORK_HPP

#include "metroworks/types.hpp"
#include "metroworks/graph.hpp"
#include "metroworks/station.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace metroworks {

/**
 * @brief Represents the complete Delhi Metro transit network.
 */
class MetroNetwork {
public:
    MetroNetwork();

    /**
     * @brief Registers a transit line with its metadata.
     */
    void registerLine(const LineInfo& line);

    /**
     * @brief Adds a track connection between two stations.
     */
    void addConnection(const std::string& fromStation,
                       const std::string& toStation,
                       DistanceKm distanceKm,
                       const std::string& lineId = "",
                       bool bidirectional = true);

    /**
     * @brief Access the station catalog.
     */
    StationCatalog& getStationCatalog();
    const StationCatalog& getStationCatalog() const;

    /**
     * @brief Access the underlying graph.
     */
    const Graph<std::string, DistanceKm>& getGraph() const;

    /**
     * @brief Retrieves all registered lines.
     */
    std::vector<LineInfo> getAllLines() const;

    /**
     * @brief Retrieves line info by lineId.
     */
    std::optional<LineInfo> getLineInfo(const std::string& lineId) const;

    /**
     * @brief Retrieves all registered physical edges.
     */
    const std::vector<NetworkEdge>& getAllEdges() const;

    /**
     * @brief Looks up lineId for an edge between u and v.
     */
    std::optional<std::string> getLineIdForEdge(const std::string& u, const std::string& v) const;

    /**
     * @brief Clears the network.
     */
    void clear();

private:
    Graph<std::string, DistanceKm> graph_;
    StationCatalog stationCatalog_;
    std::vector<NetworkEdge> edges_;
    std::unordered_map<std::string, LineInfo> lines_;
    // Map of "u::v" -> lineId
    std::unordered_map<std::string, std::string> edgeToLine_;

    static std::string makeEdgeKey(const std::string& u, const std::string& v);
};

} // namespace metroworks

#endif // METROWORKS_METRO_NETWORK_HPP
