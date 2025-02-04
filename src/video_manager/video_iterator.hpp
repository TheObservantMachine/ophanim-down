#pragma once


#include <vector>

#include "downloaded_videos.hpp"
#include "video.hpp"
#include "video_wrapper.hpp"


class VideoIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = VideoWrapper;
    using difference_type = std::ptrdiff_t;

    VideoIterator(const std::vector<Video> &videos, size_t pos, DownloadedVideos &downloaded_videos);

    // Pre-increment.
    VideoIterator &operator++();

    // Post-increment.
    VideoIterator operator++(int);

    // Equality operators.
    bool operator==(const VideoIterator &other) const;
    bool operator!=(const VideoIterator &other) const;

    // Dereference operator returns a VideoWrapper.
    VideoWrapper operator*() const;

private:
    const std::vector<Video> &m_videos;
    size_t m_pos;
    DownloadedVideos &m_downloaded_videos;
};
