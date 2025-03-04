#include "utils.hpp"

#include <fcntl.h>
#include <string.h>

#include <string>
#include <stdexcept>
#include <bitset>

using namespace BDRM::Utils;

DeviceNode::DeviceNode(std::string_view path) {
    this->fd = open(path.data(), O_RDWR | O_CLOEXEC);
    if (this->fd < 0)
        throw std::runtime_error("Failed to open device: " + std::string(strerror(errno)));

    this->gbm = gbm_create_device(this->fd);
    if (this->gbm == nullptr) {
        close(this->fd);
        throw std::runtime_error("Failed to create GBM device: " + std::string(strerror(errno)));
    }
}

DeviceNode::~DeviceNode() {
    gbm_device_destroy(this->gbm);
    close(this->fd);
}

bool BDRM::Utils::supports_crtc(int fd, uint32_t crtc_idx, std::vector<uint32_t> encoder_ids) {
    for (uint32_t encoder_id : encoder_ids) {
        drmModeEncoder* encoder = drmModeGetEncoder(fd, encoder_id);
        if (encoder == nullptr) continue;

        // check if bit at index is set in possible_crtcs
        std::bitset<32> possible_crtcs(encoder->possible_crtcs);
        if (possible_crtcs[crtc_idx]) {
            drmModeFreeEncoder(encoder);
            return true;
        }

        drmModeFreeEncoder(encoder);
    }

    return false;
}
