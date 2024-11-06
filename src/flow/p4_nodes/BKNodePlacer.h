#ifndef BK_NODE_PLACER_HPP
#define BK_NODE_PLACER_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "BKAlignedLayout.h"

namespace GuiBridge {
class Graph;
class Edge;
class Node;
class NeighborhoodInformation;

class BKNodePlacer {
public:
    void process(std::shared_ptr<Graph> &layeredGraph);
    static std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> getBlocks(
        std::shared_ptr<BKAlignedLayout> &bal);
    static std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> getClasses(
        std::shared_ptr<BKAlignedLayout> &bal);
    static std::shared_ptr<Edge> getEdge(std::shared_ptr<Node> &source, std::shared_ptr<Node> &target);

private:
    int MIN_LAYERS_FOR_CONFLICTS = 3;
    std::shared_ptr<Graph> lGraph;
    // 标记边的集合（类型1冲突）
    std::set<std::shared_ptr<Edge>> markedEdges;
    // 节点邻域等的预计算信息
    std::shared_ptr<NeighborhoodInformation> ni;
    // 是否生成平衡布局
    bool produceBalancedLayout = true;

    // 冲突检测相关方法
    void markConflicts(std::shared_ptr<Graph> &layeredGraph);
    bool incidentToInnerSegment(std::shared_ptr<Node> &node, int layer1, int layer2);
    // 布局平衡相关方法
    std::shared_ptr<BKAlignedLayout> createBalancedLayout(const std::vector<std::shared_ptr<BKAlignedLayout>> &layouts,
                                                          int nodeCount);
    // 工具方法

    bool checkOrderConstraint(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<BKAlignedLayout> &bal);
};
}  // namespace GuiBridge

#endif