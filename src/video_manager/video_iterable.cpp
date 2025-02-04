#include "video_iterable.hpp"

VideoIterable::VideoIterable(const std::vector<Video> &videos, DownloadedVideos &downloaded_videos) :
    m_videos(videos), m_downloaded_videos(downloaded_videos) {}

VideoIterator VideoIterable::begin() const { return {m_videos, 0, m_downloaded_videos}; }

VideoIterator VideoIterable::end() const { return {m_videos, m_videos.size(), m_downloaded_videos}; }
