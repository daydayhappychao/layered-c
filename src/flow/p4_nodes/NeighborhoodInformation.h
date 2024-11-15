#ifndef NEIGHBORHOODINFORMATION_H
#define NEIGHBORHOODINFORMATION_H

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace GuiBridge {
class Node;
class Graph;
class Edge;

using Neighbor = std::pair<std::shared_ptr<Node>, std::shared_ptr<Edge>>;

class NeighborhoodInformation {
public:
    static std::shared_ptr<NeighborhoodInformation> buildFor(const std::shared_ptr<Graph> &graph);

    void cleanup();

    int getNodeCount() const { return nodeCount; }
    const std::vector<int> &getLayerIndex() const { return layerIndex; }
    const std::vector<int> &getNodeIndex() const { return nodeIndex; }
    const std::vector<std::vector<Neighbor>> &getLeftNeighbors() const { return leftNeighbors; }
    const std::vector<std::vector<Neighbor>> &getRightNeighbors() const { return rightNeighbors; }

    std::vector<int> layerIndex;
    std::vector<int> nodeIndex;
    std::vector<std::vector<Neighbor>> leftNeighbors;
    std::vector<std::vector<Neighbor>> rightNeighbors;

private:
    static void determineAllLeftNeighbors(const std::shared_ptr<NeighborhoodInformation> &ni,
                                          const std::shared_ptr<Graph> &graph);
    static void determineAllRightNeighbors(const std::shared_ptr<NeighborhoodInformation> &ni,
                                           const std::shared_ptr<Graph> &graph);

    // 节点数量
    int nodeCount;
};

}  // namespace GuiBridge

#endif  // NEIGHBORHOODINFORMATION_H