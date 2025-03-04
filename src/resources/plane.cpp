#include "resources/plane.hpp"

#include <cstring>

using namespace BDRM::Resources;

Plane::Plane(int fd, drmModePlane* plane) {
    // copy basic properties
    this->plane_id = plane->plane_id;
    this->possible_crtcs = plane->possible_crtcs;

    // browse properties
    drmModeObjectProperties* props = drmModeObjectGetProperties(fd, plane->plane_id, DRM_MODE_OBJECT_PLANE);
    for (uint32_t i = 0; i < props->count_props; ++i) {
        drmModePropertyRes* prop = drmModeGetProperty(fd, props->props[i]);
        if (prop == nullptr) continue;

        // store ids for later
        this->props[prop->name] = prop->prop_id;

        // store values if needed
        if (strcmp(prop->name, "type") == 0) {
            this->type = static_cast<PlaneType>(props->prop_values[i]);
        } else if (strcmp(prop->name, "IN_FORMATS") == 0) {
            // parse formats
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

            // copy into struct
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
