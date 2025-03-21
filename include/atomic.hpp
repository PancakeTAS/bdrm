#ifndef ATOMIC_HPP
#define ATOMIC_HPP

#include "atomic/connectorreq.hpp"
#include "atomic/crtcreq.hpp"
#include "atomic/planereq.hpp"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"
#include "resources.hpp"

#include <vector>

// forward decl
namespace BDRM { class Bdrm; }

namespace BDRM::Atomic {

    /// Atomic request instance
    class AtomicRequest {

        friend class BDRM::Bdrm;

        private:
            int fd;
            drmModeAtomicReq* req;
            std::vector<ConnectorReq> connectors; //!< keep track of all resources in order to free them
            std::vector<CrtcReq> crtcs;
            std::vector<PlaneReq> planes;

            AtomicRequest(int fd); //!< create an empty request
            AtomicRequest(const int fd, const BDRM::Resources::Res& resources); //!< create a request zeroing all resources

        public:

            /*
             * An atomic request is a collection of resources that are to be modified atomically.
             * The following functions are used to add resources to the request.
             */

            /// Add a connector to the request
            ConnectorReq& addConnector(const BDRM::Resources::Connector& connector);

            /// Add a CRTC to the request
            CrtcReq& addCrtc(const BDRM::Resources::Crtc& crtc);

            /// Add a plane to the request
            PlaneReq& addPlane(const BDRM::Resources::Plane& plane);

            ~AtomicRequest();

    };

}

#endif // ATOMIC_HPP
