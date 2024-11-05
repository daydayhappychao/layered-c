#ifndef CROSSING_COUNTER_HPP
#define CROSSING_COUNTER_HPP

#include <deque>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include "../../../opts/PortType.h"
#include "BinaryIndexedTree.h"

namespace GuiBridge {
class BinaryIndexedTree;
class Node;
class Port;
class Edge;

class CrossingCounter {
public:
    explicit CrossingCounter(std::vector<int> &portPositions);

    int countCrossingsBetweenLayers(const std::vector<std::shared_ptr<Node>> &leftLayerNodes,
                                    const std::vector<std::shared_ptr<Node>> &rightLayerNodes);

    int countInLayerCrossingsOnSide(const std::vector<std::shared_ptr<Node>> &nodes, PortType type);
    std::pair<int, int> countCrossingsBetweenPortsInBothOrders(std::shared_ptr<Port> &upperPort,
                                                               std::shared_ptr<Port> &lowerPort);
    std::pair<int, int> countInLayerCrossingsBetweenNodesInBothOrders(std::shared_ptr<Node> &upperNode,
                                                                      std::shared_ptr<Node> &lowerNode, PortType type);
    void initForCountingBetween(std::vector<std::shared_ptr<Node>> &leftLayerNodes,
                                std::vector<std::shared_ptr<Node>> &rightLayerNodes);

    std::list<std::shared_ptr<Port>> initPortPositionsForInLayerCrossings(
        const std::vector<std::shared_ptr<Node>> &nodes, PortType type);
    void switchPorts(std::shared_ptr<Port> &topPort, std::shared_ptr<Port> &bottomPort);
    void switchNodes(std::shared_ptr<Node> &wasUpperNode, std::shared_ptr<Node> &wasLowerNode, PortType type);
    int positionOf(const std::shared_ptr<Port> &port) const;

private:
    std::vector<int> &portPositions;
    BinaryIndexedTree *indexTree;
    std::deque<int> ends;
    std::vector<int> nodeCardinalities;
    PortType INDEXING_SIDE = PortType::INPUT;
    PortType STACK_SIDE = PortType::OUTPUT;

    std::list<std::shared_ptr<Port>> connectedInLayerPortsSortedByPosition(std::shared_ptr<Node> &upperNode,
                                                                           std::shared_ptr<Node> &lowerNode,
                                                                           PortType type);
    std::list<std::shared_ptr<Port>> connectedPortsSortedByPosition(std::shared_ptr<Port> &upperPort,
                                                                    std::shared_ptr<Port> &lowerPort);
    int countCrossingsOnPorts(std::list<std::shared_ptr<Port>> &ports);
    int countInLayerCrossingsOnPorts(std::list<std::shared_ptr<Port>> &ports);

    void initPositions(const std::vector<std::shared_ptr<Node>> &nodes, std::list<std::shared_ptr<Port>> &ports,
                       PortType side, bool topDown, bool getCardinalities);

    std::list<std::shared_ptr<Port>> initPortPositionsCounterClockwise(
        const std::vector<std::shared_ptr<Node>> &leftLayerNodes,
        const std::vector<std::shared_ptr<Node>> &rightLayerNodes);

    int emptyStack(std::deque<std::shared_ptr<Node>> &stack, std::list<std::shared_ptr<Port>> &ports, PortType type,
                   int startIndex);

    std::vector<std::shared_ptr<Port>> getPorts(const std::shared_ptr<Node> &node, PortType side, bool topDown);

    int start(const std::vector<std::shared_ptr<Node>> &nodes, bool topDown);
    bool end(int i, bool topDown, const std::vector<std::shared_ptr<Node>> &nodes);
    int step(bool topDown);
    bool isInLayer(std::shared_ptr<Edge> &edge);
    std::shared_ptr<Port> otherEndOf(std::shared_ptr<Edge> &edge, std::shared_ptr<Port> &fromPort);
};

struct ComparePosition {
    const CrossingCounter *that;

    explicit ComparePosition(CrossingCounter *that) : that(that) {}

    bool operator()(const std::shared_ptr<Port> &a, const std::shared_ptr<Port> &b) {
        return that->positionOf(a) < that->positionOf(b);  // 根据 positionOf 进行比较
    }
};
}  // namespace GuiBridge

#endif