#pragma once

#include <cstddef>

void setup_logging(size_t max_file_size = 1048576 * 5, // 5 MB
                   size_t max_files = 3);
