#include <iostream>
#include "spdlog/spdlog.h"
#include "sqlite3.h"

#include "cli.hpp"
#include "logging.hpp"
#include "video_manager/video_manager.hpp"


int main(int argc, char *argv[]) {
    Cli cli;
    try {
        cli = Cli::parse_cli(argc, argv);
    } catch (const InvalidCommandException &e) {
        std::cerr << "Invalid parameters. Use (--help | -h) for help:\n " << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    setup_logging();


    sqlite3 *db = nullptr;
    spdlog::debug("Opening database at {}", cli.db_path.string());
    if (sqlite3_open(cli.db_path.string().c_str(), &db)) {
        spdlog::error("Cannot open database: {}", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }
    spdlog::debug("Opened database successfully");

    auto video_manager = VideoManager::create(db, cli.id_dir / "downloaded-ids.json");
    for (auto wrapped: video_manager) {
        auto &video = wrapped.get_video();
    }

    video_manager.save();
}
