#include "resources/connector.hpp"

#include <cstring>
#include <format>

using namespace BDRM;

Connector::Connector(int fd, drmModeConnector* conn) {
    // format name
    const char* type_name = drmModeGetConnectorTypeName(conn->connector_type);
    this->name = std::format("{}-{}",
        type_name ? type_name : "Unknown",
        conn->connector_type_id);

    // copy modes
    this->modes = std::vector<drmModeModeInfo>(static_cast<size_t>(conn->count_modes));
    if (conn->count_modes > 0)
        std::memcpy(this->modes.data(), conn->modes, static_cast<size_t>(conn->count_modes) * sizeof(drmModeModeInfo));

    // copy encoders
    this->encoder_ids = std::vector<uint32_t>(static_cast<size_t>(conn->count_encoders));
    if (conn->count_encoders > 0)
        std::memcpy(this->encoder_ids.data(), conn->encoders, static_cast<size_t>(conn->count_encoders) * sizeof(uint32_t));

    // copy basic properties
    this->connected = conn->connection == DRM_MODE_CONNECTED;
    this->subpixelLayout = conn->subpixel;
    this->conn_id = conn->connector_id;
    this->width = conn->mmWidth;
    this->height = conn->mmHeight;

    // browse properties
    for (int i = 0; i < conn->count_props; ++i) {
        drmModePropertyRes* prop = drmModeGetProperty(fd, conn->props[i]);
        if (prop == nullptr) continue;

        // store ids for later
        this->props[prop->name] = prop->prop_id;

        // store values if needed
        if (strcmp(prop->name, "non-desktop") == 0) {
            this->non_desktop = conn->prop_values[i];
        } else if (strcmp(prop->name, "vrr_capable") == 0) {
            this->vrr_capable = conn->prop_values[i];
        } else if (strcmp(prop->name, "EDID") == 0) {
            // parse EDID
            drmModePropertyBlobRes* blob = drmModeGetPropertyBlob(fd, static_cast<uint32_t>(conn->prop_values[i]));
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

            // copy required fields
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
