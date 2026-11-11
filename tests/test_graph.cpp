#include "test_runner.hpp"
#include "metroworks/graph.hpp"

using namespace metroworks;

TEST_CASE(GraphTest, EmptyGraph) {
    Graph<std::string, double> g;
    ASSERT_EQ(g.nodeCount(), 0);
    ASSERT_FALSE(g.hasNode("A"));
}

TEST_CASE(GraphTest, SimpleDijkstra) {
    Graph<std::string, double> g;
    g.addEdge("A", "B", 4.0);
    g.addEdge("A", "C", 2.0);
    g.addEdge("C", "B", 1.0);
    g.addEdge("B", "D", 5.0);
    g.addEdge("C", "D", 8.0);

    ASSERT_EQ(g.nodeCount(), 4);
    ASSERT_TRUE(g.hasNode("A"));
    ASSERT_TRUE(g.hasNode("D"));

    auto result = g.dijkstra("A");
    ASSERT_NEAR(result.distances["A"], 0.0, 1e-6);
    // Path A -> C -> B (cost 3.0) vs A -> B (cost 4.0)
    ASSERT_NEAR(result.distances["B"], 3.0, 1e-6);
    ASSERT_NEAR(result.distances["C"], 2.0, 1e-6);
    // A -> C -> B -> D (cost 8.0) vs A -> C -> D (cost 10.0)
    ASSERT_NEAR(result.distances["D"], 8.0, 1e-6);

    auto path = g.reconstructPath("A", "D", result.predecessors);
    std::vector<std::string> expected = {"A", "C", "B", "D"};
    ASSERT_EQ(path, expected);
}

TEST_CASE(GraphTest, DisconnectedNodes) {
    Graph<std::string, double> g;
    g.addEdge("A", "B", 2.0);
    g.addEdge("C", "D", 3.0);

    auto result = g.dijkstra("A");
    ASSERT_TRUE(std::isinf(result.distances["C"]));
    ASSERT_TRUE(std::isinf(result.distances["D"]));

    auto path = g.reconstructPath("A", "C", result.predecessors);
    ASSERT_TRUE(path.empty());
}

TEST_CASE(GraphTest, SelfLoopAndSameSourceDest) {
    Graph<std::string, double> g;
    g.addEdge("A", "B", 1.0);

    auto result = g.dijkstra("A");
    auto path = g.reconstructPath("A", "A", result.predecessors);
    ASSERT_EQ(path.size(), 1);
    ASSERT_EQ(path[0], "A");
}

TEST_CASE(GraphTest, RejectNegativeWeights) {
    Graph<std::string, double> g;
    bool caught = false;
    try {
        g.addEdge("A", "B", -1.0);
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    ASSERT_TRUE(caught);
}

