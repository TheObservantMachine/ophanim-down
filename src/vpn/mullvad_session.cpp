#include "mullvad_session.hpp"

#include "../constants.hpp"
#include "../video.hpp"

#include "../format.hpp"
#include "curl/easy.h"
#include "spdlog/spdlog.h"

#include <algorithm>

namespace vpn {

MullvadSession::MullvadSession() : m_curl(curl_easy_init()), m_is_proxy_enabled(false), m_proxy(nullptr) {
    if (!m_curl)
        throw std::runtime_error("Failed to initialize CURL");
}

MullvadSession::~MullvadSession() {
    if (m_curl)
        curl_easy_cleanup(m_curl);
    delete[] m_proxy;
}

void MullvadSession::enable_proxy(bool enable) {
    if (enable != (m_proxy == nullptr))
        return;

    if (enable) {
        m_proxy = std::format("socks5h://{}", SOCKS5_IP).c_str();
    } else {
        delete[] m_proxy;
    }
}

size_t write_string_callback(void *contents, size_t size, size_t nmemb, void *userdata) {
    std::string *response = static_cast<std::string *>(userdata);
    size_t total_size = size * nmemb;
    response->append(static_cast<char *>(contents), total_size);
    return total_size;
}

std::string MullvadSession::get(const char *url) {
    curl_easy_reset(m_curl);

    if (m_proxy)
        curl_easy_setopt(m_curl, CURLOPT_PROXY, m_proxy);
    curl_easy_setopt(m_curl, CURLOPT_URL, url);

    std::string response_text;
    std::string error_buffer(CURL_ERROR_SIZE, '\0');

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_string_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response_text);
    curl_easy_setopt(m_curl, CURLOPT_ERRORBUFFER, error_buffer.data());
    curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);

    CURLcode res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
        if (error_buffer.empty())
            error_buffer = curl_easy_strerror(res);
        throw InvalidStatusCode(error_buffer);
    }

    int status_code;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &status_code);
    spdlog::debug("Got status code {} for {}", status_code, url);
    return response_text;
}

AmIMullvad MullvadSession::am_i_mullvad() {
    std::string response = get("https://am.i.mullvad.net");

    AmIMullvad result;
    std::istringstream stream(response);
    std::string line, lower;

    // Process the response line by line.
    while (std::getline(stream, line)) {
        lower.clear();
        std::ranges::transform(line, lower.begin(), ::tolower);

        // Check for Mullvad connection status.
        if (lower.find("you are using mullvad vpn") != std::string::npos) {
            result.is_mullvad = true;
            result.parse_server_config(line);
        } else if (lower.find("you are not using mullvad vpn") != std::string::npos)
            result.is_mullvad = false;
        else if (lower.find("ip address:") != std::string::npos)
            result.parse_ip_address_location(line);
    }

    if (result.ip_address.empty())
        throw std::runtime_error("Faulty html: IP address not found");

    return result;
}
void MullvadSession::download_video(const std::filesystem::path &save_dir, const Video &video) {
    size_t pos = video.link.rfind('/');
    if (pos == std::string::npos)
        throw std::runtime_error(std::format("Failed to find filename in '{}'", video.link));
    std::string filename = video.link.substr(pos + 1);

    spdlog::info("Downloading {} {}-file ({}x{}): {}", filename, video.media_type, video.width, video.height,
                 video.link);

    std::filesystem::path full_path = save_dir / filename;

    FILE *fp = fopen(full_path.string().c_str(), "wb");
    if (!fp)
        throw std::runtime_error("Failed to open file: " + full_path.string());

    // Reset the curl handle before the new transfer.
    curl_easy_reset(m_curl);
    if (m_proxy)
        curl_easy_setopt(m_curl, CURLOPT_PROXY, m_proxy);
    curl_easy_setopt(m_curl, CURLOPT_URL, video.link.c_str());
    // Use fwrite to write directly to the file.
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, fwrite);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(m_curl, CURLOPT_FAILONERROR, 1L);

    CURLcode res = curl_easy_perform(m_curl);
    fclose(fp);

    if (res != CURLE_OK)
        throw InvalidStatusCode("Download failed: " + std::string(curl_easy_strerror(res)));

    long http_code = 0;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200)
        throw InvalidStatusCode("Download failed with HTTP code: " + std::to_string(http_code));
}

bool AmIMullvad::parse_server_config(const std::string_view line) {
    // Extract the server configuration between parentheses.
    size_t start = line.find('(');
    if (start != std::string::npos)
        return false;
    start++; // move past '('
    size_t end = line.find(')', start);
    if (end != std::string::npos)
        server_config = line.substr(start, end - start);

    return true;
}

bool AmIMullvad::parse_ip_address_location(const std::string_view line) {
    size_t colonPos = line.find(":");
    if (colonPos == std::string::npos)
        return false;

    // Skip the colon and following space.
    size_t ipStart = colonPos + 2;
    size_t commaPos = line.find(",", ipStart);
    if (commaPos == std::string::npos)
        return false;

    ip_address = line.substr(ipStart, commaPos - ipStart);
    // The location is assumed to follow the comma and a space.
    size_t locationStart = commaPos + 2;
    location = line.substr(locationStart);
    return true;
}

} // namespace vpn
