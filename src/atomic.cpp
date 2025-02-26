#include "atomic.hpp"
#include <stdexcept>
#include <xf86drmMode.h>

using namespace BDRM;

// setting properties

#define ADD_PROPERTY(obj_id, props, prop_name, value) \
    { auto prop = (props).find(prop_name); \
    if (prop == (props).end()) \
        throw std::runtime_error("Object has no " prop_name " property"); \
    if (drmModeAtomicAddProperty(req, obj_id, prop->second, value) < 0) \
        throw std::runtime_error("Failed to add " prop_name " property to atomic request"); }

void ConnectorReq::setCrtc(const Crtc& crtc) {
    ADD_PROPERTY(this->connector.conn_id, this->connector.props, "CRTC_ID", crtc.crtc_id)
}
        
void CrtcReq::setActive(bool active) {
    ADD_PROPERTY(this->crtc.crtc_id, this->crtc.props, "ACTIVE", active)
}

void CrtcReq::setMode(const drmModeModeInfo* mode) {
    uint32_t blob_id;
    if (drmModeCreatePropertyBlob(this->fd, mode, sizeof(drmModeModeInfo), &blob_id) < 0)
        throw std::runtime_error("Failed to create mode blob");
    
    ADD_PROPERTY(this->crtc.crtc_id, this->crtc.props, "MODE_ID", blob_id)
    this->blobs.push_back(blob_id);
}

void CrtcReq::setVrr(bool vrr) {
    ADD_PROPERTY(this->crtc.crtc_id, this->crtc.props, "VRR_ENABLED", vrr)
}

void CrtcReq::setGammaLut(const std::vector<drm_color_lut>& lut) {
    uint32_t blob_id;
    if (drmModeCreatePropertyBlob(this->fd, lut.data(), sizeof(drm_color_lut) * lut.size(), &blob_id) < 0)
        throw std::runtime_error("Failed to create gamma lut blob");
    
    ADD_PROPERTY(this->crtc.crtc_id, this->crtc.props, "GAMMA_LUT", blob_id)
    this->blobs.push_back(blob_id);
}

void CrtcReq::setCTM(const struct drm_color_ctm* ctm) {
    uint32_t blob_id;
    if (drmModeCreatePropertyBlob(this->fd, ctm, sizeof(drm_color_ctm), &blob_id) < 0)
        throw std::runtime_error("Failed to create ctm blob");
    
    ADD_PROPERTY(this->crtc.crtc_id, this->crtc.props, "CTM", blob_id)
    this->blobs.push_back(blob_id);
}

void CrtcReq::setDegammaLut(const std::vector<drm_color_lut>& lut) {
    uint32_t blob_id;
    if (drmModeCreatePropertyBlob(this->fd, lut.data(), sizeof(drm_color_lut) * lut.size(), &blob_id) < 0)
        throw std::runtime_error("Failed to create degamma lut blob");
    
    ADD_PROPERTY(this->crtc.crtc_id, this->crtc.props, "DEGAMMA_LUT", blob_id)
    this->blobs.push_back(blob_id);
}

void PlaneReq::setCrtc(const Crtc& crtc, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "CRTC_ID", crtc.crtc_id)
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "CRTC_X", x)
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "CRTC_Y", y)
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "CRTC_W", w)
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "CRTC_H", h)
}

void PlaneReq::setFb(uint32_t fb_id, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "FB_ID", fb_id)
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "SRC_X", x << 16)
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "SRC_Y", y << 16)
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "SRC_W", w << 16)
    ADD_PROPERTY(this->plane.plane_id, this->plane.props, "SRC_H", h << 16)
}

// main request

AtomicRequest::AtomicRequest(const int fd) : fd(fd) {
    this->req = drmModeAtomicAlloc();
    if (!this->req)
        throw std::runtime_error("Failed to allocate atomic request");
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
    for (const auto& crtc : this->crtcs) {
        for (const uint32_t blob_id : crtc.blobs)
            drmModeDestroyPropertyBlob(this->fd, blob_id);
    }
}
