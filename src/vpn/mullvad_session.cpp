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

class WriteCallback : public cpr::WriteCallback {
public:
    explicit WriteCallback(std::ofstream ofs) : m_ofs(std::move(ofs)) {}
    bool operator()(const std::string_view &data, intptr_t _) {
        try {
            m_ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
        } catch (std::ofstream::failure &e) {
            spdlog::error("Write failed: {}", e.what());
            return false;
        }
        return true;
    }

private:
    std::ofstream m_ofs;
};

void MullvadSession::download_video(std::filesystem::path save_dir, Video &video) {
    auto filename = video.link.substr(video.link.rfind('/'));
    spdlog::info("Downloading {} {}-file ({}x{}): {}", filename, video.media_type, video.width, video.height,
                 video.link);
    std::ofstream file(save_dir / filename, std::ios::binary);
    SetOption(WriteCallback(std::move(file)));

    auto response = Get(cpr::Url{filename});
    if (response.status_code != 200)
        throw InvalidStatusCode("Download failed: " + response.error.message);
}
