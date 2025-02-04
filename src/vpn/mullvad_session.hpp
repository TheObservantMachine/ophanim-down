#pragma once

#include "cpr/session.h"

class Video;

class InvalidStatusCode final : public std::runtime_error {
public:
    explicit InvalidStatusCode(const std::string &__arg) : runtime_error(__arg) {}
    explicit InvalidStatusCode(const char *string) : runtime_error(string) {}
    explicit InvalidStatusCode(runtime_error &&runtime_error) : runtime_error(runtime_error) {}
    explicit InvalidStatusCode(const runtime_error &runtime_error) : runtime_error(runtime_error) {}
};

class MullvadSession : public cpr::Session {
public:
    MullvadSession();

    cpr::Response Get(const cpr::Url &url);
    void download_video(std::filesystem::path save_dir, Video &video);
    bool am_i_mullvad();
};
