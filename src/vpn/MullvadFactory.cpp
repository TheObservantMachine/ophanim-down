#include "MullvadFactory.hpp"

#include "../constants.hpp"
#include "mullvad_wireguard.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ranges>
#include "../format.hpp"
#include "../unzip.hpp"

namespace vpn {

MullvadFactory::MullvadFactory(std::filesystem::path zip_path, std::string config_prefix) :
    m_config_prefix(std::move(config_prefix)), m_random_engine(std::random_device{}()) {
    m_zip_path = zip_path.empty() ? find_zip_path() : std::move(zip_path);
    m_temp_dir = create_temp_dir();
    extract_zip();
}

MullvadFactory::~MullvadFactory() {
    if (!m_temp_dir.empty())
        std::filesystem::remove_all(m_temp_dir);
}

std::unique_ptr<MullvadWireGuard> MullvadFactory::make_mullvad(int32_t config_index) {
    if (m_config_files.empty())
        throw std::runtime_error("No config files available");

    if (config_index == -1) {
        std::uniform_int_distribution<std::mt19937::result_type> random_index(0, m_config_files.size());
        config_index = random_index(m_random_engine);
    }

    if (static_cast<size_t>(config_index) >= m_config_files.size())
        throw std::out_of_range("Invalid config index");

    return std::make_unique<MullvadWireGuard>(m_config_files[config_index]);
}

std::string MullvadFactory::invalid_environment() {
    if (!command_exists("wg-quick"))
        return "wg-quick not found. Install WireGuard tools.";
    return "";
}

void MullvadFactory::extract_zip() {
    auto unzipped_files = unzip(m_zip_path, m_temp_dir);

    for (auto &path: unzipped_files) {
        if (path.extension() != ".conf")
            continue;

        modify_allowed_ips(path);
        m_config_files.push_back(std::move(path));
    }
}

void MullvadFactory::modify_allowed_ips(const std::filesystem::path &config_path) {
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

std::filesystem::path MullvadFactory::find_zip_path() const {
    for (const auto &entry: std::filesystem::directory_iterator(".")) {
        if (entry.path().string().find("mullvad_wireguard") != std::string::npos && entry.path().extension() == ".zip")
            return entry.path();
    }
    throw std::runtime_error("Mullvad ZIP not found");
}

} // namespace vpn
