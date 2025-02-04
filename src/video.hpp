#pragma once

#include <cstdint>
#include <string>

struct Video {
    int64_t id;
    int64_t duration_ms;
    int32_t width;
    int32_t height;
    int64_t race_id;
    std::string media_type, link;
};
