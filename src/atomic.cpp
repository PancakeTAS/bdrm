#include "atomic.hpp"
#include "resources.hpp"

#include <stdexcept>

using namespace BDRM::Atomic;

AtomicRequest::AtomicRequest(const int fd) : fd(fd) {
    this->req = drmModeAtomicAlloc();
    if (!this->req)
        throw std::runtime_error("Failed to allocate atomic request");
}

AtomicRequest::AtomicRequest(const int fd, const Res& resources) : fd(fd) {
    this->req = drmModeAtomicAlloc();
    if (!this->req)
        throw std::runtime_error("Failed to allocate atomic request");

    for (const Connector& connector : resources.connectors)
        this->addConnector(connector).zeroedRequest();
    for (const Crtc& crtc : resources.crtcs)
        this->addCrtc(crtc).zeroedRequest();
    for (const Plane& plane : resources.planes)
        this->addPlane(plane).zeroedRequest();
}

ConnectorReq& AtomicRequest::addConnector(const Connector& connector) {
    this->connectors.push_back(ConnectorReq(connector, this->req));
    return this->connectors.back();
}

CrtcReq& AtomicRequest::addCrtc(const Crtc& crtc) {
    this->crtcs.push_back(CrtcReq(crtc, this->fd, this->req));
    return this->crtcs.back();
}

PlaneReq& AtomicRequest::addPlane(const Plane& plane) {
    this->planes.push_back(PlaneReq(plane, this->req));
    return this->planes.back();
}

AtomicRequest::~AtomicRequest() {
    drmModeAtomicFree(this->req);
    for (const CrtcReq& reqs : this->crtcs) {
        for (const uint32_t blob_id : reqs.blobs)
            drmModeDestroyPropertyBlob(this->fd, blob_id);
    }
}
