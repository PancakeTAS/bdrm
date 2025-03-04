#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "resources/buffer.hpp"
#include "utils.hpp"

#include <xf86drmMode.h>
#include <gbm.h>

#include <optional>

// forward decl
namespace BDRM::Atomic { class PlaneReq; }
namespace BDRM { class Bdrm; }

namespace BDRM::Resources {

    using namespace BDRM::Utils;

    /// Nonvolatile drm framebuffer instance
    class Framebuffer {

        friend class BDRM::Bdrm;
        friend class BDRM::Atomic::PlaneReq;

        public:
            uint32_t width, height;
            uint32_t format;
            std::optional<uint64_t> modifier;
            uint32_t flags;

            uint32_t handles[4] = { 0 };
            uint32_t pitches[4] = { 0 };
            uint32_t offset[4] = { 0 };

            ~Framebuffer();

        private:
            int fd;
            uint32_t fb_id;
            SP<Buffer> buffer;

            Framebuffer(int fd, drmModeFB2* fb, SP<Buffer> buffer);

    };

}

#endif // FRAMEBUFFER_HPP
