#ifndef PLANEREQ_HPP
#define PLANEREQ_HPP

#include "resources/plane.hpp"
#include "resources/crtc.hpp"
#include "bdrm/buffer.hpp"

#include <xf86drmMode.h>

#include <cstdint>

namespace BDRM::Atomic {

    /// Plane request properties
    class PlaneReq {

        friend class AtomicRequest;

        public:
            // TODO: unsetting should be possible, or at least filling up with zeros

            /// Attach a framebuffer to the plane
            void setFb(const Buffer& fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
            /// Attach a framebuffer to the plane, with the same size as the framebuffer
            void setFb(const Buffer& fb) {
                this->setFb(fb, 0, 0, fb.width, fb.height);
            }
            /// Attach a CRTC to the plane
            void setCrtc(const BDRM::Resources::Crtc& crtc, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
            /// Attach a CRTC to the plane, with the same size as the CRTC
            void setCrtc(const BDRM::Resources::Crtc& crtc, const drmModeModeInfo* mode) {
                this->setCrtc(crtc, 0, 0, mode->hdisplay, mode->vdisplay);
            }

        private:
            const BDRM::Resources::Plane& plane;
            drmModeAtomicReq* req;

            PlaneReq(const BDRM::Resources::Plane& plane, drmModeAtomicReq* req) : plane(plane), req(req) {}
            void zeroedRequest(); //!< set all properties to zero

    };

}

#endif // PLANEREQ_HPP
