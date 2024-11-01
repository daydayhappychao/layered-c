#include "HyperedgeCrossingsCounter.h"
#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "../../../Edge.h"
#include "../../../Node.h"
#include "../../../utils/VectorUtil.h"
namespace GuiBridge {
HyperedgeCrossingsCounter::HyperedgeCrossingsCounter(std::vector<int> &portPos) : portPos(portPos) {}

bool Hyperedge::operator<(const Hyperedge &other) const {
    if (upperLeft != other.upperLeft) {
        return upperLeft < other.upperLeft;
    }
    if (upperRight != other.upperRight) {
        return upperRight < other.upperRight;
    }
    return false;
}

bool HyperedgeCorner::operator<(const HyperedgeCorner &other) const {
    if (position != other.position) {
        return position < other.position;
    }
    if (oppositePosition != other.oppositePosition) {
        return oppositePosition < other.oppositePosition;
    }
    if (hyperedge != other.hyperedge) {
        return false;
    }
    return (type == Type::UPPER && other.type == Type::LOWER);
}

int HyperedgeCrossingsCounter::countCrossings(const std::vector<std::shared_ptr<Node>> &leftLayer,
                                              const std::vector<std::shared_ptr<Node>> &rightLayer) {
    int sourceCount = 0;
    for (const auto &node : leftLayer) {
        for (auto &port : node->getOutputPorts()) {
            int portEdges = 0;
            for (auto &edge : port->getEdges()) {
                if (node->getLayer() != edge->getDst()->getNode()->getLayer()) {
                    portEdges++;
                }
            }
            if (portEdges > 0) {
                portPos[port->id] = sourceCount++;
            }
        }
    }

    int targetCount = 0;
    for (const auto &node : rightLayer) {
        int northInputPorts = 0;
        int otherInputPorts = 0;
        auto allPorts = node->getAllPorts();
        std::reverse(allPorts.begin(), allPorts.end());
        for (auto &port : allPorts) {
            int portEdges = 0;
            if (port->getType() == PortType::INPUT) {
                for (auto &edge : port->getEdges()) {
                    if (node->getLayer() != edge->getSrc()->getNode()->getLayer()) {
                        portEdges++;
                    }
                }
            }
            if (portEdges > 0) {
                portPos[port->id] = targetCount + northInputPorts + otherInputPorts;
                otherInputPorts++;
            }
        }
        targetCount += otherInputPorts;
    }

    std::map<std::shared_ptr<Port>, Hyperedge *> port2HyperedgeMap;
    std::set<Hyperedge *> hyperedgeSet;
    for (const auto &node : leftLayer) {
        for (auto &sourcePort : node->getOutputPorts()) {
            for (auto &edge : sourcePort->getEdges()) {
                auto targetPort = edge->getDst();
                if (node->getLayer() != targetPort->getNode()->getLayer()) {
                    auto &sourceHE = port2HyperedgeMap[sourcePort];
                    auto &targetHE = port2HyperedgeMap[targetPort];

                    if (sourceHE == nullptr && targetHE == nullptr) {
                        auto *hyperedge = new Hyperedge();
                        hyperedgeSet.insert(hyperedge);
                        hyperedge->edges.push_back(edge);
                        hyperedge->ports.push_back(sourcePort);
                        port2HyperedgeMap[sourcePort] = hyperedge;
                        hyperedge->ports.push_back(targetPort);
                        port2HyperedgeMap[targetPort] = hyperedge;
                    } else if (sourceHE == nullptr) {
                        targetHE->edges.push_back(edge);
                        targetHE->ports.push_back(sourcePort);
                        port2HyperedgeMap[sourcePort] = targetHE;
                    } else if (targetHE == nullptr) {
                        sourceHE->edges.push_back(edge);
                        sourceHE->ports.push_back(targetPort);
                        port2HyperedgeMap[targetPort] = sourceHE;
                    } else if (sourceHE == targetHE) {
                        sourceHE->edges.push_back(edge);
                    } else {
                        sourceHE->edges.push_back(edge);
                        for (auto &p : targetHE->ports) {
                            port2HyperedgeMap[p] = sourceHE;
                        }
                        vecAddAll(sourceHE->edges, targetHE->edges);
                        vecAddAll(sourceHE->ports, targetHE->ports);
                        hyperedgeSet.erase(targetHE);
                    }
                }
            }
        }
    }

    std::vector<Hyperedge *> hyperedges(hyperedgeSet.begin(), hyperedgeSet.end());
    auto &leftLayerRef = leftLayer[0]->getLayer();
    auto &rightLayerRef = rightLayer[0]->getLayer();
    for (auto *he : hyperedges) {
        he->upperLeft = sourceCount;
        he->upperRight = targetCount;
        for (auto &port : he->ports) {
            auto pos = portPos[port->id];
            if (port->getNode()->getLayer() == leftLayerRef) {
                if (pos < he->upperLeft) {
                    he->upperLeft = pos;
                }
                if (pos > he->lowerLeft) {
                    he->lowerLeft = pos;
                }
            } else if (port->getNode()->getLayer() == rightLayerRef) {
                if (pos < he->upperRight) {
                    he->upperRight = pos;
                }
                if (pos > he->lowerRight) {
                    he->lowerRight = pos;
                }
            }
        }
    }

    std::sort(hyperedges.begin(), hyperedges.end());

    std::vector<int> southSequence(hyperedges.size());
    std::vector<int> compressDeltas(targetCount + 1);
    for (int i = 0; i < hyperedges.size(); i++) {
        southSequence[i] = hyperedges[i]->upperRight;
        compressDeltas[southSequence[i]] = 1;
    }
    int delta = 0;
    for (int i = 0; i < compressDeltas.size(); i++) {
        if (compressDeltas[i] == 1) {
            compressDeltas[i] = delta;
        } else {
            delta--;
        }
    }
    int q = 0;
    for (int i = 0; i < southSequence.size(); i++) {
        southSequence[i] += compressDeltas[southSequence[i]];
        q = std::max(q, southSequence[i] + 1);
    }

    int firstIndex = 1;
    while (firstIndex < q) {
        firstIndex *= 2;
    }
    int treeSize = 2 * firstIndex - 1;
    firstIndex -= 1;
    std::vector<int> tree(treeSize);

    int crossings = 0;
    for (int k = 0; k < southSequence.size(); k++) {
        int index = southSequence[k] + firstIndex;
        tree[index]++;
        while (index > 0) {
            if (index % 2 > 0) {
                crossings += tree[index + 1];
            }
            index = (index - 1) / 2;
            tree[index]++;
        }
    }

    std::vector<HyperedgeCorner *> leftCorners(hyperedges.size() * 2);
    for (int i = 0; i < hyperedges.size(); i++) {
        leftCorners[2 * i] = new HyperedgeCorner(hyperedges[i], hyperedges[i]->upperLeft, hyperedges[i]->lowerLeft,
                                                 HyperedgeCorner::Type::UPPER);
        leftCorners[2 * i + 1] = new HyperedgeCorner(hyperedges[i], hyperedges[i]->lowerLeft, hyperedges[i]->upperLeft,
                                                     HyperedgeCorner::Type::LOWER);
    }

    std::sort(leftCorners.begin(), leftCorners.end());

    int openHyperedges = 0;
    for (auto &leftCorner : leftCorners) {
        switch (leftCorner->type) {
            case HyperedgeCorner::Type::UPPER:
                openHyperedges++;
                break;
            case HyperedgeCorner::Type::LOWER:
                openHyperedges--;
                crossings += openHyperedges;
                break;
        }
    }

    std::vector<HyperedgeCorner *> rightCorners(hyperedges.size() * 2);
    for (int i = 0; i < hyperedges.size(); i++) {
        rightCorners[2 * i] = new HyperedgeCorner(hyperedges[i], hyperedges[i]->upperRight, hyperedges[i]->lowerRight,
                                                  HyperedgeCorner::Type::UPPER);
        rightCorners[2 * i + 1] = new HyperedgeCorner(hyperedges[i], hyperedges[i]->lowerRight,
                                                      hyperedges[i]->upperRight, HyperedgeCorner::Type::LOWER);
    }

    std::sort(rightCorners.begin(), rightCorners.end());

    openHyperedges = 0;
    for (auto &rightCorner : rightCorners) {
        switch (rightCorner->type) {
            case HyperedgeCorner::Type::UPPER:
                openHyperedges++;
                break;
            case HyperedgeCorner::Type::LOWER:
                openHyperedges--;
                crossings += openHyperedges;
                break;
        }
    }
    return crossings;
}
}  // namespace GuiBridge
