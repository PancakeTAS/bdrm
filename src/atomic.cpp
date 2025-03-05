#include "atomic.hpp"

#include <stdexcept>

using namespace BDRM::Atomic;

AtomicRequest::AtomicRequest(const int fd) : fd(fd) {
    this->req = drmModeAtomicAlloc();
    if (!this->req)
        throw std::runtime_error("Failed to allocate atomic request");
}

AtomicRequest::AtomicRequest(const int fd, const BDRM::Resources::Res& resources) : fd(fd) {
    this->req = drmModeAtomicAlloc();
    if (!this->req)
        throw std::runtime_error("Failed to allocate atomic request");

    for (const BDRM::Resources::Connector& connector : resources.connectors)
        this->addConnector(connector).zeroedRequest();
    for (const BDRM::Resources::Crtc& crtc : resources.crtcs)
        this->addCrtc(crtc).zeroedRequest();
    for (const BDRM::Resources::Plane& plane : resources.planes)
        this->addPlane(plane).zeroedRequest();
}

ConnectorReq& AtomicRequest::addConnector(const BDRM::Resources::Connector& connector) {
    this->connectors.push_back(ConnectorReq(connector, this->req));
    return this->connectors.back();
}

CrtcReq& AtomicRequest::addCrtc(const BDRM::Resources::Crtc& crtc) {
    this->crtcs.push_back(CrtcReq(crtc, this->fd, this->req));
    return this->crtcs.back();
}

PlaneReq& AtomicRequest::addPlane(const BDRM::Resources::Plane& plane) {
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
