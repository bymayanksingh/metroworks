#include "metroworks/utils.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <cmath>

#if defined(_WIN32)
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

namespace metroworks::utils {

std::string trim(std::string_view sv) {
    auto start = sv.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos) {
        return "";
    }
    auto end = sv.find_last_not_of(" \t\n\r");
    return std::string(sv.substr(start, end - start + 1));
}

std::string toLowerCase(std::string_view sv) {
    std::string res;
    res.reserve(sv.size());
    for (char c : sv) {
        res.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return res;
}

bool equalsIgnoreCase(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    return std::equal(a.begin(), a.end(), b.begin(), [](char c1, char c2) {
        return std::tolower(static_cast<unsigned char>(c1)) ==
               std::tolower(static_cast<unsigned char>(c2));
    });
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

size_t levenshteinDistance(std::string_view s1, std::string_view s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();

    if (m == 0) return n;
    if (n == 0) return m;

    std::vector<size_t> prevRow(n + 1);
    std::vector<size_t> currRow(n + 1);

    for (size_t j = 0; j <= n; ++j) {
        prevRow[j] = j;
    }

    for (size_t i = 1; i <= m; ++i) {
        currRow[0] = i;
        char c1 = static_cast<char>(std::tolower(static_cast<unsigned char>(s1[i - 1])));
        for (size_t j = 1; j <= n; ++j) {
            char c2 = static_cast<char>(std::tolower(static_cast<unsigned char>(s2[j - 1])));
            size_t cost = (c1 == c2) ? 0 : 1;
            currRow[j] = std::min({
                currRow[j - 1] + 1,      // insertion
                prevRow[j] + 1,          // deletion
                prevRow[j - 1] + cost    // substitution
            });
        }
        prevRow = currRow;
    }

    return prevRow[n];
}

std::vector<std::string> findClosestMatches(std::string_view input,
                                            const std::vector<std::string>& candidates,
                                            size_t maxSuggestions) {
    if (candidates.empty() || maxSuggestions == 0) {
        return {};
    }

    std::string lowInput = toLowerCase(trim(input));

    // First, look for substring matches (starts with or contains)
    std::vector<std::string> prefixMatches;
    std::vector<std::string> substringMatches;

    for (const auto& candidate : candidates) {
        std::string lowCand = toLowerCase(candidate);
        if (lowCand.find(lowInput) == 0) {
            prefixMatches.push_back(candidate);
        } else if (lowCand.find(lowInput) != std::string::npos) {
            substringMatches.push_back(candidate);
        }
    }

    std::vector<std::string> results = prefixMatches;
    for (const auto& match : substringMatches) {
        if (results.size() >= maxSuggestions) break;
        results.push_back(match);
    }

    if (results.size() >= maxSuggestions) {
        results.resize(maxSuggestions);
        return results;
    }

    // Levenshtein ranking for the rest
    struct ScoredCandidate {
        std::string name;
        size_t distance;
    };

    std::vector<ScoredCandidate> scored;
    scored.reserve(candidates.size());

    for (const auto& candidate : candidates) {
        // Skip if already in results
        if (std::find(results.begin(), results.end(), candidate) != results.end()) {
            continue;
        }
        size_t dist = levenshteinDistance(lowInput, toLowerCase(candidate));
        scored.push_back({candidate, dist});
    }

    std::sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) {
        return a.distance < b.distance;
    });

    for (const auto& item : scored) {
        if (results.size() >= maxSuggestions) break;
        // Don't suggest wildly different strings (threshold relative to length)
        if (item.distance <= std::max<size_t>(3, lowInput.length())) {
            results.push_back(item.name);
        }
    }

    return results;
}

std::string escapeDotString(const std::string& str) {
    std::string res;
    for (char c : str) {
        if (c == '"' || c == '\\') {
            res.push_back('\\');
        }
        res.push_back(c);
    }
    return res;
}

std::string escapeJsonString(const std::string& str) {
    std::ostringstream ss;
    for (char c : str) {
        switch (c) {
            case '"':  ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b";  break;
            case '\f': ss << "\\f";  break;
            case '\n': ss << "\\n";  break;
            case '\r': ss << "\\r";  break;
            case '\t': ss << "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    // Control character
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    ss << buf;
                } else {
                    ss << c;
                }
                break;
        }
    }
    return ss.str();
}

bool isColorSupported() {
    static const bool supported = (ISATTY(FILENO(stdout)) != 0);
    return supported;
}

namespace color {
    const char* RESET   = "\033[0m";
    const char* BOLD    = "\033[1m";
    const char* DIM     = "\033[2m";
    const char* RED     = "\033[31m";
    const char* GREEN   = "\033[32m";
    const char* YELLOW  = "\033[33m";
    const char* BLUE    = "\033[34m";
    const char* MAGENTA = "\033[35m";
    const char* CYAN    = "\033[36m";
    const char* WHITE   = "\033[37m";
}

} // namespace metroworks::utils
