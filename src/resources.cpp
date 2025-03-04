#include "resources.hpp"

using namespace BDRM::Resources;

Res::Res(int fd) : fd(fd) {
    drmModeRes* res = drmModeGetResources(fd);
    if (res == nullptr)
        throw std::runtime_error("Failed to get drm resources");

    // create connectors
    for (int i = 0; i < res->count_connectors; i++) {
        drmModeConnector* conn = drmModeGetConnector(fd, res->connectors[i]);
        if (conn == nullptr) continue;

        this->connectors.emplace_back(Connector(fd, conn));
        drmModeFreeConnector(conn);
    }

    // create crtcs
    for (int i = 0; i < res->count_crtcs; i++) {
        drmModeCrtc* crtc = drmModeGetCrtc(fd, res->crtcs[i]);
        if (crtc == nullptr) continue;

        this->crtcs.emplace_back(Crtc(fd, crtc));
        drmModeFreeCrtc(crtc);
    }

    drmModeFreeResources(res);

    drmModePlaneRes* plane_res = drmModeGetPlaneResources(fd);
    if (plane_res == nullptr)
        throw std::runtime_error("Failed to get drm plane resources");

    // create planes
    for (uint32_t i = 0; i < plane_res->count_planes; i++) {
        drmModePlane* plane = drmModeGetPlane(fd, plane_res->planes[i]);
        if (plane == nullptr) continue;

        this->planes.emplace_back(Plane(fd, plane));
        drmModeFreePlane(plane);
    }

    drmModeFreePlaneResources(plane_res);
}

const std::vector<CRef<Connector>> Res::search_connectors(ConnectorQueryArgs args) const {
    std::vector<CRef<Connector>> found_connectors;
    for (const Connector& conn : this->connectors) {
        if (args.name.has_value() && conn.name != args.name.value())
            continue;
        if (args.connected.has_value() && conn.connected != args.connected.value())
            continue;
        if (args.subpixel_layout.has_value() && conn.subpixelLayout != args.subpixel_layout.value())
            continue;
        if (args.non_desktop.has_value() && conn.non_desktop != args.non_desktop.value())
            continue;
        if (args.vrr_capable.has_value() && conn.vrr_capable != args.vrr_capable.value())
            continue;
        if (args.make.has_value() && conn.make != args.make.value())
            continue;
        if (args.model.has_value() && conn.model != args.model.value())
            continue;
        if (args.serial.has_value() && conn.serial != args.serial.value())
            continue;

        found_connectors.push_back(conn);
    }

    return found_connectors;
}

const std::vector<drmModeModeInfo> Res::search_modes(ModeQueryArgs args) const {
    std::vector<drmModeModeInfo> found_modes;
    for (const Connector& conn : this->connectors) {
        for (const drmModeModeInfo& mode : conn.modes) {
            if (args.width.has_value() && mode.hdisplay != args.width.value())
                continue;
            if (args.height.has_value() && mode.vdisplay != args.height.value())
                continue;
            if (args.min_refresh.has_value() && mode.vrefresh < args.min_refresh.value())
                continue;
            if (args.max_refresh.has_value() && mode.vrefresh > args.max_refresh.value())
                continue;

            found_modes.push_back(mode);
        }
    }

    return found_modes;
}

const std::vector<CRef<Crtc>> Res::search_crtcs(CrtcQueryArgs args) const {
    std::vector<CRef<Crtc>> found_crtcs;
    for (uint32_t i = 0; i < this->crtcs.size(); i++) {
        const Crtc& crtc = this->crtcs[i];

        if (args.connector.has_value() && !supports_crtc(this->fd, i, args.connector->get().encoder_ids))
            continue;
        if (args.gamma_lut.has_value() && crtc.gamma_lut_size.has_value() != args.gamma_lut.value())
            continue;
        if (args.degamma_lut.has_value() && crtc.degamma_lut_size.has_value() != args.degamma_lut.value())
            continue;
        if (args.ctm.has_value() && crtc.ctm != args.ctm.value())
            continue;
        if (args.explicit_sync.has_value() && crtc.explicit_sync != args.explicit_sync.value())
            continue;

        found_crtcs.push_back(crtc);
    }

    return found_crtcs;
}

const std::vector<CRef<Plane>> Res::search_planes(PlaneQueryArgs args) const {
    // find the CRTC index for later
    std::optional<uint32_t> crtc_idx = std::nullopt;
    if (args.crtc.has_value()) {
        for (size_t i = 0; i < this->crtcs.size(); i++) {
            if (this->crtcs[i].crtc_id != args.crtc->get().crtc_id)
                continue;

            crtc_idx = i;
            break;
        }

        if (!crtc_idx.has_value())
            throw std::invalid_argument("Invalid CRTC argument");
    }

    std::vector<CRef<Plane>> found_planes;
    for (const Plane& plane : this->planes) {
        if (args.crtc.has_value() && !plane.possible_crtcs[*crtc_idx])
            continue;
        if (args.type.has_value() && plane.type != args.type.value())
            continue;
        if (args.format.has_value()) {
            // try to find the requested format
            auto fmt = std::ranges::find_if(plane.supported_formats, [&](const PlaneFormat& pf) {
                return pf.format == args.format.value();
            });

            if (fmt == plane.supported_formats.end())
                continue;

            // .. and the modifier
            if (args.modifier && std::ranges::find(fmt->modifiers, args.modifier.value()) == fmt->modifiers.end())
                continue;
        }

        found_planes.push_back(plane);
    }

    return found_planes;
}
