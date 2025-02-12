#pragma once


#include <exception>
#include <filesystem>
#include <string>

namespace cli {

// Base cli exception
class CliExcpetion : public std::exception {};

// Wrong argument, or failed parsing
class InvalidCommandException : public CliExcpetion {
public:
    InvalidCommandException(std::string msg) : m_msg(std::move(msg)) {}
    [[nodiscard]] const char *what() const noexcept override { return m_msg.c_str(); }

private:
    std::string m_msg;
};

// The argument was help: We should exit now
class HelpExcpetion : public CliExcpetion {
public:
    HelpExcpetion() {}
};

class Cli {
public:
    std::filesystem::path db_path, video_dir, id_dir, mullvad_zip;
    int switch_mullvad_after, max_mib_ps;
};

Cli parse_cli(int argc, char **argv);
void show_help();

} // namespace cli
