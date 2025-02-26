#ifndef CRTC_HPP
#define CRTC_HPP

#include <optional>
#include <xf86drmMode.h>

namespace BDRM {

    /// Nonvolatile drm ctrc instance
    class Crtc {

        public:
            std::optional<size_t> gamma_lut_size;
            std::optional<size_t> degamma_lut_size;
            
            bool ctm; //!< color transformation matrix
            bool explicit_sync;

            Crtc(int fd, drmModeCrtc* crtc); // FIXME: temporary

        private:
            uint32_t crtc_id;

            //Crtc(int fd, drmModeCrtc* crtc);

    };

}

#endif // CRTC_HPP
