#pragma once

#include "curl/curl.h"

#include <filesystem>
#include <string>


class Video;

namespace vpn {

class InvalidStatusCode final : public std::runtime_error {
public:
    explicit InvalidStatusCode(const std::string &s, long code = -1) : runtime_error(s), code(code) {}
    explicit InvalidStatusCode(const char *string, long code = -1) : runtime_error(string), code(code) {}
    long code;
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
    MullvadSession(bool proxy_enabled = false, int32_t max_speed_mib_ps = 0);
    ~MullvadSession();

    std::string get(const char *url);
    void download_video(const std::filesystem::path &save_dir, const Video &video);
    AmIMullvad am_i_mullvad();
    void enable_proxy(bool enable);
    /** Set max download speed in MiB/s. 0 Means disabled */
    void set_max_speed_mib_ps(int32_t mbps);
    /** Get max download speed in MiB/s. 0 means disabled */
    int32_t get_max_speed_mib_ps() const;

private:
    void set_user_agent();

private:
    CURL *m_curl;
    bool m_is_proxy_enabled;
    std::string m_proxy;
    /** Max speed in bytes/s. 0 Is diasabled */
    curl_off_t m_max_speed;
    std::string m_user_agent;
};

} // namespace vpn
