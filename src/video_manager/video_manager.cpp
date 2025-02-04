#include "video_manager.hpp"

#include <unordered_set>
#include <utility>
#include "spdlog/spdlog.h"

#include "sqlite3.h"


VideoManager::VideoManager(std::filesystem::path save_path, DownloadedVideos downloaded_videos,
                           const std::vector<Video> &to_download) :
    m_save_path{std::move(save_path)}, m_downloaded_videos{std::move(downloaded_videos)}, m_to_download{to_download} {}

VideoIterator VideoManager::begin() { return {m_to_download, 0, m_downloaded_videos}; }
VideoIterator VideoManager::end() { return {m_to_download, m_to_download.size(), m_downloaded_videos}; }

VideoManager VideoManager::into_filtered() const {
    const std::unordered_set downloaded_ids(m_downloaded_videos.get_ids().begin(), m_downloaded_videos.get_ids().end());
    std::vector<Video> filtered;
    for (const auto &v: m_to_download) {
        if (!downloaded_ids.contains(v.id))
            filtered.push_back(v);
    }
    return {m_save_path, m_downloaded_videos, filtered};
}

VideoManager VideoManager::create(sqlite3 *db, const std::filesystem::path &save_path) {
    DownloadedVideos downloaded_videos = DownloadedVideos::load(save_path);
    std::vector<Video> to_download;

    const auto query = "SELECT id, duration_ms, media_type, link, width, height, race_id FROM video";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("Failed to prepare query: " + std::string(sqlite3_errmsg(db)));

    // Execute the query and populate the to_download vector.
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Video video;
        video.id = sqlite3_column_int64(stmt, 0);
        video.duration_ms = sqlite3_column_int64(stmt, 1);
        const auto media_text = sqlite3_column_text(stmt, 2);
        video.media_type = media_text ? reinterpret_cast<const char *>(media_text) : "";
        const auto link_text = sqlite3_column_text(stmt, 3);
        video.link = link_text ? reinterpret_cast<const char *>(link_text) : "";
        video.width = sqlite3_column_int(stmt, 4);
        video.height = sqlite3_column_int(stmt, 5);
        video.race_id = sqlite3_column_int64(stmt, 6);
        to_download.push_back(video);
    }
    sqlite3_finalize(stmt);

    spdlog::info("Loaded {} videos to download", to_download.size());
    return {save_path, downloaded_videos, to_download};
}

void VideoManager::save() const { m_downloaded_videos.save(m_save_path); }

std::size_t VideoManager::len() const { return m_downloaded_videos.get_ids().size(); }
