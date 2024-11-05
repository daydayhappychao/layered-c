#ifndef BK_NODE_PLACER_HPP
#define BK_NODE_PLACER_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace GuiBridge {
class Graph;
class Edge;
class Node;
class NeighborhoodInformation;

class BKNodePlacer {
public:
    // 布局处理器配置类型定义

    BKNodePlacer();
    void process(std::shared_ptr<Graph> &layeredGraph);

private:
    std::shared_ptr<Graph> lGraph;
    // 标记边的集合（类型1冲突）
    std::set<std::shared_ptr<Edge>> markedEdges;
    // 节点邻域等的预计算信息
    std::unique_ptr<NeighborhoodInformation> ni;
    // 是否生成平衡布局
    bool produceBalancedLayout;

    // 冲突检测相关方法
    void markConflicts(std::shared_ptr<Graph> &layeredGraph);
    bool incidentToInnerSegment(std::shared_ptr<Node> node, int layer1, int layer2);
    static std::shared_ptr<Edge> getEdge(std::shared_ptr<Node> source, std::shared_ptr<Node> target);
    // 布局平衡相关方法
    BKAlignedLayout *createBalancedLayout(const std::vector<BKAlignedLayout *> &layouts, int nodeCount);
    // 工具方法
    static std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> getBlocks(BKAlignedLayout *bal);
    static std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> getClasses(BKAlignedLayout *bal);
    bool checkOrderConstraint(std::shared_ptr<Graph> &layeredGraph, BKAlignedLayout *bal);
};
}  // namespace GuiBridge

#endif