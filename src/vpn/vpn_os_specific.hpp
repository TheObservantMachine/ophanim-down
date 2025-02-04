#pragma once

#include <memory>
#include <string>


class VpnOsSpecific {
public:
    [[nodiscard]] virtual bool is_connected(const std::string &interface_name) const = 0;
    virtual ~VpnOsSpecific() = default;
};

std::unique_ptr<VpnOsSpecific> get_vpn_os_specific();
