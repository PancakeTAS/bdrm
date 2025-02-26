#include "atomic.hpp"
#include "bdrm.hpp"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"
#include "utils.hpp"
#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <xf86drmMode.h>

// this is a rapidly changing test file!

int main() {
    BDRM::Bdrm bdrm("/dev/dri/card1");

    // print connectors

    std::cerr << "Connectors:" << std::endl;
    for (BDRM::CRef<BDRM::Connector> c_conn : bdrm.get_all_connectors()) {
        const BDRM::Connector& conn = c_conn;
        std::cerr << "Connector:\n"
            << "  Name: " << conn.name << "\n"
            << "  Connected: " << (conn.connected ? "Yes" : "No") << "\n"
            << "  Subpixel Layout: " << conn.subpixelLayout << "\n"
            << "  Dimensions: " << conn.width << "mm x " << conn.height << "mm\n"
            << "  Non-Desktop: " << (conn.non_desktop ? "Yes" : "No") << "\n"
            << "  VRR Capable: " << (conn.vrr_capable ? "Yes" : "No") << "\n"
            << "  Make: " << (conn.make ? *conn.make : "Unknown") << "\n"
            << "  Serial: " << (conn.serial ? *conn.serial : "Unknown") << "\n"
            << "  Model: " << (conn.model ? *conn.model : "Unknown") << "\n"
            << "  Primaries: " << (conn.color_primaries ? "Available" : "Not Available") << "\n"
            << "  HDR Metadata: " << (conn.hdr_metadata ? "Available" : "Not Available") << "\n"
            << "  Modes: " << conn.modes.size() << std::endl;
    }
    
    // print crtcs

    std::cerr << "Crtcs:" << std::endl;
    for (BDRM::CRef<BDRM::Crtc> c_crtc : bdrm.get_all_crtcs()) {
        const BDRM::Crtc& crtc = c_crtc;
        std::cerr << "Crtc:\n"
            << "  Gamma LUT Size: " << (crtc.gamma_lut_size ? *crtc.gamma_lut_size : 0) << "\n"
            << "  Degamma LUT Size: " << (crtc.degamma_lut_size ? *crtc.degamma_lut_size : 0) << "\n"
            << "  CTM: " << (crtc.ctm ? "Yes" : "No") << "\n"
            << "  Explicit Sync: " << (crtc.explicit_sync ? "Yes" : "No") << std::endl;
    }

    // print planes

    std::cerr << "Planes:" << std::endl;
    for (BDRM::CRef<BDRM::Plane> c_plane : bdrm.get_all_planes()) {
        const BDRM::Plane& plane = c_plane;
        std::cerr << "Plane:\n"
            << "  Type: " << plane.type << "\n"
            << "  Supported Formats: " << plane.supported_formats.size() << std::endl;
        for (BDRM::PlaneFormat format : plane.supported_formats) {
            std::cerr << "    - " << format.fourcc[0] << format.fourcc[1] << format.fourcc[2] << format.fourcc[3]
              << " (" << format.modifiers.size() << " modifers)" << std::endl;
        }
    }

    // create an atomic request

    {

        // get the integrated display
        auto connectors = bdrm.get_all_connectors();
        auto connector = std::find_if(connectors.begin(), connectors.end(), [](auto c) {
            return c.get().name == "eDP-1";
        })->get();

        auto mode = std::find_if(connector.modes.begin(), connector.modes.end(), [](auto m) {
            return m.hdisplay == 1920 && m.vdisplay == 1080 && m.vrefresh == 60;
        })[0];

        // find a suitable crtc and plane
        auto crtc = bdrm.suitable_crtcs(connector).front().get();
        auto plane = bdrm.suitable_planes(crtc, BDRM::PlaneType::PRIMARY).front().get();

        // build request
        BDRM::AtomicRequest request = bdrm.create_atomic_request();

        BDRM::ConnectorReq& conn_req = request.addConnector(connector);
        conn_req.setCrtc(crtc);

        BDRM::CrtcReq& crtc_req = request.addCrtc(crtc);
        crtc_req.setActive(true);
        crtc_req.setMode(&mode);

        BDRM::PlaneReq& plane_req = request.addPlane(plane);
        plane_req.setCrtc(crtc, &mode);

        // sleep 1s
        sleep(1);
        bdrm.commit(request);

    }

    // sleep 5 seconds
    sleep(5);

    return 0;
}
