#pragma once

#include <filesystem>
#include <vector>

std::vector<std::filesystem::path> unzip(const std::filesystem::path &zip_file, const std::filesystem::path &to_dir);