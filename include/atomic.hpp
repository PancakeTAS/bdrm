#ifndef ATOMIC_HPP
#define ATOMIC_HPP

#include "drm_mode.h"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"
#include <cstdint>
#include <vector>
#include <xf86drmMode.h>

// TODO: ponder whether this should be a sub-namespace like BDRM::Atomic

// TODO: Colorspace connector property and hdr output metadata

// TODO: understanding explicit sync or whatever these fences do

namespace BDRM {

    /// Connector request properties
    class ConnectorReq {

        friend class AtomicRequest;

        public:
            // methods for configuring the connector
            void setCrtc(const Crtc& crtc);
            void clearProperties(); //!< set all properties to default

        private:
            const Connector& connector;
            drmModeAtomicReq* req;

            ConnectorReq(const Connector& connector, drmModeAtomicReq* req) : connector(connector), req(req) {}

    };

    /// CRTC request properties
    class CrtcReq {

        friend class AtomicRequest;

        public:
            // methods for configurting the crtc
            void setActive(bool active);
            void setMode(const drmModeModeInfo* mode);
            void setVrr(bool vrr);
            void setGammaLut(const std::vector<drm_color_lut>& lut);
            void setCTM(const struct drm_color_ctm* ctm);
            void setDegammaLut(const std::vector<drm_color_lut>& lut);
            void clearProperties(); //!< set all properties to default

        private:
            const Crtc& crtc;
            const int fd;
            drmModeAtomicReq* req;
            std::vector<uint32_t> blobs;

            CrtcReq(const Crtc& crtc, const int fd, drmModeAtomicReq* req) : crtc(crtc), fd(fd), req(req) {}

    };

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
            void clearProperties(); //!< set all properties to default

        private:
            const Plane& plane;
            drmModeAtomicReq* req;

            PlaneReq(const Plane& plane, drmModeAtomicReq* req) : plane(plane), req(req) {}

    };

    /// Atomic request class
    class AtomicRequest {

        friend class Bdrm;

        private:
            int fd;
            drmModeAtomicReq* req;
            std::vector<ConnectorReq> connectors;
            std::vector<CrtcReq> crtcs;
            std::vector<PlaneReq> planes;

            AtomicRequest(int fd);
            
        public:
            ConnectorReq& addConnector(const Connector& connector);
            CrtcReq& addCrtc(const Crtc& crtc);
            PlaneReq& addPlane(const Plane& plane);
            
            ~AtomicRequest();

    };

}

#endif // ATOMIC_HPP
