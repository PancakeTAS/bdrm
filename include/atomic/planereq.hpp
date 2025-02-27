#ifndef PLANEREQ_HPP
#define PLANEREQ_HPP

#include "resources/plane.hpp"
#include "resources/crtc.hpp"

#include <xf86drmMode.h>
#include <drm_mode.h>

#include <cstdint>

namespace BDRM {

    /// Plane request properties
    class PlaneReq {

        friend class AtomicRequest;

        public:
            // methods for configuring the plane
            void setFb(uint32_t fb_id, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
            /*void setFb(uint32_t fb_id) { // FIXME: class for framebuffer
                throw std::runtime_error("Not implemented!");
            }*/
            void setCrtc(const Crtc& crtc, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
            void setCrtc(const Crtc& crtc, const drmModeModeInfo* mode) {
                this->setCrtc(crtc, 0, 0, mode->hdisplay, mode->vdisplay);
            }

        private:
            const Plane& plane;
            drmModeAtomicReq* req;

            PlaneReq(const Plane& plane, drmModeAtomicReq* req) : plane(plane), req(req) {}
            void zeroedRequest(); //!< set all properties to zero

    };

}

#endif // PLANEREQ_HPP
