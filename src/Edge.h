#ifndef EDGE_HPP
#define EDGE_HPP

#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "math/KVectorChain.h"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

class Graph;
class Node;
class Port;

enum OppositeType { Src, Dst, None };

// using EdgeTarget = std::pair<std::shared_ptr<Node>, std::shared_ptr<Port>>;

// 定义 EdgeTarget 结构体来封装 std::pair
struct EdgeTarget {
    std::shared_ptr<Node> node;
    std::shared_ptr<Port> port;

    // 构造函数
    EdgeTarget(std::shared_ptr<Node> &n, std::shared_ptr<Port> &p) : node(n), port(p) {}
    EdgeTarget(std::weak_ptr<Node> &n, std::weak_ptr<Port> &p) : node(n), port(p) {}

    // 复制构造函数
    EdgeTarget(const EdgeTarget &other) = default;
};

class Edge : public std::enable_shared_from_this<Edge> {
public:
    Edge(std::shared_ptr<Node> &srcNode, std::shared_ptr<Port> &srcPort, std::shared_ptr<Node> &dstNode,
         std::shared_ptr<Port> &dstPort);

    EdgeTarget getSrc();
    EdgeTarget getDst();
    EdgeTarget getOther(const std::shared_ptr<Node> &someNode);
    EdgeTarget getOther(EdgeTarget &edgeTarget);
    // void setSrc(EdgeTarget &nextSrc);
    // void setDst(EdgeTarget &nextDst);

    KVectorChain getBendPoints();

    // std::shared_ptr<Port> setOppositePort(OppositeType oppositeType);
    // void revertOppositePort();

    // void reverse(const std::shared_ptr<Graph> &layeredGraph, bool adaptPorts);
    // bool reversed = false;

    bool isInLayerEdge();

    nlohmann::json json();

    // 用于 networksimplex 的属性
    int internalId;
    bool treeEdge;
    int delta = 1;
    int weight = 1;

private:
    std::weak_ptr<Node> srcNode;
    std::weak_ptr<Port> srcPort;
    std::weak_ptr<Node> dstNode;
    std::weak_ptr<Port> dstPort;

    std::string name;
    KVectorChain bendPoints;
    // OppositeType oppositeType = OppositeType::None;
    // 一端 port 被隐藏了，就存放在这里
    // std::weak_ptr<Port> oppositePort;

    // 存放原始数据
    // std::weak_ptr<Port> originSrc;
    // std::weak_ptr<Port> originDst;
};
}  // namespace GuiBridge
#endif  // EDGE_HPP
