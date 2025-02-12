#pragma once

#include <filesystem>
#include <vector>

std::vector<std::filesystem::path> unzip(std::filesystem::path zip_file, std::filesystem::path to_dir);