#ifndef CRTCREQ_HPP
#define CRTCREQ_HPP

#include "resources/crtc.hpp"

#include <xf86drmMode.h>
#include <drm_mode.h>

#include <cstdint>
#include <vector>

namespace BDRM {

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

            private:
            const Crtc& crtc;
            const int fd;
            drmModeAtomicReq* req;
            std::vector<uint32_t> blobs;

            CrtcReq(const Crtc& crtc, const int fd, drmModeAtomicReq* req) : crtc(crtc), fd(fd), req(req) {}
            void clearProperties();

    };

}

#endif // CRTCREQ_HPP
