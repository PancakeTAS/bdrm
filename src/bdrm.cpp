#include "bdrm.hpp"

#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <errno.h>
#include <unistd.h>
#include <xf86drm.h>
#include <drm.h>

using namespace BDRM;

DeviceNode::DeviceNode(std::string_view path) {
    this->fd = open(path.data(), O_RDWR | O_CLOEXEC);
    if (this->fd < 0)
        throw std::runtime_error("Failed to open device: " + std::string(strerror(errno)));
}

DeviceNode::~DeviceNode() {
    close(this->fd);
}

Bdrm::Bdrm(std::string_view path) : node(path) {
    int fd = this->node.get_fd();

    // capabilities

    if (int ret = drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1))
        throw std::runtime_error("DRM device does not support universal planes (err " + std::to_string(ret) + ")");

    if (int ret = drmSetClientCap(fd, DRM_CLIENT_CAP_ATOMIC, 1))
        throw std::runtime_error("DRM device does not support atomic modesetting (err " + std::to_string(ret) + ")");

    uint64_t cap;
    if (int ret = drmGetCap(fd, DRM_CAP_ADDFB2_MODIFIERS, &cap) || !cap)
        throw std::runtime_error("DRM device does not support modifiers (err " + std::to_string(ret) + ")");

    // FIXME: find a suitable fallback value
    if (drmGetCap(fd, DRM_CAP_CURSOR_WIDTH, &this->cursor_width))
        this->cursor_width = 64;
    if (drmGetCap(fd, DRM_CAP_CURSOR_HEIGHT, &this->cursor_height))
        this->cursor_height = 64;

    // resources

    drmModeRes* res = drmModeGetResources(fd);
    if (res == nullptr)
        throw std::runtime_error("Failed to get drm resources");

    for (int i = 0; i < res->count_connectors; i++) {
        drmModeConnector* conn = drmModeGetConnector(fd, res->connectors[i]);
        if (conn == nullptr) continue;

        this->connectors.emplace_back(Connector(fd, conn));
        drmModeFreeConnector(conn);
    }

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

    for (int i = 0; i < plane_res->count_planes; i++) {
        drmModePlane* plane = drmModeGetPlane(fd, plane_res->planes[i]);
        if (plane == nullptr) continue;

        this->planes.emplace_back(Plane(fd, plane));
        drmModeFreePlane(plane);
    }
}
