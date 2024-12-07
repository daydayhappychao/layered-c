#ifndef EDGE_HPP
#define EDGE_HPP

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "./Node.h"
#include "./Port.h"
#include "math/KVector.h"
#include "math/KVectorChain.h"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

class Graph;
class Node;
class Port;

enum OppositeType { Src, Dst, None };

struct EdgeTarget {
    std::shared_ptr<Node> node;
    std::shared_ptr<Port> port;

    // 构造函数
    EdgeTarget(std::shared_ptr<Node> &n, std::shared_ptr<Port> &p) : node(n), port(p) {}
    EdgeTarget(std::weak_ptr<Node> &n, std::weak_ptr<Port> &p) : node(n), port(p) {}

    // 复制构造函数
    EdgeTarget(const EdgeTarget &other) = default;

    // 定义相等运算符
    bool operator==(const EdgeTarget &other) const {
        return node->_id == other.node->_id && port->_id == other.port->_id;
    }
};

struct EdgeTargetHash {
    size_t operator()(const EdgeTarget &target) const {
        size_t hash_node = std::hash<int>{}(target.node->_id);
        size_t hash_port = std::hash<int>{}(target.port->_id);
        return hash_node ^ (hash_port << 1);  // Combine the hashes
    }
};

class Edge : public std::enable_shared_from_this<Edge> {
public:
    Edge(std::shared_ptr<Node> &srcNode, std::shared_ptr<Port> &srcPort, std::shared_ptr<Node> &dstNode,
         std::shared_ptr<Port> &dstPort);

    EdgeTarget getSrc();
    EdgeTarget getDst();
    EdgeTarget getOther(const std::shared_ptr<Node> &someNode);
    EdgeTarget getOther(EdgeTarget &edgeTarget);
    // 该方法会修改原始数据，使数据与原始数据不一致，一定要记得在事后将数据修复回来
    void setSrc(EdgeTarget &nextSrc);
    // 该方法会修改原始数据，使数据与原始数据不一致，一定要记得在事后将数据修复回来
    void setDst(EdgeTarget &nextDst);

    // 获取src坐标
    KVector getSrcPoint();
    // 获取dst坐标
    KVector getDstPoint();
    KVectorChain &getBendPoints();

    bool isInLayerEdge();

    nlohmann::json json();

    // 用于 networksimplex 的属性
    int internalId;
    bool treeEdge;
    int delta = 1;
    int weight = 1;

    // 隐藏相关

    // 是否隐藏
    bool isHidden = false;
    // 设置状态为隐藏，在node中不再有该edge信息
    void hidden();
    // 恢复状态为显示，在node中保存该 edge 信息
    void show();

private:
    std::weak_ptr<Node> srcNode;
    std::weak_ptr<Port> srcPort;
    std::weak_ptr<Node> dstNode;
    std::weak_ptr<Port> dstPort;

    std::string name;
    KVectorChain bendPoints;
};
}  // namespace GuiBridge
#endif  // EDGE_HPP
