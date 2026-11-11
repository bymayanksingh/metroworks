#include "test_runner.hpp"
#include "metroworks/station.hpp"

using namespace metroworks;

TEST_CASE(CatalogTest, CaseInsensitiveLookup) {
    StationCatalog catalog;
    catalog.addStation(1, "Rajiv Chowk");
    catalog.addStation(2, "Kashmere Gate");

    ASSERT_TRUE(catalog.hasStation("Rajiv Chowk"));
    ASSERT_TRUE(catalog.hasStation("rajiv chowk"));
    ASSERT_TRUE(catalog.hasStation("  RAJIV CHOWK  "));
    ASSERT_FALSE(catalog.hasStation("Unknown Station"));

    auto name = catalog.resolveCanonicalName("rajiv chowk");
    ASSERT_TRUE(name.has_value());
    ASSERT_EQ(*name, "Rajiv Chowk");
}

TEST_CASE(CatalogTest, EmptyAndWhitespaceHandling) {
    StationCatalog catalog;
    catalog.addStation(1, "Rajiv Chowk");

    ASSERT_FALSE(catalog.hasStation(""));
    ASSERT_FALSE(catalog.hasStation("   "));
    ASSERT_FALSE(catalog.resolveCanonicalName("").has_value());
    ASSERT_FALSE(catalog.resolveCanonicalName("   ").has_value());
}

TEST_CASE(CatalogTest, AliasResolution) {
    StationCatalog catalog;
    catalog.addStation(59, "Noida City Centre");
    catalog.addAlias("Noida City Center", "Noida City Centre");

    ASSERT_TRUE(catalog.hasStation("Noida City Centre"));
    ASSERT_TRUE(catalog.hasStation("Noida City Center"));
    ASSERT_TRUE(catalog.hasStation("noida city center"));

    auto resolved = catalog.resolveCanonicalName("noida city center");
    ASSERT_TRUE(resolved.has_value());
    ASSERT_EQ(*resolved, "Noida City Centre");

    auto id = catalog.getPrimaryId("Noida City Center");
    ASSERT_TRUE(id.has_value());
    ASSERT_EQ(*id, 59);
}

TEST_CASE(CatalogTest, InterchangeMultipleIds) {
    StationCatalog catalog;
    catalog.addStation(8, "Kashmere Gate");
    catalog.addStation(33, "Kashmere Gate");
    catalog.addStation(133, "Kashmere Gate");

    auto ids = catalog.getStationIds("kashmere gate");
    ASSERT_EQ(ids.size(), 3);
    ASSERT_EQ(ids[0], 8);
    ASSERT_EQ(ids[1], 33);
    ASSERT_EQ(ids[2], 133);
}

TEST_CASE(CatalogTest, TypoSuggestions) {
    StationCatalog catalog;
    catalog.addStation(1, "Rajiv Chowk");
    catalog.addStation(2, "Central Secretariat");
    catalog.addStation(3, "Hauz Khas");

    auto suggestions = catalog.suggestStations("Rajiv Chwk", 2);
    ASSERT_FALSE(suggestions.empty());
    ASSERT_EQ(suggestions[0], "Rajiv Chowk");
}

TEST_CASE(CatalogTest, ClearCatalog) {
    StationCatalog catalog;
    catalog.addStation(1, "Rajiv Chowk");
    ASSERT_EQ(catalog.size(), 1);

    catalog.clear();
    ASSERT_EQ(catalog.size(), 0);
    ASSERT_FALSE(catalog.hasStation("Rajiv Chowk"));
}
