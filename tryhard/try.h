#pragma once

#include <utility>

namespace tag {

inline constexpr struct Exception {
} kEx;

}  // namespace tag

template <class T>
class Try;

template <>
class Try<void>;

template <class Function, class... Args>
auto TryRun(Function func, Args&&... args) {
    using ReturnType = decltype(func(std::forward<Args>(args)...));
}
