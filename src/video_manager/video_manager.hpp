#pragma once

#include "../video.hpp"
#include "downloaded_videos.hpp"
#include "video_iterator.hpp"


class sqlite3;

namespace manager {

/**
 * VideoManager for keeping track of already downloaded IDS.
 * It will autosave upon destruction.
 */
class VideoManager {
public:
    VideoManager(std::filesystem::path save_path, DownloadedVideos downloaded_videos,
                 const std::vector<Video> &to_download);

    ~VideoManager();

    // Iterator support.
    VideoIterator begin();
    VideoIterator end();

    /** Returns a new VideoManager instance filtering out videos that have been
     * already downloaded (by checking against downloaded_videos.downloaded_ids).
     */
    [[nodiscard]] VideoManager into_filtered() const;

    /** Creates a VideoManager instance by querying the database.
     * (This is the synchronous equivalent to the asynchronous Python method.)
     */
    static VideoManager create(sqlite3 *db, const std::filesystem::path &save_path);

    /** Save the downloaded video IDs. */
    void save() const;

    /** Returns the number of downloaded video IDs. */
    [[nodiscard]] std::size_t len() const;

private:
    std::filesystem::path m_save_path;
    DownloadedVideos m_downloaded_videos;
    std::vector<Video> m_to_download;
};

} // namespace manager