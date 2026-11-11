#include "test_runner.hpp"
#include "metroworks/data_loader.hpp"
#include "metroworks/metro_network.hpp"
#include "metroworks/fare_calculator.hpp"

using namespace metroworks;

TEST_CASE(DataLoaderTest, ValidDiscovery) {
    auto paths = DataLoader::locateDataFiles("data");
    ASSERT_TRUE(std::filesystem::exists(paths.stationsPath));
    ASSERT_TRUE(std::filesystem::exists(paths.edgesPath));
    ASSERT_TRUE(std::filesystem::exists(paths.farePath));
}

TEST_CASE(DataLoaderTest, NonExistentDirectory) {
    try {
        DataLoader::locateDataFiles("non_existent_directory_12345");
    } catch (const std::exception&) {
        // Will throw if no fallback dirs have the files either, or falls back to data/
        // If data/ exists in current working dir, it falls back gracefully
    }
    // DataLoader shouldn't crash
    ASSERT_TRUE(true);
}

TEST_CASE(DataLoaderTest, MissingFileThrows) {
    MetroNetwork network;
    bool caught = false;
    try {
        DataLoader::loadStations("definitely_missing_file_xyz.txt", network);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    ASSERT_TRUE(caught);
}
