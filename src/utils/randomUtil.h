#ifndef RANDOM_UTIL_HPP
#define RANDOM_UTIL_HPP

#include <random>
namespace GuiBridge {
class RandomUtil {
public:
    RandomUtil() : rng(std::random_device{}()){};
    bool randomBoolean() {
        std::uniform_int_distribution<int> distribution(0, 1);
        return distribution(rng) == 1;  // 生成 0 或 1
    }

private:
    std::mt19937 rng;
};
}  // namespace GuiBridge

auto randomUtil = new GuiBridge::RandomUtil();
#endif