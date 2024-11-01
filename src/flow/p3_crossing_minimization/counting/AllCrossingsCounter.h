#ifndef ALL_CROSSINGS_COUNTER_H
#define ALL_CROSSINGS_COUNTER_H

#include <memory>
#include <vector>

namespace GuiBridge {
class Node;  // 前向声明
class Edge;
class HyperedgeCrossingsCounter;
class CrossingCounter;

class AllCrossingsCounter {
public:
    explicit AllCrossingsCounter(const std::vector<std::vector<std::shared_ptr<Node>>> &graph);

    // 计数所有交叉
    int countAllCrossings(const std::vector<std::vector<std::shared_ptr<Node>>> &currentOrder);

    // 节点级别初始化
    void initAtNodeLevel(int l, int n, const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);

    // 端口级别初始化
    void initAtPortLevel(int l, int n, int p, const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);

    // 边级别初始化
    void initAtEdgeLevel(int l, int n, int p, int e, const std::shared_ptr<Edge> &edge,
                         const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);

    // 遍历后初始化
    void initAfterTraversal();

private:
    int countCrossingsAt(int layerIndex, const std::vector<std::vector<std::shared_ptr<Node>>> &currentOrder);

    CrossingCounter *crossingCounter;                      // 交叉计数器
    std::vector<bool> hasHyperEdgesEastOfIndex;            // 检查是否有超边在索引东侧
    HyperedgeCrossingsCounter *hyperedgeCrossingsCounter;  // 超边交叉计数器

    // 初始化相关
    std::vector<int> inLayerEdgeCounts;  // 每层的边计数
    int nPorts;                          // 端口数量
};
}  // namespace GuiBridge

#endif  // ALL_CROSSINGS_COUNTER_H