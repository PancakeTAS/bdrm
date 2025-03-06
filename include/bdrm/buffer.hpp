#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdint>
#include <vector>
#include <gbm.h>

#include <optional>

// forward decl
namespace BDRM::Atomic { class PlaneReq; }

namespace BDRM {

    class Buffer {

        friend class Bdrm;
        friend class BDRM::Atomic::PlaneReq;

        public:
            /// Width and height in pixels
            uint32_t width, height;
            /// Fourcc buffer format
            uint32_t format;
            /// Bits per pixel, not bits per actual pixel data
            uint32_t bpp;
            /// Optional modifier
            std::optional<uint64_t> modifier;

            /// Buffer handles
            uint32_t handle[4] = { 0 };
            /// Buffer pitches
            uint32_t stride[4] = { 0 };
            /// Buffer offsets
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
