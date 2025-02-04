#pragma once

#include <filesystem>
#include <vector>


class DownloadedVideos {
public:
    std::vector<int64_t> downloaded_ids;

    DownloadedVideos() = default;
    DownloadedVideos(const std::vector<int> &ids);

    static DownloadedVideos load(const std::filesystem::path &path);
    void save(const std::filesystem::path &path) const;
};
