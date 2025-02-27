#ifndef CRTC_HPP
#define CRTC_HPP

#include <xf86drmMode.h>

#include <optional>
#include <string>
#include <unordered_map>

namespace BDRM {

    /// Nonvolatile drm ctrc instance
    class Crtc {

        friend class Bdrm;
        friend class ConnectorReq;
        friend class CrtcReq;
        friend class PlaneReq;

        public:
            std::optional<size_t> gamma_lut_size; //!< amount of rows in the gamma lut
            std::optional<size_t> degamma_lut_size;
            bool ctm; //!< color transformation matrix
            bool explicit_sync;

        private:
            uint32_t crtc_id;
            std::unordered_map<std::string, uint32_t> props;

            Crtc(int fd, drmModeCrtc* crtc);

    };

}

#endif // CRTC_HPP
