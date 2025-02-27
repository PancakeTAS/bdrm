#ifndef ATOMIC_HPP
#define ATOMIC_HPP

#include "atomic/connectorreq.hpp"
#include "atomic/crtcreq.hpp"
#include "atomic/planereq.hpp"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"

#include <xf86drmMode.h>

#include <vector>

namespace BDRM {

    /// Atomic request class
    class AtomicRequest {

        friend class Bdrm;

        private:
            int fd;
            drmModeAtomicReq* req;
            std::vector<ConnectorReq> connectors;
            std::vector<CrtcReq> crtcs;
            std::vector<PlaneReq> planes;

            AtomicRequest(int fd);
            
        public:
            ConnectorReq& addConnector(const Connector& connector);
            CrtcReq& addCrtc(const Crtc& crtc);
            PlaneReq& addPlane(const Plane& plane);
            
            ~AtomicRequest();

    };

}

#endif // ATOMIC_HPP
