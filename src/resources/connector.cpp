#include "resources/connector.hpp"
#include <cstring>
#include <format>

#include <xf86drmMode.h>

extern "C" {
#include <libdisplay-info/info.h>
}

using namespace BDRM;

// TODO: ponder whether using "continue" in case of a property failure is the best course of action

Connector::Connector(int fd, drmModeConnector* conn) {
    const char* type_name = drmModeGetConnectorTypeName(conn->connector_type);
    this->name = std::format("{}-{}",
        type_name ? type_name : "Unknown",
        conn->connector_type_id);

    this->modes = std::vector<drmModeModeInfo>(conn->count_modes);
    if (conn->count_modes > 0)
        std::memcpy(this->modes.data(), conn->modes, conn->count_modes * sizeof(drmModeModeInfo));

    this->connected = conn->connection == DRM_MODE_CONNECTED;
    this->subpixelLayout = conn->subpixel;
    this->conn_id = conn->connector_id;
    this->width = conn->mmWidth;
    this->height = conn->mmHeight;

    for (int i = 0; i < conn->count_props; ++i) {
        drmModePropertyRes* prop = drmModeGetProperty(fd, conn->props[i]);
        if (prop == nullptr) continue;
    
        if (strcmp(prop->name, "non-desktop") == 0) {
            this->non_desktop = conn->prop_values[i];
        } else if (strcmp(prop->name, "vrr_capable") == 0) {
            this->vrr_capable = conn->prop_values[i];
        } else if (strcmp(prop->name, "EDID") == 0) {
            drmModePropertyBlobRes* blob = drmModeGetPropertyBlob(fd, conn->prop_values[i]);
            if (blob == nullptr) {
                drmModeFreeProperty(prop);
                continue;
            }

            struct di_info* edid = di_info_parse_edid(blob->data, blob->length);
            if (!edid) {
                drmModeFreePropertyBlob(blob);
                drmModeFreeProperty(prop);
                continue;
            }

            if (char* ptr = di_info_get_make(edid)) {
                this->make = ptr;
                free(ptr);
            }
            if (char* ptr = di_info_get_serial(edid)) {
                this->serial = ptr;
                free(ptr);
            }
            if (char* ptr = di_info_get_model(edid)) {
                this->model = ptr;
                free(ptr);
            
            }

            if (const di_color_primaries* ptr = di_info_get_default_color_primaries(edid))
                this->color_primaries.emplace(*ptr);
            if (const di_hdr_static_metadata* ptr = di_info_get_hdr_static_metadata(edid))
                this->hdr_metadata.emplace(*ptr);

            di_info_destroy(edid);
            drmModeFreePropertyBlob(blob);
        }

        drmModeFreeProperty(prop);
    }
}
