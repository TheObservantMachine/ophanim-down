#include "MullvadFactory.hpp"
#include "../constants.hpp"
#include "mullvad_wireguard.hpp"

#include <filesystem>
#include <format>
#include <fstream>


MullvadFactory::MullvadFactory(const std::string &zip_path, const std::string &config_prefix) :
    config_prefix_(config_prefix) {
    zip_path_ = zip_path.empty() ? find_zip_path() : zip_path;
    temp_dir_ = create_temp_dir();
    extract_zip();
}

MullvadFactory::~MullvadFactory() {
    if (!temp_dir_.empty())
        std::filesystem::remove_all(temp_dir_);
}

MullvadWireGuard MullvadFactory::make_mullvad(int config_index) const {
    if (config_files_.empty())
        throw std::runtime_error("No config files available");

    if (config_index == -1)
        config_index = rand() % config_files_.size();

    if (static_cast<size_t>(config_index) >= config_files_.size())
        throw std::out_of_range("Invalid config index");

    return {config_files_[config_index]};
}

std::string MullvadFactory::invalid_environment() {
    if (!command_exists("wg-quick"))
        return "wg-quick not found. Install WireGuard tools.";
    return "";
}

void MullvadFactory::extract_zip() {
    std::string cmd = "unzip -o " + zip_path_ + " -d " + temp_dir_;
    if (system(cmd.c_str()) != 0)
        throw std::runtime_error("Failed to extract ZIP");

    for (const auto &entry: std::filesystem::directory_iterator(temp_dir_)) {
        if (entry.path().extension() == ".conf") {
            std::string config_path = entry.path().string();
            modify_allowed_ips(config_path);
            config_files_.push_back(config_path);
        }
    }
}

void MullvadFactory::modify_allowed_ips(const std::string &config_path) {
    std::vector<std::string> lines;
    std::string current_section;
    std::ifstream in(config_path);
    std::string line;

    while (std::getline(in, line)) {
        if (line.find("[Peer]") != std::string::npos)
            current_section = "Peer";
        else if (line.find("[Interface]") != std::string::npos)
            current_section = "Interface";

        if (current_section == "Peer" && line.find("AllowedIPs") != std::string::npos)
            line = std::format("AllowedIPs = {}/32", SOCKS5_IP);
        else if (current_section == "Interface" && line.find("DNS") != std::string::npos)
            continue;

        lines.push_back(line);
    }
    in.close();

    std::ofstream out(config_path);
    for (const auto &l: lines)
        out << l << std::endl;
    out.close();
}

std::string MullvadFactory::create_temp_dir() {
    char dir_template[] = "/tmp/mullvad_XXXXXX";
    if (mkdtemp(dir_template) == nullptr)
        throw std::runtime_error("Failed to create temp directory");
    return dir_template;
}

bool MullvadFactory::command_exists(const std::string &cmd) {
    return system(("command -v " + cmd + " >/dev/null 2>&1").c_str()) == 0;
}

std::string MullvadFactory::find_zip_path() const {
    for (const auto &entry: std::filesystem::directory_iterator(".")) {
        if (entry.path().string().find("mullvad_wireguard") != std::string::npos && entry.path().extension() == ".zip")
            return entry.path().string();
    }
    throw std::runtime_error("Mullvad ZIP not found");
}
