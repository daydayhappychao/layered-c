#ifndef LAYER_SWEEP_TYPE_DECIDER_H
#define LAYER_SWEEP_TYPE_DECIDER_H

#include <memory>
#include <string>
#include <vector>
#include "../../Edge.h"
#include "../../Node.h"
#include "./counting/Initializable.h"
#include "GraphInfoHolder.h"

namespace GuiBridge {
class Node;
class Edge;
/**
 * 决定是否使用自下而上的扫掠方法。
 */
class LayerSweepTypeDecider {
private:
    struct NodeInfo {
        int connectedEdges = 0;         // 连接边的数量
        int hierarchicalInfluence = 0;  // 分层影响
        int randomInfluence = 0;        // 随机影响

        void transfer(const NodeInfo &nodeInfo);
        std::string toString() const;
    };

    std::vector<std::vector<NodeInfo>> nodeInfo;  // 节点信息数组
    GraphInfoHolder &graphData;                   // 图形信息持有者

public:
    explicit LayerSweepTypeDecider(GraphInfoHolder &graphData);
    bool useBottomUp();
    void initAtLayerLevel(int l, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);
    void initAtNodeLevel(int l, int n, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);

private:
    void transferInfoToTarget(NodeInfo &currentNode, std::shared_ptr<Edge> &edge);
    void transferInfoTo(NodeInfo &currentNode, std::shared_ptr<Node> &target);
    NodeInfo &nodeInfoFor(std::shared_ptr<Node> &n);
    bool rootNode();
    bool fixedPortOrder();
    bool fewerThanTwoInOutEdges();
    bool bottomUpForced(double boundary);
    bool hasNoEasternPorts(std::shared_ptr<Node> &node);
    bool hasNoWesternPorts(std::shared_ptr<Node> &node);
    bool isExternalPortDummy(std::shared_ptr<Node> &node);
    bool isNorthSouthDummy(std::shared_ptr<Node> &node);
    bool isEasternDummy(std::shared_ptr<Node> &node);
};
}  // namespace GuiBridge

#endif  // LAYER_SWEEP_TYPE_DECIDER_H