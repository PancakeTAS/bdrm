#ifndef BDRM_HPP
#define BDRM_HPP

#include "atomic.hpp"
#include "utils.hpp"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"
#include <cstdint>
#include <string_view>
#include <vector>

namespace BDRM {

    /// device node
    class DeviceNode {
        
        friend class Bdrm;

        int fd;
        
        DeviceNode(std::string_view path);
        ~DeviceNode();

        int get_fd() { return this->fd; }

    };

    /// better drm interface
    class Bdrm {

        private:
            DeviceNode node;

            uint64_t cursor_width, cursor_height;

            std::vector<UP<Connector>> connectors;
            std::vector<UP<Crtc>> crtcs;
            std::vector<UP<Plane>> planes;

        public:
            Bdrm(std::string_view path);

            uint64_t get_cursor_width() { return this->cursor_width; } // <! preferred width of the cursor plane
            uint64_t get_cursor_height() { return this->cursor_height; } // <! preferred height

            const std::vector<CRef<Crtc>> suitable_crtcs(CRef<Connector> connector);

            const std::vector<CRef<Connector>> get_all_connectors();
            const std::vector<CRef<Crtc>> get_all_crtcs();
            const std::vector<CRef<Plane>> get_all_planes();

            AtomicRequest create_atomic_request(); // <! create a new empty atomic request
            void commit(AtomicRequest& request); // <! commit an atomic request
        
    };

}

#endif // BDRM_HPP
