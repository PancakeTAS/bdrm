#include "bdrm.hpp"
#include "atomic.hpp"
#include "drm_mode.h"
#include "resources/connector.hpp"
#include "resources/crtc.hpp"
#include "resources/plane.hpp"
#include "utils.hpp"

#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <errno.h>
#include <unistd.h>
#include <xf86drm.h>
#include <drm.h>
#include <xf86drmMode.h>

using namespace BDRM;

// node

DeviceNode::DeviceNode(std::string_view path) {
    this->fd = open(path.data(), O_RDWR | O_CLOEXEC);
    if (this->fd < 0)
        throw std::runtime_error("Failed to open device: " + std::string(strerror(errno)));
}

DeviceNode::~DeviceNode() {
    close(this->fd);
}

// init

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

        this->connectors.emplace_back(UP<Connector>(new Connector(fd, conn)));
        drmModeFreeConnector(conn);
    }

    for (int i = 0; i < res->count_crtcs; i++) {
        drmModeCrtc* crtc = drmModeGetCrtc(fd, res->crtcs[i]);
        if (crtc == nullptr) continue;

        this->crtcs.emplace_back(UP<Crtc>(new Crtc(fd, crtc)));
        drmModeFreeCrtc(crtc);
    }
    
    drmModeFreeResources(res);

    drmModePlaneRes* plane_res = drmModeGetPlaneResources(fd);
    if (plane_res == nullptr)
        throw std::runtime_error("Failed to get drm plane resources");

    for (uint32_t i = 0; i < plane_res->count_planes; i++) {
        drmModePlane* plane = drmModeGetPlane(fd, plane_res->planes[i]);
        if (plane == nullptr) continue;

        this->planes.emplace_back(UP<Plane>(new Plane(fd, plane)));
        drmModeFreePlane(plane);
    }

    drmModeFreePlaneResources(plane_res);

    // build reset atomic request

    AtomicRequest request = this->create_atomic_request();
    
    for (CRef<Crtc> crtc : this->get_all_crtcs()) {
        CrtcReq req = request.addCrtc(crtc);
        req.clearProperties();
    }

    for (CRef<Connector> conn : this->get_all_connectors()) {
        ConnectorReq req = request.addConnector(conn);
        req.clearProperties();
    }

    for (CRef<Plane> plane : this->get_all_planes()) {
        PlaneReq req = request.addPlane(plane);
        req.clearProperties();
    }

    this->commit(request);
}

/*const std::vector<CRef<Crtc>> Bdrm::suitable_crtcs(CRef<Connector> connector) {
    auto supported_crtcs = std::vector<CRef<Crtc>>();
    auto all_crtcs = this->get_all_crtcs();

    for (uint32_t encoder_id : connector.get().encoder_ids) {
        drmModeEncoder* encoder = drmModeGetEncoder(this->node.get_fd(), encoder_id);
        if (encoder == nullptr) continue;

        std::bitset<32> possible_crtcs(encoder->possible_crtcs);
        for (size_t i = 0; i < all_crtcs.size(); i++) {
            if (!possible_crtcs[i])
                continue;

            supported_crtcs.push_back(all_crtcs[i]);
        }
    }

    return supported_crtcs;
}*/

// filters

const std::vector<CRef<Connector>> Bdrm::get_all_connectors() {
    std::vector<CRef<Connector>> connectors;
    for (UP<Connector>& conn : this->connectors)
        connectors.push_back(*conn);
    return connectors;
}

const std::vector<CRef<Crtc>> Bdrm::get_all_crtcs() {
    std::vector<CRef<Crtc>> crtcs;
    for (UP<Crtc>& crtc : this->crtcs)
        crtcs.push_back(*crtc);
    return crtcs;
}

const std::vector<CRef<Plane>> Bdrm::get_all_planes() {
    std::vector<CRef<Plane>> planes;
    for (UP<Plane>& plane : this->planes)
        planes.push_back(*plane);
    return planes;
}

// atomic requests

AtomicRequest Bdrm::create_atomic_request() {
    return AtomicRequest(this->node.get_fd());
}

void Bdrm::commit(AtomicRequest& request) {
    if (drmModeAtomicCommit(this->node.get_fd(), request.req, DRM_MODE_ATOMIC_ALLOW_MODESET, nullptr) < 0)
        throw std::runtime_error("Failed to commit atomic request");
}
