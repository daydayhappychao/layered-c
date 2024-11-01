#ifndef ABSTRACT_BARYCENTER_PORT_DISTRIBUTOR_H
#define ABSTRACT_BARYCENTER_PORT_DISTRIBUTOR_H

#include <iostream>
#include <memory>
#include <vector>
#include "../../opts/PortType.h"
#include "ISweepPortDistributor.h"

// 定义一些可能需要的枚举类型

namespace GuiBridge {
class Node;
class Port;
class Edge;
/**
 * AbstractBarycenterPortDistributor 类
 */
class AbstractBarycenterPortDistributor : public ISweepPortDistributor {
private:
    // 端口排名数组
    std::vector<float> portRanks;
    float minBarycenter;
    float maxBarycenter;
    int nPorts;
    // 节点位置数组
    std::vector<std::vector<int>> nodePositions;
    std::vector<float> portBarycenter;
    std::vector<std::shared_ptr<Port>> inLayerPorts;

    // 计算给定节点的输入或输出端口的排名
    virtual float calculatePortRanks(std::shared_ptr<Node> &node, float rankSum, PortType type) = 0;

    // 分发端口
    void distributePorts(std::shared_ptr<Node> &node, PortType side);
    void distributePorts(std::shared_ptr<Node> &node, std::vector<std::shared_ptr<Port>> &ports, PortType side);

    void iteratePortsAndCollectInLayerPorts(std::shared_ptr<Node> &node, std::vector<std::shared_ptr<Port>> &ports,
                                            PortType side);

    // 计算端口的重心值
    void calculateInLayerPortsBarycenterValues(std::shared_ptr<Node> &node);

    // 获取节点位置
    int positionOf(const std::shared_ptr<Node> &node);

    // 更新节点位置
    void updateNodePositions(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex);

    // 判断是否不是第一层
    bool isNotFirstLayer(int length, int currentIndex, bool isForwardSweep);

    // 获取端口类型
    PortType portTypeFor(bool isForwardSweep);

    // 对端口进行排序
    void sortPorts(std::shared_ptr<Node> &node);

public:
    // 构造函数
    explicit AbstractBarycenterPortDistributor(int numLayers);
    // 确定所有特定类型端口在给定层的排名
    void calculatePortRanks(std::vector<std::shared_ptr<Node>> &layer, PortType portType);
    // 获取端口排名
    std::vector<float> &getPortRanks();

    // 分发端口
    bool distributePortsWhileSweeping(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex,
                                      bool isForwardSweep);

    // 初始化层级别
    void initAtLayerLevel(int l, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);

    // 初始化节点级别
    void initAtNodeLevel(int l, int n, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);

    // 初始化端口级别
    void initAtPortLevel(int l, int n, int p, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);

    // 初始化遍历后
    void initAfterTraversal();
};
}  // namespace GuiBridge

#endif  // ABSTRACT_BARYCENTER_PORT_DISTRIBUTOR_H