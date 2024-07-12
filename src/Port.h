#ifndef PORT_HPP
#define PORT_HPP

#include <memory>
#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"

namespace GuiBridge {
class Node;  // 前向声明
class Edge;
class Port {
public:
    explicit Port(std::string name);

    std::shared_ptr<Node> getNode();
    void setNode(const std::shared_ptr<Node> &node);

    void addEdge(const std::shared_ptr<Edge> &edge);
    std::vector<std::shared_ptr<Edge>> getEdges();

    std::string getName();

    void setInternalCollect(bool);
    bool getInternalCollect();

    std::vector<std::shared_ptr<Port>> getConnectedPorts(); 
    void addConnectedPort(const std::shared_ptr<Port> &port);

    nlohmann::json json();

private:
    std::string name;
    std::weak_ptr<Node> node;
    std::vector<std::weak_ptr<Edge>> edges;

    std::vector<std::weak_ptr<Port>> connectedPorts;

    bool internal_collect = false;
    int x = 0;
    int y = 0;
};
}  // namespace GuiBridge
#endif  // PORT_HPP
