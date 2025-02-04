#include "downloaded_videos.hpp"

#include <fstream>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

std::vector<int> downloaded_ids;

DownloadedVideos::DownloadedVideos(const std::vector<int> &ids) : downloaded_ids(ids) {}

DownloadedVideos DownloadedVideos::load(const std::filesystem::path &path) {
    if (!exists(path) || is_regular_file(path)) {
        spdlog::warn("DownloadedVideos: The file '{}' isn't valid. Will not load it.", path);
        return {};
    }

    try {
        std::ifstream f(path);
        if (!f.is_open()) {
            spdlog::warn("Could not open file '{}'.", path);
            return {};
        }
        nlohmann::json data;
        f >> data;

        std::vector<int> ids;
        if (data.contains("downloaded_ids") && data["downloaded_ids"].is_array()) {
            ids = data["downloaded_ids"].get<std::vector<int>>();
        }
        return {ids};
    } catch (const std::exception &e) {
        spdlog::error("Error loading file '{}': {}", path, e.what());
        return {};
    }
}

void DownloadedVideos::save(const std::filesystem::path &path) const {
    try {
        std::ofstream f(path);
        if (!f.is_open())
            throw std::runtime_error("Could not open file");
        nlohmann::json data;
        data["downloaded_ids"] = downloaded_ids;
        f << data.dump();
    } catch (const std::exception &e) {
        spdlog::error("Error saving to file {}: {}", path, e.what());
        throw;
    }
}
