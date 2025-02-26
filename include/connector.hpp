#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <optional>
#include <string>
#include <vector>
#include <xf86drmMode.h>

extern "C" {
#include <libdisplay-info/info.h>
}

namespace BDRM {

    /// Nonvolatile drm connector instance
    class Connector {

        public:
            std::string name; //!< e.g. "HDMI-A-1"
            std::vector<drmModeModeInfo> modes;
            bool connected;

            drmModeSubPixel subpixelLayout; //!< not reliable 
            uint32_t width, height; //!< of the display in mm

            bool non_desktop; //!< something like a vr headset
            bool vrr_capable;
            
            std::optional<std::string> make, serial, model; //!< obtained through edid
            std::optional<di_color_primaries> color_primaries;
            std::optional<di_hdr_static_metadata> hdr_metadata;

            Connector(int fd, drmModeConnector* conn); // FIXME: temporary

        private:
            uint32_t conn_id;

            //Connector(int fd, drmModeConnector* conn);

    };

}

#endif // CONNECTOR_HPP
