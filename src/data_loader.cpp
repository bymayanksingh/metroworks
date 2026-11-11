#include "metroworks/data_loader.hpp"
#include "metroworks/utils.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace metroworks {

DataPaths DataLoader::locateDataFiles(const std::string& preferredDir) {
    std::vector<std::filesystem::path> searchDirs;

    if (!preferredDir.empty()) {
        searchDirs.emplace_back(preferredDir);
    }

    // Standard search locations
    searchDirs.emplace_back("data");
    searchDirs.emplace_back(".");
    searchDirs.emplace_back("..");
    searchDirs.emplace_back("../data");

    DataPaths paths;
    bool foundStations = false;
    bool foundEdges = false;
    bool foundFare = false;

    for (const auto& dir : searchDirs) {
        if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
            continue;
        }

        auto stPath = dir / "stations.txt";
        if (!foundStations && std::filesystem::exists(stPath)) {
            paths.stationsPath = stPath;
            foundStations = true;
        }

        auto edPath = dir / "data.txt";
        if (!foundEdges && std::filesystem::exists(edPath)) {
            paths.edgesPath = edPath;
            foundEdges = true;
        }

        auto frPath = dir / "fare.csv";
        if (!foundFare && std::filesystem::exists(frPath)) {
            paths.farePath = frPath;
            foundFare = true;
        }

        if (foundStations && foundEdges && foundFare) {
            break;
        }
    }

    if (!foundStations) {
        throw std::runtime_error("Could not find stations.txt in search paths.");
    }
    if (!foundEdges) {
        throw std::runtime_error("Could not find data.txt in search paths.");
    }
    if (!foundFare) {
        throw std::runtime_error("Could not find fare.csv in search paths.");
    }

    return paths;
}

void DataLoader::loadStations(const std::filesystem::path& path, MetroNetwork& network) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open stations file: " + path.string());
    }

    auto& catalog = network.getStationCatalog();
    std::string line;
    size_t lineNum = 0;

    while (std::getline(file, line)) {
        lineNum++;
        std::string trimmed = utils::trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        // Stations.txt is tab or comma separated: <ID>\t<Station Name>
        char delim = (trimmed.find('\t') != std::string::npos) ? '\t' : ',';
        auto tokens = utils::split(trimmed, delim);

        if (tokens.size() < 2) {
            continue;
        }

        try {
            int id = std::stoi(utils::trim(tokens[0]));
            std::string name = utils::trim(tokens[1]);
            catalog.addStation(id, name);
        } catch (const std::exception& e) {
            throw std::runtime_error("Error parsing stations.txt at line " +
                                     std::to_string(lineNum) + ": " + e.what());
        }
    }

    // Register 2026 official renames and common spelling variations
    catalog.addAlias("HUDA City Centre", "Millennium City Centre Gurugram");
    catalog.addAlias("Huda City Centre", "Millennium City Centre Gurugram");
    catalog.addAlias("Millennium City Centre", "Millennium City Centre Gurugram");
    catalog.addAlias("Pragati Maidan", "Supreme Court");
    catalog.addAlias("Pragati Maiden", "Supreme Court");
    catalog.addAlias("Noida City Center", "Noida City Centre");
    catalog.addAlias("Janptah", "Janpath");
    catalog.addAlias("Mayur Vihar Extention", "Mayur Vihar Extension");
    catalog.addAlias("Vishwavidalaya", "Vishwavidyalaya");
    catalog.addAlias("Anand Vihar", "Anand Vihar ISBT");
    catalog.addAlias("INA", "Dilli Haat - INA");
    catalog.addAlias("Dilli Haat INA", "Dilli Haat - INA");
    catalog.addAlias("Karkardooma", "Karkarduma");
    catalog.addAlias("Terminal 1 Airport", "Terminal 1 IGI Airport");
    catalog.addAlias("T1 Airport", "Terminal 1 IGI Airport");
    catalog.addAlias("Airport", "Airport (T3)");
    catalog.addAlias("T3 Airport", "Airport (T3)");
    catalog.addAlias("Dwarka 21", "Dwarka Sector 21");
    catalog.addAlias("Yashobhoomi", "Yashobhoomi Dwarka Sector 25");
}

void DataLoader::loadEdges(const std::filesystem::path& path, MetroNetwork& network) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open network edges file: " + path.string());
    }

    std::string line;
    size_t lineNum = 0;

    // Default sequential line tracking for data.txt
    std::string currentLine = "red";

    while (std::getline(file, line)) {
        lineNum++;
        std::string trimmed = utils::trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        auto tokens = utils::split(trimmed, ',');
        if (tokens.size() < 3) {
            continue;
        }

        std::string u = utils::trim(tokens[0]);
        std::string v = utils::trim(tokens[1]);
        double dist = 0.0;

        try {
            dist = std::stod(utils::trim(tokens[2]));
        } catch (const std::exception& e) {
            throw std::runtime_error("Error parsing edge distance in " + path.string() +
                                     " at line " + std::to_string(lineNum) + ": " + e.what());
        }

        std::string lineId;
        if (tokens.size() >= 4 && !utils::trim(tokens[3]).empty()) {
            lineId = utils::toLowerCase(utils::trim(tokens[3]));
        } else {
            // Sequential line boundaries from Delhi Metro dataset
            if (lineNum <= 12) {
                lineId = "red";
            } else if (lineNum <= 42) {
                lineId = "blue";
            } else if (lineNum <= 48) {
                lineId = "green";
            } else if (lineNum <= 58) {
                lineId = "violet";
            } else {
                lineId = "yellow";
            }
        }

        network.addConnection(u, v, dist, lineId, true);
    }
}

void DataLoader::loadFares(const std::filesystem::path& path, FareCalculator& fareCalculator) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open fare file: " + path.string());
    }

    std::vector<std::vector<int>> matrix;
    std::string line;

    while (std::getline(file, line)) {
        std::string trimmed = utils::trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        std::vector<int> row;
        std::stringstream ss(trimmed);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            std::string cleanCell = utils::trim(cell);
            if (!cleanCell.empty()) {
                try {
                    row.push_back(std::stoi(cleanCell));
                } catch (...) {
                    row.push_back(0);
                }
            }
        }

        if (!row.empty()) {
            matrix.push_back(std::move(row));
        }
    }

    fareCalculator.setMatrix(std::move(matrix));
}

void DataLoader::loadAll(const std::string& preferredDir,
                         MetroNetwork& network,
                         FareCalculator& fareCalculator) {
    DataPaths paths = locateDataFiles(preferredDir);
    loadStations(paths.stationsPath, network);
    loadEdges(paths.edgesPath, network);
    loadFares(paths.farePath, fareCalculator);
}

} // namespace metroworks
