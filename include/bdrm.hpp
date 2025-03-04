#ifndef BDRM_HPP
#define BDRM_HPP

#include "bdrm/types.hpp"
#include "bdrm/node.hpp"
#include "bdrm/buffer.hpp"
#include "resources.hpp"
#include "atomic.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

namespace BDRM {

    using namespace BDRM::Resources;
    using namespace BDRM::Atomic;

    /// Main class for interfacing with the DRM subsystem
    class Bdrm {

        private:
            DeviceNode node; //!< fd
            UP<Res> resources; //!< connectors, crtcs, planes
            std::vector<UP<Buffer>> buffers; //!< framebuffers
            uint64_t cursor_width, cursor_height;

        public:
            Bdrm(std::string_view path); //!< open the device node and reset all properties

            const Res& get_resources() { return *this->resources; } //!< get the resources

            uint64_t get_cursor_width() { return this->cursor_width; } //!< preferred width of the cursor plane
            uint64_t get_cursor_height() { return this->cursor_height; } //!< preferred height

            AtomicRequest create_atomic_request(); //!< create a new empty atomic request
            void commit(AtomicRequest& request); //!< commit an atomic request

            const Buffer& allocate_framebuffer(uint32_t width, uint32_t height,
                uint32_t format, std::vector<uint64_t> modifiers,
                bool is_cursor); //!< allocate a new framebuffer
            void free_framebuffer(const Buffer& framebuffer); //!< free a framebuffer

    };

}

#endif // BDRM_HPP
