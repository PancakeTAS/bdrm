#include "bdrm.hpp"

#include <drm_mode.h>
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <drm.h>

using namespace BDRM;

Bdrm::Bdrm(std::string_view path) : node(path) {
    int fd = this->node.get_fd();

    // set client capabilities
    if (int ret = drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1))
        throw std::runtime_error("DRM device does not support universal planes (err " + std::to_string(ret) + ")");

    if (int ret = drmSetClientCap(fd, DRM_CLIENT_CAP_ATOMIC, 1))
        throw std::runtime_error("DRM device does not support atomic modesetting (err " + std::to_string(ret) + ")");

    // get cursor capabilities
    if (drmGetCap(fd, DRM_CAP_CURSOR_WIDTH, &this->cursor_width))
        this->cursor_width = 64;
    if (drmGetCap(fd, DRM_CAP_CURSOR_HEIGHT, &this->cursor_height))
        this->cursor_height = 64;

    // fetch all resources
    this->resources = UP<Res>(new Res(fd));

    // zero out all properties
    AtomicRequest request = AtomicRequest(fd, *this->resources);
    this->commit(request);
}

AtomicRequest Bdrm::create_atomic_request() {
    return AtomicRequest(this->node.get_fd());
}

void Bdrm::commit(AtomicRequest& request) {
    if (drmModeAtomicCommit(this->node.get_fd(), request.req, DRM_MODE_ATOMIC_ALLOW_MODESET, nullptr) < 0)
        throw std::runtime_error("Failed to commit atomic request");
}

const Buffer& Bdrm::allocate_framebuffer(uint32_t width, uint32_t height,
    uint32_t format, std::vector<uint64_t> modifiers, bool is_cursor) {

    Buffer* buffer = new Buffer(this->node.get_fd(), this->node.get_gbm(),
        width, height, format, modifiers, is_cursor);
    return *this->buffers.emplace_back(buffer);
}
