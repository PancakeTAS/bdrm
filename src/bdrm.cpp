#include "bdrm.hpp"
#include "utils/gbm.hpp"

#include <drm_mode.h>
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <array>
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
    this->resources = UP<Resources>(new Resources(fd));

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

const Framebuffer& Bdrm::create_framebuffer(uint32_t width, uint32_t height,
    uint32_t format, std::vector<uint64_t> modifiers, bool is_cursor) {

    // allocate a buffer
    struct gbm_bo* bo = allocate_gbm_buffer(node.get_gbm(), width, height, format, modifiers, is_cursor);
    Buffer* bufferb = new Buffer(bo);
    SP<Buffer> buffer = SP<Buffer>(bufferb);

    // create a framebuffer
    int ret = 0;
    uint32_t fb_id;
    if (buffer->modifier.has_value()) {
        std::array<uint64_t, 4> modifiers = { 0, 0, 0, 0 };
        modifiers[0] = buffer->modifier.value();
        ret = drmModeAddFB2WithModifiers(node.get_fd(),
            buffer->width, buffer->height, buffer->format,
            buffer->handle, buffer->stride, buffer->offset,
            modifiers.data(), &fb_id, DRM_MODE_FB_MODIFIERS);
    } else {
        ret = drmModeAddFB2(node.get_fd(),
            buffer->width, buffer->height, buffer->format,
            buffer->handle, buffer->stride, buffer->offset,
            &fb_id, 0);
    }

    if (ret)
        throw std::runtime_error("Failed to create framebuffer: " + std::string(strerror(errno)));

    // get the framebuffer
    drmModeFB2* fb = drmModeGetFB2(node.get_fd(), fb_id);
    const Framebuffer& framebuffer = *this->framebuffers.emplace_back(new Framebuffer(node.get_fd(), fb, buffer));
    drmModeFreeFB2(fb);

    return framebuffer;

}
