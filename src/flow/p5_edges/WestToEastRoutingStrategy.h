#pragma once

#include <memory>
#include <set>
#include <vector>
#include "../../opts/PortType.h"

namespace GuiBridge {
class Port;
class Node;
class HyperEdgeSegment;
class KVector;
class Edge;

class WestToEastRoutingStrategy {
public:
    float getPortPositionOnHyperNode(std::shared_ptr<Node> &node, std::shared_ptr<Port> &port);
    PortType getSourcePortSide();
    PortType getTargetPortSide();
    void calculateBendPoints(std::shared_ptr<HyperEdgeSegment> &segment, double startPos, double edgeSpacing);

    std::vector<KVector> createdJunctionPoints;

    void clearCreatedJunctionPoints();

private:
    void addJunctionPointIfNecessary(std::shared_ptr<Edge> &edge, std::shared_ptr<HyperEdgeSegment> &segment,
                                     KVector &pos, bool vertical);
};
}  // namespace GuiBridge
