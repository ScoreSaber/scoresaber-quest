#pragma once

#include <string>
#include <vector>

// Simple class to represent all kinds of versions and allow comparison between them. Does not respect semver rules beyond basic numeric comparison of parts.
class version {
    std::vector<int> parts;
public:
    version() = default;

    version(const std::string& versionStr) {
        // strip weird beatgames suffixes and semver prerelease/build metadata
        std::string cleanedVersionStr = versionStr;
        size_t suffixPos = cleanedVersionStr.find_first_of("_-+");
        if (suffixPos != std::string::npos) {
            cleanedVersionStr = cleanedVersionStr.substr(0, suffixPos);
        }

        size_t start = 0;
        size_t end = cleanedVersionStr.find('.');
        while (end != std::string::npos) {
            parts.push_back(std::stoi(cleanedVersionStr.substr(start, end - start)));
            start = end + 1;
            end = cleanedVersionStr.find('.', start);
        }
        parts.push_back(std::stoi(cleanedVersionStr.substr(start)));
    }

    bool operator<(const version& other) const {
        size_t maxLength = std::max(parts.size(), other.parts.size());
        for (size_t i = 0; i < maxLength; ++i) {
            int thisPart = (i < parts.size()) ? parts[i] : 0;
            int otherPart = (i < other.parts.size()) ? other.parts[i] : 0;
            if (thisPart < otherPart) return true;
            if (thisPart > otherPart) return false;
        }
        return false;
    }

    bool operator==(const version& other) const {
        size_t maxLength = std::max(parts.size(), other.parts.size());
        for (size_t i = 0; i < maxLength; ++i) {
            int thisPart = (i < parts.size()) ? parts[i] : 0;
            int otherPart = (i < other.parts.size()) ? other.parts[i] : 0;
            if (thisPart != otherPart) return false;
        }
        return true;
    }

    bool operator<=(const version& other) const {
        return *this < other || *this == other;
    }

    std::string str() const {
        std::string result;
        for (size_t i = 0; i < parts.size(); ++i) {
            result += std::to_string(parts[i]);
            if (i < parts.size() - 1) {
                result += ".";
            }
        }
        return result;
    }
};