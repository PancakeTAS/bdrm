#include "resources/plane.hpp"

#include <cstring>

using namespace BDRM;

Plane::Plane(int fd, drmModePlane* plane) {
    this->plane_id = plane->plane_id;
    this->possible_crtcs = plane->possible_crtcs;

    drmModeObjectProperties* props = drmModeObjectGetProperties(fd, plane->plane_id, DRM_MODE_OBJECT_PLANE);
    for (uint32_t i = 0; i < props->count_props; ++i) {
        drmModePropertyRes* prop = drmModeGetProperty(fd, props->props[i]);
        if (prop == nullptr) continue;

        this->props[prop->name] = prop->prop_id;

        if (strcmp(prop->name, "type") == 0) {
            this->type = static_cast<PlaneType>(props->prop_values[i]);
        } else if (strcmp(prop->name, "IN_FORMATS") == 0) {
            drmModePropertyBlobRes* blob = drmModeGetPropertyBlob(fd, static_cast<uint32_t>(props->prop_values[i]));
            if (blob == nullptr) {
                drmModeFreeProperty(prop);
                continue;
            }

            std::unordered_map<uint32_t, std::vector<uint64_t>> formats;

            drmModeFormatModifierIterator iter = {};
            while (drmModeFormatModifierBlobIterNext(blob, &iter)) {
                formats[iter.fmt].push_back(iter.mod);
            }

            this->supported_formats.reserve(formats.size());
            for (const auto& [format, modifiers] : formats) {
                this->supported_formats.push_back({ .format = format, .modifiers = modifiers });
            }

            drmModeFreePropertyBlob(blob);
        }

        drmModeFreeProperty(prop);
    }

    drmModeFreeObjectProperties(props);
}
