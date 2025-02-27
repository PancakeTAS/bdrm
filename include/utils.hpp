#ifndef UTILS_HPP
#define UTILS_HPP

#include <xf86drmMode.h>

#include <string_view>
#include <memory>
#include <optional>
#include <vector>

namespace BDRM {

    // convenience types
    template <typename T>
    using Ref = std::reference_wrapper<T>;

    template <typename T>
    using CRef = Ref<const T>;

    template <typename T>
    using UP = std::unique_ptr<T>;

    template <typename T>
    using OP = std::optional<T>;

    // convenience classes
    class DeviceNode {
        
        friend class Bdrm;
        
        private:
            int fd;
            
            DeviceNode(std::string_view path);
            ~DeviceNode();

            int get_fd() { return this->fd; }

    };

    bool supports_crtc(int fd, uint32_t crtc_idx, std::vector<uint32_t> encoder_ids);

}

#endif // UTILS_HPP
