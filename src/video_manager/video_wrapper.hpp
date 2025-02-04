#pragma once

#include "../video.hpp"
#include "downloaded_videos.hpp"

class VideoWrapper {
public:
    VideoWrapper(const Video &video, DownloadedVideos &downloaded_videos);

    void mark_done() const;

    [[nodiscard]] const Video &get_video() const;

private:
    const Video &m_video;
    DownloadedVideos &m_downloaded_videos;
};
