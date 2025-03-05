#ifndef CONNECTORREQ_HPP
#define CONNECTORREQ_HPP

#include "resources/connector.hpp"
#include "resources/crtc.hpp"

#include <xf86drmMode.h>

// TODO: Colorspace connector property and hdr output metadata

// TODO: understanding explicit sync or whatever these fences do

namespace BDRM::Atomic {

    /// Connector request properties
    class ConnectorReq {

        friend class AtomicRequest;

        public:
            void setCrtc(const BDRM::Resources::Crtc& crtc); //!< set the CRTC and reset link status

        private:
            const BDRM::Resources::Connector& connector;
            drmModeAtomicReq* req;

            ConnectorReq(const BDRM::Resources::Connector& connector, drmModeAtomicReq* req) : connector(connector), req(req) {}
            void zeroedRequest(); //!< set all properties to zero

    };

}

#endif // CONNECTORREQ_HPP
