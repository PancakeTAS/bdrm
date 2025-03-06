#ifndef PLANE_HPP
#define PLANE_HPP

#include <xf86drmMode.h>

#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

// forward decl
namespace BDRM::Atomic { class PlaneReq; }

namespace BDRM::Resources {

    // FIXME: when not doing atomic modesetting, there's "format" and "gamma_size"
    // in the plane struct which needs to be properly handled

    enum PlaneType {
        OVERLAY = 0,
        PRIMARY = 1,
        CURSOR = 2
    }; //!< Type of plane

    struct PlaneFormat {
        union {
            uint32_t format; //!< Check drm_fourcc.h for formats
            char fourcc[4];
        };
        std::vector<uint64_t> modifiers;
    }; //!< Plane format and modifiers

    class Plane {

        friend class Res;
        friend class BDRM::Atomic::PlaneReq;

        public:
            /// Type of plane
            PlaneType type = PRIMARY;
            /// List of formats supported by the plane. May be empty!
            std::vector<PlaneFormat> supported_formats;

        private:
            uint32_t plane_id;
            std::bitset<32> possible_crtcs; //!< bitset of possible CRTCs
            std::unordered_map<std::string, uint32_t> props;

            Plane(int fd, drmModePlane* plane);

    };

}

#endif // PLANE_HPP
