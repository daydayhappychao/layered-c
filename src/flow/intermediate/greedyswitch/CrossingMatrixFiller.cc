#include "CrossingMatrixFiller.h"
#include "../../../Node.h"

namespace GuiBridge {
CrossingMatrixFiller::CrossingMatrixFiller(CrossMinType greedySwitchType,
                                           std::vector<std::vector<std::shared_ptr<Node>>> &graph, int freeLayerIndex,
                                           CrossingCountSide direction)
    : direction(direction), oneSided(greedySwitchType == CrossMinType::ONE_SIDED_GREEDY_SWITCH) {
    auto &freeLayer = graph[freeLayerIndex];
    isCrossingMatrixFilled.resize(freeLayer.size(), std::vector<bool>(freeLayer.size(), false));
    crossingMatrix.resize(freeLayer.size(), std::vector<int>(freeLayer.size(), 0));

    inBetweenLayerCrossingCounter = std::make_unique<BetweenLayerEdgeTwoNodeCrossingsCounter>(graph, freeLayerIndex);
}

int CrossingMatrixFiller::getCrossingMatrixEntry(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode) {
    if (!isCrossingMatrixFilled[upperNode->getId()][lowerNode->getId()]) {
        fillCrossingMatrix(upperNode, lowerNode);
        isCrossingMatrixFilled[upperNode->getId()][lowerNode->getId()] = true;
        isCrossingMatrixFilled[lowerNode->getId()][upperNode->getId()] = true;
    }
    return crossingMatrix[upperNode->getId()][lowerNode->getId()];
}

void CrossingMatrixFiller::fillCrossingMatrix(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode) {
    if (oneSided) {
        switch (direction) {
            case CrossingCountSide::EAST:
                inBetweenLayerCrossingCounter->countEasternEdgeCrossings(upperNode, lowerNode);
                break;
            case CrossingCountSide::WEST:
                inBetweenLayerCrossingCounter->countWesternEdgeCrossings(upperNode, lowerNode);
                break;
        }
    } else {
        inBetweenLayerCrossingCounter->countBothSideCrossings(upperNode, lowerNode);
    }

    crossingMatrix[upperNode->getId()][lowerNode->getId()] = inBetweenLayerCrossingCounter->getUpperLowerCrossings();
    crossingMatrix[lowerNode->getId()][upperNode->getId()] = inBetweenLayerCrossingCounter->getLowerUpperCrossings();
}
};  // namespace GuiBridge