#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "spdlog/spdlog.h"
#include "sqlite3.h"

#include "cli.hpp"
#include "logging.hpp"
#include "video_manager/video_manager.hpp"
#include "vpn/MullvadFactory.hpp"
#include "vpn/mullvad_session.hpp"
#include "vpn/mullvad_wireguard.hpp"


static std::atomic<bool> should_shutdown = false;
void signal_handler(int signal) {
    if (signal != SIGINT)
        return;
    should_shutdown = true;
    spdlog::warn("Shutting down, when done!");
}


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
    std::signal(SIGINT, signal_handler);

    setup_logging();

    sqlite3 *db = nullptr;
    spdlog::debug("Opening database at {}", cli.db_path.string());
    if (sqlite3_open(cli.db_path.string().c_str(), &db)) {
        spdlog::error("Cannot open database: {}", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }
    spdlog::debug("Opened database successfully");

    vpn::MullvadFactory factory(std::move(cli.mullvad_zip));
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
    session.set_max_speed_mib_ps(cli.max_mib_ps);
    if (cli.max_mib_ps)
        spdlog::info("Speedlimit is set to {} Mib/s", cli.max_mib_ps);
    else
        spdlog::info("Speedlimit is set to unlimited");

    size_t counter = 0;
    const size_t MAX_ERR = 3;

    // The videomanager will autosave
    auto video_manager = manager::VideoManager::create(db, cli.id_dir / "downloaded-ids.json").into_filtered();
    try {
        for (auto wrapped_video: video_manager) {
            if (counter++ % cli.switch_mullvad_after == 0) {

                bool is_err;
                for (size_t err_count = 0; err_count < MAX_ERR; err_count++) {
                    if (mullvad)
                        mullvad->close();
                    mullvad = factory.make_mullvad();
                    if (!mullvad->is_connected()) {
                        spdlog::error("Mullvad isn't connected");
                        return EXIT_FAILURE;
                    }
                    try {
                        auto aim = session.am_i_mullvad();
                        if (!aim.is_mullvad) {
                            spdlog::error("We aren't mullvad!!");
                            return EXIT_FAILURE;
                        }
                        spdlog::info("Connected at {} with ip {}", aim.location, aim.ip_address);
                        is_err = false;
                        break;
                    } catch (const std::exception &e) {
                        size_t to_sleep = (err_count + 1) * 5;
                        spdlog::error("Failed to check mullvad ({}/{}). Waiting {} s. Got error: {}", err_count,
                                      MAX_ERR, to_sleep, e.what());
                        std::this_thread::sleep_for(std::chrono::seconds(to_sleep));
                        is_err = true;
                    }
                }
                if (is_err) {
                    spdlog::error("Failed to check mullvad ({}/{})", MAX_ERR, MAX_ERR);
                    break;
                }
            }

            bool is_err;
            for (size_t err_count = 0; err_count < MAX_ERR; err_count++) {
                try {
                    session.download_video(cli.video_dir, wrapped_video.get_video());
                    is_err = false;
                    break;
                } catch (const std::exception &e) {
                    size_t to_sleep = (err_count + 1) * 5;
                    spdlog::error("Failed to get video ({}/{}). Waiting {} s. Got error: {}", err_count, MAX_ERR,
                                  to_sleep, e.what());
                    std::this_thread::sleep_for(std::chrono::seconds(to_sleep));
                    is_err = true;
                }
            }
            if (is_err) {
                spdlog::error("Failed to get video ({}/{})", MAX_ERR, MAX_ERR);
                break;
            }

            wrapped_video.mark_done();

            if (should_shutdown)
                break;
        }
    } catch (const std::exception &e) {
        spdlog::error("Encountered unhandled error: {}", e.what());
    }

    spdlog::info("Downloaded {} tasks this run.", counter);
}
