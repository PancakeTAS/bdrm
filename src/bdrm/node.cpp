#include "bdrm/node.hpp"

#include <stdexcept>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>

using namespace BDRM;

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

