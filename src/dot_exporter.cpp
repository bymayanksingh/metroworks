#include "metroworks/dot_exporter.hpp"
#include "metroworks/utils.hpp"
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cstdlib>
#include <array>
#include <memory>

namespace metroworks {

std::string DotExporter::generateDot(const MetroNetwork& network,
                                     const RouteResult* routeResult) {
    std::ostringstream ss;
    ss << "graph DelhiMetro {\n";
    ss << "  graph [rankdir=TB, bgcolor=\"#FFFFFF\", dpi=300, margin=0.5];\n";
    ss << "  node [shape=box, style=\"rounded,filled\", fillcolor=\"#F5F5F5\", "
       << "color=\"#9E9E9E\", fontname=\"Helvetica,Arial,sans-serif\", fontsize=10, height=0.3];\n";
    ss << "  edge [fontname=\"Helvetica,Arial,sans-serif\", fontsize=8];\n\n";

    // Set of edges in shortest path for quick lookup: "u::v"
    std::unordered_set<std::string> pathEdges;
    std::unordered_set<std::string> pathNodes;
    std::string startNode;
    std::string destNode;
    std::unordered_set<std::string> interchangeNodes;

    if (routeResult && routeResult->found && routeResult->path.size() >= 2) {
        startNode = routeResult->path.front();
        destNode = routeResult->path.back();

        for (size_t i = 0; i + 1 < routeResult->path.size(); ++i) {
            std::string u = utils::toLowerCase(utils::trim(routeResult->path[i]));
            std::string v = utils::toLowerCase(utils::trim(routeResult->path[i + 1]));
            if (u > v) std::swap(u, v);
            pathEdges.insert(u + "::" + v);
            pathNodes.insert(routeResult->path[i]);
        }
        pathNodes.insert(destNode);

        for (const auto& ic : routeResult->interchanges) {
            interchangeNodes.insert(ic.station);
        }
    }

    // Node styling for path nodes
    if (!startNode.empty()) {
        ss << "  // Highlighted path endpoints and interchanges\n";
        ss << "  \"" << utils::escapeDotString(startNode)
           << "\" [fillcolor=\"#C8E6C9\", color=\"#2E7D32\", penwidth=2.5, fontname=\"Helvetica-Bold\"];\n";
        ss << "  \"" << utils::escapeDotString(destNode)
           << "\" [fillcolor=\"#FFCDD2\", color=\"#C62828\", penwidth=2.5, fontname=\"Helvetica-Bold\"];\n";

        for (const auto& icStation : interchangeNodes) {
            if (icStation != startNode && icStation != destNode) {
                ss << "  \"" << utils::escapeDotString(icStation)
                   << "\" [fillcolor=\"#FFF9C4\", color=\"#F57F17\", penwidth=2.0];\n";
            }
        }
        ss << "\n";
    }

    // Edges
    ss << "  // Metro Network Edges\n";
    for (const auto& edge : network.getAllEdges()) {
        std::string uKey = utils::toLowerCase(utils::trim(edge.fromStation));
        std::string vKey = utils::toLowerCase(utils::trim(edge.toStation));
        if (uKey > vKey) std::swap(uKey, vKey);

        bool isOnPath = (pathEdges.find(uKey + "::" + vKey) != pathEdges.end());

        std::string colorName = "gray";
        auto lineOpt = network.getLineInfo(edge.lineId);
        if (lineOpt) {
            colorName = lineOpt->dotColor;
        }

        ss << "  \"" << utils::escapeDotString(edge.fromStation) << "\" -- \""
           << utils::escapeDotString(edge.toStation) << "\" [label=\""
           << edge.distanceKm << "\", color=\"" << colorName << "\"";

        if (isOnPath) {
            ss << ", penwidth=5.0, fontcolor=\"#B71C1C\", style=bold";
        } else {
            ss << ", penwidth=1.5, fontcolor=\"#757575\"";
        }
        ss << "];\n";
    }

    ss << "}\n";
    return ss.str();
}

void DotExporter::exportToFile(const std::string& filePath,
                              const MetroNetwork& network,
                              const RouteResult* routeResult) {
    std::ofstream out(filePath);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open file for writing DOT output: " + filePath);
    }
    out << generateDot(network, routeResult);
}

bool DotExporter::isDotAvailable() {
#if defined(_WIN32)
    int code = std::system("where dot > nul 2>&1");
#else
    int code = std::system("which dot > /dev/null 2>&1");
#endif
    return (code == 0);
}

bool DotExporter::renderToPng(const std::string& dotFilePath,
                              const std::string& pngFilePath,
                              std::string& errorMessage) {
    if (!isDotAvailable()) {
        errorMessage = "Graphviz 'dot' tool is not installed or not in PATH.\n"
                       "To enable graph visualization:\n"
                       "  macOS:  brew install graphviz\n"
                       "  Ubuntu: sudo apt-get install graphviz\n"
                       "DOT file has still been saved successfully.";
        return false;
    }

    std::ostringstream cmd;
    cmd << "dot -Tpng \"" << dotFilePath << "\" -o \"" << pngFilePath << "\"";
    int ret = std::system(cmd.str().c_str());
    if (ret != 0) {
        errorMessage = "Graphviz dot command returned non-zero exit code: " + std::to_string(ret);
        return false;
    }

    return true;
}

bool DotExporter::openImageViewer(const std::string& imagePath) {
#if defined(__APPLE__)
    std::string cmd = "open \"" + imagePath + "\" &";
#elif defined(_WIN32)
    std::string cmd = "start \"\" \"" + imagePath + "\"";
#else
    std::string cmd = "xdg-open \"" + imagePath + "\" > /dev/null 2>&1 &";
#endif
    return (std::system(cmd.c_str()) == 0);
}

} // namespace metroworks

