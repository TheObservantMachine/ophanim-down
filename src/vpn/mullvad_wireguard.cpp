#include "mullvad_wireguard.hpp"
#include "spdlog/spdlog.h"

namespace vpn {

MullvadWireGuard::MullvadWireGuard(std::filesystem::path config_file) :
    m_is_connected(false), m_config_file(std::move(config_file)), m_vpn_os_specific(get_vpn_os_specific()) {
    start();
}

MullvadWireGuard::~MullvadWireGuard() { close(); }

void MullvadWireGuard::start() {
    spdlog::info("Starting WireGuard connection...");
    execute_command("wg-quick up " + m_config_file.string());
    int counter = 0;
    while (!is_connected()) {
        if (++counter > 10) {
            close();
            throw std::runtime_error("WireGuard connection timeout");
        }
        sleep(2);
    }
    m_is_connected = true;
    spdlog::info("WireGuard connection established.");
}

void MullvadWireGuard::close() {
    if (!m_is_connected)
        return;

    spdlog::info("Stopping WireGuard connection...");
    try {
        execute_command("wg-quick down " + m_config_file.string());
        m_is_connected = false;
    } catch (const std::exception &e) {
        spdlog::error(e.what());
    }
}

bool MullvadWireGuard::is_connected() const { return m_vpn_os_specific->is_connected(get_interface_name()); }

std::string MullvadWireGuard::get_interface_name() const { return m_config_file.stem().string(); }

void MullvadWireGuard::execute_command(const std::string &cmd) {
    spdlog::debug("Executing: " + cmd);
    const std::string full_cmd = cmd + " 2>&1";
    FILE *pipe = popen(full_cmd.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("Failed to execute command");

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        result += buffer;

    const int status = pclose(pipe);
    if (WEXITSTATUS(status) != 0)
        throw std::runtime_error("Command failed: " + result);
}

} // namespace vpn
