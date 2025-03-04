#ifndef GBM_HPP
#define GBM_HPP

#include <gbm.h>

#include <vector>

namespace BDRM::Utils {

    struct gbm_bo* allocate_gbm_buffer(
        struct gbm_device* gbm, uint32_t width, uint32_t height,
        uint32_t format, std::vector<uint64_t> modifiers, bool is_cursor
    ); //!< try to create a buffer with the given modifiers using gbm

}

#endif // GBM_HPP
