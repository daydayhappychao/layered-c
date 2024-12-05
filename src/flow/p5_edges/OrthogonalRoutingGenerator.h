#ifndef ORTHOGONAL_ROUTING_GENERATOR_HPP
#define ORTHOGONAL_ROUTING_GENERATOR_HPP
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>
#include "../../opts/PortType.h"

namespace GuiBridge {

class Graph;
class Node;
class HyperEdgeSegment;
class Port;
class EdgeTarget;
class HyperEdgeSegmentSplitter;
class WestToEastRoutingStrategy;
class EdgeTargetHash;

using PortToEdgeSegmentMap = std::unordered_map<EdgeTarget, std::shared_ptr<HyperEdgeSegment>, EdgeTargetHash>;

class OrthogonalRoutingGenerator : public std::enable_shared_from_this<OrthogonalRoutingGenerator> {
public:
    constexpr static const double TOLERANCE = 1e-3;
    static const int CRITICAL_CONFLICTS_DETECTED = -1;
    constexpr static const double CONFLICT_THRESHOLD_FACTOR = 0.5;
    constexpr static const double CRITICAL_CONFLICT_THRESHOLD_FACTOR = 0.2;
    static const int CONFLICT_PENALTY = 1;
    static const int CROSSING_PENALTY = 16;

    explicit OrthogonalRoutingGenerator(double edgeSpacing);
    int routeEdges(std::shared_ptr<Graph> &layeredGraph, std::vector<std::shared_ptr<Node>> &sourceLayerNodes,
                   int sourceLayerIndex, std::vector<std::shared_ptr<Node>> &targetLayerNodes, double startPos);
    int createDependencyIfNecessary(std::shared_ptr<HyperEdgeSegment> &he1, std::shared_ptr<HyperEdgeSegment> &he2);
    static int countCrossings(const std::vector<double> &posis, double start, double end);

private:
    double minimumHorizontalSegmentDistance(const std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments);
    double minimumDifference(const std::vector<double> &numberStream);
    void createHyperEdgeSegments(std::vector<std::shared_ptr<Node>> &nodes, PortType portSide,
                                 std::vector<std::shared_ptr<HyperEdgeSegment>> &hyperEdges,
                                 PortToEdgeSegmentMap &portToHyperEdgeSegmentMap);
    int countConflicts(const std::vector<double> &posis1, const std::vector<double> &posis2);
    void breakCriticalCycles(std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments);
    void breakNonCriticalCycles(std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments);
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