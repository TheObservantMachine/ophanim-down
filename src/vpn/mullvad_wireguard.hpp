#pragma once

#include "vpn_os_specific.hpp"

#include <filesystem>

namespace vpn {

class MullvadWireGuard {
public:
    MullvadWireGuard(std::filesystem::path config_file);

    ~MullvadWireGuard();

    void start();

    void close();

    [[nodiscard]] bool is_connected() const;

private:
    std::string get_interface_name() const;
    static void execute_command(const std::string &cmd);

private:
    bool m_is_connected;
    std::filesystem::path m_config_file;
    std::unique_ptr<VpnOsSpecific> m_vpn_os_specific;
};

} // namespace vpn