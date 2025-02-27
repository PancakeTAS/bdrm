#include "resources/crtc.hpp"

#include <cstring>

using namespace BDRM;

Crtc::Crtc(int fd, drmModeCrtc* crtc) {
    this->crtc_id = crtc->crtc_id;

    drmModeObjectProperties* props = drmModeObjectGetProperties(fd, crtc->crtc_id, DRM_MODE_OBJECT_CRTC);
    for (uint32_t i = 0; i < props->count_props; ++i) {
        drmModePropertyRes* prop = drmModeGetProperty(fd, props->props[i]);
        if (prop == nullptr) continue;
    
        this->props[prop->name] = prop->prop_id;

        if (strcmp(prop->name, "GAMMA_LUT_SIZE") == 0) {
            this->gamma_lut_size = props->prop_values[i];
        } else if (strcmp(prop->name, "DEGAMMA_LUT_SIZE") == 0) {
            this->degamma_lut_size = props->prop_values[i];
        } else if (strcmp(prop->name, "CTM") == 0) {
            this->ctm = true;
        } else if (strcmp(prop->name, "OUT_FENCE_PTR") == 0) {
            this->explicit_sync = true; // FIXME: no idea how explicit sync works yet, i think it's a capability
        }

        drmModeFreeProperty(prop);
    }

    drmModeFreeObjectProperties(props);
}
