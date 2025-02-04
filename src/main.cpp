#include <iostream>

#include "cli.hpp"
#include "logging.hpp"


int main(int argc, char *argv[]) {
    Cli cli;
    try {
        cli = Cli::parse_cli(argc, argv);
    } catch (const InvalidCommandException &e) {
        std::cerr << "Invalid parameters. Use (--help | -h) for help:\n " << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    setup_logging();
    std::cout << "Hello, world!" << std::endl;
}
