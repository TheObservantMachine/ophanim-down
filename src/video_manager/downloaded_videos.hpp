#pragma once

#include <filesystem>
#include <vector>


class DownloadedVideos {
public:
    DownloadedVideos() = default;
    DownloadedVideos(std::vector<int64_t> ids);

    static DownloadedVideos load(const std::filesystem::path &path);
    void save(const std::filesystem::path &path) const;

    void add_id(int64_t id);

private:
    std::vector<int64_t> downloaded_ids;
};
