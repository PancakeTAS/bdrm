#ifndef CONNECTORREQ_HPP
#define CONNECTORREQ_HPP

#include "resources/connector.hpp"
#include "resources/crtc.hpp"

#include <xf86drmMode.h>
#include <drm_mode.h>

// TODO: Colorspace connector property and hdr output metadata

// TODO: understanding explicit sync or whatever these fences do

namespace BDRM {

    /// Connector request properties
    class ConnectorReq {

        friend class AtomicRequest;

        public:
            // methods for configuring the connector
            void setCrtc(const Crtc& crtc);
            void clearProperties(); //!< set all properties to default

        private:
            const Connector& connector;
            drmModeAtomicReq* req;

            ConnectorReq(const Connector& connector, drmModeAtomicReq* req) : connector(connector), req(req) {}

    };

}

#endif // CONNECTORREQ_HPP
