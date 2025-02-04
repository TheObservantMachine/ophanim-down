#include "mullvad_session.hpp"

#include <spdlog/spdlog.h>

#include "../constants.hpp"
#include "../video.hpp"


MullvadSession::MullvadSession() {
    std::string socks_5 = "socks5h://" + std::string(SOCKS5_IP);
    SetProxies({{"http", socks_5}, {"https", socks_5}});
}
cpr::Response MullvadSession::Get(const cpr::Url &url) {
    SetUrl(url);
    return Session::Get();
}

bool MullvadSession::am_i_mullvad() {
    auto response = Get(cpr::Url{"https://am.i.mullvad.net"});
    if (response.status_code != 200)
        throw std::runtime_error("Request failed: " + response.error.message);

    return response.text.find("You are using Mullvad VPN") != std::string::npos;
}


// Callback function that writes data to an output file stream.
// The callback signature matches what CPR (via libcurl) expects:
//   - ptr: pointer to the received data
//   - size: size of each data element
//   - nmemb: number of data elements
//   - userdata: pointer to user data (we pass our std::ofstream pointer here)
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::ofstream *ofs = static_cast<std::ofstream *>(userdata);
    const std::size_t total_size = size * nmemb;
    ofs->write(ptr, total_size);
    return total_size; // Return the number of bytes written
}

void MullvadSession::download_video(std::filesystem::path save_dir, Video &video) {
    auto filename = video.link.substr(video.link.rfind('/'));
    spdlog::info("Downloading {} {}-file ({}x{}): {}", filename, video.media_type, video.width, video.height,
                 video.link);
    std::ofstream file(save_dir / filename, std::ios::binary);
    SetOption(cpr::WriteCallbackOption(write_callback));
    SetOption(cpr::UserDataOption(&ofs));
    auto response = Get(cpr::Url{filename});
    if (response.status_code != 200)
        throw InvalidStatusCode("Download failed: " + response.error.message);
}
