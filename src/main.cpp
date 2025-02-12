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
    cli::Cli cli;
    try {
        cli = cli::parse_cli(argc, argv);
    } catch (const cli::InvalidCommandException &e) {
        std::cerr << "Invalid parameters. Use (--help | -h) for help:\n " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const cli::HelpExcpetion &_) {
        return EXIT_SUCCESS;
    }

    setup_logging();

    sqlite3 *db = nullptr;
    spdlog::debug("Opening database at {}", cli.db_path.string());
    if (sqlite3_open(cli.db_path.string().c_str(), &db)) {
        spdlog::error("Cannot open database: {}", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }
    spdlog::debug("Opened database successfully");

    const vpn::MullvadFactory factory(cli.mullvad_zip);
    std::unique_ptr<vpn::MullvadWireGuard> mullvad;
    vpn::MullvadSession session;
    std::string real_ip;
    {
        auto aim = session.am_i_mullvad();
        if (aim.is_mullvad) {
            spdlog::error("We are mullvad before we should be!");
            return EXIT_FAILURE;
        }
        spdlog::info("Real ip is {}", aim.ip_address);
        real_ip = std::move(aim.ip_address);
    }
    session.enable_proxy(true);
    size_t counter = 0;

    // The videomanager will autosave
    auto video_manager = manager::VideoManager::create(db, cli.id_dir / "downloaded-ids.json");
    for (auto wrapped_video: video_manager) {
        if (counter++ % cli.switch_mullvad_after == 0) {
            mullvad = factory.make_mullvad();
            if (!mullvad->is_connected()) {
                spdlog::error("Mullvad isn't connected");
                return EXIT_FAILURE;
            }
            auto aim = session.am_i_mullvad();
            if (!aim.is_mullvad) {
                spdlog::error("We aren't mullvad!!");
                return EXIT_FAILURE;
            }
            spdlog::info("Connected at {} with ip {}", aim.location, aim.ip_address);
        }

        session.download_video(cli.video_dir, wrapped_video.get_video());
        wrapped_video.mark_done();
    }
}
