#include "atomic/crtcreq.hpp"

#include <stdexcept>

using namespace BDRM;

#define SET(prop_name, value) \
    { auto prop = this->crtc.props.find(prop_name); \
    if (prop == this->crtc.props.end()) \
        throw std::runtime_error("CRTC has no " prop_name " property"); \
    if (drmModeAtomicAddProperty(req, this->crtc.crtc_id, prop->second, value) < 0) \
        throw std::runtime_error("Failed to add " prop_name " property to atomic request"); }

void CrtcReq::setActive(bool active) {
    SET("ACTIVE", active)
}

void CrtcReq::setMode(const drmModeModeInfo* mode) {
    uint32_t blob_id;
    if (drmModeCreatePropertyBlob(this->fd, mode, sizeof(drmModeModeInfo), &blob_id) < 0)
        throw std::runtime_error("Failed to create mode blob");

    SET("MODE_ID", blob_id)
    this->blobs.push_back(blob_id);
}

void CrtcReq::setVrr(bool vrr) {
    SET("VRR_ENABLED", vrr)
}

void CrtcReq::setGammaLut(const std::vector<drm_color_lut>& lut) {
    uint32_t blob_id;
    if (drmModeCreatePropertyBlob(this->fd, lut.data(), sizeof(drm_color_lut) * lut.size(), &blob_id) < 0)
        throw std::runtime_error("Failed to create gamma lut blob");

    SET("GAMMA_LUT", blob_id)
    this->blobs.push_back(blob_id);
}

void CrtcReq::setCTM(const struct drm_color_ctm* ctm) {
    uint32_t blob_id;
    if (drmModeCreatePropertyBlob(this->fd, ctm, sizeof(drm_color_ctm), &blob_id) < 0)
        throw std::runtime_error("Failed to create ctm blob");

    SET("CTM", blob_id)
    this->blobs.push_back(blob_id);
}

void CrtcReq::setDegammaLut(const std::vector<drm_color_lut>& lut) {
    uint32_t blob_id;
    if (drmModeCreatePropertyBlob(this->fd, lut.data(), sizeof(drm_color_lut) * lut.size(), &blob_id) < 0)
        throw std::runtime_error("Failed to create degamma lut blob");

    SET("DEGAMMA_LUT", blob_id)
    this->blobs.push_back(blob_id);
}

void CrtcReq::clearProperties() {
    SET("ACTIVE", 0)
    SET("MODE_ID", 0)
    SET("VRR_ENABLED", 0)
    SET("GAMMA_LUT", 0)
    SET("CTM", 0)
    SET("DEGAMMA_LUT", 0)
}
