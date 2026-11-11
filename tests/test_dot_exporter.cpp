#include "test_runner.hpp"
#include "metroworks/metro_network.hpp"
#include "metroworks/fare_calculator.hpp"
#include "metroworks/data_loader.hpp"
#include "metroworks/route_planner.hpp"
#include "metroworks/dot_exporter.hpp"

using namespace metroworks;

TEST_CASE(DotExporterTest, GenerateDotContainsHighlightedPath) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    auto route = planner.findShortestRoute("Rajiv Chowk", "Mandi House");

    std::string dot = DotExporter::generateDot(network, &route);

    ASSERT_TRUE(dot.find("graph DelhiMetro") != std::string::npos);
    ASSERT_TRUE(dot.find("\"Rajiv Chowk\"") != std::string::npos);
    ASSERT_TRUE(dot.find("\"Mandi House\"") != std::string::npos);
    // Path edge should be styled with penwidth=5.0
    ASSERT_TRUE(dot.find("penwidth=5.0") != std::string::npos);
}
