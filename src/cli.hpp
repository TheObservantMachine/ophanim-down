#pragma once


#include <exception>
#include <string>
#include <filesystem>


class InvalidCommandException : public std::exception {
public:
    InvalidCommandException(std::string msg) : m_msg(std::move(msg)) {}

    [[nodiscard]] const char *what() const noexcept override { return m_msg.c_str(); }

private:
    std::string m_msg;
};

class Cli{
public:
    std::filesystem::path db_path,video_dir, id_dir, mullvad_zip;
    int switch_mullvad_after;

    static Cli parse_cli(int argc, char **argv);
    static void show_help();

};
