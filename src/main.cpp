#include "bdrm.hpp"

#include <libdrm/drm_fourcc.h>
#include <xf86drmMode.h>

#include <fcntl.h>
#include <iostream>
#include <unistd.h>

// this is a rapidly changing test file!

int main() {
    BDRM::Bdrm bdrm("/dev/dri/card1");
    const BDRM::Res& resources = bdrm.get_resources();

    // create an atomic request
    {

        auto& connector = resources.search_connectors({ .name = "eDP-1", .connected = true} ).front().get();
        std::cerr << "Connector:\n"
            << "  Name: " << connector.name << "\n"
            << "  Connected: " << (connector.connected ? "Yes" : "No") << "\n"
            << "  Subpixel Layout: " << connector.subpixelLayout << "\n"
            << "  Dimensions: " << connector.width << "mm x " << connector.height << "mm\n"
            << "  Non-Desktop: " << (connector.non_desktop ? "Yes" : "No") << "\n"
            << "  VRR Capable: " << (connector.vrr_capable ? "Yes" : "No") << "\n"
            << "  Make: " << (connector.make ? *connector.make : "Unknown") << "\n"
            << "  Serial: " << (connector.serial ? *connector.serial : "Unknown") << "\n"
            << "  Model: " << (connector.model ? *connector.model : "Unknown") << "\n"
            << "  Primaries: " << (connector.color_primaries ? "Available" : "Not Available") << "\n"
            << "  HDR Metadata: " << (connector.hdr_metadata ? "Available" : "Not Available") << "\n"
            << "  Modes: " << connector.modes.size() << std::endl;

        auto mode = resources.search_modes({ .width = 1920, .height = 1080, .min_refresh = 59, .max_refresh = 60 }).front();
        std::cerr << "Mode:\n"
            << "  Name: " << mode.name << "\n"
            << "  Dimensions: " << mode.hdisplay << "x" << mode.vdisplay << "\n"
            << "  Refresh Rate: " << mode.vrefresh << "Hz\n"
            << "  Flags: " << mode.flags << "\n"
            << "  Type: " << mode.type << std::endl;

        auto& crtc = resources.search_crtcs({ .connector = connector }).front().get();
        std::cerr << "Crtc:\n"
            << "  Gamma LUT Size: " << (crtc.gamma_lut_size ? *crtc.gamma_lut_size : 0) << "\n"
            << "  Degamma LUT Size: " << (crtc.degamma_lut_size ? *crtc.degamma_lut_size : 0) << "\n"
            << "  CTM: " << (crtc.ctm ? "Yes" : "No") << "\n"
            << "  Explicit Sync: " << (crtc.explicit_sync ? "Yes" : "No") << std::endl;

        auto& plane = resources.search_planes({ .crtc = crtc, .type = BDRM::PlaneType::PRIMARY }).front().get();
        std::cerr << "Plane:\n"
            << "  Type: " << plane.type << "\n"
            << "  Supported Formats: " << plane.supported_formats.size() << std::endl;
        for (BDRM::PlaneFormat format : plane.supported_formats) {
            std::cerr << "    - " << format.fourcc[0] << format.fourcc[1] << format.fourcc[2] << format.fourcc[3]
              << " (" << format.modifiers.size() << " modifers)" << std::endl;
        }

        // allocate a framebuffer
        std::vector<uint64_t> modifiers = { 0x0300000000606015 }; // some hardcoded nvidia modifier
        const BDRM::Framebuffer& fb = bdrm.create_framebuffer(
            mode.hdisplay, mode.vdisplay,
            DRM_FORMAT_XRGB8888, modifiers,
            false
        );

        // build request
        BDRM::AtomicRequest request = bdrm.create_atomic_request();

        BDRM::ConnectorReq& conn_req = request.addConnector(connector);
        conn_req.setCrtc(crtc);

        BDRM::CrtcReq& crtc_req = request.addCrtc(crtc);
        crtc_req.setActive(true);
        crtc_req.setMode(&mode);

        BDRM::PlaneReq& plane_req = request.addPlane(plane);
        plane_req.setCrtc(crtc, &mode);
        plane_req.setFb(fb);

        bdrm.commit(request);

    }

    // sleep 5 seconds
    sleep(5);

    return 0;
}
