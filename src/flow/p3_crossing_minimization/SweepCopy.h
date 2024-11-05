#ifndef SWEEP_COPY_H
#define SWEEP_COPY_H

#include <algorithm>
#include <memory>
#include <set>
#include <vector>

namespace GuiBridge {
class Node;
class Graph;
class Port;

/**
 * 存储节点和端口顺序以供扫掠使用。
 */
class SweepCopy {
private:
    std::vector<std::vector<std::shared_ptr<Node>>> nodeOrder;                     // 保存节点顺序的副本
    std::vector<std::vector<std::vector<std::shared_ptr<Port>>>> inputPortOrders;  // 每个入节点的端口顺序副本
    std::vector<std::vector<std::vector<std::shared_ptr<Port>>>> outputPortOrders;  // 每个出节点的端口顺序副本

public:
    explicit SweepCopy(const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrderIn);
    explicit SweepCopy(const std::shared_ptr<SweepCopy> &sc);

    std::vector<std::vector<std::shared_ptr<Node>>> nodes() const;
    void transferNodeAndPortOrdersToGraph(std::shared_ptr<Graph> &lGraph, bool setPortConstraints);

private:
    std::vector<std::vector<std::shared_ptr<Node>>> deepCopy(
        const std::vector<std::vector<std::shared_ptr<Node>>> &currentlyBestNodeOrder);
    std::shared_ptr<Node> assertCorrectPortSides(const std::shared_ptr<Node> &dummy);
};
}  // namespace GuiBridge

#endif  // SWEEP_COPY_H