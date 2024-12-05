#include "HyperEdgeSegment.h"
#include <algorithm>
#include <memory>
#include <utility>
#include "../../Edge.h"
#include "../../Node.h"
#include "../../Port.h"
#include "HyperEdgeSegmentDependency.h"
#include "OrthogonalRoutingGenerator.h"
#include "WestToEastRoutingStrategy.h"

namespace GuiBridge {
HyperEdgeSegment::HyperEdgeSegment(std::shared_ptr<WestToEastRoutingStrategy> &routingStrategy)
    : routingStrategy(routingStrategy){};

void HyperEdgeSegment::addPortPositions(std::shared_ptr<Node> &node, std::shared_ptr<Port> &port,
                                        PortToEdgeSegmentMap &hyperEdgeSegmentMap) {
    hyperEdgeSegmentMap.insert({EdgeTarget(node, port), shared_from_this()});
    ports.emplace_back(node, port);
    auto portPos = routingStrategy->getPortPositionOnHyperNode(node, port);

    if (port->portType == routingStrategy->getSourcePortSide()) {
        insertSorted(incomingConnectionCoordinates, portPos);
    } else {
        insertSorted(outgoingConnectionCoordinates, portPos);
    }

    recomputeExtent();

    for (auto &otherPort : node->getConnectedPorts(port)) {
        if (hyperEdgeSegmentMap.find(EdgeTarget(otherPort.node, otherPort.port)) == hyperEdgeSegmentMap.end()) {
            addPortPositions(otherPort.node, otherPort.port, hyperEdgeSegmentMap);
        }
    }
};

void HyperEdgeSegment::insertSorted(std::vector<double> &list, double value) {
    // 遍历 vector，找到插入的位置
    for (auto it = list.begin(); it != list.end(); ++it) {
        double next = *it;
        if (next == value) {
            // 如果 vector 中已存在相同的值
            return;
        }
        if (next > value) {
            // 找到第一个大于 value 的元素
            list.insert(it, value);  // 在当前位置插入 value
            return;
        }
    }

    // 如果遍历结束仍未插入，说明 value 比所有元素都大，直接加到末尾
    list.push_back(value);
};

std::vector<EdgeTarget> HyperEdgeSegment::getPorts() { return ports; };

int HyperEdgeSegment::getRoutingSlot() { return routingSlot; };

void HyperEdgeSegment::setRoutingSlot(int slot) { routingSlot = slot; };
double HyperEdgeSegment::getStartCoordinate() { return startPosition; };
double HyperEdgeSegment::getEndCoordinate() { return endPosition; };

std::vector<double> HyperEdgeSegment::getIncomingConnectionCoordinates() { return incomingConnectionCoordinates; };
std::vector<double> HyperEdgeSegment::getOutgoingConnectionCoordinates() { return outgoingConnectionCoordinates; };
std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &HyperEdgeSegment::getOutgoingSegmentDependencies() {
    return outgoingSegmentDependencies;
};
int HyperEdgeSegment::getOutWeight() { return outDepWeight; };
void HyperEdgeSegment::setOutWeight(int outWeight) { outDepWeight = outWeight; };
int HyperEdgeSegment::getCriticalOutWeight() { return criticalOutDepWeight; };
void HyperEdgeSegment::setCriticalOutWeight(int outWeight) { criticalInDepWeight = outWeight; };
std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &HyperEdgeSegment::getIncomingSegmentDependencies() {
    return incomingSegmentDependencies;
};

int HyperEdgeSegment::getInWeight() { return inDepWeight; };
void HyperEdgeSegment::setInWeight(int inWeight) { inDepWeight = inWeight; };
int HyperEdgeSegment::getCriticalInWeight() { return criticalInDepWeight; };
void HyperEdgeSegment::setCriticalInWeight(int inWeight) { criticalInDepWeight = inWeight; };

std::shared_ptr<HyperEdgeSegment> HyperEdgeSegment::getSplitPartner() { return splitPartner; };
void HyperEdgeSegment::setSplitPartner(std::shared_ptr<HyperEdgeSegment> &splitPartner) {
    this->splitPartner = splitPartner;
};
std::shared_ptr<HyperEdgeSegment> HyperEdgeSegment::getSplitBy() { return splitBy; };
void HyperEdgeSegment::setSplitBy(std::shared_ptr<HyperEdgeSegment> &splitBy) { this->splitBy = splitBy; };

double HyperEdgeSegment::getLength() { return getEndCoordinate() - getStartCoordinate(); };

bool HyperEdgeSegment::representsHyperedge() {
    return getIncomingConnectionCoordinates().size() + getOutgoingConnectionCoordinates().size() > 2;
};

bool HyperEdgeSegment::isDummy() { return splitPartner != nullptr && splitBy == nullptr; };

void HyperEdgeSegment::recomputeExtent() {
    startPosition = -9999;
    endPosition = -9999;

    recomputeExtent(incomingConnectionCoordinates);
    recomputeExtent(outgoingConnectionCoordinates);
};
void HyperEdgeSegment::recomputeExtent(std::vector<double> &positions) {
    if (!positions.empty()) {
        if (startPosition == -9999) {
            startPosition = positions.front();
        } else {
            startPosition = std::min(startPosition, positions.front());
        }

        if (endPosition == -9999) {
            endPosition = positions.back();
        } else {
            endPosition = std::max(endPosition, positions.back());
        }
    }
};

std::pair<std::shared_ptr<HyperEdgeSegment>, std::shared_ptr<HyperEdgeSegment>> HyperEdgeSegment::simulateSplit() {
    auto newSplit = std::make_shared<HyperEdgeSegment>(routingStrategy);
    auto newSplitPartner = std::make_shared<HyperEdgeSegment>(routingStrategy);

    newSplit->incomingConnectionCoordinates = incomingConnectionCoordinates;
    newSplit->splitBy = splitBy;
    newSplit->splitPartner = newSplitPartner;
    newSplit->recomputeExtent();

    newSplitPartner->outgoingConnectionCoordinates = outgoingConnectionCoordinates;
    newSplitPartner->splitPartner = newSplit;
    newSplitPartner->recomputeExtent();

    return std::make_pair(newSplit, newSplitPartner);
};

std::shared_ptr<HyperEdgeSegment> HyperEdgeSegment::splitAt(double splitPosition) {
    splitPartner = std::make_shared<HyperEdgeSegment>(routingStrategy);

    auto thisPtr = shared_from_this();
    splitPartner->setSplitPartner(thisPtr);

    splitPartner->outgoingConnectionCoordinates = outgoingConnectionCoordinates;
    outgoingConnectionCoordinates.clear();

    outgoingConnectionCoordinates.emplace_back(splitPosition);
    splitPartner->incomingConnectionCoordinates.emplace_back(splitPosition);

    recomputeExtent();
    splitPartner->recomputeExtent();

    while (!incomingSegmentDependencies.empty()) {
        incomingSegmentDependencies.at(0)->remove();
    }

    while (!outgoingSegmentDependencies.empty()) {
        outgoingSegmentDependencies.at(0)->remove();
    }
    return splitPartner;
};

}  // namespace GuiBridge