#pragma once

#include "curl/curl.h"

#include <filesystem>
#include <string>


class Video;

namespace vpn {

class InvalidStatusCode final : public std::runtime_error {
public:
    explicit InvalidStatusCode(const std::string &s) : runtime_error(s) {}
    explicit InvalidStatusCode(const char *string) : runtime_error(string) {}
};


struct AmIMullvad {
    bool is_mullvad = false;
    std::string ip_address, server_config, location;

public:
    bool parse_server_config(const std::string_view line);
    bool parse_ip_address_location(const std::string_view line);
};

class MullvadSession {
public:
    MullvadSession();
    ~MullvadSession();

    std::string get(const char *url);
    void download_video(const std::filesystem::path &save_dir, const Video &video);
    AmIMullvad am_i_mullvad();
    void enable_proxy(bool enable);

private:
    CURL *m_curl;
    bool m_is_proxy_enabled;
    const char *m_proxy;
};

} // namespace vpn