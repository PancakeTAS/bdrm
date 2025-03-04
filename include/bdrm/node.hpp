#ifndef NODE_HPP
#define NODE_HPP

#include <gbm.h>

#include <string_view>

// forward decl
namespace BDRM { class Bdrm; }

namespace BDRM {

    // convenience classes
    class DeviceNode {

        friend class BDRM::Bdrm;

        private:
            int fd;
            struct gbm_device* gbm;

            DeviceNode(std::string_view path);

            int get_fd() { return this->fd; }
            struct gbm_device* get_gbm() { return this->gbm; }

        public:
            ~DeviceNode();

    };

}

#endif // NODE_HPP
