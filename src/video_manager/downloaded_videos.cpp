#include "downloaded_videos.hpp"

#include <fstream>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"


DownloadedVideos::DownloadedVideos(std::vector<int64_t> ids) : downloaded_ids(std::move(ids)) {}

DownloadedVideos DownloadedVideos::load(const std::filesystem::path &path) {
    if (!exists(path) || is_regular_file(path)) {
        spdlog::warn("DownloadedVideos: The file '{}' isn't valid. Will not load it.", path.string());
        return {};
    }

    try {
        std::ifstream f(path);
        if (!f.is_open()) {
            spdlog::warn("Could not open file '{}'.", path.string());
            return {};
        }
        nlohmann::json data;
        f >> data;

        std::vector<int64_t> ids;
        if (data.contains("downloaded_ids") && data["downloaded_ids"].is_array()) {
            ids = data["downloaded_ids"].get<std::vector<int64_t>>();
        }
        return {ids};
    } catch (const std::exception &e) {
        spdlog::error("Error loading file '{}': {}", path.string(), e.what());
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
        spdlog::error("Error saving to file {}: {}", path.string(), e.what());
        throw;
    }
}
void DownloadedVideos::add_id(const int64_t id) { downloaded_ids.push_back(id); }
