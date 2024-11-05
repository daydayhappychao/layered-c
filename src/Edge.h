#ifndef EDGE_HPP
#define EDGE_HPP

#include <memory>
#include <optional>
#include <string>

#include "Node.h"
#include "Port.h"
#include "math/KVectorChain.h"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

class Graph;

enum OppositeType { Src, Dst, None };
class Edge : public std::enable_shared_from_this<Edge> {
public:
    Edge(const std::shared_ptr<Port> &src, const std::shared_ptr<Port> &dst);

    std::shared_ptr<Port> getSrc();
    void setSrc(const std::shared_ptr<Port> &nextSrc);
    std::shared_ptr<Port> getDst();
    void setDst(const std::shared_ptr<Port> &nextDst);

    std::shared_ptr<Port> getOther(const std::shared_ptr<Node> &someNode);

    KVectorChain getBendPoints();

    std::shared_ptr<Port> setOppositePort(OppositeType oppositeType);
    void revertOppositePort();

    void reverse(const std::shared_ptr<Graph> &layeredGraph, bool adaptPorts);
    bool reversed = false;

    bool isInLayerEdge();

    nlohmann::json json();

    // 用于 networksimplex 的属性
    int internalId;
    bool treeEdge;
    int delta = 1;
    int weight = 1;

private:
    std::weak_ptr<Port> src;
    std::weak_ptr<Port> dst;
    std::string name;
    KVectorChain bendPoints;
    OppositeType oppositeType = OppositeType::None;
    // 一端 port 被隐藏了，就存放在这里
    std::weak_ptr<Port> oppositePort;

    // 存放原始数据
    std::weak_ptr<Port> originSrc;
    std::weak_ptr<Port> originDst;
};
}  // namespace GuiBridge
#endif  // EDGE_HPP
