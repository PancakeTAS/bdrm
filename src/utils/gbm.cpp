#include "utils/gbm.hpp"

#include <libdrm/drm_fourcc.h>

#include <stdexcept>

using namespace BDRM;

struct gbm_bo* BDRM::allocate_gbm_buffer(
    struct gbm_device* gbm, uint32_t width, uint32_t height,
    uint32_t format, std::vector<uint64_t> modifiers, bool is_cursor
) {
    // check if linear is supported
    bool supports_linear = false;
    std::vector<uint64_t> modifiers_no_linear;
    for (auto& modifier : modifiers) {
        if (modifier == DRM_FORMAT_MOD_LINEAR) {
            supports_linear = true;
            continue;
        }

        modifiers_no_linear.push_back(modifier);
    }

    struct gbm_bo* buf = nullptr;

    // there's multiple ways to allocate a buffer, some are more likely to work than others.
    // first we try to create a buffer using all modifiers passed to us, except for linear.
    if (modifiers_no_linear.size() >= 1) {
        buf = gbm_bo_create_with_modifiers2(gbm,
            width, height, format,
            modifiers.data(), static_cast<unsigned int>(modifiers.size()),
            GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }

    // this first one is most likely to be skipped on cursor planes, as they're usually linear only.
    // so if the list of supported modifiers contained linear, we try to create a buffer with that.
    if (buf == nullptr && supports_linear) {
        uint64_t linear[1] = { DRM_FORMAT_MOD_LINEAR };
        buf = gbm_bo_create_with_modifiers2(gbm,
            width, height, format,
            linear, 1,
            GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }

    // if that still failed, we can create a buffer without any usage flags, because for some
    // reason those can be used as cursor planes, though we don't bother with non-cursor planes.
    if (buf == nullptr && is_cursor && supports_linear) {
        uint64_t linear[1] = { DRM_FORMAT_MOD_LINEAR };
        buf = gbm_bo_create_with_modifiers(gbm,
            width, height, format,
            linear, 1);
    }

    // if we still don't have a buffer, we're most likely screwed, but just in case
    // we try creating a buffer without any modifiers or usage flags using the basic
    // gbm_bo_create function.
    if (buf == nullptr) {
        buf = gbm_bo_create(gbm,
            width, height, format,
            GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }

    if (buf == nullptr) {
        throw std::runtime_error("Failed to create framebuffer");
    }

    return buf;
}

