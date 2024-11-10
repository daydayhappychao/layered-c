#ifndef HYPER_EDGE_SEGMENT_HPP
#define HYPER_EDGE_SEGMENT_HPP

#include <list>
#include <map>
#include <vector>

namespace GuiBridge {

class Port;
class WestToEastRoutingStrategy;
class HyperEdgeSegmentDependency;

class HyperEdgeSegment : public std::enable_shared_from_this<HyperEdgeSegment> {
public:
    explicit HyperEdgeSegment(std::shared_ptr<WestToEastRoutingStrategy> &routingStrategy);

    void addPortPositions(std::shared_ptr<Port> &port,
                          std::map<std::shared_ptr<Port>, std::shared_ptr<HyperEdgeSegment>> &hyperEdgeSegmentMap);
    std::list<double> getIncomingConnectionCoordinates();
    std::list<double> getOutgoingConnectionCoordinates();
    std::vector<std::shared_ptr<Port>> getPorts();
    int getRoutingSlot();
    void setRoutingSlot(int slot);
    double getStartCoordinate();
    double getEndCoordinate();
    std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &getOutgoingSegmentDependencies();
    int getOutWeight();
    void setOutWeight(int outWeight);
    int getCriticalOutWeight();
    void setCriticalOutWeight(int outWeight);
    std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &getIncomingSegmentDependencies();
    int getInWeight();
    void setInWeight(int inWeight);
    int getCriticalInWeight();
    void setCriticalInWeight(int inWeight);
    std::shared_ptr<HyperEdgeSegment> getSplitPartner();
    void setSplitPartner(std::shared_ptr<HyperEdgeSegment> &splitPartner);
    std::shared_ptr<HyperEdgeSegment> getSplitBy();
    void setSplitBy(std::shared_ptr<HyperEdgeSegment> &splitBy);
    double getLength();
    bool representsHyperedge();
    bool isDummy();
    void recomputeExtent();
    std::pair<std::shared_ptr<HyperEdgeSegment>, std::shared_ptr<HyperEdgeSegment>> simulateSplit();
    std::shared_ptr<HyperEdgeSegment> splitAt(double splitPosition);
    int compareTo(std::shared_ptr<HyperEdgeSegment> &other);
    bool equals(std::shared_ptr<HyperEdgeSegment> &other);

private:
    static void insertSorted(std::list<double> &list, double value);
    void recomputeExtent(std::list<double> &positions);

    std::shared_ptr<WestToEastRoutingStrategy> routingStrategy;
    std::vector<std::shared_ptr<Port>> ports;
    int mark;
    int routingSlot;
    double startPosition;
    double endPosition;
    std::list<double> incomingConnectionCoordinates;
    std::list<double> outgoingConnectionCoordinates;
    std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> outgoingSegmentDependencies;
    int outDepWeight;
    int criticalOutDepWeight;
    std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> incomingSegmentDependencies;
    int inDepWeight;
    int criticalInDepWeight;
    std::shared_ptr<HyperEdgeSegment> splitPartner;
    std::shared_ptr<HyperEdgeSegment> splitBy;
};
}  // namespace GuiBridge

#endif