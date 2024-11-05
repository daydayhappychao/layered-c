#include "./CrossingCounter.h"
#include <algorithm>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include "../../../Edge.h"
#include "../../../Node.h"
#include "../../../Port.h"
#include "BinaryIndexedTree.h"
#include "CrossMinUtil.h"

namespace GuiBridge {
CrossingCounter::CrossingCounter(std::vector<int> &portPositions) : portPositions(portPositions), indexTree(nullptr) {}

int CrossingCounter::countCrossingsBetweenLayers(const std::vector<std::shared_ptr<Node>> &leftLayerNodes,
                                                 const std::vector<std::shared_ptr<Node>> &rightLayerNodes) {
    auto ports = initPortPositionsCounterClockwise(leftLayerNodes, rightLayerNodes);
    indexTree = new BinaryIndexedTree(ports.size());
    return countCrossingsOnPorts(ports);
}

int CrossingCounter::countInLayerCrossingsOnSide(const std::vector<std::shared_ptr<Node>> &nodes, PortType type) {
    auto ports = initPortPositionsForInLayerCrossings(nodes, type);
    return countInLayerCrossingsOnPorts(ports);
}

std::pair<int, int> CrossingCounter::countCrossingsBetweenPortsInBothOrders(std::shared_ptr<Port> &upperPort,
                                                                            std::shared_ptr<Port> &lowerPort) {
    auto ports = connectedPortsSortedByPosition(upperPort, lowerPort);
    auto upperLowerCrossings = countCrossingsOnPorts(ports);
    indexTree->clear();
    switchPorts(upperPort, lowerPort);

    std::vector<std::shared_ptr<Port>> portsArr(ports.begin(), ports.end());
    std::sort(portsArr.begin(), portsArr.end(), [this](auto &a, auto &b) { return positionOf(a) < positionOf(b); });
    ports = std::list(portsArr.begin(), portsArr.end());

    int lowerUpperCrossings = countCrossingsOnPorts(ports);
    indexTree->clear();
    switchPorts(lowerPort, upperPort);
    return std::make_pair(upperLowerCrossings, lowerUpperCrossings);
}

std::pair<int, int> CrossingCounter::countInLayerCrossingsBetweenNodesInBothOrders(std::shared_ptr<Node> &upperNode,
                                                                                   std::shared_ptr<Node> &lowerNode,
                                                                                   PortType type) {
    auto ports = connectedInLayerPortsSortedByPosition(upperNode, lowerNode, type);
    int upperLowerCrossings = countInLayerCrossingsOnPorts(ports);
    switchNodes(upperNode, lowerNode, type);

    indexTree->clear();
    std::vector<std::shared_ptr<Port>> portsArr(ports.begin(), ports.end());
    std::sort(portsArr.begin(), portsArr.end(), [this](auto &a, auto &b) { return positionOf(a) < positionOf(b); });
    ports = std::list<std::shared_ptr<Port>>(portsArr.begin(), portsArr.end());
    int lowerUpperCrossings = countInLayerCrossingsOnPorts(ports);
    switchNodes(lowerNode, upperNode, type);
    indexTree->clear();
    return std::make_pair(upperLowerCrossings, lowerUpperCrossings);
}

void CrossingCounter::initForCountingBetween(std::vector<std::shared_ptr<Node>> &leftLayerNodes,
                                             std::vector<std::shared_ptr<Node>> &rightLayerNodes) {
    auto ports = initPortPositionsCounterClockwise(leftLayerNodes, rightLayerNodes);
    indexTree = new BinaryIndexedTree(ports.size());
}

std::list<std::shared_ptr<Port>> CrossingCounter::initPortPositionsForInLayerCrossings(
    const std::vector<std::shared_ptr<Node>> &nodes, PortType type) {
    std::list<std::shared_ptr<Port>> ports;
    initPositions(nodes, ports, type, true, true);
    this->indexTree = new BinaryIndexedTree(ports.size());
    return ports;
}

void CrossingCounter::switchPorts(std::shared_ptr<Port> &topPort, std::shared_ptr<Port> &bottomPort) {
    int maxLen = std::max(topPort->id, bottomPort->id);
    if (portPositions.size() < maxLen + 1) {
        portPositions.resize(maxLen + 1);
    }
    int topPortPos = portPositions[topPort->id];
    portPositions[topPort->id] = portPositions[bottomPort->id];
    portPositions[bottomPort->id] = topPortPos;
}

void CrossingCounter::switchNodes(std::shared_ptr<Node> &wasUpperNode, std::shared_ptr<Node> &wasLowerNode,
                                  PortType type) {
    auto ports = inNorthSouthEastWestOrder(wasUpperNode, type);
    for (auto &port : ports) {
        if (portPositions.size() < port->id + 1) {
            portPositions.resize(port->id + 1);
        }
        portPositions[port->id] = positionOf(port) + nodeCardinalities[wasLowerNode->getId()];
    }
    ports = inNorthSouthEastWestOrder(wasLowerNode, type);
    for (auto &port : ports) {
        if (portPositions.size() < port->id + 1) {
            portPositions.resize(port->id + 1);
        }
        portPositions[port->id] = positionOf(port) - nodeCardinalities[wasUpperNode->getId()];
    }
}

std::list<std::shared_ptr<Port>> CrossingCounter::connectedInLayerPortsSortedByPosition(
    std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode, PortType type) {
    ComparePosition comparePos(this);
    std::set<std::shared_ptr<Port>, ComparePosition> ports(comparePos);
    std::vector<std::shared_ptr<Node>> nodeList = {upperNode, lowerNode};
    for (auto &node : nodeList) {
        for (auto &port : inNorthSouthEastWestOrder(node, type)) {
            for (auto &edge : port->getEdges()) {
                ports.insert(port);
                if (isInLayer(edge)) {
                    ports.insert(otherEndOf(edge, port));
                }
            }
        }
    }
    std::list<std::shared_ptr<Port>> portsList(ports.begin(), ports.end());
    return portsList;
}

std::list<std::shared_ptr<Port>> CrossingCounter::connectedPortsSortedByPosition(std::shared_ptr<Port> &upperPort,
                                                                                 std::shared_ptr<Port> &lowerPort) {
    ComparePosition comparePos(this);
    std::set<std::shared_ptr<Port>, ComparePosition> ports(comparePos);
    std::vector<std::shared_ptr<Port>> portList = {upperPort, lowerPort};

    for (auto &port : portList) {
        ports.insert(port);
        for (auto &edge : port->getEdges()) {
            ports.insert(otherEndOf(edge, port));
        }
    }
    std::list<std::shared_ptr<Port>> portsList(ports.begin(), ports.end());
    return portsList;
};

int CrossingCounter::countCrossingsOnPorts(std::list<std::shared_ptr<Port>> &ports) {
    std::vector<std::shared_ptr<Port>> portVec(ports.begin(), ports.end());
    int crossings = 0;
    for (auto &port : portVec) {
        indexTree->removeAll(positionOf(port));
        for (auto &edge : port->getEdges()) {
            auto ptr = otherEndOf(edge, port);
            int endPosition = positionOf(ptr);
        }

        while (!ends.empty()) {
            indexTree->add(ends.back());
            ends.pop_back();
        }
    }
    return crossings;
}

std::list<std::shared_ptr<Port>> CrossingCounter::initPortPositionsCounterClockwise(
    const std::vector<std::shared_ptr<Node>> &leftLayerNodes,
    const std::vector<std::shared_ptr<Node>> &rightLayerNodes) {
    std::list<std::shared_ptr<Port>> ports;
    this->initPositions(leftLayerNodes, ports, PortType::OUTPUT, true, false);
    this->initPositions(rightLayerNodes, ports, PortType::INPUT, false, false);
    return ports;
}
void CrossingCounter::initPositions(const std::vector<std::shared_ptr<Node>> &nodes,
                                    std::list<std::shared_ptr<Port>> &ports, PortType side, bool topDown,
                                    bool getCardinalities) {
    int numPorts = ports.size();
    if (getCardinalities) {
        nodeCardinalities.resize(nodes.size());
    }
    for (int i = start(nodes, topDown); end(i, topDown, nodes); i += step(topDown)) {
        const auto &node = nodes[i];
        auto nodePorts = this->getPorts(node, side, topDown);
        if (getCardinalities) {
            nodeCardinalities[node->getId()] = nodePorts.size();
        }
        for (const auto &port : nodePorts) {
            std::cout << "Processing port ID: " << port->id << ", portPositions: " << portPositions.size() << std::endl;
            if (portPositions.size() < (port->id + 1)) {
                portPositions.resize(port->id + 1);
            }
            portPositions[port->id] = numPorts++;
        }
        ports.insert(ports.end(), nodePorts.begin(), nodePorts.end());
    }
}

int CrossingCounter::emptyStack(std::deque<std::shared_ptr<Node>> &stack, std::list<std::shared_ptr<Port>> &ports,
                                PortType type, int startIndex) {
    int index = startIndex;
    while (!stack.empty()) {
        auto &dummy = stack.back();
        stack.pop_back();
        auto &p = dummy->getPortsByPortType(type)[0];
        if (portPositions.size() < p->id + 1) {
            portPositions.resize(p->id + 1);
        }
        portPositions[p->id] = index++;
        ports.push_back(p);
    }
    return index;
}

int CrossingCounter::start(const std::vector<std::shared_ptr<Node>> &nodes, bool topDown) {
    return topDown ? 0 : nodes.size() - 1;
}

bool CrossingCounter::end(int i, bool topDown, const std::vector<std::shared_ptr<Node>> &nodes) {
    return topDown ? i < nodes.size() : i >= 0;
}
int CrossingCounter::step(bool topDown) { return topDown ? 1 : -1; }

std::vector<std::shared_ptr<Port>> CrossingCounter::getPorts(const std::shared_ptr<Node> &node, PortType side,
                                                             bool topDown) {
    if (side == PortType::OUTPUT) {
        auto _outputPorts = node->getOutputPorts();
        auto outputPorts = _outputPorts;
        if (!topDown) {
            std::reverse(outputPorts.begin(), outputPorts.end());
        }
        return outputPorts;
    }
    auto _inputPorts = node->getInputPorts();
    auto inputPorts = _inputPorts;
    if (topDown) {
        std::reverse(inputPorts.begin(), inputPorts.end());
    }
    return inputPorts;
}

bool CrossingCounter::isInLayer(std::shared_ptr<Edge> &edge) {
    auto &sourceLayer = edge->getSrc()->getNode()->getLayer();
    auto &targetLayer = edge->getDst()->getNode()->getLayer();
    return sourceLayer == targetLayer;
}

int CrossingCounter::positionOf(const std::shared_ptr<Port> &port) const { return portPositions[port->id]; }

std::shared_ptr<Port> CrossingCounter::otherEndOf(std::shared_ptr<Edge> &edge, std::shared_ptr<Port> &fromPort) {
    return fromPort == edge->getSrc() ? edge->getDst() : edge->getSrc();
}

int CrossingCounter::countInLayerCrossingsOnPorts(std::list<std::shared_ptr<Port>> &ports) {
    int crossings = 0;
    for (auto &port : ports) {
        indexTree->removeAll(positionOf(port));
        int numBetweenLayerEdges = 0;
        for (auto &edge : port->getEdges()) {
            if (isInLayer(edge)) {
                int endPosition = positionOf(otherEndOf(edge, port));
                if (endPosition > positionOf(port)) {
                    crossings += indexTree->rank(endPosition);
                    ends.push_back(endPosition);
                }
            } else {
                numBetweenLayerEdges++;
            }
        }
        crossings += indexTree->size() * numBetweenLayerEdges;
        while (!ends.empty()) {
            auto end = ends.back();
            indexTree->add(end);
            ends.pop_back();
        }
    }
    return crossings;
}

}  // namespace GuiBridge