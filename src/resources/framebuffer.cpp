#include "resources/framebuffer.hpp"

#include <drm_fourcc.h>
#include <xf86drmMode.h>

using namespace BDRM::Resources;

Framebuffer::Framebuffer(int fd, drmModeFB2* fb, SP<Buffer> buffer) : fd(fd) {
    this->fb_id = fb->fb_id;
    this->buffer = buffer;

    this->width = fb->width;
    this->height = fb->height;
    this->format = fb->pixel_format;
    this->modifier = fb->modifier;
    if (this->modifier == DRM_FORMAT_MOD_INVALID)
        this->modifier = std::nullopt;
    this->flags = fb->flags;

    for (int i = 0; i < 4; i++) {
        this->handles[i] = fb->handles[i];
        this->pitches[i] = fb->pitches[i];
        this->offset[i] = fb->offsets[i];
    }
}

Framebuffer::~Framebuffer() {
    drmModeRmFB(fd, this->fb_id);
}
