#pragma once


#include <exception>
#include <filesystem>
#include <string>

namespace cli {

class InvalidCommandException : public std::exception {
public:
    InvalidCommandException(std::string msg) : m_msg(std::move(msg)) {}

    [[nodiscard]] const char *what() const noexcept override { return m_msg.c_str(); }

private:
    std::string m_msg;
};

class Cli {
public:
    std::filesystem::path db_path, video_dir, id_dir, mullvad_zip;
    int switch_mullvad_after;
};

Cli parse_cli(int argc, char **argv);
void show_help();

} // namespace cli