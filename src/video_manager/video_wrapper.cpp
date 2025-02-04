#include "video_wrapper.hpp"


VideoWrapper::VideoWrapper(const Video &video, DownloadedVideos &downloaded_videos) :
    m_video(video), m_downloaded_videos(downloaded_videos) {}

void VideoWrapper::mark_done() const { m_downloaded_videos.add_id(m_video.id); }

const Video &VideoWrapper::get_video() const { return m_video; };
