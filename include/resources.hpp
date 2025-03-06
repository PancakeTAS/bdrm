#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include "bdrm/types.hpp"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"

#include <vector>

// forward decl
namespace BDRM::Atomic { class AtomicRequest; }
namespace BDRM { class Bdrm; }

namespace BDRM::Resources {

    /// DRM resources
    class Res {

        friend class BDRM::Bdrm;
        friend class BDRM::Atomic::AtomicRequest;

        private:
            Res(int fd); //!< fetch all resources
            int fd;

            std::vector<Connector> connectors;
            std::vector<Crtc> crtcs;
            std::vector<Plane> planes;

        public:

            /*
             * The following functions are used to search for resources using a filter.
             * Each function takes a struct with optional arguments that can be used to
             * narrow down the search. If an argument is not set, it is ignored.
             */

            struct ConnectorQueryArgs {
                OP<std::string> name = std::nullopt; //!< e.g. "HDMI-A-1"
                OP<bool> connected = std::nullopt; //!< monitor must be connected
                OP<drmModeSubPixel> subpixel_layout = std::nullopt;
                OP<bool> non_desktop = std::nullopt; //!< only look for normal monitors
                OP<bool> vrr_capable = std::nullopt; //!< monitor must support vrr
                OP<std::string> make = std::nullopt; //!< monitor manufacturer/model/serial
                OP<std::string> model = std::nullopt;
                OP<std::string> serial = std::nullopt;
            }; //!< Query arguments for searching connectors

            /// Search for connectors using a filter
            const std::vector<CRef<Connector>> search_connectors(ConnectorQueryArgs args) const;

            struct ModeQueryArgs {
                OP<uint32_t> width = std::nullopt;
                OP<uint32_t> height = std::nullopt;
                OP<uint32_t> min_refresh = std::nullopt; //!< min refresh rate (inclusive)
                OP<uint32_t> max_refresh = std::nullopt; //!< max refresh rate (inclusive)
            }; //!< Query arguments for searching modes

            /// Search for modes using a filter
            const std::vector<drmModeModeInfo> search_modes(ModeQueryArgs args) const;

            struct CrtcQueryArgs {
                OP<CRef<Connector>> connector = std::nullopt; //!< must support this connector
                OP<bool> gamma_lut = std::nullopt; //!< must support a gamma lut
                OP<bool> degamma_lut = std::nullopt;
                OP<bool> ctm = std::nullopt; //!< must support a color transformation matrix
                OP<bool> explicit_sync = std::nullopt;
            }; //!< Query arguments for searching crtcs

            /// Search for crtcs using a filter
            const std::vector<CRef<Crtc>> search_crtcs(CrtcQueryArgs args) const;

            struct PlaneQueryArgs {
                OP<CRef<Crtc>> crtc = std::nullopt; //!< must support this crtc
                OP<PlaneType> type = std::nullopt;
                OP<uint32_t> format = std::nullopt;
                OP<uint64_t> modifier = std::nullopt; //!< format must be set!
            }; //!< Query arguments for searching planes

            /// Search for planes using a filter
            const std::vector<CRef<Plane>> search_planes(PlaneQueryArgs args) const;

            struct PlaneFormatQueryArgs {
                OP<CRef<Plane>> plane = std::nullopt; //!< must support this plane
                OP<uint32_t> format = std::nullopt;
            }; //!< Query arguments for searching plane formats

            /// Search for plane formats using a filter
            const std::vector<CRef<PlaneFormat>> search_plane_formats(PlaneFormatQueryArgs args) const;

    };

}

#endif // RESOURCES_HPP
