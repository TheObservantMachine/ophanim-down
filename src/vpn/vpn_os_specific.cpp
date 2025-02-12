#include "vpn_os_specific.hpp"

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

namespace vpn {

class LinuxVpnOsSpecific final : public VpnOsSpecific {
public:
    [[nodiscard]] bool is_connected(const std::string &interface_name) const override {
        const std::string cmd = "ip addr show " + interface_name;
        const std::string ret = exec(cmd.c_str());
        return ret.find("inet ") != std::string::npos;
    }
};

class MacVpnOsSpecific final : public VpnOsSpecific {
public:
    [[nodiscard]] bool is_connected(const std::string &interface_name) const override {
        const std::string cmd = "ifconfig " + interface_name;
        const std::string ret = exec(cmd.c_str());
        return ret.find("inet ") != std::string::npos;
    }
};

std::unique_ptr<VpnOsSpecific> get_vpn_os_specific() {
    return std::make_unique<
#if defined(__linux__)
            LinuxVpnOsSpecific
#elif defined(__APPLE__)
            MacVpnOsSpecific
#else
#error UNSUPPORTED_OS
#endif
            >();
}

} // namespace vpn
