#include "atomic.hpp"
#include "bdrm.hpp"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"
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
    for (BDRM::Connector connector : bdrm.get_connectors()) {
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
    }
    
    // print crtcs

    std::cerr << "Crtcs:" << std::endl;
    for (BDRM::Crtc crtc : bdrm.get_crtcs()) {
        std::cerr << "Crtc:\n"
            << "  Gamma LUT Size: " << (crtc.gamma_lut_size ? *crtc.gamma_lut_size : 0) << "\n"
            << "  Degamma LUT Size: " << (crtc.degamma_lut_size ? *crtc.degamma_lut_size : 0) << "\n"
            << "  CTM: " << (crtc.ctm ? "Yes" : "No") << "\n"
            << "  Explicit Sync: " << (crtc.explicit_sync ? "Yes" : "No") << std::endl;
    }

    // print planes

    std::cerr << "Planes:" << std::endl;
    for (BDRM::Plane plane : bdrm.get_planes()) {
        std::cerr << "Plane:\n"
            << "  Type: " << plane.type << "\n"
            << "  Supported Formats: " << plane.supported_formats.size() << std::endl;
        for (BDRM::PlaneFormat format : plane.supported_formats) {
            std::cerr << "    - " << format.fourcc[0] << format.fourcc[1] << format.fourcc[2] << format.fourcc[3]
              << " (" << format.modifiers.size() << " modifers)" << std::endl;
        }
    }

    // create an atomic request

    BDRM::AtomicRequest request = bdrm.create_atomic_request();

    const BDRM::Connector& connector = bdrm.get_connectors()[0];
    const BDRM::Crtc& crtc = bdrm.get_crtcs()[0];
    const BDRM::Plane& plane = bdrm.get_planes()[0];
    
    BDRM::ConnectorReq& conn_req = request.addConnector(connector);
    conn_req.setCrtc(crtc);

    BDRM::CrtcReq& crtc_req = request.addCrtc(crtc);
    crtc_req.setActive(true);
    crtc_req.setMode(&connector.modes.front());
    
    BDRM::PlaneReq& plane_req = request.addPlane(plane);
    plane_req.setCrtc(crtc, &connector.modes.front());

    bdrm.commit(request);

    return 0;
}
