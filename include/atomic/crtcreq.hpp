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
            /// Enable or disable the CRTC
            void setActive(bool active);
            /// Set the video mode of the CRTC
            void setMode(const drmModeModeInfo* mode);
            /// Hint to enable or disable VRR, may fail if not supported (as you cannot rely on vrr_capable)
            void setVrr(bool vrr);
            /// Set the gamma LUT of the CRTC, ensure this property is supported
            void setGammaLut(const std::vector<drm_color_lut>& lut);
            /// Set the gamma LUT of the CRTC, ensure this property is supported
            void setCTM(const struct drm_color_ctm* ctm);
            /// Set the gamma LUT of the CRTC, ensure this property is supported
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
