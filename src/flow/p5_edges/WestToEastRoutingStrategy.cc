#include "WestToEastRoutingStrategy.h"
#include <type_traits>
#include <valarray>
#include "../../Edge.h"
#include "../../Node.h"
#include "../../Port.h"
#include "HyperEdgeSegment.h"
#include "OrthogonalRoutingGenerator.h"

namespace GuiBridge {
float WestToEastRoutingStrategy::getPortPositionOnHyperNode(std::shared_ptr<Node> &node, std::shared_ptr<Port> &port) {
    return node->getPos().y + port->getPos().y + port->getAnchor().y;
}

PortType WestToEastRoutingStrategy::getSourcePortSide() { return PortType::OUTPUT; };
PortType WestToEastRoutingStrategy::getTargetPortSide() { return PortType::INPUT; };

void WestToEastRoutingStrategy::calculateBendPoints(std::shared_ptr<HyperEdgeSegment> &segment, double startPos,
                                                    double edgeSpacing) {
    if (segment->isDummy()) {
        return;
    }

    float segmentX = startPos + segment->getRoutingSlot() * edgeSpacing;

    for (auto &edgeTarget : segment->getPorts()) {
        float sourceY = edgeTarget.node->getPortPos(edgeTarget.port).y;

        if (edgeTarget.port->portType == PortType::OUTPUT) {
            for (auto &edge : edgeTarget.node->getEdgesByPort(edgeTarget.port)) {
                auto target = edge->getDst();
                float targetY = edge->getDstPoint().y;

                if (std::abs(sourceY - targetY) > OrthogonalRoutingGenerator::TOLERANCE) {
                    float currentX = segmentX;
                    auto currentSegment = segment;

                    KVector bend(currentX, sourceY);
                    edge->getBendPoints().emplace_back(bend);
                    addJunctionPointIfNecessary(edge, currentSegment, bend, true);

                    auto splitPartner = segment->getSplitPartner();
                    // if (splitPartner != nullptr) {
                    //     float splitY = splitPartner->getIncomingConnectionCoordinates().at(0);

                    //     bend = KVector(currentX, splitY);
                    //     edge->getBendPoints().emplace_back(bend);
                    //     addJunctionPointIfNecessary(edge, currentSegment, bend, true);

                    //     currentX = startPos + splitPartner->getRoutingSlot() * edgeSpacing;
                    //     currentSegment = splitPartner;

                    //     bend = KVector(currentX, splitY);
                    //     edge->getBendPoints().emplace_back(bend);
                    //     addJunctionPointIfNecessary(edge, currentSegment, bend, true);
                    // }

                    bend = KVector(currentX, targetY);
                    edge->getBendPoints().emplace_back(bend);
                    addJunctionPointIfNecessary(edge, currentSegment, bend, true);
                }
            }
        }
    }
};

void WestToEastRoutingStrategy::clearCreatedJunctionPoints() { createdJunctionPoints.clear(); };

void WestToEastRoutingStrategy::addJunctionPointIfNecessary(std::shared_ptr<Edge> &edge,
                                                            std::shared_ptr<HyperEdgeSegment> &segment, KVector &pos,
                                                            bool vertical) {
    float p = pos.y;
    for (auto &kv : createdJunctionPoints) {
        if (kv.equals(pos)) {
            return;
        }
    }
    bool pointInsideEdgeSegment = p > segment->getStartCoordinate() && p < segment->getEndCoordinate();

    bool pointAtSegmentBoundary = false;

    if (!segment->getIncomingConnectionCoordinates().empty() && !segment->getOutgoingConnectionCoordinates().empty()) {
        pointAtSegmentBoundary |=
            std::abs(p - segment->getIncomingConnectionCoordinates().front()) < OrthogonalRoutingGenerator::TOLERANCE &&
            std::abs(p - segment->getOutgoingConnectionCoordinates().front()) < OrthogonalRoutingGenerator::TOLERANCE;

        pointAtSegmentBoundary |=
            std::abs(p - segment->getIncomingConnectionCoordinates().back()) < OrthogonalRoutingGenerator::TOLERANCE &&
            std::abs(p - segment->getOutgoingConnectionCoordinates().back()) < OrthogonalRoutingGenerator::TOLERANCE;

        if (pointInsideEdgeSegment || pointAtSegmentBoundary) {
            const KVector &jpoint(pos);
            createdJunctionPoints.emplace_back(jpoint);
        }
    }
};

}  // namespace GuiBridge