#ifndef VECTOR_UTIL_HPP
#define VECTOR_UTIL_HPP
#include <algorithm>
#include <cstddef>
#include <optional>
#include <vector>
namespace GuiBridge {
template <typename T>
void vecRemove(std::vector<T> &vec, T &value) {
    auto newEnd = std::remove(vec.begin(), vec.end(), value);

    vec.erase(newEnd, vec.end());
};

template <typename T>
bool vecInclude(std::vector<T> &vec, const T &value) {
    auto it = std::find(vec.begin(), vec.end(), value);
    return it != vec.end();
}

template <typename T>
void vecAddAll(std::vector<T> &target, const std::vector<T> &source) {
    target.insert(target.end(), source.begin(), source.end());
}

template <typename T, typename Predicate>
std::optional<T> vecFind(const std::vector<T> &items, Predicate predicate) {
    auto it = std::find_if(items.begin(), items.end(), predicate);

    if (it != items.end()) {
        return *it;  // 返回找到的元素
    }
    return std::nullopt;  // 未找到则返回 std::nullopt
}

template <typename T>
int vecIndexOf(std::vector<T> &vec, const T &value) {
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] == value) {
            return i;
        }
    }
    return -1;
}

}  // namespace GuiBridge
#endif