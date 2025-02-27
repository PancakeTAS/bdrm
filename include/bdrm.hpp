#ifndef BDRM_HPP
#define BDRM_HPP

#include "atomic.hpp"
#include "resources/framebuffer.hpp"
#include "utils.hpp"
#include "resources.hpp"

#include <cstdint>
#include <string_view>

namespace BDRM {

    /// Main class for interfacing with the DRM subsystem
    class Bdrm {

        private:
            DeviceNode node; //!< fd
            UP<Resources> resources; //!< connectors, crtcs, planes
            uint64_t cursor_width, cursor_height;
            std::vector<UP<Framebuffer>> framebuffers; //!< framebuffers

        public:
            Bdrm(std::string_view path); //!< open the device node and reset all properties

            const Resources& get_resources() { return *this->resources; } //!< get the resources

            uint64_t get_cursor_width() { return this->cursor_width; } //!< preferred width of the cursor plane
            uint64_t get_cursor_height() { return this->cursor_height; } //!< preferred height

            AtomicRequest create_atomic_request(); // <! create a new empty atomic request
            void commit(AtomicRequest& request); // <! commit an atomic request

            const Framebuffer& create_framebuffer(uint32_t width, uint32_t height,
                uint32_t format, std::vector<uint64_t> modifiers,
                bool is_cursor);

    };

}

#endif // BDRM_HPP
