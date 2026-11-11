#include "metroworks/types.hpp"
#include "metroworks/metro_network.hpp"
#include "metroworks/fare_calculator.hpp"
#include "metroworks/data_loader.hpp"
#include "metroworks/route_planner.hpp"
#include "metroworks/dot_exporter.hpp"
#include "metroworks/utils.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

namespace {

void printVersion() {
    std::cout << "MetroWorks v2.0.0 (Production Edition)\n"
              << "Copyright (c) Delhi Metro Route & Fare Engine\n";
}

void printUsage(const char* progName) {
    using namespace metroworks::utils::color;
    bool c = metroworks::utils::isColorSupported();
    auto col = [c](const char* code) { return c ? code : ""; };

    std::cout << col(BOLD) << "Usage:" << col(RESET) << " " << progName << " [OPTIONS]\n\n"
              << col(BOLD) << "Options:" << col(RESET) << "\n"
              << "  -s, --source <station>   Source station name\n"
              << "  -d, --dest <station>     Destination station name\n"
              << "  -l, --list               List all available Delhi Metro stations\n"
              << "  -j, --json               Output route information in JSON format\n"
              << "  -V, --visualize          Generate and open Graphviz route diagram\n"
              << "      --dot <file>         Export route Graphviz DOT file to path\n"
              << "      --data-dir <path>    Specify custom directory for data files\n"
              << "  -h, --help               Show this help message and exit\n"
              << "  -v, --version            Display version information\n\n"
              << col(BOLD) << "Examples:" << col(RESET) << "\n"
              << "  " << progName << " --source \"Rajiv Chowk\" --dest \"Noida City Centre\"\n"
              << "  " << progName << " -s \"Rithala\" -d \"Welcome\" --json\n"
              << "  " << progName << " -s \"Vishwavidyalaya\" -d \"Huda City Centre\" -V\n"
              << "  " << progName << "                   (Runs in interactive mode)\n\n";
}

void printBanner() {
    using namespace metroworks::utils::color;
    bool c = metroworks::utils::isColorSupported();
    auto col = [c](const char* code) { return c ? code : ""; };

    std::cout << col(CYAN) << col(BOLD)
              << "\t  __  __      _              __        __         _ \n"
              << "\t |  \\/  | ___| |_ _ __ ___   \\ \\      / /__  _ __| | _____\n"
              << "\t | |\\/| |/ _ \\ __| '__/ _ \\   \\ \\ /\\ / / _ \\| '__| |/ / __|\n"
              << "\t | |  | |  __/ |_| | | (_) |   \\ V  V / (_) | |  |   <\\__ \\\n"
              << "\t |_|  |_|\\___|\\__|_|  \\___/     \\_/\\_/ \\___/|_|  |_|\\_\\___/\n"
              << col(RESET)
              << "\t         " << col(YELLOW) << "Delhi Metro Route Planner & Fare System v2.0" << col(RESET) << "\n\n";
}

} // namespace

int main(int argc, char* argv[]) {
    std::string source;
    std::string destination;
    std::string dataDir;
    std::string dotOutPath = "finalmap.dot";
    bool listStations = false;
    bool jsonOutput = false;
    bool visualize = false;
    bool hasDotExport = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-l" || arg == "--list") {
            listStations = true;
        } else if (arg == "-j" || arg == "--json") {
            jsonOutput = true;
        } else if (arg == "-V" || arg == "--visualize") {
            visualize = true;
        } else if (arg == "--dot") {
            if (i + 1 < argc) {
                dotOutPath = argv[++i];
                hasDotExport = true;
            } else {
                std::cerr << "Error: --dot requires an output file path.\n";
                return 1;
            }
        } else if (arg == "--data-dir") {
            if (i + 1 < argc) {
                dataDir = argv[++i];
            } else {
                std::cerr << "Error: --data-dir requires a directory path.\n";
                return 1;
            }
        } else if (arg == "-s" || arg == "--source") {
            if (i + 1 < argc) {
                source = argv[++i];
            } else {
                std::cerr << "Error: " << arg << " requires a station name.\n";
                return 1;
            }
        } else if (arg == "-d" || arg == "--dest" || arg == "--destination") {
            if (i + 1 < argc) {
                destination = argv[++i];
            } else {
                std::cerr << "Error: " << arg << " requires a station name.\n";
                return 1;
            }
        } else {
            std::cerr << "Unknown option: " << arg << "\n"
                      << "Run '" << argv[0] << " --help' for usage.\n";
            return 1;
        }
    }

    // Initialize core components
    metroworks::MetroNetwork network;
    metroworks::FareCalculator fareCalc;

    try {
        metroworks::DataLoader::loadAll(dataDir, network, fareCalc);
    } catch (const std::exception& e) {
        if (jsonOutput) {
            std::cout << "{\"found\": false, \"error\": \""
                      << metroworks::utils::escapeJsonString(e.what()) << "\"}\n";
        } else {
            std::cerr << "Error initializing MetroWorks: " << e.what() << "\n";
        }
        return 1;
    }

    const auto& catalog = network.getStationCatalog();

    // Handle listing stations
    if (listStations) {
        auto names = catalog.getAllCanonicalNames();
        if (jsonOutput) {
            std::cout << "[\n";
            for (size_t i = 0; i < names.size(); ++i) {
                std::cout << "  \"" << metroworks::utils::escapeJsonString(names[i]) << "\""
                          << (i + 1 < names.size() ? "," : "") << "\n";
            }
            std::cout << "]\n";
        } else {
            std::cout << "Available Delhi Metro Stations (" << names.size() << " stations):\n";
            for (size_t i = 0; i < names.size(); ++i) {
                std::cout << "  " << std::setw(3) << (i + 1) << ". " << names[i] << "\n";
            }
        }
        return 0;
    }

    metroworks::RoutePlanner planner(network, fareCalc);

    // Interactive mode if source/dest not supplied
    if (source.empty() || destination.empty()) {
        if (jsonOutput) {
            std::cout << "{\"found\": false, \"error\": \"Source and destination stations are required.\"}\n";
            return 1;
        }

        printBanner();

        while (source.empty()) {
            std::cout << "Enter Source Station: ";
            if (!std::getline(std::cin, source)) return 0;
            source = metroworks::utils::trim(source);
            if (!catalog.hasStation(source)) {
                auto sug = catalog.suggestStations(source, 3);
                std::cout << " Station '" << source << "' not recognized.";
                if (!sug.empty()) {
                    std::cout << " Did you mean: ";
                    for (size_t k = 0; k < sug.size(); ++k) {
                        if (k > 0) std::cout << ", ";
                        std::cout << "'" << sug[k] << "'";
                    }
                }
                std::cout << "\n Please try again.\n\n";
                source.clear();
            }
        }

        while (destination.empty()) {
            std::cout << "Enter Destination Station: ";
            if (!std::getline(std::cin, destination)) return 0;
            destination = metroworks::utils::trim(destination);
            if (!catalog.hasStation(destination)) {
                auto sug = catalog.suggestStations(destination, 3);
                std::cout << " Station '" << destination << "' not recognized.";
                if (!sug.empty()) {
                    std::cout << " Did you mean: ";
                    for (size_t k = 0; k < sug.size(); ++k) {
                        if (k > 0) std::cout << ", ";
                        std::cout << "'" << sug[k] << "'";
                    }
                }
                std::cout << "\n Please try again.\n\n";
                destination.clear();
            }
        }
    }

    // Plan route
    auto route = planner.findShortestRoute(source, destination);

    if (jsonOutput) {
        std::cout << planner.toJson(route) << "\n";
    } else {
        std::cout << planner.formatRoute(route, metroworks::utils::isColorSupported());
    }

    // Handle DOT export or visualization
    if (route.found && (visualize || hasDotExport)) {
        try {
            metroworks::DotExporter::exportToFile(dotOutPath, network, &route);
            if (!jsonOutput) {
                std::cout << "Graphviz DOT route map exported to: " << dotOutPath << "\n";
            }

            if (visualize) {
                std::string pngPath = "path.png";
                std::string renderErr;
                bool ok = metroworks::DotExporter::renderToPng(dotOutPath, pngPath, renderErr);
                if (ok) {
                    if (!jsonOutput) {
                        std::cout << "Rendered visualization to: " << pngPath << "\n";
                    }
                    metroworks::DotExporter::openImageViewer(pngPath);
                } else if (!jsonOutput) {
                    std::cout << "\n[Notice] " << renderErr << "\n";
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Visualization error: " << e.what() << "\n";
        }
    }

    return route.found ? 0 : 1;
}

