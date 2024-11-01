#include "AbstractBarycenterPortDistributor.h"
#include <algorithm>
#include <vector>
#include "../../Edge.h"
#include "../../Layer.h"
#include "../../Node.h"
#include "../../Port.h"
#include "../../opts/PortType.h"

namespace GuiBridge {

AbstractBarycenterPortDistributor::AbstractBarycenterPortDistributor(int numLayers) { nodePositions.resize(numLayers); }

void AbstractBarycenterPortDistributor::calculatePortRanks(std::vector<std::shared_ptr<Node>> &layer,
                                                           PortType portType) {
    float consumedRank = 0;
    for (auto &nodeIx : layer) {
        consumedRank += calculatePortRanks(nodeIx, consumedRank, portType);
    }
}

void AbstractBarycenterPortDistributor::distributePorts(std::shared_ptr<Node> &node, PortType side) {
    // 分发端口
    distributePorts(node, node->getPortsByPortType(side), side);
    // 对端口进行排序
    sortPorts(node);
}

void AbstractBarycenterPortDistributor::distributePorts(std::shared_ptr<Node> &node,
                                                        std::vector<std::shared_ptr<Port>> &ports, PortType side) {
    inLayerPorts.clear();
    iteratePortsAndCollectInLayerPorts(node, ports, side);
    if (!inLayerPorts.empty()) {
        calculateInLayerPortsBarycenterValues(node);
    }
}

void AbstractBarycenterPortDistributor::iteratePortsAndCollectInLayerPorts(std::shared_ptr<Node> &node,
                                                                           std::vector<std::shared_ptr<Port>> &ports,
                                                                           PortType side) {
    minBarycenter = 0.0F;
    maxBarycenter = 0.0F;

    float absurdlyLargeFloat = 2 * node->getLayer()->getNodes().size() + 1;

    for (auto &port : ports) {
        bool globalContinue = false;
        float sum = 0;
        if (side == PortType::OUTPUT) {
            for (auto &outgoingEdge : port->getEdges()) {
                const auto &connectedPort = outgoingEdge->getDst();
                if (connectedPort->getNode()->getLayer() == node->getLayer()) {
                    inLayerPorts.push_back(port);
                    globalContinue = true;
                    break;
                }
                sum += portRanks[connectedPort->id];
            }
            if (globalContinue) {
                continue;
            }
        } else {
            for (auto &incomingEdge : port->getEdges()) {
                const auto &connectedPort = incomingEdge->getSrc();
                if (connectedPort->getNode()->getLayer() == node->getLayer()) {
                    inLayerPorts.push_back(port);
                    globalContinue = true;
                    break;
                }
                sum -= portRanks[connectedPort->id];
            }
        }

        if (!port->getEdges().empty()) {
            portBarycenter[port->id] = sum / port->getEdges().size();
            minBarycenter = std::min(minBarycenter, portBarycenter[port->id]);
            maxBarycenter = std::max(maxBarycenter, portBarycenter[port->id]);
        }
    }
}

void AbstractBarycenterPortDistributor::calculateInLayerPortsBarycenterValues(std::shared_ptr<Node> &node) {
    // 具体实现
    int nodeIndexInLayer = positionOf(node) + 1;
    int layerSize = node->getLayer()->getNodes().size() + 1;
    for (auto &inLayerPort : inLayerPorts) {
        int sum = 0;
        int inLayerConnections = 0;
        for (auto &connectedPort : inLayerPort->getConnectedPorts()) {
            if (connectedPort->getNode()->getLayer() == node->getLayer()) {
                sum += positionOf(connectedPort->getNode()) + 1;
                inLayerConnections++;
            }
        }
        float barycenter = sum / inLayerConnections;

        auto portType = inLayerPort->getType();
        if (portType == PortType::INPUT) {
            if (barycenter < nodeIndexInLayer) {
                portBarycenter[inLayerPort->id] = maxBarycenter + barycenter;
            } else {
                portBarycenter[inLayerPort->id] = minBarycenter - (layerSize - barycenter);
            }
        } else {
            if (barycenter < nodeIndexInLayer) {
                portBarycenter[inLayerPort->id] = minBarycenter - barycenter;
            } else {
                portBarycenter[inLayerPort->id] = maxBarycenter + (layerSize - barycenter);
            }
        }
    }
}

int AbstractBarycenterPortDistributor::positionOf(const std::shared_ptr<Node> &node) {
    return nodePositions[node->getLayer()->id][node->getId()];
}

void AbstractBarycenterPortDistributor::updateNodePositions(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder,
                                                            int currentIndex) {
    auto &layer = nodeOrder[currentIndex];
    for (int i = 0; i < layer.size(); i++) {
        auto &node = layer[i];
        nodePositions[node->getLayer()->id][node->getId()] = i;
    }
}

bool AbstractBarycenterPortDistributor::isNotFirstLayer(int length, int currentIndex, bool isForwardSweep) {
    return isForwardSweep ? currentIndex != 0 : currentIndex != length - 1;
}

PortType AbstractBarycenterPortDistributor::portTypeFor(bool isForwardSweep) {
    return isForwardSweep ? PortType::OUTPUT : PortType::INPUT;
}

void AbstractBarycenterPortDistributor::sortPorts(std::shared_ptr<Node> &node) {
    auto &inputPorts = node->getInputPorts();
    auto &outputPorts = node->getOutputPorts();
    auto fn = [this](std::shared_ptr<GuiBridge::Port> &port1, std::shared_ptr<GuiBridge::Port> &port2) {
        auto port1Bary = portBarycenter[port1->id];
        auto port2Bary = portBarycenter[port2->id];
        if (port1Bary == 0 && port2Bary == 0) {
            return true;
        }
        if (port1Bary == 0) {
            return false;
        }
        if (port2Bary == 0) {
            return true;
        }
        return port1Bary < port2Bary;
    };
    std::sort(inputPorts.begin(), inputPorts.end(), fn);
    std::sort(outputPorts.begin(), outputPorts.end(), fn);
}

std::vector<float> &AbstractBarycenterPortDistributor::getPortRanks() { return portRanks; }

bool AbstractBarycenterPortDistributor::distributePortsWhileSweeping(
    std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex, bool isForwardSweep) {
    updateNodePositions(nodeOrder, currentIndex);
    auto &freeLayer = nodeOrder[currentIndex];
    PortType side = isForwardSweep ? PortType::INPUT : PortType::OUTPUT;
    if (isNotFirstLayer(nodeOrder.size(), currentIndex, isForwardSweep)) {
        auto &fixedLayer = nodeOrder[isForwardSweep ? currentIndex - 1 : currentIndex + 1];
        calculatePortRanks(fixedLayer, portTypeFor(isForwardSweep));
        for (auto &node : freeLayer) {
            distributePorts(node, side);
        }

        calculatePortRanks(freeLayer, portTypeFor(!isForwardSweep));
        for (auto &node : fixedLayer) {
            distributePorts(node, opposedPortType(side));
        }
    } else {
        for (auto &node : freeLayer) {
            distributePorts(node, side);
        }
    }
    // 不能与总是改进交叉最小化启发式一起使用
    return false;
}

void AbstractBarycenterPortDistributor::initAtLayerLevel(int l,
                                                         std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    nodePositions[l].resize(nodeOrder[l].size());
}

void AbstractBarycenterPortDistributor::initAtNodeLevel(int l, int n,
                                                        std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    auto &node = nodeOrder[l][n];
    node->setId(n);
    nodePositions[l][n] = n;
}

void AbstractBarycenterPortDistributor::initAtPortLevel(int l, int n, int p,
                                                        std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    nodeOrder[l][n]->getAllPorts()[p]->id = nPorts++;
}

void AbstractBarycenterPortDistributor::initAfterTraversal() {
    portRanks.resize(nPorts);
    portBarycenter.resize(nPorts);
}
}  // namespace GuiBridge
