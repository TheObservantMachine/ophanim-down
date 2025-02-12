#pragma once

#include "vpn_os_specific.hpp"

#include <filesystem>

namespace vpn {

class MullvadWireGuard {
public:
    MullvadWireGuard(std::string config_file);

    ~MullvadWireGuard();

    void start();

    void close();

    [[nodiscard]] bool is_connected() const;

private:
    static std::string get_interface_name(const std::string &path);
    static void execute_command(const std::string &cmd);

private:
    bool m_is_connected;
    std::string m_config_file;
    std::string m_interface_name;
    std::unique_ptr<VpnOsSpecific> m_vpn_os_specific;
};

} // namespace vpn