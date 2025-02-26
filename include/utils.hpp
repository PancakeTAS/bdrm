#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <memory>
#include <vector>

// TODO: ponder whether this should be a sub-namespace like BDRM::Utils

namespace BDRM {

    template <typename T>
    using Ref = std::reference_wrapper<T>;

    template <typename T>
    using CRef = Ref<const T>;

    template <typename T>
    using UP = std::unique_ptr<T>;

    template <typename T, typename Compare>
    T* find_first_unique(std::vector<T>& A, const std::vector<T>& B, Compare comp) {
        for (auto& a : A) {
            bool found = std::any_of(B.begin(), B.end(), [&](const T& b) { return comp(a, b); });
            if (!found)
                return &a;
        }
        return nullptr;
    }
    
}

#endif // UTILS_HPP
