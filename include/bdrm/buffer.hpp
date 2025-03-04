#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdint>
#include <vector>
#include <gbm.h>

#include <optional>

// forward decl
namespace BDRM::Atomic { class PlaneReq; }

namespace BDRM {

    /// Nonvolatile gbm buffer instance
    class Buffer {

        friend class Bdrm;
        friend class BDRM::Atomic::PlaneReq;

        public:
            uint32_t width, height;
            uint32_t format;
            uint32_t bpp;
            std::optional<uint64_t> modifier;

            uint32_t handle[4] = { 0 };
            uint32_t stride[4] = { 0 };
            uint32_t offset[4] = { 0 };

            ~Buffer();

        private:
            struct gbm_bo* bo;
            int fd;
            uint32_t fb_id; //!< drm framebuffer id

            Buffer(int fd, struct gbm_device* gbm,
                uint32_t width, uint32_t height,
                uint32_t format, std::vector<uint64_t> modifiers, bool is_cursor);

    };

}

#endif // BUFFER_HPP
