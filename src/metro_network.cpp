#include "metroworks/metro_network.hpp"
#include "metroworks/utils.hpp"
#include <algorithm>

namespace metroworks {

MetroNetwork::MetroNetwork() {
    // Register standard Delhi Metro lines (2026 active lines)
    registerLine({"red", "Red Line", "#E53935", "red"});
    registerLine({"yellow", "Yellow Line", "#FBC02D", "yellow"});
    registerLine({"blue", "Blue Line", "#1E88E5", "blue"});
    registerLine({"green", "Green Line", "#43A047", "green"});
    registerLine({"violet", "Violet Line", "#8E24AA", "violet"});
    registerLine({"orange", "Airport Express", "#FB8C00", "orange"});
    registerLine({"magenta", "Magenta Line", "#D81B60", "magenta"});
    registerLine({"pink", "Pink Line", "#EC407A", "pink"});
    registerLine({"grey", "Grey Line", "#78909C", "gray"});
}

void MetroNetwork::registerLine(const LineInfo& line) {
    lines_[line.id] = line;
}

std::string MetroNetwork::makeEdgeKey(const std::string& u, const std::string& v) {
    std::string s1 = utils::toLowerCase(utils::trim(u));
    std::string s2 = utils::toLowerCase(utils::trim(v));
    if (s1 > s2) {
        std::swap(s1, s2);
    }
    return s1 + "::" + s2;
}

void MetroNetwork::addConnection(const std::string& fromStation,
                                 const std::string& toStation,
                                 DistanceKm distanceKm,
                                 const std::string& lineId,
                                 bool bidirectional) {
    std::string canonFrom = stationCatalog_.resolveCanonicalName(fromStation).value_or(utils::trim(fromStation));
    std::string canonTo = stationCatalog_.resolveCanonicalName(toStation).value_or(utils::trim(toStation));

    graph_.addEdge(canonFrom, canonTo, distanceKm, bidirectional);
    edges_.push_back({canonFrom, canonTo, distanceKm, lineId});

    if (!lineId.empty()) {
        std::string key = makeEdgeKey(canonFrom, canonTo);
        edgeToLine_[key] = lineId;
    }
}

StationCatalog& MetroNetwork::getStationCatalog() {
    return stationCatalog_;
}

const StationCatalog& MetroNetwork::getStationCatalog() const {
    return stationCatalog_;
}

const Graph<std::string, DistanceKm>& MetroNetwork::getGraph() const {
    return graph_;
}

std::vector<LineInfo> MetroNetwork::getAllLines() const {
    std::vector<LineInfo> result;
    result.reserve(lines_.size());
    for (const auto& [id, info] : lines_) {
        result.push_back(info);
    }
    return result;
}

std::optional<LineInfo> MetroNetwork::getLineInfo(const std::string& lineId) const {
    auto it = lines_.find(lineId);
    if (it != lines_.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::vector<NetworkEdge>& MetroNetwork::getAllEdges() const {
    return edges_;
}

std::optional<std::string> MetroNetwork::getLineIdForEdge(const std::string& u, const std::string& v) const {
    std::string key = makeEdgeKey(u, v);
    auto it = edgeToLine_.find(key);
    if (it != edgeToLine_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void MetroNetwork::clear() {
    graph_.clear();
    stationCatalog_.clear();
    edges_.clear();
    lines_.clear();
    edgeToLine_.clear();
}

} // namespace metroworks

