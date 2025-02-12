#pragma once


#include <memory>
#include <string>
#include <vector>

class MullvadWireGuard;

class MullvadFactory {
public:
    MullvadFactory(const std::string &zip_path = "", std::string config_prefix = "");

    ~MullvadFactory();

    [[nodiscard]] std::unique_ptr<MullvadWireGuard> make_mullvad(int config_index = -1) const;

    static std::string invalid_environment();

private:
    std::string m_zip_path;
    std::string m_config_prefix;
    std::string m_temp_dir;
    std::vector<std::string> m_config_files;

    void extract_zip();

    void modify_allowed_ips(const std::string &config_path);

    static std::string create_temp_dir();

    static bool command_exists(const std::string &cmd);

    [[nodiscard]] std::string find_zip_path() const;
};
