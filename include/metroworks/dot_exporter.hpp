#ifndef METROWORKS_DOT_EXPORTER_HPP
#define METROWORKS_DOT_EXPORTER_HPP

#include "metroworks/metro_network.hpp"
#include "metroworks/types.hpp"
#include <string>
#include <optional>

namespace metroworks {

/**
 * @brief Graphviz DOT file generator and safe external renderer.
 */
class DotExporter {
public:
    DotExporter() = default;

    /**
     * @brief Generates DOT content representing the metro network with optional route highlight.
     */
    static std::string generateDot(const MetroNetwork& network,
                                  const RouteResult* routeResult = nullptr);

    /**
     * @brief Writes DOT file to disk.
     */
    static void exportToFile(const std::string& filePath,
                             const MetroNetwork& network,
                             const RouteResult* routeResult = nullptr);

    /**
     * @brief Safely renders a DOT file to PNG using Graphviz `dot`.
     * 
     * @return true if rendering succeeded, false if `dot` missing or failed.
     */
    static bool renderToPng(const std::string& dotFilePath,
                            const std::string& pngFilePath,
                            std::string& errorMessage);

    /**
     * @brief Safely opens the rendered image using platform-native viewer.
     */
    static bool openImageViewer(const std::string& imagePath);

    /**
     * @brief Checks if `dot` command-line tool is installed.
     */
    static bool isDotAvailable();
};

} // namespace metroworks

#endif // METROWORKS_DOT_EXPORTER_HPP
