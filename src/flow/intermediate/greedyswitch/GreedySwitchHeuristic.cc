#include "GreedySwitchHeuristic.h"
#include <memory>
#include "../../../Layer.h"
#include "CrossingMatrixFiller.h"
#include "SwitchDecider.h"

namespace GuiBridge {
GreedySwitchHeuristic::GreedySwitchHeuristic(CrossMinType greedyType, std::shared_ptr<GraphInfoHolder> &graphData)
    : graphData(graphData), greedySwitchType(greedyType) {}

bool GreedySwitchHeuristic::minimizeCrossings(std::vector<std::vector<std::shared_ptr<Node>>> &order,
                                              int freeLayerIndex, bool forwardSweep, bool isFirstSweep) {
    setUp(order, freeLayerIndex, forwardSweep);
    return continueSwitchingUntilNoImprovementInLayer(freeLayerIndex);
}

bool GreedySwitchHeuristic::setFirstLayerOrder(std::vector<std::vector<std::shared_ptr<Node>>> &order,
                                               bool forwardSweep) {
    auto startIndex_ = startIndex(forwardSweep, currentNodeOrder.size());
    setUp(currentNodeOrder, startIndex_, forwardSweep);
    return sweepDownwardInLayer(startIndex_);
}

void GreedySwitchHeuristic::setUp(std::vector<std::vector<std::shared_ptr<Node>>> &order, int freeLayerIndex,
                                  bool forwardSweep) {
    currentNodeOrder = order;
    CrossingCountSide side = forwardSweep ? CrossingCountSide::WEST : CrossingCountSide::EAST;
    switchDecider = getNewSwitchDecider(freeLayerIndex, side);
}
std::unique_ptr<SwitchDecider> GreedySwitchHeuristic::getNewSwitchDecider(int freeLayerIndex, CrossingCountSide side) {
    auto crossingMatrixFiller =
        std::make_shared<CrossingMatrixFiller>(greedySwitchType, currentNodeOrder, freeLayerIndex, side);
    return std::make_unique<SwitchDecider>(freeLayerIndex, currentNodeOrder, crossingMatrixFiller, portPositions,
                                           graphData, greedySwitchType == CrossMinType::ONE_SIDED_GREEDY_SWITCH);
}

bool GreedySwitchHeuristic::continueSwitchingUntilNoImprovementInLayer(int freeLayerIndex) {
    bool improved = false;
    bool continueSwitching;
    do {
        continueSwitching = sweepDownwardInLayer(freeLayerIndex);
        improved |= continueSwitching;
    } while (continueSwitching);
    return improved;
}

bool GreedySwitchHeuristic::sweepDownwardInLayer(int layerIndex) {
    bool continueSwitching = false;
    int lengthOfFreeLayer = currentNodeOrder[layerIndex].size();
    for (int upperNodeIndex = 0; upperNodeIndex < lengthOfFreeLayer - 1; ++upperNodeIndex) {
        int lowerNodeIndex = upperNodeIndex + 1;
        continueSwitching |= switchIfImproves(layerIndex, upperNodeIndex, lowerNodeIndex);
    }
    return continueSwitching;
}

bool GreedySwitchHeuristic::switchIfImproves(int layerIndex, int upperNodeIndex, int lowerNodeIndex) {
    bool continueSwitching = false;
    if (switchDecider->doesSwitchReduceCrossings(upperNodeIndex, lowerNodeIndex)) {
        exchangeNodes(upperNodeIndex, lowerNodeIndex, layerIndex);
        continueSwitching = true;
    }
    return continueSwitching;
}

void GreedySwitchHeuristic::exchangeNodes(int indexOne, int indexTwo, int layerIndex) {
    switchDecider->notifyOfSwitch(currentNodeOrder[layerIndex][indexOne], currentNodeOrder[layerIndex][indexTwo]);
    auto &layer = currentNodeOrder[layerIndex];
    std::swap(layer[indexTwo], layer[indexOne]);
}

int GreedySwitchHeuristic::startIndex(bool isForwardSweep, int length) { return isForwardSweep ? 0 : length - 1; }

bool GreedySwitchHeuristic::alwaysImproves() { return !(greedySwitchType == CrossMinType::ONE_SIDED_GREEDY_SWITCH); }

bool GreedySwitchHeuristic::isDeterministic() { return true; }

void GreedySwitchHeuristic::initAtPortLevel(int l, int n, int p,
                                            std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    nPorts++;
}

void GreedySwitchHeuristic::initAtLayerLevel(int l, const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    nodeOrder[l][0]->getLayer()->id = l;
}

void GreedySwitchHeuristic::initAfterTraversal() { portPositions.resize(nPorts); }

}  // namespace GuiBridge