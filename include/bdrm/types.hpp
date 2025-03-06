#ifndef TYPES_HPP
#define TYPES_HPP

#include <memory>
#include <optional>

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

}

#endif // TYPES_HPP
