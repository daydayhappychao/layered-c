#pragma once

#include <random>
namespace GuiBridge {
class RandomUtil {
public:
    RandomUtil() : rng(std::random_device{}()){};
    bool randomBoolean() {
        std::uniform_int_distribution<int> distribution(0, 1);
        return distribution(rng) == 1;  // 生成 0 或 1
    }
    float rangeFloat(float start, float end) {
        std::uniform_real_distribution<> dist(start, end);
        double random_number = dist(rng);
        return random_number;
    }
    int rangeInt(int start, int end) {
        std::uniform_real_distribution<> dist(start, end);
        int random_number = dist(rng);
        return random_number;
    }

private:
    std::mt19937 rng;
};
extern std::unique_ptr<RandomUtil> randomUtil;

}  // namespace GuiBridge
