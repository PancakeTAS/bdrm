#ifndef UTILS_HPP
#define UTILS_HPP

#include <xf86drmMode.h>
#include <gbm.h>

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
    using SP = std::shared_ptr<T>;

    template <typename T>
    using OP = std::optional<T>;

    // convenience classes
    class DeviceNode {

        friend class Bdrm;

        private:
            int fd;
            struct gbm_device* gbm;

            DeviceNode(std::string_view path);

            int get_fd() { return this->fd; }
            struct gbm_device* get_gbm() { return this->gbm; }

        public:
            ~DeviceNode();

    };

    bool supports_crtc(int fd, uint32_t crtc_idx, std::vector<uint32_t> encoder_ids); //!< check if a connector's encoders support a CRTC

}

#endif // UTILS_HPP
