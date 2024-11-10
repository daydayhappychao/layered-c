#ifndef PORT_HPP
#define PORT_HPP

#include <memory>
#include <string>
#include <vector>

#include "Shape.h"
#include "math/KVector.h"
#include "nlohmann/json_fwd.hpp"
#include "opts/PortType.h"

namespace GuiBridge {
class Node;  // 前向声明
class Edge;
class NodeProto;
class Port : public std::enable_shared_from_this<Port>, public Shape {
public:
    explicit Port(std::string name, PortType portType);

    // std::shared_ptr<Node> getNode();
    // void setNode(const std::shared_ptr<Node> &node);

    // void addEdge(const std::shared_ptr<Edge> &edge);
    // std::vector<std::shared_ptr<Edge>> getEdges();
    // void removeEdge(std::shared_ptr<Edge> &edge);

    // std::string getName();

    // void setInternalCollect(bool);
    // bool getInternalCollect();

    // std::vector<std::shared_ptr<Port>> getConnectedPorts();

    // PortType getType();
    PortType portType;
    std::shared_ptr<NodeProto> nodeProto;

    KVector getAnchor();

    nlohmann::json json();

    int id;
    std::string name;

private:
    KVector anchor;

    bool internal_collect = false;
};
}  // namespace GuiBridge
#endif  // PORT_HPP
