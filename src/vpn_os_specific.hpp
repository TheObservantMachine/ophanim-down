#pragma once

#include <string>
#include <memory>


class VpnOsSpecific {
public:
    virtual bool is_connected(const std::string& interface_name) const = 0;
    virtual ~VpnOsSpecific() = default;
};

std::unique_ptr<VpnOsSpecific> get_vpn_os_specific(); {
    return std::make_unique<LinuxVpnOsSpecific>(); // Adjust for other OS
}

