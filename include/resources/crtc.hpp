#ifndef CRTC_HPP
#define CRTC_HPP

#include <xf86drmMode.h>

#include <optional>
#include <string>
#include <unordered_map>

// forward decl
namespace BDRM::Atomic {
    class ConnectorReq;
    class CrtcReq;
    class PlaneReq;
}

namespace BDRM::Resources {

    class Crtc {

        friend class Res;
        friend class BDRM::Atomic::ConnectorReq;
        friend class BDRM::Atomic::CrtcReq;
        friend class BDRM::Atomic::PlaneReq;

        public:
            /// Size of the gamma look-up table
            std::optional<size_t> gamma_lut_size;
            /// Size of the degamma look-up table
            std::optional<size_t> degamma_lut_size;
            /// Whether a color transformation matrix is supported
            bool ctm;
            /// Whether the properties for explicit sync are present
            bool explicit_sync;

        private:
            uint32_t crtc_id;
            std::unordered_map<std::string, uint32_t> props;

            Crtc(int fd, drmModeCrtc* crtc);

    };

}

#endif // CRTC_HPP
