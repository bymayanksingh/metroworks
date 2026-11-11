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
    ASSERT_EQ(route.totalStops, 16);
    ASSERT_NEAR(route.totalDistanceKm, 19.1, 0.2);
    ASSERT_TRUE(route.fare.has_value());
    ASSERT_EQ(*route.fare, 43);
    ASSERT_EQ(*route.smartCardFare, 39);
    ASSERT_EQ(*route.sundayFare, 32);
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
    ASSERT_EQ(*route.fare, 32);
    ASSERT_EQ(*route.smartCardFare, 29);
    ASSERT_EQ(*route.sundayFare, 21);
}

TEST_CASE(RoutePlannerTest, LineTransferDetection) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    // Rithala is on Red Line; Millennium City Centre Gurugram (formerly Huda City Centre) is on Yellow Line.
    auto route = planner.findShortestRoute("Rithala", "Millennium City Centre Gurugram");

    ASSERT_TRUE(route.found);
    ASSERT_FALSE(route.interchanges.empty());

    bool foundTransferAtKashmereGate = false;
    for (const auto& ic : route.interchanges) {
        if (ic.station == "Kashmere Gate" || ic.station == "Inderlok" || ic.station == "Netaji Subhash Place") {
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
    ASSERT_EQ(*route.fare, 43);
    ASSERT_EQ(*route.smartCardFare, 39);
}

TEST_CASE(RoutePlannerTest, RenamedStationsResolution) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    // User types legacy "HUDA City Centre" and "Pragati Maidan"
    auto route = planner.findShortestRoute("HUDA City Centre", "Pragati Maidan");

    ASSERT_TRUE(route.found);
    ASSERT_EQ(route.source, "Millennium City Centre Gurugram");
    ASSERT_EQ(route.destination, "Supreme Court");
    ASSERT_TRUE(*route.fare > 0);
}

TEST_CASE(RoutePlannerTest, MagentaLineDirect) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    // Direct Magenta Line journey
    auto route = planner.findShortestRoute("Janakpuri West", "Botanical Garden");

    ASSERT_TRUE(route.found);
    ASSERT_EQ(route.path.front(), "Janakpuri West");
    ASSERT_EQ(route.path.back(), "Botanical Garden");
    ASSERT_EQ(*route.fare, 54);
}

TEST_CASE(RoutePlannerTest, AirportExpressDirect) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    // Airport Express: New Delhi to 2023 extension Yashobhoomi Dwarka Sector 25
    auto route = planner.findShortestRoute("New Delhi", "Yashobhoomi Dwarka Sector 25");

    ASSERT_TRUE(route.found);
    ASSERT_EQ(route.totalStops, 6);
    ASSERT_EQ(route.path.front(), "New Delhi");
    ASSERT_EQ(route.path.back(), "Yashobhoomi Dwarka Sector 25");
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

TEST_CASE(RoutePlannerTest, SameSourceAndDestination) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    auto route = planner.findShortestRoute("Rajiv Chowk", "Rajiv Chowk");

    ASSERT_TRUE(route.found);
    ASSERT_EQ(route.totalStops, 0);
    ASSERT_NEAR(route.totalDistanceKm, 0.0, 1e-6);
    ASSERT_TRUE(route.fare.has_value());
    ASSERT_EQ(*route.fare, 0);
    ASSERT_EQ(*route.smartCardFare, 0);
    ASSERT_EQ(*route.sundayFare, 0);
    ASSERT_EQ(route.path.size(), 1);
    ASSERT_EQ(route.path[0], "Rajiv Chowk");
}

TEST_CASE(RoutePlannerTest, BranchToBranchRouting) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    // Routing from Vaishali branch across to Noida City Centre branch via Yamuna Bank
    auto route = planner.findShortestRoute("Vaishali", "Noida City Centre");

    ASSERT_TRUE(route.found);
    ASSERT_EQ(route.totalStops, 16);
    ASSERT_NEAR(route.totalDistanceKm, 17.7, 0.2);
    ASSERT_TRUE(route.fare.has_value());
    ASSERT_EQ(*route.fare, 43);

    // Verify 2026 routing utilizes the Pink Line link via Anand Vihar ISBT / Mayur Vihar-I
    bool foundPinkLineCut = false;
    for (const auto& stn : route.path) {
        if (stn == "Anand Vihar ISBT" || stn == "Trilokpuri - Sanjay Lake" || stn == "Mayur Vihar-I") {
            foundPinkLineCut = true;
            break;
        }
    }
    ASSERT_TRUE(foundPinkLineCut);
}

TEST_CASE(RoutePlannerTest, JsonOutputVerification) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    auto route = planner.findShortestRoute("Rajiv Chowk", "Mandi House");

    std::string json = planner.toJson(route);
    ASSERT_TRUE(json.find("\"found\": true") != std::string::npos);
    ASSERT_TRUE(json.find("\"source\": \"Rajiv Chowk\"") != std::string::npos);
    ASSERT_TRUE(json.find("\"destination\": \"Mandi House\"") != std::string::npos);
    ASSERT_TRUE(json.find("\"fareInr\":") != std::string::npos);
    ASSERT_TRUE(json.find("\"smartCardFareInr\":") != std::string::npos);
    ASSERT_TRUE(json.find("\"sundayFareInr\":") != std::string::npos);
    ASSERT_TRUE(json.find("\"path\": [") != std::string::npos);
}

TEST_CASE(RoutePlannerTest, FormattedTextVerification) {
    MetroNetwork network;
    FareCalculator fareCalc;
    DataLoader::loadAll("data", network, fareCalc);

    RoutePlanner planner(network, fareCalc);
    auto route = planner.findShortestRoute("Rajiv Chowk", "Mandi House");

    std::string text = planner.formatRoute(route, false);
    ASSERT_TRUE(text.find("METROWORKS ROUTE") != std::string::npos);
    ASSERT_TRUE(text.find("Rajiv Chowk") != std::string::npos);
    ASSERT_TRUE(text.find("Mandi House") != std::string::npos);
    ASSERT_TRUE(text.find("Distance:") != std::string::npos);
    ASSERT_TRUE(text.find("Fare (2026):") != std::string::npos);
    ASSERT_TRUE(text.find("Smart Card:") != std::string::npos);
}
