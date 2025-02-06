#pragma once

#include "cpr/session.h"

class Video;

class InvalidStatusCode final : public std::runtime_error {
public:
    explicit InvalidStatusCode(const std::string &s) : runtime_error(s) {}
    explicit InvalidStatusCode(const char *string) : runtime_error(string) {}
};

class MullvadSession : public cpr::Session {
public:
    MullvadSession();

    cpr::Response Get(const cpr::Url &url);
    void download_video(const std::filesystem::path &save_dir, const Video &video);
    bool am_i_mullvad();
};
