#include "ISweepPortDistributor.h"
#include "GreedyPortDistributor.h"
#include "LayerTotalPortDistributor.h"
#include "NodeRelativePortDistributor.h"

namespace GuiBridge {
std::shared_ptr<ISweepPortDistributor> sweepPortDistributorCreate(
    CrossMinType cmt, std::mt19937 &r, std::vector<std::vector<std::shared_ptr<Node>>> &currentOrder) {
    if (cmt == CrossMinType::TWO_SIDED_GREEDY_SWITCH) {
        std::shared_ptr<ISweepPortDistributor> ptr = std::make_shared<GreedyPortDistributor>();
        return ptr;
    }
    std::uniform_int_distribution<int> distribution(0, 1);

    if (distribution(r) == 1) {
        // 随机选择 NodeRelativePortDistributor
        return std::make_shared<NodeRelativePortDistributor>(currentOrder.size());
    }
    // 随机选择 LayerTotalPortDistributor
    return std::make_shared<LayerTotalPortDistributor>(currentOrder.size());
};
}  // namespace GuiBridge