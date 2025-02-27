#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"
#include "utils.hpp"

namespace BDRM {

    /// DRM resources
    class Resources {

        friend class Bdrm;
        friend class AtomicRequest;

        private:
            Resources(int fd); //!< fetch all resources
            int fd;

            std::vector<Connector> connectors;
            std::vector<Crtc> crtcs;
            std::vector<Plane> planes;

        public:

            struct ConnectorQueryArgs {
                OP<std::string> name = std::nullopt; //!< e.g. "HDMI-A-1"
                OP<bool> connected = std::nullopt; //!< monitor must be connected
                OP<drmModeSubPixel> subpixel_layout = std::nullopt;
                OP<bool> non_desktop = std::nullopt; //!< only look for normal monitors
                OP<bool> vrr_capable = std::nullopt; //!< monitor must support vrr
                OP<std::string> make = std::nullopt; //!< monitor manufacturer/model/serial
                OP<std::string> model = std::nullopt;
                OP<std::string> serial = std::nullopt;
            }; //!< query arguments for searching connectors
            const std::vector<CRef<Connector>> search_connectors(ConnectorQueryArgs args) const; //!< search for specific connectors

            struct ModeQueryArgs {
                OP<uint32_t> width = std::nullopt;
                OP<uint32_t> height = std::nullopt;
                OP<uint32_t> min_refresh = std::nullopt; //!< min refresh rate (inclusive)
                OP<uint32_t> max_refresh = std::nullopt; //!< max refresh rate (inclusive)
            }; //!< query arguments for searching modes
            const std::vector<drmModeModeInfo> search_modes(ModeQueryArgs args) const; //!< search for specific modes

            struct CrtcQueryArgs {
                OP<CRef<Connector>> connector = std::nullopt; //!< must support this connector
                OP<bool> gamma_lut = std::nullopt; //!< must support a gamma lut
                OP<bool> degamma_lut = std::nullopt;
                OP<bool> ctm = std::nullopt; //!< must support a color transformation matrix
                OP<bool> explicit_sync = std::nullopt;
            }; //!< query arguments for searching crtcs
            const std::vector<CRef<Crtc>> search_crtcs(CrtcQueryArgs args) const; //!< search for specific crtcs

            struct PlaneQueryArgs {
                OP<CRef<Crtc>> crtc = std::nullopt; //!< must support this crtc
                OP<PlaneType> type = std::nullopt;
                OP<uint32_t> format = std::nullopt;
                OP<uint64_t> modifier = std::nullopt; //!< format must be set!
            }; //!< query arguments for searching planes
            const std::vector<CRef<Plane>> search_planes(PlaneQueryArgs args) const; //!< search for specific planes

    };

}

#endif // RESOURCES_HPP
