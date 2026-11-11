#ifndef METROWORKS_GRAPH_HPP
#define METROWORKS_GRAPH_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <limits>
#include <algorithm>
#include <optional>
#include <stdexcept>

namespace metroworks {

/**
 * @brief Generic Weighted Graph with Dijkstra's Shortest Path Algorithm.
 * 
 * @tparam Node Node identifier type (must be hashable and equality-comparable).
 * @tparam Weight Weight / distance type (numeric type, defaults to double).
 */
template <typename Node = std::string, typename Weight = double>
class Graph {
public:
    struct Edge {
        Node target;
        Weight weight;
    };

    struct DijkstraResult {
        std::unordered_map<Node, Weight> distances;
        std::unordered_map<Node, Node> predecessors;
    };

    Graph() = default;

    /**
     * @brief Adds an edge between node u and node v.
     */
    void addEdge(const Node& u, const Node& v, Weight weight, bool bidirectional = true) {
        if (weight < static_cast<Weight>(0)) {
            throw std::invalid_argument("Negative edge weights are not supported in Dijkstra algorithm.");
        }
        adjacencyList_[u].push_back({v, weight});
        nodes_.insert(u);
        nodes_.insert(v);

        if (bidirectional) {
            adjacencyList_[v].push_back({u, weight});
        }
    }

    /**
     * @brief Checks if a node exists in the graph.
     */
    bool hasNode(const Node& node) const {
        return nodes_.find(node) != nodes_.end();
    }

    /**
     * @brief Returns all unique nodes in the graph.
     */
    const std::unordered_set<Node>& getNodes() const {
        return nodes_;
    }

    /**
     * @brief Returns outgoing edges from a given node.
     */
    std::vector<Edge> getNeighbors(const Node& node) const {
        auto it = adjacencyList_.find(node);
        if (it != adjacencyList_.end()) {
            return it->second;
        }
        return {};
    }

    /**
     * @brief Returns edge weight if an edge exists between u and v.
     */
    std::optional<Weight> getEdgeWeight(const Node& u, const Node& v) const {
        auto it = adjacencyList_.find(u);
        if (it != adjacencyList_.end()) {
            for (const auto& edge : it->second) {
                if (edge.target == v) {
                    return edge.weight;
                }
            }
        }
        return std::nullopt;
    }

    /**
     * @brief Number of nodes in the graph.
     */
    size_t nodeCount() const {
        return nodes_.size();
    }

    /**
     * @brief Clears all nodes and edges.
     */
    void clear() {
        adjacencyList_.clear();
        nodes_.clear();
    }

    /**
     * @brief Runs Dijkstra's Single-Source Shortest Path algorithm.
     * 
     * Time Complexity: O((V + E) log V)
     * Space Complexity: O(V)
     * 
     * @param source The starting node.
     * @return DijkstraResult containing minimum distances and predecessors for path reconstruction.
     */
    DijkstraResult dijkstra(const Node& source) const {
        if (!hasNode(source)) {
            throw std::invalid_argument("Source node does not exist in graph.");
        }

        DijkstraResult result;
        const Weight inf = std::numeric_limits<Weight>::infinity();

        for (const auto& node : nodes_) {
            result.distances[node] = inf;
        }
        result.distances[source] = static_cast<Weight>(0);

        // Min-priority queue: stores (distance, node)
        using Element = std::pair<Weight, Node>;
        std::priority_queue<Element, std::vector<Element>, std::greater<Element>> pq;

        pq.push({static_cast<Weight>(0), source});

        while (!pq.empty()) {
            auto [currentDist, currentNode] = pq.top();
            pq.pop();

            // Skip outdated distance entries in priority queue
            if (currentDist > result.distances[currentNode]) {
                continue;
            }

            auto it = adjacencyList_.find(currentNode);
            if (it == adjacencyList_.end()) {
                continue;
            }

            for (const auto& edge : it->second) {
                const Node& neighbor = edge.target;
                Weight newDist = currentDist + edge.weight;

                if (newDist < result.distances[neighbor]) {
                    result.distances[neighbor] = newDist;
                    result.predecessors[neighbor] = currentNode;
                    pq.push({newDist, neighbor});
                }
            }
        }

        return result;
    }

    /**
     * @brief Reconstructs the shortest path from source to destination.
     * 
     * @param source Source node.
     * @param destination Destination node.
     * @param predecessors Predecessor map returned by dijkstra().
     * @return Vector of nodes along the path from source to destination, or empty vector if unreachable.
     */
    std::vector<Node> reconstructPath(const Node& source,
                                      const Node& destination,
                                      const std::unordered_map<Node, Node>& predecessors) const {
        if (!hasNode(source) || !hasNode(destination)) {
            return {};
        }

        if (source == destination) {
            return {source};
        }

        std::vector<Node> path;
        Node curr = destination;

        std::unordered_set<Node> visitedInPath;

        while (true) {
            path.push_back(curr);
            visitedInPath.insert(curr);

            if (curr == source) {
                break;
            }

            auto it = predecessors.find(curr);
            if (it == predecessors.end()) {
                // Destination is unreachable from source
                return {};
            }

            if (visitedInPath.find(it->second) != visitedInPath.end()) {
                // Cycle detected in predecessor map (should not happen in valid Dijkstra)
                return {};
            }

            curr = it->second;
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

private:
    std::unordered_map<Node, std::vector<Edge>> adjacencyList_;
    std::unordered_set<Node> nodes_;
};

} // namespace metroworks

#endif // METROWORKS_GRAPH_HPP
