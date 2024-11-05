#include "SwitchDecider.h"
#include "../../../Node.h"
#include "../../../Port.h"
#include "../../../opts/PortType.h"
#include "../../p3_crossing_minimization/counting/CrossingCounter.h"
#include "./CrossingMatrixFiller.h"

namespace GuiBridge {
SwitchDecider::SwitchDecider(int freeLayerIndex, const std::vector<std::vector<std::shared_ptr<Node>>> &graph,
                             std::shared_ptr<CrossingMatrixFiller> &crossingMatrixFiller,
                             std::vector<int> &portPositions, std::shared_ptr<GraphInfoHolder> &graphData,
                             bool oneSided)
    : crossingMatrixFiller(crossingMatrixFiller), graphData(graphData) {
    if (freeLayerIndex >= graph.size()) {
        throw std::out_of_range("Greedy SwitchDecider: Free layer not in graph.");
    }
    freeLayer = graph[freeLayerIndex];

    leftInLayerCounter = std::make_shared<CrossingCounter>(portPositions);
    leftInLayerCounter->initPortPositionsForInLayerCrossings(freeLayer, PortType::INPUT);
    rightInLayerCounter = std::make_shared<CrossingCounter>(portPositions);
    rightInLayerCounter->initPortPositionsForInLayerCrossings(freeLayer, PortType::OUTPUT);
}

void SwitchDecider::notifyOfSwitch(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode) {
    leftInLayerCounter->switchNodes(upperNode, lowerNode, PortType::INPUT);
    rightInLayerCounter->switchNodes(upperNode, lowerNode, PortType::OUTPUT);
}

bool SwitchDecider::doesSwitchReduceCrossings(int upperNodeIndex, int lowerNodeIndex) const {
    if (constraintsPreventSwitch(upperNodeIndex, lowerNodeIndex)) {
        return false;
    }

    auto upperNode = freeLayer[upperNodeIndex];
    auto lowerNode = freeLayer[lowerNodeIndex];

    auto leftInlayer =
        leftInLayerCounter->countInLayerCrossingsBetweenNodesInBothOrders(upperNode, lowerNode, PortType::OUTPUT);
    auto rightInlayer =
        rightInLayerCounter->countInLayerCrossingsBetweenNodesInBothOrders(upperNode, lowerNode, PortType::INPUT);

    int upperLowerCrossings =
        crossingMatrixFiller->getCrossingMatrixEntry(upperNode, lowerNode) + leftInlayer.first + rightInlayer.first;
    int lowerUpperCrossings =
        crossingMatrixFiller->getCrossingMatrixEntry(lowerNode, upperNode) + leftInlayer.second + rightInlayer.second;

    return upperLowerCrossings > lowerUpperCrossings;
}

bool SwitchDecider::constraintsPreventSwitch(int nodeIndex, int lowerNodeIndex) const {
    auto upperNode = freeLayer[nodeIndex];
    auto lowerNode = freeLayer[lowerNodeIndex];

    return haveSuccessorConstraints(upperNode, lowerNode) || haveLayoutUnitConstraints(upperNode, lowerNode);
}

bool SwitchDecider::haveSuccessorConstraints(const std::shared_ptr<Node> &upperNode,
                                             const std::shared_ptr<Node> &lowerNode) const {
    return false;
}

bool SwitchDecider::haveLayoutUnitConstraints(const std::shared_ptr<Node> &upperNode,
                                              const std::shared_ptr<Node> &lowerNode) const {
    return false;
}

}  // namespace GuiBridge