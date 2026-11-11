<p align="center">
  <img src="./images/logo/train.png" width="120">
</p>

<h1 align="center">MetroWorks</h1>

<p align="center">
  <strong>Production-Grade Delhi Metro Route Planner, Fare Calculator & Visualizer</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=for-the-badge&logo=c%2B%2B" alt="C++17">
  <img src="https://img.shields.io/badge/Build-CMake%20%7C%20Make-brightgreen?style=for-the-badge" alt="Build Systems">
  <img src="https://img.shields.io/badge/Tests-18%20Passed-success?style=for-the-badge" alt="Tests">
  <img src="https://img.shields.io/badge/Platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey?style=for-the-badge" alt="Platforms">
  <img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="License">
</p>

---

## 📖 Overview

**MetroWorks** is a high-performance C++ application for calculating shortest paths, multi-line transfers, travel times, and fare prices across the Delhi Metro network.

Originally conceived as a data structures academic project, MetroWorks has been refactored into a **modular, production-ready system**:
- **Modern C++17 Architecture**: Decoupled domain models, zero global state, standard headers, and RAII.
- **Fast Algorithm**: Min-priority-queue Dijkstra's Single-Source Shortest Path running in $\mathcal{O}((V + E) \log V)$.
- **Intelligent Routing**: Computes step-by-step itineraries, identifies line interchanges (transfers), and estimates transit times.
- **Fare Matrix Engine**: Direct fare lookups from Delhi Metro fare matrices with station alias handling (e.g. British vs. American spellings like *Centre* / *Center*).
- **Fuzzy Search & Typo Suggestions**: Employs Levenshtein edit distance and substring matching to guide users when entering station names.
- **Multiple Output Modes**: Interactive terminal UX with ANSI colors, machine-readable JSON for backend services, and Graphviz DOT graph export.
- **Cross-Platform**: Zero OS-exclusive shell dependencies (`fim`, `zenity`, `sl` removed); natively builds and runs on macOS, Linux, and Windows.
- **100% Tested**: Comprehensive unit and integration test suite covering algorithms, parsers, and Delhi Metro routing.

---

## 🏛️ Architecture & Project Structure

```
metroworks/
├── CMakeLists.txt                # Modern CMake build configuration
├── Makefile                      # Portable Makefile (macOS & Linux)
├── include/metroworks/           # Public C++ API headers
│   ├── types.hpp                 # Core domain types (StationId, RouteResult, etc.)
│   ├── graph.hpp                 # Generic Dijkstra & graph algorithm template
│   ├── station.hpp               # Station catalog & fuzzy typo matching
│   ├── fare_calculator.hpp       # Fare matrix lookup & bounds validation
│   ├── metro_network.hpp         # Delhi Metro network & line topology
│   ├── route_planner.hpp         # High-level route finding & transfer detection
│   ├── data_loader.hpp           # Robust CSV/TSV data parsers
│   ├── dot_exporter.hpp          # Graphviz DOT generator & safe renderer
│   └── utils.hpp                 # String, Levenshtein & ANSI color helpers
├── src/                          # Implementation files
│   ├── main.cpp                  # CLI application entry point
│   ├── station.cpp
│   ├── fare_calculator.cpp
│   ├── metro_network.cpp
│   ├── data_loader.cpp
│   ├── route_planner.cpp
│   ├── dot_exporter.cpp
│   └── utils.cpp
├── tests/                        # Comprehensive test suite
│   ├── test_runner.hpp           # Zero-dependency test harness
│   ├── test_main.cpp             # Test executable entry
│   ├── test_graph.cpp            # Dijkstra algorithm unit tests
│   ├── test_catalog.cpp          # Station catalog & alias tests
│   ├── test_fare.cpp             # Fare calculator matrix tests
│   ├── test_route_planner.cpp    # End-to-end Delhi Metro route tests
│   └── test_dot_exporter.cpp     # Graphviz export tests
├── data/                         # Datasets
│   ├── data.txt                  # Physical network connections & line boundaries
│   ├── stations.txt              # 235 Delhi Metro station IDs and names
│   └── fare.csv                  # Official 235x235 fare matrix
└── finalmap.dot                  # Sample Graphviz DOT route export
```

---

## 🚀 Building & Running

### Requirements
- **C++ Compiler**: Clang (Apple Clang 11+ or LLVM 10+), GCC 9+, or MSVC 2019+
- **Build Tool**: `make` or `cmake` (3.14+)
- **Optional**: `graphviz` (`dot`) for rendering PNG route diagrams

### Build with Make

```bash
# Build the CLI application and run all unit tests
make

# Run the test suite explicitly
make test

# Clean build artifacts
make clean
```

### Build with CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

---

## 💻 CLI Usage

The compiled executable is placed in `./bin/metroworks`.

```bash
Usage: ./bin/metroworks [OPTIONS]

Options:
  -s, --source <station>   Source station name
  -d, --dest <station>     Destination station name
  -l, --list               List all available Delhi Metro stations
  -j, --json               Output route information in JSON format
  -V, --visualize          Generate and open Graphviz route diagram
      --dot <file>         Export route Graphviz DOT file to path (default: finalmap.dot)
      --data-dir <path>    Specify custom directory for data files
  -h, --help               Show this help message and exit
  -v, --version            Display version information
```

### Examples

#### 1. Plan a route between two stations
```bash
./bin/metroworks -s "Rithala" -d "Huda City Centre"
```
```
========================================================
                   METROWORKS ROUTE                     
========================================================

  From:        Rithala
  To:          Huda City Centre
  Distance:    46.6 km
  Stops:       34
  Interchanges: 1
  Est. Time:   ~76 mins
  Fare:        Rs 60

--- Journey Itinerary ---

  * [Red Line] Ride from Rithala to Kashmere Gate
    (9 stops, 14.1 km)
    -> Transfer at Kashmere Gate to Yellow Line

  * [Yellow Line] Ride from Kashmere Gate to Huda City Centre
    (25 stops, 32.5 km)

--- Station Sequence ---
  [START] Rithala
     |   Netaji Subhash Place
     ...
  [XFER]  Kashmere Gate (Change Line)
     |   Chandni Chowk
     ...
  [DEST]  Huda City Centre
```

#### 2. JSON Output Mode (for APIs & Services)
```bash
./bin/metroworks -s "Rajiv Chowk" -d "Noida Sector-18" --json
```
```json
{
  "found": true,
  "source": "Rajiv Chowk",
  "destination": "Noida Sector-18",
  "totalDistanceKm": 13.50,
  "totalStops": 12,
  "estimatedTimeMinutes": 25,
  "fareInr": 40,
  "interchanges": [],
  "path": [
    "Rajiv Chowk",
    "Barakhamba Road",
    "Mandi House",
    "Pragati Maiden",
    "Inderprastha",
    "Yamuna Bank",
    "Akshardham",
    "Mayur Vihar Phase-1",
    "Mayur Vihar Extention",
    "New Ashok Nagar",
    "Noida Sector-15",
    "Noida Sector-16",
    "Noida Sector-18"
  ]
}
```

#### 3. Typo-Tolerant Suggestions
```bash
./bin/metroworks -s "rajiv chwk" -d "noida sec 18"
```
```
Error: Unknown source station 'rajiv chwk'. Did you mean: 'Rajiv Chowk', 'Patel Chowk', 'Bata Chowk'?
```

#### 4. Route Visualization
```bash
./bin/metroworks -s "Rajiv Chowk" -d "Welcome" -V
```
Generates `finalmap.dot` with the traversed path highlighted (`penwidth=5.0`) and renders `path.png` if Graphviz `dot` is installed.

#### 5. Interactive Mode
Simply run `./bin/metroworks` without arguments:
```bash
./bin/metroworks
```

---

## 🧪 Testing

MetroWorks includes a self-contained unit test suite verifying:
- Graph construction, Dijkstra shortest paths, disconnected graphs, cycles.
- Case-insensitivity, station aliases, and Levenshtein typo suggestion algorithms.
- Fare matrix boundary safety and multi-candidate interchange lookup.
- Real Delhi Metro end-to-end paths across Red, Yellow, Blue, Green, and Violet lines.
- Graphviz DOT generation and path-highlighting accuracy.

Run all tests via:
```bash
make test
```

Output:
```
==================================================
           RUNNING METROWORKS TEST SUITE          
==================================================

 RUNNING: [GraphTest] EmptyGraph ... PASSED
 RUNNING: [GraphTest] SimpleDijkstra ... PASSED
 RUNNING: [GraphTest] DisconnectedNodes ... PASSED
 RUNNING: [GraphTest] SelfLoopAndSameSourceDest ... PASSED
 RUNNING: [GraphTest] RejectNegativeWeights ... PASSED
 RUNNING: [CatalogTest] CaseInsensitiveLookup ... PASSED
 RUNNING: [CatalogTest] AliasResolution ... PASSED
 RUNNING: [CatalogTest] InterchangeMultipleIds ... PASSED
 RUNNING: [CatalogTest] TypoSuggestions ... PASSED
 RUNNING: [FareTest] BasicMatrixLookup ... PASSED
 RUNNING: [FareTest] OutOfBoundsSafety ... PASSED
 RUNNING: [FareTest] MultiCandidateLookup ... PASSED
 RUNNING: [RoutePlannerTest] EndToEndRedLine ... PASSED
 RUNNING: [RoutePlannerTest] EndToEndYellowLine ... PASSED
 RUNNING: [RoutePlannerTest] LineTransferDetection ... PASSED
 RUNNING: [RoutePlannerTest] AliasResolutionRoute ... PASSED
 RUNNING: [RoutePlannerTest] InvalidStationHandling ... PASSED
 RUNNING: [DotExporterTest] GenerateDotContainsHighlightedPath ... PASSED

--------------------------------------------------
Total Tests: 18 | Passed: 18 | Failed: 0
--------------------------------------------------
```

---

## 📄 License
This project is licensed under the [MIT License](LICENSE).
Original implementation by [Mayank Singh](https://github.com/bymayanksingh).
