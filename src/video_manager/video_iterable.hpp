#pragma once

#include <vector>

#include "../video.hpp"
#include "downloaded_videos.hpp"
#include "video_iterator.hpp"

namespace manager {

class VideoIterable {
public:
    VideoIterable(const std::vector<Video> &videos, DownloadedVideos &downloaded_videos);

    [[nodiscard]] VideoIterator begin() const;
    [[nodiscard]] VideoIterator end() const;

private:
    const std::vector<Video> &m_videos;
    DownloadedVideos &m_downloaded_videos;
};

} // namespace manager
