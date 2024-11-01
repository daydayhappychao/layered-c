#ifndef CREATE_SWEEP_PORT_DISTRIBUTOR_H
#define CREATE_SWEEP_PORT_DISTRIBUTOR_H

#include <memory>
#include <random>
#include <vector>
#include "../../Node.h"
#include "../../opts/CrossMinType.h"
#include "GreedyPortDistributor.h"
#include "ISweepPortDistributor.h"
#include "LayerTotalPortDistributor.h"
#include "NodeRelativePortDistributor.h"

namespace GuiBridge {
class Node;

std::shared_ptr<ISweepPortDistributor> createSweepPortDistributor(
    CrossMinType cmt, std::mt19937 &rng, const std::vector<std::vector<std::shared_ptr<Node>>> &currentOrder) {
    if (cmt == CrossMinType::TWO_SIDED_GREEDY_SWITCH) {
        return std::make_shared<GreedyPortDistributor>();
    }
    if (rng() % 2 == 0) {  // 随机选择
        return std::make_shared<NodeRelativePortDistributor>(currentOrder.size());
    }
    return std::make_shared<LayerTotalPortDistributor>(currentOrder.size());
}

}  // namespace GuiBridge

#endif  // ISWEEP_PORT_DISTRIBUTOR_H