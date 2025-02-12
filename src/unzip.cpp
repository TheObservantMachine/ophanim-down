#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "format.hpp"

#include "zip.h"

std::vector<std::filesystem::path> unzip(std::filesystem::path zip_file, std::filesystem::path to_dir) {
    int err_code = 0;
    std::unique_ptr<zip, decltype(&zip_close)> archive(zip_open(zip_file.c_str(), ZIP_RDONLY, &err_code), zip_close);
    if (!archive) {
        std::string error_message = zip_strerror(archive.get());
        throw std::runtime_error(std::format("Failed to open ZIP file ({}): {}", err_code, error_message));
    }

    zip_int64_t num_entries = zip_get_num_entries(archive.get(), 0);
    std::vector<std::filesystem::path> extracted_paths;
    extracted_paths.reserve(num_entries);

    for (zip_int64_t i = 0; i < num_entries; i++) {
        zip_stat_t st;
        if (zip_stat_index(archive.get(), i, 0, &st) != 0)
            continue; // Skip entry if stat cannot be retrieved.

        std::string filename = st.name;
        std::filesystem::path out_path = to_dir / filename;

        if (!filename.empty() && filename.back() == '/') {
            std::filesystem::create_directories(out_path);
            continue;
        }

        if (out_path.has_parent_path())
            std::filesystem::create_directories(out_path.parent_path());

        std::unique_ptr<zip_file_t, decltype(&zip_fclose)> zf(zip_fopen_index(archive.get(), i, 0), zip_fclose);
        if (!zf)
            throw std::runtime_error("Failed to open file in ZIP: " + filename);

        // Open an output file stream for writing the extracted file.
        std::ofstream ofs(out_path, std::ios::binary);
        if (!ofs)
            throw std::runtime_error("Failed to create output file: " + out_path.string());

        // Read the file's data from the ZIP archive and write it to disk.
        char buffer[8192];
        zip_int64_t bytes_read = 0;
        while ((bytes_read = zip_fread(zf.get(), buffer, sizeof(buffer))) > 0)
            ofs.write(buffer, bytes_read);

        if (bytes_read < 0)
            throw std::runtime_error("Error reading file from ZIP: " + filename);

        extracted_paths.push_back(std::move(out_path));
    }

    return extracted_paths;
}
