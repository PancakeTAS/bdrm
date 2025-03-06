#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <xf86drmMode.h>

extern "C" {
#include <libdisplay-info/info.h>
}

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// forward decl
namespace BDRM::Atomic { class ConnectorReq; }

namespace BDRM::Resources {

    class Connector {

        friend class Res;
        friend class BDRM::Atomic::ConnectorReq;

        public:
            /// Name of the connector (e.g. "HDMI-A-1")
            std::string name;
            /// List of video modes supported by the connected monitor
            std::vector<drmModeModeInfo> modes;
            /// Whether the connector is connected
            bool connected;

            /// Subpixel layout of the connected monitor. Do not use this.
            drmModeSubPixel subpixelLayout;
            /// Physical width and height of the connected monitor
            uint32_t width, height;

            /// Whether the connected monitor is non-desktop (e.g. VR headset)
            bool non_desktop;
            /// Whether the connected monitor supports VRR. Do not rely on this.
            bool vrr_capable;

            /// Manufacturer, serial number, and model of the connected monitor
            std::optional<std::string> make, serial, model;
            /// Color primaries of the connected monitor
            std::optional<di_color_primaries> color_primaries;
            /// HDR metadata of the connected monitor
            std::optional<di_hdr_static_metadata> hdr_metadata;

        private:
            uint32_t conn_id;
            std::vector<uint32_t> encoder_ids; //!< supported encoder ids
            std::unordered_map<std::string, uint32_t> props;

            Connector(int fd, drmModeConnector* conn);

    };

}

#endif // CONNECTOR_HPP
