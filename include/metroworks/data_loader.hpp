#ifndef METROWORKS_DATA_LOADER_HPP
#define METROWORKS_DATA_LOADER_HPP

#include "metroworks/metro_network.hpp"
#include "metroworks/fare_calculator.hpp"
#include <string>
#include <vector>
#include <filesystem>

namespace metroworks {

/**
 * @brief Configuration and file discovery paths for MetroWorks datasets.
 */
struct DataPaths {
    std::filesystem::path stationsPath;
    std::filesystem::path edgesPath;
    std::filesystem::path farePath;
};

/**
 * @brief Production data loader with validation and multi-location fallback discovery.
 */
class DataLoader {
public:
    DataLoader() = default;

    /**
     * @brief Resolves data paths from candidate directory or search paths.
     */
    static DataPaths locateDataFiles(const std::string& preferredDir = "");

    /**
     * @brief Loads stations into the network's catalog from stations.txt.
     */
    static void loadStations(const std::filesystem::path& path, MetroNetwork& network);

    /**
     * @brief Loads track connections and assigns line metadata from data.txt.
     */
    static void loadEdges(const std::filesystem::path& path, MetroNetwork& network);

    /**
     * @brief Loads fare matrix from fare.csv.
     */
    static void loadFares(const std::filesystem::path& path, FareCalculator& fareCalculator);

    /**
     * @brief Loads all data into the network and fare calculator using discovered paths.
     */
    static void loadAll(const std::string& preferredDir,
                        MetroNetwork& network,
                        FareCalculator& fareCalculator);
};

} // namespace metroworks

#endif // METROWORKS_DATA_LOADER_HPP
