#include <iostream>
#include "spdlog/spdlog.h"
#include "sqlite3.h"

#include "cli.hpp"
#include "logging.hpp"
#include "video_manager/video_manager.hpp"
#include "vpn/MullvadFactory.hpp"
#include "vpn/mullvad_session.hpp"
#include "vpn/mullvad_wireguard.hpp"


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

    const MullvadFactory factory(cli.mullvad_zip);
    std::unique_ptr<MullvadWireGuard> mullvad;
    MullvadSession session;
    size_t counter = 0;

    auto video_manager = VideoManager::create(db, cli.id_dir / "downloaded-ids.json");
    for (auto wrapped: video_manager) {
        if (++counter % cli.switch_mullvad_after == 0) {
            mullvad = factory.make_mullvad(-1);
            if (!mullvad->is_connected()) {
                spdlog::error("Mullvad isn't connected");
                return EXIT_FAILURE;
            }
        }
        auto &video = wrapped.get_video();
        session.download_video(cli.video_dir, video);
    }

    video_manager.save();
}
