#ifndef BDRM_HPP
#define BDRM_HPP

#include "connector.hpp"
#include "crtc.hpp"
#include "plane.hpp"
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

        const int get_fd() const { return this->fd; }

    };

    /// better drm interface
    class Bdrm {

        private:
            DeviceNode node;

            uint64_t cursor_width, cursor_height;

            std::vector<Connector> connectors;
            std::vector<Crtc> crtcs;
            std::vector<Plane> planes;

        public:
            Bdrm(std::string_view path);

            const uint64_t get_cursor_width() const { return this->cursor_width; }
            const uint64_t get_cursor_height() const { return this->cursor_height; }

            // TODO: methods for filtering
            const std::vector<Connector>& get_connectors() const { return this->connectors; }
            const std::vector<Crtc>& get_crtcs() const { return this->crtcs; }
            const std::vector<Plane>& get_planes() const { return this->planes; }
        
    };

}

#endif // BDRM_HPP