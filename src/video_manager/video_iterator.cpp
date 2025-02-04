#include "video_iterator.hpp"


VideoIterator::VideoIterator(const std::vector<Video> &videos, const size_t pos, DownloadedVideos &downloaded_videos) :
    m_videos(videos), m_pos(pos), m_downloaded_videos(downloaded_videos) {}

VideoIterator &VideoIterator::operator++() {
    ++m_pos;
    return *this;
}

VideoIterator VideoIterator::operator++(int) {
    const VideoIterator temp = *this;
    ++(*this);
    return temp;
}

bool VideoIterator::operator==(const VideoIterator &other) const {
    return m_pos == other.m_pos && &m_videos == &other.m_videos;
}

bool VideoIterator::operator!=(const VideoIterator &other) const { return !(*this == other); }

VideoWrapper VideoIterator::operator*() const { return {m_videos[m_pos], m_downloaded_videos}; }
