#ifndef UTILS_HPP
#define UTILS_HPP

#include <memory>

// TODO: ponder whether this should be a sub-namespace like BDRM::Utils


namespace BDRM {

    template <typename T>
    using Ref = std::reference_wrapper<T>;

    template <typename T>
    using CRef = Ref<const T>;

    template <typename T>
    using UP = std::unique_ptr<T>;

}

#endif // UTILS_HPP
