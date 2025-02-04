#include "logging.hpp"

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <filesystem>
#include <memory>
#include <vector>


void setup_logging(size_t max_file_size, size_t max_files) {
    const std::filesystem::path exec_path = std::filesystem::current_path();
    const std::filesystem::path logs_dir = exec_path / "logs";
    if (!exists(logs_dir))
        create_directories(logs_dir);

    const std::filesystem::path log_file = logs_dir / "ophanim-down.log";

    const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);

    const auto rotating_file_sink =
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file.string(), max_file_size, max_files);
    rotating_file_sink->set_level(spdlog::level::debug);

    std::vector<spdlog::sink_ptr> sinks{console_sink, rotating_file_sink};
    const auto logger = std::make_shared<spdlog::logger>("default_logger", sinks.begin(), sinks.end());

    logger->set_pattern("[%Y-%m-%d %H:%M:%S] %l: %v");
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::info);
    spdlog::debug("Logger initialized. Logging to both console and rotating file: {}", log_file.string());
}
