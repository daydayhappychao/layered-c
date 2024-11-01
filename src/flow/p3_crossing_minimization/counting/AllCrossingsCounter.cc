#include "AllCrossingsCounter.h"
#include <vector>
#include "../../../Edge.h"
#include "../../../Node.h"
#include "../../../opts/PortType.h"
#include "CrossingCounter.h"
#include "HyperedgeCrossingsCounter.h"

namespace GuiBridge {
AllCrossingsCounter::AllCrossingsCounter(const std::vector<std::vector<std::shared_ptr<Node>>> &graph) {
    inLayerEdgeCounts.resize(graph.size());
    hasHyperEdgesEastOfIndex.resize(graph.size());
}

int AllCrossingsCounter::countAllCrossings(const std::vector<std::vector<std::shared_ptr<Node>>> &currentOrder) {
    if (currentOrder.empty()) {
        return 0;
    }

    int crossings = crossingCounter->countInLayerCrossingsOnSide(currentOrder[0], PortType::INPUT);

    crossings += crossingCounter->countInLayerCrossingsOnSide(currentOrder[currentOrder.size() - 1], PortType::OUTPUT);

    for (int layerIndex = 0; layerIndex < currentOrder.size(); layerIndex++) {
        crossings += countCrossingsAt(layerIndex, currentOrder);
    }
    return crossings;
}

int AllCrossingsCounter::countCrossingsAt(int layerIndex,
                                          const std::vector<std::vector<std::shared_ptr<Node>>> &currentOrder) {
    int totalCrossings = 0;
    const auto &leftLayer = currentOrder[layerIndex];  // 左侧层
    if (layerIndex < currentOrder.size() - 1) {
        const auto &rightLayer = currentOrder[layerIndex + 1];  // 右侧层
        if (hasHyperEdgesEastOfIndex[layerIndex]) {
            totalCrossings = hyperedgeCrossingsCounter->countCrossings(leftLayer, rightLayer);
            totalCrossings += crossingCounter->countInLayerCrossingsOnSide(leftLayer, PortType::OUTPUT);
            totalCrossings += crossingCounter->countInLayerCrossingsOnSide(rightLayer, PortType::INPUT);
        } else {
            totalCrossings = crossingCounter->countCrossingsBetweenLayers(leftLayer, rightLayer);
        }
    }
    return totalCrossings;
}

void AllCrossingsCounter::initAtNodeLevel(int l, int n,
                                          const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    const auto &node = nodeOrder[l][n];
}

void AllCrossingsCounter::initAtPortLevel(int l, int n, int p,
                                          const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    auto allPorts = nodeOrder[l][n]->getAllPorts();
    auto &port = allPorts[p];
    port->id = nPorts++;
    if (port->getType() == PortType::OUTPUT) {
        hasHyperEdgesEastOfIndex[l] = true;
    }
}

void AllCrossingsCounter::initAtEdgeLevel(int l, int n, int p, int e, const std::shared_ptr<Edge> &edge,
                                          const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    auto allPorts = nodeOrder[l][n]->getAllPorts();
    auto &port = allPorts[p];
    if (edge->getSrc() == port && edge->getSrc()->getNode()->getLayer() == edge->getDst()->getNode()->getLayer()) {
        inLayerEdgeCounts[l]++;
    }
}

void AllCrossingsCounter::initAfterTraversal() {
    std::vector<int> portPos(nPorts);

    hyperedgeCrossingsCounter = new HyperedgeCrossingsCounter(portPos);
    crossingCounter = new CrossingCounter(portPos);
}

}  // namespace GuiBridge