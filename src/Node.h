

#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Shape.h"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

class Graph;
class Layer;
class Edge;
class Port;

class Node : public std::enable_shared_from_this<Node>, public Shape {
public:
    explicit Node(std::string name);
    int getId() const;
    void setId(int id);
    void addInputPort(const std::shared_ptr<Port> &port);
    void addOutputPort(const std::shared_ptr<Port> &port);
    std::vector<std::shared_ptr<Port>> &getInputPorts();
    std::vector<std::shared_ptr<Port>> &getOutputPorts();
    std::vector<std::shared_ptr<Port>> getAllPorts();
    std::vector<std::shared_ptr<Edge>> getEdges();
    std::shared_ptr<Layer> &getLayer();
    std::shared_ptr<Graph> &getGraph();
    void setGraph(std::shared_ptr<Graph> &nextGraph);

    nlohmann::json json();

private:
    int id;
    std::string name;
    std::vector<std::shared_ptr<Port>> inputPorts;
    std::vector<std::shared_ptr<Port>> outputPorts;
    std::shared_ptr<Graph> graph;
    std::shared_ptr<Layer> layer = nullptr;
};
}  // namespace GuiBridge
#endif  // NODE_HPP