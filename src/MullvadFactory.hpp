#pragma once


#include <string>
#include <vector>

class MullvadWireGuard;

class MullvadFactory {
public:
    MullvadFactory(const std::string& zip_path = "", const std::string& config_prefix = "");

    ~MullvadFactory();

    MullvadWireGuard make_mullvad(int config_index = -1);

    static std::string invalid_environment();

private:
    std::string zip_path_;
    std::string config_prefix_;
    std::string temp_dir_;
    std::vector<std::string> config_files_;

    void extract_zip() ;

    void modify_allowed_ips(const std::string& config_path) ;

    static std::string create_temp_dir() ;

    static bool command_exists(const std::string& cmd);

    std::string find_zip_path() const;
};
