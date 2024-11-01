#ifndef CROSSING_MATRIX_FILLER_H
#define CROSSING_MATRIX_FILLER_H

#include <memory>
#include <vector>
#include "../../../opts/CrossMinType.h"
#include "./SwitchDecider.h"
#include "BetweenLayerEdgeTwoNodeCrossingsCounter.h"

namespace GuiBridge {

class Node;

class CrossingMatrixFiller {
public:
    CrossingMatrixFiller(CrossMinType greedySwitchType, std::vector<std::vector<std::shared_ptr<Node>>> &graph,
                         int freeLayerIndex, CrossingCountSide direction);

    int getCrossingMatrixEntry(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode);

private:
    void fillCrossingMatrix(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode);

    std::vector<std::vector<bool>> isCrossingMatrixFilled;
    std::vector<std::vector<int>> crossingMatrix;
    std::unique_ptr<BetweenLayerEdgeTwoNodeCrossingsCounter> inBetweenLayerCrossingCounter;
    CrossingCountSide direction;
    bool oneSided;
};
}  // namespace GuiBridge

#endif  // CROSSING_MATRIX_FILLER_H