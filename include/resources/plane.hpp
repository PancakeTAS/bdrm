#ifndef PLANE_HPP
#define PLANE_HPP

#include <xf86drmMode.h>

#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace BDRM {

    // FIXME: when not doing atomic modesetting, there's "format" and "gamma_size"
    // in the plane struct which needs to be properly handled

    // FIXME: need a good way to handle possible_crtcs

    /// Type of plane
    enum PlaneType {
        OVERLAY = 0,
        PRIMARY = 1,
        CURSOR = 2
    };

    /// Plane format with modifiers
    struct PlaneFormat {
        union {
            uint32_t format; //!< check drm_fourcc.h for formats
            char fourcc[4];
        };
        std::vector<uint64_t> modifiers;
    };

    /// Nonvolatile drm plane instance
    class Plane {

        friend class Resources;
        friend class PlaneReq;

        public:
            PlaneType type = PRIMARY;
            std::vector<PlaneFormat> supported_formats;

        private:
            uint32_t plane_id;
            std::bitset<32> possible_crtcs;
            std::unordered_map<std::string, uint32_t> props;

            Plane(int fd, drmModePlane* plane);

    };

}

#endif // PLANE_HPP
