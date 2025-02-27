#include "bdrm.hpp"

#include <drm_mode.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <drm.h>

using namespace BDRM;

Bdrm::Bdrm(std::string_view path) : node(path) {
    int fd = this->node.get_fd();

    if (int ret = drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1))
        throw std::runtime_error("DRM device does not support universal planes (err " + std::to_string(ret) + ")");

    if (int ret = drmSetClientCap(fd, DRM_CLIENT_CAP_ATOMIC, 1))
        throw std::runtime_error("DRM device does not support atomic modesetting (err " + std::to_string(ret) + ")");

    uint64_t cap; // TODO: definitely need check more caps
    if (int ret = drmGetCap(fd, DRM_CAP_ADDFB2_MODIFIERS, &cap) || !cap)
        throw std::runtime_error("DRM device does not support modifiers (err " + std::to_string(ret) + ")");

    if (drmGetCap(fd, DRM_CAP_CURSOR_WIDTH, &this->cursor_width))
        this->cursor_width = 64;
    if (drmGetCap(fd, DRM_CAP_CURSOR_HEIGHT, &this->cursor_height))
        this->cursor_height = 64;

    this->resources = UP<Resources>(new Resources(fd));

    AtomicRequest request = AtomicRequest(fd, *this->resources);
    this->commit(request); // this will reset all properties to their default values
}

AtomicRequest Bdrm::create_atomic_request() {
    return AtomicRequest(this->node.get_fd());
}

void Bdrm::commit(AtomicRequest& request) {
    if (drmModeAtomicCommit(this->node.get_fd(), request.req, DRM_MODE_ATOMIC_ALLOW_MODESET, nullptr) < 0)
        throw std::runtime_error("Failed to commit atomic request");
}
