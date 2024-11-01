#include "GreedyPortDistributor.h"
#include <algorithm>
#include "../../Node.h"
#include "../../Port.h"
#include "../intermediate/greedyswitch/BetweenLayerEdgeTwoNodeCrossingsCounter.h"
#include "./counting/CrossingCounter.h"

namespace GuiBridge {

bool GreedyPortDistributor::distributePortsWhileSweeping(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder,
                                                         int currentIndex, bool isForwardSweep) {
    initialize(nodeOrder, currentIndex, isForwardSweep);
    return distributePortsInLayer(nodeOrder, currentIndex, isForwardSweep);
}

bool GreedyPortDistributor::distributePortsInLayer(const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder,
                                                   int currentIndex, bool isForwardSweep) {
    PortType side = isForwardSweep ? PortType::INPUT : PortType::OUTPUT;
    bool improved = false;

    return improved;
}

void GreedyPortDistributor::initForLayers(std::vector<std::shared_ptr<Node>> &leftLayer,
                                          std::vector<std::shared_ptr<Node>> &rightLayer) {
    crossingsCounter->initForCountingBetween(leftLayer, rightLayer);
}

void GreedyPortDistributor::initialize(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex,
                                       bool isForwardSweep) {
    if (isForwardSweep && currentIndex > 0) {
        initForLayers(nodeOrder[currentIndex - 1], nodeOrder[currentIndex]);
    } else if (!isForwardSweep && currentIndex < nodeOrder.size() - 1) {
        initForLayers(nodeOrder[currentIndex], nodeOrder[currentIndex + 1]);
    } else {
        crossingsCounter->initPortPositionsForInLayerCrossings(nodeOrder[currentIndex],
                                                               isForwardSweep ? PortType::INPUT : PortType::OUTPUT);
    }
}

void GreedyPortDistributor::initAtNodeLevel(int l, int n, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    auto node = nodeOrder[l][n];
    nPorts += node->getAllPorts().size();
}

void GreedyPortDistributor::initAfterTraversal() {
    portPos.resize(nPorts);
    crossingsCounter = new CrossingCounter(portPos);
}

}  // namespace GuiBridge