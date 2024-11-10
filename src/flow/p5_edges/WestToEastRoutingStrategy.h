#ifndef WEST_TO_EAST_ROUTING_STRATEGY
#define WEST_TO_EAST_ROUTING_STRATEGY

#include <memory>
#include <set>
#include "../../opts/PortType.h"

namespace GuiBridge {
class Port;
class HyperEdgeSegment;
class KVector;
class Edge;

class WestToEastRoutingStrategy {
public:
    double getPortPositionOnHyperNode(std::shared_ptr<Port> &port);
    PortType getSourcePortSide();
    PortType getTargetPortSide();
    void calculateBendPoints(std::shared_ptr<HyperEdgeSegment> &segment, double startPos, double edgeSpacing);

    std::set<std::shared_ptr<KVector>> createdJunctionPoints;

    void clearCreatedJunctionPoints();

private:
    void addJunctionPointIfNecessary(std::shared_ptr<Edge> &edge, std::shared_ptr<HyperEdgeSegment> &segment,
                                     std::shared_ptr<KVector> &pos, bool vertical);
};
}  // namespace GuiBridge

#endif