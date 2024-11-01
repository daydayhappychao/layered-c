#ifndef VECTOR_UTIL_HPP
#define VECTOR_UTIL_HPP
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
}  // namespace GuiBridge
#endif