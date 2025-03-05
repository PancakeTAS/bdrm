#include "atomic/connectorreq.hpp"

#include <stdexcept>

using namespace BDRM::Atomic;

#define SET(prop_name, value) \
    { auto prop = this->connector.props.find(prop_name); \
    if (prop == this->connector.props.end()) \
        throw std::runtime_error("Connector has no " prop_name " property"); \
    if (drmModeAtomicAddProperty(req, this->connector.conn_id, prop->second, value) < 0) \
        throw std::runtime_error("Failed to add " prop_name " property to atomic request"); }

void ConnectorReq::setCrtc(const BDRM::Resources::Crtc& crtc) {
    SET("CRTC_ID", crtc.crtc_id)
    SET("link-status", DRM_MODE_LINK_STATUS_GOOD)
}

void ConnectorReq::zeroedRequest() {
    SET("CRTC_ID", 0)
    SET("link-status", DRM_MODE_LINK_STATUS_GOOD)
}
