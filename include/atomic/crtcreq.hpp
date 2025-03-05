#ifndef CRTCREQ_HPP
#define CRTCREQ_HPP

#include "resources/crtc.hpp"

#include <xf86drmMode.h>

#include <cstdint>
#include <vector>

namespace BDRM::Atomic {

    /// CRTC request properties
    class CrtcReq {

        friend class AtomicRequest;

        public:
            // methods for configurting the crtc
            void setActive(bool active);
            void setMode(const drmModeModeInfo* mode);
            void setVrr(bool vrr); //!< hint the driver to enable variable refresh rate, does not guarantee it
            void setGammaLut(const std::vector<drm_color_lut>& lut);
            void setCTM(const struct drm_color_ctm* ctm);
            void setDegammaLut(const std::vector<drm_color_lut>& lut);

        private:
            const BDRM::Resources::Crtc& crtc;
            const int fd;
            drmModeAtomicReq* req;
            std::vector<uint32_t> blobs; //!< list of blobs to free

            CrtcReq(const BDRM::Resources::Crtc& crtc, const int fd, drmModeAtomicReq* req) : crtc(crtc), fd(fd), req(req) {}
            void zeroedRequest(); //!< set all properties to zero

    };

}

#endif // CRTCREQ_HPP
