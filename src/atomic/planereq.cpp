#include "atomic/planereq.hpp"

#include <stdexcept>

using namespace BDRM;

#define SET(prop_name, value) \
    { auto prop = this->plane.props.find(prop_name); \
    if (prop == this->plane.props.end()) \
        throw std::runtime_error("Plane has no " prop_name " property"); \
    if (drmModeAtomicAddProperty(req, this->plane.plane_id, prop->second, value) < 0) \
        throw std::runtime_error("Failed to add " prop_name " property to atomic request"); }

void PlaneReq::setCrtc(const Crtc& crtc, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    SET("CRTC_ID", crtc.crtc_id)
    SET("CRTC_X", x)
    SET("CRTC_Y", y)
    SET("CRTC_W", w)
    SET("CRTC_H", h)
}

void PlaneReq::setFb(uint32_t fb_id, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    SET("FB_ID", fb_id)
    SET("SRC_X", x << 16)
    SET("SRC_Y", y << 16)
    SET("SRC_W", w << 16)
    SET("SRC_H", h << 16)
}

void PlaneReq::clearProperties() {
    SET("CRTC_ID", 0)
    SET("FB_ID", 0)
    SET("SRC_X", 0)
    SET("SRC_Y", 0)
    SET("SRC_W", 0)
    SET("SRC_H", 0)
    SET("CRTC_X", 0)
    SET("CRTC_Y", 0)
    SET("CRTC_W", 0)
    SET("CRTC_H", 0)
}
