#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <xf86drmMode.h>
#include <gbm.h>

#include <optional>

// forward decl
namespace BDRM { class Bdrm; }

namespace BDRM::Resources {

    /// Nonvolatile gbm buffer instance
    class Buffer {

        friend class BDRM::Bdrm;

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

            Buffer(struct gbm_bo* bo);

    };

}

#endif // BUFFER_HPP
