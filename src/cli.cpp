#include "cli.hpp"

#include <cstdlib>
#include <filesystem>
#include "format.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

// A simple helper to check if a string starts with a given pattern.
bool starts_with(std::string_view s, std::string_view pattern) {
    const size_t size = pattern.size();
    if (size == 0)
        return true;
    if (size > s.size())
        return false;
    for (size_t i = 0; i < size; ++i) {
        if (s[i] != pattern[i])
            return false;
    }
    return true;
}


class Missing {
public:
    void add(const char *arg) {
        if (number_missing) {
            missing += ", ";
        }
        number_missing += 1;
        missing += arg;
    }

    void throw_if() {
        if (!number_missing)
            return;
        const auto str = std::format("Missing required {} for: {}", option(), missing);
        throw InvalidCommandException(str);
    }

private:
    [[nodiscard]] const char *option() const { return number_missing > 1 ? "options" : "option"; }

private:
    std::string missing;
    int number_missing = 0;
};

//
// Cli::parse_cli
//
// Parses the command-line arguments for the following options:
//
//   -p, --db-path <path>            (required)
//   -d, --video-dir <path>          (required)
//   -i, --id-dir <path>             (required)
//   -z, --mullvad-zip <path>        (required)
//   -s, --switch-mullvad-after <int> (optional, default: 10)
//   -h, --help                     Show help message
//
Cli Cli::parse_cli(int argc, char *argv[]) {
    Cli cli;
    cli.switch_mullvad_after = 10;

    // Process each argument.
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            show_help();
            std::exit(0);
        } else if (arg == "--db-path" || arg == "-p") {
            if (++i >= argc)
                throw InvalidCommandException("Missing value for " + std::string(arg));
            cli.db_path = std::filesystem::path(argv[i]);
        } else if (arg == "--video-dir" || arg == "-d") {
            if (++i >= argc)
                throw InvalidCommandException("Missing value for " + std::string(arg));
            cli.video_dir = std::filesystem::path(argv[i]);
        } else if (arg == "--id-dir" || arg == "-i") {
            if (++i >= argc)
                throw InvalidCommandException("Missing value for " + std::string(arg));
            cli.id_dir = std::filesystem::path(argv[i]);
        } else if (arg == "--mullvad-zip" || arg == "-z") {
            if (++i >= argc)
                throw InvalidCommandException("Missing value for " + std::string(arg));
            cli.mullvad_zip = std::filesystem::path(argv[i]);
        } else if (arg == "--switch-mullvad-after" || arg == "-s") {
            if (++i >= argc)
                throw InvalidCommandException("Missing value for " + std::string(arg));
            try {
                cli.switch_mullvad_after = std::stoi(argv[i]);
            } catch (const std::exception &ex) {
                throw InvalidCommandException("Invalid integer value for " + std::string(arg));
            }
        } else if (starts_with(arg, "-")) {
            throw InvalidCommandException("Unknown option: " + std::string(arg));
        } else {
            // We do not expect any positional arguments.
            throw InvalidCommandException("Unexpected positional argument: " + std::string(arg));
        }
    }

    // Validate that all required options have been provided.
    Missing missing;
    if (cli.db_path.empty())
        missing.add("--db-path/-p");
    if (cli.video_dir.empty())
        missing.add("--video-dir/-d");
    if (cli.id_dir.empty())
        missing.add("--id-dir/-i");
    if (cli.mullvad_zip.empty())
        missing.add("--mullvad-zip/-z");
    missing.throw_if();

    return cli;
}

consteval const char *get_help() {
    return R"(Usage: ophanim-down [OPTIONS]
Options:
  -h, --help                      Show this help message and exit
  -p, --db-path <path>            Path to sqlite3 db file (required)
  -d, --video-dir <path>          Path to dir to download videos to (required)
  -i, --id-dir <path>             Path to dir where to save the ids of downloaded videos (required)
  -z, --mullvad-zip <path>        Path to mullvad zip file (wireguard) (required)
  -s, --switch-mullvad-after <int> Switch mullvad connection after (default: 10)
)";
}

//
// show_help
//
// Outputs the help message to standard error.
//
void Cli::show_help() { std::cerr << get_help() << std::flush; }
