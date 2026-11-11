<p align="center">
  <img src="./assets/logo/train.png" width="120">
</p>

<h1 align="center">MetroWorks</h1>

<p align="center">
  <strong>Production-Grade Delhi Metro Route Planner, Fare Calculator & Visualizer</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=for-the-badge&logo=c%2B%2B" alt="C++17">
  <img src="https://img.shields.io/badge/Build-CMake%20%7C%20Make-brightgreen?style=for-the-badge" alt="Build Systems">
  <img src="https://img.shields.io/badge/Tests-31%20Passed-success?style=for-the-badge" alt="Tests">
  <img src="https://img.shields.io/badge/Platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey?style=for-the-badge" alt="Platforms">
  <img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="License">
</p>

---

## 📖 Overview

**MetroWorks** is a high-performance C++ application for calculating shortest paths, multi-line transfers, travel times, and fare prices across the entire Delhi Metro network, **fully updated as per 2026**.

Originally conceived as a data structures academic project, MetroWorks has been refactored into a **modular, production-ready system**:
- **Modern C++17 Architecture**: Decoupled domain models, zero global state, standard headers, and RAII.
- **Fast Algorithm**: Min-priority-queue Dijkstra's Single-Source Shortest Path running in $\mathcal{O}((V + E) \log V)$.
- **Full 2026 Network Coverage**: 231 operational stations across 9 color-coded lines (Red, Yellow, Blue, Green, Violet, Orange/Airport Express, Magenta, Pink, and Grey lines) including major modern extensions (Yashobhoomi Dwarka Sector 25, Shaheed Sthal Ghaziabad, Noida Electronic City, Trilokpuri Pink Line ring link, and Bahadurgarh).
- **Official 2026 Fare Revision Engine**: Models the August 2025 DMRC fare revision (slabs: ₹11, ₹21, ₹32, ₹43, ₹54, ₹64) with Smart Card 10% discount and Sunday/National Holiday concession rates.
- **Station Renames & Aliases**: Supports modern 2026 official renames (e.g. `Millennium City Centre Gurugram` $\leftrightarrow$ `HUDA City Centre`, `Supreme Court` $\leftrightarrow$ `Pragati Maidan`).
- **Fuzzy Search & Typo Suggestions**: Employs Levenshtein edit distance and substring matching to guide users when entering station names.
- **Multiple Output Modes**: Interactive terminal UX with ANSI colors, machine-readable JSON for backend services, and Graphviz DOT graph export.
- **Cross-Platform**: Zero OS-exclusive shell dependencies; natively builds and runs on macOS, Linux, and Windows.
- **100% Tested**: Comprehensive unit and integration test suite covering algorithms, parsers, and Delhi Metro routing (31 automated tests).

---

## 🏛️ Architecture & Project Structure

```
metroworks/
├── CMakeLists.txt                # Modern CMake build configuration
├── Makefile                      # Portable Unix Makefile
├── README.md                     # Project documentation & usage
├── LICENSE                       # MIT License
├── .gitignore                    # Git ignore rules
├── include/                      # Public C++ API headers
│   └── metroworks/
│       ├── types.hpp             # Core domain types (StationId, RouteResult, etc.)
│       ├── graph.hpp             # Generic Dijkstra & graph algorithm template
│       ├── station.hpp           # Station catalog & fuzzy typo matching
│       ├── fare_calculator.hpp   # Fare matrix lookup & bounds validation
│       ├── metro_network.hpp     # Delhi Metro network & line topology
│       ├── route_planner.hpp     # High-level route finding & transfer detection
│       ├── data_loader.hpp       # Robust CSV/TSV data parsers
│       ├── dot_exporter.hpp      # Graphviz DOT generator & safe renderer
│       └── utils.hpp             # String, Levenshtein & ANSI color helpers
├── src/                          # Implementation files
│   ├── main.cpp                  # CLI application entry point
│   ├── station.cpp
│   ├── fare_calculator.cpp
│   ├── metro_network.cpp
│   ├── data_loader.cpp
│   ├── route_planner.cpp
│   ├── dot_exporter.cpp
│   └── utils.cpp
├── tests/                        # Comprehensive test suite (27 automated tests)
│   ├── test_runner.hpp           # Zero-dependency test harness
│   ├── test_main.cpp             # Test executable entry
│   ├── test_graph.cpp            # Dijkstra algorithm unit tests
│   ├── test_catalog.cpp          # Station catalog & alias tests
│   ├── test_fare.cpp             # Fare calculator matrix tests
│   ├── test_data_loader.cpp      # Data loader tests
│   ├── test_route_planner.cpp    # End-to-end Delhi Metro route tests
│   └── test_dot_exporter.cpp     # Graphviz export tests
├── data/                         # Delhi Metro datasets
│   ├── stations.txt              # 235 Delhi Metro station IDs and names
│   ├── data.txt                  # Physical network connections & line boundaries
│   ├── fare.csv                  # Processed 235x235 fare matrix
│   └── raw/                      # Raw source data
│       └── Fare Matrix Delhi Metro.xlsx
├── docs/                         # Documentation, academic report & vector maps
│   ├── DS_MetroWork_Project.pdf  # Course project report
│   └── maps/                     # Vector SVG system maps
├── assets/                       # Media, branding & diagrams
│   ├── logo/
│   │   ├── train.png
│   │   └── train1.png
│   ├── screenshots/              # Application screenshots
│   └── diagrams/                 # Rendered network graphs
└── legacy/                       # Archived v1 academic prototype
    ├── metroworks_v1.cpp         # Original monolithic prototype
    └── graphvizdotfiles/         # Original dot exploration files
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
./bin/metroworks -s "Shaheed Sthal" -d "Millennium City Centre Gurugram"
```
```
========================================================
                   METROWORKS ROUTE                     
========================================================

  From:        Shaheed Sthal
  To:          Millennium City Centre Gurugram
  Distance:    51.5 km
  Stops:       40
  Interchanges: 1
  Est. Time:   ~89 mins
  Fare (2026): Rs 64 (Smart Card: Rs 58) [Sun/Holiday: Rs 54]

--- Journey Itinerary ---

  * [Red Line] Ride from Shaheed Sthal to Kashmere Gate
    (15 stops, 19.0 km)
    -> Transfer at Kashmere Gate to Yellow Line

  * [Yellow Line] Ride from Kashmere Gate to Millennium City Centre Gurugram
    (25 stops, 32.5 km)

--- Station Sequence ---
  [START] Shaheed Sthal
     |   Hindon River
     |   Arthala
     ...
  [XFER]  Kashmere Gate (Change Line)
     |   Chandni Chowk
     ...
  [DEST]  Millennium City Centre Gurugram
```

#### 2. JSON Output Mode (for APIs & Services)
```bash
./bin/metroworks -s "New Delhi" -d "Yashobhoomi Dwarka Sector 25" --json
```
```json
{
  "found": true,
  "source": "New Delhi",
  "destination": "Yashobhoomi Dwarka Sector 25",
  "totalDistanceKm": 23.00,
  "totalStops": 6,
  "estimatedTimeMinutes": 13,
  "fareInr": 54,
  "smartCardFareInr": 49,
  "sundayFareInr": 43,
  "interchanges": [],
  "path": [
    "New Delhi",
    "Shivaji Stadium",
    "Dhaula Kuan",
    "Delhi Aerocity",
    "Airport (T3)",
    "Dwarka Sector 21",
    "Yashobhoomi Dwarka Sector 25"
  ]
}
```

#### 3. Typo-Tolerant Suggestions & Historical Renames
```bash
# Supports legacy station names automatically
./bin/metroworks -s "HUDA City Centre" -d "Pragati Maidan"
```

---

## 🧪 Testing

MetroWorks includes a self-contained unit test suite verifying:
- Graph construction, Dijkstra shortest paths, disconnected graphs, cycles.
- Case-insensitivity, station aliases, and Levenshtein typo suggestion algorithms.
- 2026 official distance slab fare calculations (standard, smart card, sunday).
- Real Delhi Metro end-to-end paths across all 9 operational lines.
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

 Total Tests: 31 | Passed: 31 | Failed: 0
==================================================
```

---

## 📄 License
This project is licensed under the [MIT License](LICENSE).
Original implementation by [Mayank Singh](https://github.com/bymayanksingh).
