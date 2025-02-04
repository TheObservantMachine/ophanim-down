#include "mullvad_session.hpp"
#include "../constants.hpp"


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
