#ifndef ORTHOGONAL_ROUTING_GENERATOR_HPP
#define ORTHOGONAL_ROUTING_GENERATOR_HPP
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>
#include "../../opts/PortType.h"

namespace GuiBridge {

class Graph;
class Node;
class HyperEdgeSegment;
class Port;
class HyperEdgeSegmentSplitter;
class WestToEastRoutingStrategy;

class OrthogonalRoutingGenerator {
public:
    static const double TOLERANCE;
    static const int CRITICAL_CONFLICTS_DETECTED;
    static const double CONFLICT_THRESHOLD_FACTOR;
    static const double CRITICAL_CONFLICT_THRESHOLD_FACTOR;
    static const int CONFLICT_PENALTY;
    static const int CROSSING_PENALTY;

    explicit OrthogonalRoutingGenerator(double edgeSpacing);
    int routeEdges(std::shared_ptr<Graph> &layeredGraph, const std::vector<std::shared_ptr<Node>> &sourceLayerNodes,
                   int sourceLayerIndex, const std::vector<std::shared_ptr<Node>> &targetLayerNodes, double startPos);

private:
    double minimumHorizontalSegmentDistance(const std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments);
    double minimumDifference(const std::vector<double> &numberStream);
    void createHyperEdgeSegments(
        const std::vector<std::shared_ptr<Node>> &nodes, PortType portSide,
        std::vector<std::shared_ptr<HyperEdgeSegment>> &hyperEdges,
        std::map<std::shared_ptr<Port>, std::shared_ptr<HyperEdgeSegment>> &portToHyperEdgeSegmentMap);
    int createDependencyIfNecessary(std::shared_ptr<HyperEdgeSegment> &he1, std::shared_ptr<HyperEdgeSegment> &he2);
    int countConflicts(const std::vector<double> &posis1, const std::vector<double> &posis2);
    static int countCrossings(const std::vector<double> &posis, double start, double end);
    void breakCriticalCycles(std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments,
                             std::random_device::result_type random);
    static void breakNonCriticalCycles(std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments,
                                       std::random_device::result_type random);
    void topologicalNumbering(std::vector<std::shared_ptr<HyperEdgeSegment>> &segments);

    std::shared_ptr<HyperEdgeSegmentSplitter> segmentSplitter;
    std::shared_ptr<WestToEastRoutingStrategy> routingStrategy;
    double edgeSpacing;
    double conflictThreshold;
    double criticalConflictThreshold;
    std::string debugPrefix;
};
}  // namespace GuiBridge

#endif