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

    /**
     * TODO: documentation, better errors
     */
    class Bdrm {

        private:
            DeviceNode node; //!< fd
            UP<BDRM::Resources::Res> resources; //!< connectors, crtcs, planes
            std::vector<UP<Buffer>> buffers; //!< framebuffers
            uint64_t cursor_width, cursor_height;

        public:

            /**
             * \brief Open a BDRM device node
             *
             * This constructor tries to open a device node at the given path. It will
             * try to set the client capabilities to figure out what the device supports.
             * It will fetch all available resources and initialize them to zero (resulting
             * in a black screen).
             *
             * \param path
             *   Path to the device node
             *
             * \throws std::runtime_error
             *   Thrown if the node couldn't be opened, certain essential capabilities are missing,
             *   any DRM function fails or the initial atomic commit fails.
             */
            Bdrm(std::string_view path); //!< open the device node and reset all properties

            /**
             * \brief Return all read-only resources
             *
             * All returned resources are valid until the main Bdrm object is destroyed. The
             * returned resources will never change during the lifetime of the Bdrm object.
             * TODO: if link-status was bad, the list of modes could change.
             */
            const BDRM::Resources::Res& get_resources() { return *this->resources; } //!< get the resources

            /// Get the preferred cursor width
            uint64_t get_cursor_width() { return this->cursor_width; }
            /// Get the preferred cursor height
            uint64_t get_cursor_height() { return this->cursor_height; }

            /**
             * \brief Create an atomic request
             *
             * This function creates a new atomic request. Before calling this function,
             * ensure atomic requests are supported by the device. (TODO).
             *
             * \return
             *   A new atomic request
             *
             * \throws std::runtime_error
             *   Thrown if the atomic request couldn't be created.
             */
            BDRM::Atomic::AtomicRequest create_atomic_request();

            /**
             * \brief Commit an atomic request
             *
             * This function commits an atomic request. TODO: allow for
             * non-blocking commits and understand events, as well as explicit sync.
             *
             * \throws std::runtime_error
             *   Thrown if the atomic request couldn't be committed.
             */
            void commit(BDRM::Atomic::AtomicRequest& request); //!< commit an atomic request

            /**
             * \brief Allocate a new framebuffer
             *
             * Allocate a new framebuffer using the internal gbm device. The framebuffer
             * is cleaned up when the Bdrm object is destroyed. (TODO: mark cleanup function, SP?)
             */
            const Buffer& allocate_framebuffer(uint32_t width, uint32_t height,
                uint32_t format, std::vector<uint64_t> modifiers,
                bool is_cursor); //!< allocate a new framebuffer

    };

}

#endif // BDRM_HPP
