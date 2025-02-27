#ifndef ATOMIC_HPP
#define ATOMIC_HPP

#include "atomic/connectorreq.hpp"
#include "atomic/crtcreq.hpp"
#include "atomic/planereq.hpp"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"
#include "resources.hpp"

#include <xf86drmMode.h>

#include <vector>

namespace BDRM {

    /// Atomic request instance
    class AtomicRequest {

        friend class Bdrm;

        private:
            int fd;
            drmModeAtomicReq* req;
            std::vector<ConnectorReq> connectors; //!< keep track of all resources in order to free them
            std::vector<CrtcReq> crtcs;
            std::vector<PlaneReq> planes;

            AtomicRequest(int fd); //!< create an empty request
            AtomicRequest(const int fd, const Resources& resources); //!< create a request zeroing all resources

        public:
            ConnectorReq& addConnector(const Connector& connector); //!< add a resource
            CrtcReq& addCrtc(const Crtc& crtc);
            PlaneReq& addPlane(const Plane& plane);

            ~AtomicRequest();

    };

}

#endif // ATOMIC_HPP
