#ifndef METROWORKS_UTILS_HPP
#define METROWORKS_UTILS_HPP

#include <string>
#include <vector>
#include <string_view>

namespace metroworks::utils {

/**
 * @brief Trims leading and trailing whitespace from a string.
 */
std::string trim(std::string_view sv);

/**
 * @brief Converts a string to lowercase.
 */
std::string toLowerCase(std::string_view sv);

/**
 * @brief Compares two strings case-insensitively.
 */
bool equalsIgnoreCase(std::string_view a, std::string_view b);

/**
 * @brief Splits a string by delimiter.
 */
std::vector<std::string> split(const std::string& s, char delimiter);

/**
 * @brief Calculates Levenshtein edit distance between two strings.
 */
size_t levenshteinDistance(std::string_view s1, std::string_view s2);

/**
 * @brief Finds closest matches to an input string from candidate list using Levenshtein distance.
 */
std::vector<std::string> findClosestMatches(std::string_view input,
                                            const std::vector<std::string>& candidates,
                                            size_t maxSuggestions = 3);

/**
 * @brief Escapes characters for DOT format.
 */
std::string escapeDotString(const std::string& str);

/**
 * @brief Escapes characters for JSON output.
 */
std::string escapeJsonString(const std::string& str);

/**
 * @brief Returns whether stdout supports ANSI color codes.
 */
bool isColorSupported();

namespace color {
    extern const char* RESET;
    extern const char* BOLD;
    extern const char* DIM;
    extern const char* RED;
    extern const char* GREEN;
    extern const char* YELLOW;
    extern const char* BLUE;
    extern const char* MAGENTA;
    extern const char* CYAN;
    extern const char* WHITE;
}

} // namespace metroworks::utils

#endif // METROWORKS_UTILS_HPP
