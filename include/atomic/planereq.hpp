#ifndef PLANEREQ_HPP
#define PLANEREQ_HPP

#include "resources/framebuffer.hpp"
#include "resources/plane.hpp"
#include "resources/crtc.hpp"

#include <xf86drmMode.h>
#include <drm_mode.h>

#include <cstdint>

namespace BDRM::Atomic {

    using namespace BDRM::Resources;

    /// Plane request properties
    class PlaneReq {

        friend class AtomicRequest;

        public:
            // methods for configuring the plane
            void setFb(const Framebuffer& fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
            void setFb(const Framebuffer& fb) {
                this->setFb(fb, 0, 0, fb.width, fb.height);
            }
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
