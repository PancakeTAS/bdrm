#include "utils.hpp"

#include <fcntl.h>
#include <string.h>

#include <string>
#include <stdexcept>
#include <bitset>

using namespace BDRM;

DeviceNode::DeviceNode(std::string_view path) {
    this->fd = open(path.data(), O_RDWR | O_CLOEXEC);
    if (this->fd < 0)
        throw std::runtime_error("Failed to open device: " + std::string(strerror(errno)));
}

DeviceNode::~DeviceNode() {
    close(this->fd);
}

bool BDRM::supports_crtc(int fd, uint32_t crtc_idx, std::vector<uint32_t> encoder_ids) {
    for (uint32_t encoder_id : encoder_ids) {
        drmModeEncoder* encoder = drmModeGetEncoder(fd, encoder_id);
        if (encoder == nullptr) continue;

        std::bitset<32> possible_crtcs(encoder->possible_crtcs);
        if (possible_crtcs[crtc_idx]) {
            drmModeFreeEncoder(encoder);
            return true;
        }

        drmModeFreeEncoder(encoder);
    }

    return false;
}
