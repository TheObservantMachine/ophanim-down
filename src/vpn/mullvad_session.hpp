#pragma once

#include "curl/curl.h"

#include <filesystem>
#include <string>


class Video;

class InvalidStatusCode final : public std::runtime_error {
public:
    explicit InvalidStatusCode(const std::string &s) : runtime_error(s) {}
    explicit InvalidStatusCode(const char *string) : runtime_error(string) {}
};


struct AmIMullvad {
    bool is_mullvad = false;
    std::string ip_address, server_config, location;
};

class MullvadSession {
public:
    MullvadSession();
    ~MullvadSession();

    std::string get(const char *url);
    void download_video(const std::filesystem::path &save_dir, const Video &video);
    AmIMullvad am_i_mullvad();

private:
    CURL *m_curl;
    const std::string m_proxy;
};
