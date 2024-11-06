#ifndef BK_COMPACTOR_HPP
#define BK_COMPACTOR_HPP

#include <list>
#include <map>
#include <memory>
#include <queue>
#include <vector>
#include "ThresholdStrategy.h"

namespace GuiBridge {
class Graph;
class NeighborhoodInformation;
class BKAlignedLayout;
class ClassNode;

class ClassEdge {
public:
    // SUPPRESS CHECKSTYLE NEXT 2 VisibilityModifier
    double separation = 0;
    std::shared_ptr<ClassNode> target;
};

class ClassNode {
    // SUPPRESS CHECKSTYLE NEXT 5 VisibilityModifier
public:
    double classShift;
    std::shared_ptr<Node> node;
    std::list<std::shared_ptr<ClassEdge>> outgoing;
    int indegree = 0;

    void addEdge(std::shared_ptr<ClassNode> &target, double separation);
};

class BKCompactor {
public:
    BKCompactor(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<NeighborhoodInformation> &ni);
    void horizontalCompaction(std::shared_ptr<BKAlignedLayout> &bal);

private:
    std::shared_ptr<Graph> layeredGraph;
    std::shared_ptr<ThresholdStrategy> threshStrategy;
    std::shared_ptr<NeighborhoodInformation> ni;
    std::map<std::shared_ptr<Node>, std::shared_ptr<ClassNode>> sinkNodes;

    void placeBlock(std::shared_ptr<Node> &root, std::shared_ptr<BKAlignedLayout> &bal);
    void placeClasses(std::shared_ptr<BKAlignedLayout> &bal);
    std::shared_ptr<ClassNode> getOrCreateClassNode(std::shared_ptr<Node> &sinkNode,
                                                    std::shared_ptr<BKAlignedLayout> &bal);
};
}  // namespace GuiBridge

#endif