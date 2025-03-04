#include "resources/buffer.hpp"

#include <gbm.h>
#include <drm_fourcc.h>

using namespace BDRM::Resources;

Buffer::Buffer(struct gbm_bo* bo) {
    this->bo = bo;

    this->width = gbm_bo_get_width(bo);
    this->height = gbm_bo_get_height(bo);
    this->format = gbm_bo_get_format(bo);
    this->bpp = gbm_bo_get_bpp(bo);
    this->modifier = gbm_bo_get_modifier(bo);
    if (this->modifier == DRM_FORMAT_MOD_INVALID)
        this->modifier = std::nullopt;

    int num_planes = gbm_bo_get_plane_count(bo);
    for (int i = 0; i < num_planes; i++) {
        this->stride[i] = gbm_bo_get_stride_for_plane(bo, i);
        this->offset[i] = gbm_bo_get_offset(bo, i);
        this->handle[i] = gbm_bo_get_handle_for_plane(bo, i).u32;
    }
}

Buffer::~Buffer() {
    gbm_bo_destroy(this->bo);
}
