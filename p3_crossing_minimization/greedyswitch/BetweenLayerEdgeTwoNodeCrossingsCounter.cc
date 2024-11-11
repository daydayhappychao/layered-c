#include "BetweenLayerEdgeTwoNodeCrossingsCounter.h"
#include <algorithm>
#include <vector>
#include "../../../Edge.h"
#include "../../../Node.h"
#include "../../p3_crossing_minimization/counting/CrossMinUtil.h"

namespace GuiBridge {

AdjacencyList::AdjacencyList(std::shared_ptr<Node> &node, PortType side, BetweenLayerEdgeTwoNodeCrossingsCounter *that)
    : node(node), side(side), that(that) {
    getAdjacenciesSortedByPosition();
}

void AdjacencyList::getAdjacenciesSortedByPosition() {
    iterateTroughEdgesCollectingAdjacencies();
    std::sort(adjacencyList.begin(), adjacencyList.end());
}

void AdjacencyList::iterateTroughEdgesCollectingAdjacencies() {
    auto ports = inNorthSouthEastWestOrder(node, side);
    for (auto &port : ports) {
        const auto &edges = getEdgesConnectedTo(port);
        for (const auto &edge : edges) {
            if (isNotInLayer(edge)) {
                addAdjacencyOf(edge);
                size_++;
                currentSize++;
            }
        }
    }
}
std::vector<std::shared_ptr<Edge>> AdjacencyList::getEdgesConnectedTo(std::shared_ptr<Port> &port) {
    return port->getEdges();
}

bool AdjacencyList::isNotInLayer(const std::shared_ptr<Edge> &edge) const {
    return edge->getSrc()->getNode()->getLayer() != edge->getDst()->getNode()->getLayer();
}

void AdjacencyList::addAdjacencyOf(const std::shared_ptr<Edge> &edge) {
    auto adjacentPort = adjacentPortOf(edge, side);
    int adjacentPortPosition = that->portPositions[adjacentPort];
    int lastIndex = adjacencyList.size() - 1;
    if (!adjacencyList.empty() && adjacencyList[lastIndex]->position == adjacentPortPosition) {
        adjacencyList[lastIndex]->cardinality++;
        adjacencyList[lastIndex]->currentCardinality++;
    } else {
        adjacencyList.push_back(new Adjacency(adjacentPortPosition, adjacentPort));
    }
}

std::shared_ptr<Port> AdjacencyList::adjacentPortOf(const std::shared_ptr<Edge> &edge, PortType side) {
    return side == PortType::INPUT ? edge->getSrc() : edge->getDst();
}

void AdjacencyList::reset() {
    currentIndex = 0;
    currentSize = size_;
    if (!isEmpty()) {
        currentAdjacency()->reset();
    }
}

int AdjacencyList::countAdjacenciesBelowNodeOfFirstPort() {
    return currentSize - currentAdjacency()->currentCardinality;
}

void AdjacencyList::removeFirst() {
    if (isEmpty()) {
        return;
    }
    auto *currentEntry = currentAdjacency();
    if (currentEntry->currentCardinality == 1) {
        incrementCurrentIndex();
    } else {
        currentEntry->currentCardinality--;
    }
    currentSize--;
}

void AdjacencyList::incrementCurrentIndex() {
    currentIndex++;
    if (currentIndex < adjacencyList.size()) {
        currentAdjacency()->reset();
    }
}

bool AdjacencyList::isEmpty() const { return currentSize == 0; }

int AdjacencyList::first() { return currentAdjacency()->position; }
int AdjacencyList::size() { return currentSize; }
AdjacencyList::Adjacency *AdjacencyList::currentAdjacency() { return adjacencyList[currentIndex]; };

AdjacencyList::Adjacency::Adjacency(int adjacentPortPosition, std::shared_ptr<Port> &port) {
    position = adjacentPortPosition;
    cardinality = 1;
    currentCardinality = 1;
}

void AdjacencyList::Adjacency::reset() { currentCardinality = cardinality; }

bool AdjacencyList::Adjacency::operator<(const AdjacencyList::Adjacency &other) const {
    return position < other.position;
}

BetweenLayerEdgeTwoNodeCrossingsCounter::BetweenLayerEdgeTwoNodeCrossingsCounter(
    std::vector<std::vector<std::shared_ptr<Node>>> &currentNodeOrder, int freeLayerIndex)
    : currentNodeOrder(currentNodeOrder), freeLayerIndex(freeLayerIndex) {
    setPortPositionsForNeighbouringLayers();
};

void BetweenLayerEdgeTwoNodeCrossingsCounter::setPortPositionsForNeighbouringLayers() {
    if (freeLayerIsNotFirstLayer()) {
        setPortPositionsForLayer(freeLayerIndex - 1, PortType::OUTPUT);
    }
    if (freeLayerIsNotLastLayer()) {
        setPortPositionsForLayer(freeLayerIndex + 1, PortType::INPUT);
    }
}

bool BetweenLayerEdgeTwoNodeCrossingsCounter::freeLayerIsNotFirstLayer() const { return freeLayerIndex > 0; }

bool BetweenLayerEdgeTwoNodeCrossingsCounter::freeLayerIsNotLastLayer() const {
    return freeLayerIndex < currentNodeOrder.size() - 1;
}

void BetweenLayerEdgeTwoNodeCrossingsCounter::setPortPositionsForLayer(int layerIndex, PortType portSide) {
    int portId = 0;
    for (auto &node : currentNodeOrder[layerIndex]) {
        auto ports = inNorthSouthEastWestOrder(node, portSide);
        for (auto &port : ports) {
            portPositions[port] = portId++;
        }
    }
}

void BetweenLayerEdgeTwoNodeCrossingsCounter::countEasternEdgeCrossings(std::shared_ptr<Node> &upperNode,
                                                                        std::shared_ptr<Node> &lowerNode) {
    resetCrossingCount();
    /**
     * @todo
     * java里用了 equals 方法，没看到这个方法哪来的
     */
    if (upperNode == lowerNode) {
        return;
    }
    addEasternCrossings(upperNode, lowerNode);
}

void BetweenLayerEdgeTwoNodeCrossingsCounter::countWesternEdgeCrossings(std::shared_ptr<Node> &upperNode,
                                                                        std::shared_ptr<Node> &lowerNode) {
    resetCrossingCount();
    /**
     * @todo
     * java里用了 equals 方法，没看到这个方法哪来的
     */
    if (upperNode == lowerNode) {
        return;
    }
    addWesternCrossings(upperNode, lowerNode);
}

void BetweenLayerEdgeTwoNodeCrossingsCounter::countBothSideCrossings(std::shared_ptr<Node> &upperNode,
                                                                     std::shared_ptr<Node> &lowerNode) {
    resetCrossingCount();
    if (upperNode == lowerNode) {
        return;
    }
    addWesternCrossings(upperNode, lowerNode);
    addEasternCrossings(upperNode, lowerNode);
}

void BetweenLayerEdgeTwoNodeCrossingsCounter::resetCrossingCount() {
    upperLowerCrossings = 0;
    lowerUpperCrossings = 0;
}

void BetweenLayerEdgeTwoNodeCrossingsCounter::addEasternCrossings(std::shared_ptr<Node> &upperNode,
                                                                  std::shared_ptr<Node> &lowerNode) {
    upperAdjacencies = getAdjacencyFor(upperNode, PortType::OUTPUT, easternAdjacencies);
    lowerAdjacencies = getAdjacencyFor(lowerNode, PortType::OUTPUT, easternAdjacencies);
    if (upperAdjacencies->size() == 0 || lowerAdjacencies->size() == 0) {
        return;
    }
    countCrossingsByMergingAdjacencyLists();
}

AdjacencyList *BetweenLayerEdgeTwoNodeCrossingsCounter::getAdjacencyFor(
    std::shared_ptr<Node> &node, PortType side, std::map<std::shared_ptr<Node>, AdjacencyList *> &adjacencies) {
    if (adjacencies.empty()) {
        for (auto &n : currentNodeOrder[freeLayerIndex]) {
            adjacencies[n] = new AdjacencyList(n, side, this);
        }
    }
    auto &al = adjacencies[node];
    al->reset();
    return al;
}

void BetweenLayerEdgeTwoNodeCrossingsCounter::addWesternCrossings(std::shared_ptr<Node> &upperNode,
                                                                  std::shared_ptr<Node> &lowerNode) {
    upperAdjacencies = getAdjacencyFor(upperNode, PortType::INPUT, westernAdjacencies);
    lowerAdjacencies = getAdjacencyFor(lowerNode, PortType::INPUT, westernAdjacencies);

    if (upperAdjacencies->isEmpty() || lowerAdjacencies->isEmpty()) {
        return;
    }
    countCrossingsByMergingAdjacencyLists();
}

void BetweenLayerEdgeTwoNodeCrossingsCounter::countCrossingsByMergingAdjacencyLists() {
    while (!upperAdjacencies->isEmpty() && !lowerAdjacencies->isEmpty()) {
        if (isBelow(lowerAdjacencies->first(), upperAdjacencies->first())) {
            upperLowerCrossings += upperAdjacencies->size();
            lowerAdjacencies->removeFirst();
        } else if (isBelow(lowerAdjacencies->first(), upperAdjacencies->first())) {
            lowerUpperCrossings += lowerAdjacencies->size();
            upperAdjacencies->removeFirst();
        } else {
            upperLowerCrossings += upperAdjacencies->countAdjacenciesBelowNodeOfFirstPort();
            lowerUpperCrossings += lowerAdjacencies->countAdjacenciesBelowNodeOfFirstPort();
            upperAdjacencies->removeFirst();
            lowerAdjacencies->removeFirst();
        }
    }
}

bool BetweenLayerEdgeTwoNodeCrossingsCounter::isBelow(int firstPort, int secondPort) { return firstPort > secondPort; }

int BetweenLayerEdgeTwoNodeCrossingsCounter::getUpperLowerCrossings() { return upperLowerCrossings; }
int BetweenLayerEdgeTwoNodeCrossingsCounter::getLowerUpperCrossings() { return lowerUpperCrossings; }

}  // namespace GuiBridge