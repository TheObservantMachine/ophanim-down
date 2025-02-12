#include "vpn_os_specific.hpp"

namespace vpn {

class LinuxVpnOsSpecific final : public VpnOsSpecific {
public:
    [[nodiscard]] bool is_connected(const std::string &interface_name) const override {
        const std::string cmd = "ip link show " + interface_name + " | grep -q 'state UP'";
        return system(cmd.c_str()) == 0;
    }
};

std::unique_ptr<VpnOsSpecific> get_vpn_os_specific() { return std::make_unique<LinuxVpnOsSpecific>(); }

} // namespace vpn