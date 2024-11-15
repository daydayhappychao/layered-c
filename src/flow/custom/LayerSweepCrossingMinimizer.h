#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>
namespace GuiBridge {
class Graph;
class Node;
class Port;
class EdgeTarget;

/**
 * Neighbor 值只能用于同层比较，跨层就要比 index
 *
 */
class Neighbor {
public:
    explicit Neighbor(std::shared_ptr<Node> &node, std::shared_ptr<Graph> &graph);

    std::shared_ptr<Node> node;
    std::vector<std::shared_ptr<Node>> leftNeighborNodes;
    std::vector<std::shared_ptr<Node>> rightNeighborNodes;

    std::map<std::shared_ptr<Port>, std::vector<EdgeTarget>> portNeighbor;
    std::map<std::shared_ptr<Port>, int> portBarycenter;
    int barycenter = 0;
};

class LayerSweepCrossingMinimizer {
public:
    void process(std::shared_ptr<Graph> &graph);

private:
    std::vector<std::vector<int>> layerNodeIdPos;

    // 左侧outputPort的列表
    std::vector<std::vector<std::pair<int, int>>> layerLeftPortIdPos;
    std::vector<std::vector<std::pair<int, int>>> layerRightPortIdPos;

    std::map<std::shared_ptr<Node>, std::shared_ptr<Neighbor>> neighborData;

    void sortLayerNode(std::shared_ptr<Graph> &graph);
    void sortNodePort(std::shared_ptr<Graph> &graph);

    void syncNodePos(std::shared_ptr<Graph> &graph);
};
}  // namespace GuiBridge