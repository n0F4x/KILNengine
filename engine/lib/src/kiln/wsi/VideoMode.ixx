module;

#include <cstdint>

export module kiln.wsi.VideoMode;

import kiln.wsi.Size;

namespace kiln::wsi {

export struct VideoMode {
    Size2u   resolution;
    uint32_t red_bits_depth;
    uint32_t green_bits_depth;
    uint32_t blue_bits_depth;
    uint32_t refresh_rate;
};

}   // namespace kiln::wsi
