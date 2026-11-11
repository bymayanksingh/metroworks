#include "test_runner.hpp"
#include "metroworks/metro_network.hpp"
#include "metroworks/fare_calculator.hpp"
#include "metroworks/data_loader.hpp"
#include "metroworks/route_planner.hpp"

using namespace metroworks;

TEST_CASE(RoutePlannerTest, EndToEndRedLine) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    auto route = planner.findShortestRoute("Rithala", "Welcome");

    ASSERT_TRUE(route.found);
    ASSERT_EQ(route.source, "Rithala");
    ASSERT_EQ(route.destination, "Welcome");
    ASSERT_EQ(route.path.front(), "Rithala");
    ASSERT_EQ(route.path.back(), "Welcome");
    ASSERT_EQ(route.totalStops, 12);
    ASSERT_NEAR(route.totalDistanceKm, 19.0, 0.1);
    ASSERT_TRUE(route.fare.has_value());
    ASSERT_EQ(*route.fare, 40);
}

TEST_CASE(RoutePlannerTest, EndToEndYellowLine) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    auto route = planner.findShortestRoute("Vishwavidyalaya", "Rajiv Chowk");

    ASSERT_TRUE(route.found);
    ASSERT_EQ(route.path.front(), "Vishwavidyalaya");
    ASSERT_EQ(route.path.back(), "Rajiv Chowk");
    ASSERT_EQ(route.totalStops, 7);
    ASSERT_NEAR(route.totalDistanceKm, 7.4, 0.1);
    ASSERT_TRUE(route.fare.has_value());
    ASSERT_EQ(*route.fare, 30);
}

TEST_CASE(RoutePlannerTest, LineTransferDetection) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    // Rithala is on Red Line; Huda City Centre is on Yellow Line.
    // They intersect at Kashmere Gate.
    auto route = planner.findShortestRoute("Rithala", "Huda City Centre");

    ASSERT_TRUE(route.found);
    ASSERT_FALSE(route.interchanges.empty());

    bool foundTransferAtKashmereGate = false;
    for (const auto& ic : route.interchanges) {
        if (ic.station == "Kashmere Gate" || ic.station == "Inderlok") {
            foundTransferAtKashmereGate = true;
            break;
        }
    }
    ASSERT_TRUE(foundTransferAtKashmereGate);
}

TEST_CASE(RoutePlannerTest, AliasResolutionRoute) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    // User types "noida city center" (American spelling) instead of "Noida City Centre"
    auto route = planner.findShortestRoute("rajiv chowk", "noida city center");

    ASSERT_TRUE(route.found);
    ASSERT_EQ(route.destination, "Noida City Centre");
    ASSERT_TRUE(route.fare.has_value());
    ASSERT_EQ(*route.fare, 40);
}

TEST_CASE(RoutePlannerTest, InvalidStationHandling) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    auto route = planner.findShortestRoute("Hogwarts Express", "Rajiv Chowk");

    ASSERT_FALSE(route.found);
    ASSERT_FALSE(route.errorMessage.empty());
}
