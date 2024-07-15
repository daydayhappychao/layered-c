#include "Node.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Edge.h"
#include "Port.h"
#include "Shape.h"
#include "nlohmann/json.hpp"
namespace GuiBridge {

Node::Node(std::string name) : name(std::move(name)) {}

int Node::getId() const { return id; }
void Node::setId(int id) { this->id = id; }

void Node::addInputPort(const std::shared_ptr<Port> &port) {
    if (port->getNode() != nullptr) {
        throw std::runtime_error("port {} is already used in a node");
    }
    inputPorts.push_back(port);
    auto ptr = shared_from_this();

    port->setNode(ptr);
}

void Node::addOutputPort(const std::shared_ptr<Port> &port) {
    if (port->getNode() != nullptr) {
        throw std::runtime_error("port {} is already used in a node");
    }
    outputPorts.push_back(port);
    auto ptr = shared_from_this();

    port->setNode(ptr);
}

std::vector<std::shared_ptr<Port>> &Node::getInputPorts() { return inputPorts; }

std::vector<std::shared_ptr<Port>> &Node::getOutputPorts() { return outputPorts; }

std::vector<std::shared_ptr<Port>> Node::getAllPorts() {
    std::vector<std::shared_ptr<Port>> allPorts;
    allPorts.insert(allPorts.end(), inputPorts.begin(), inputPorts.end());
    allPorts.insert(allPorts.end(), outputPorts.begin(), outputPorts.end());
    return allPorts;
}

std::vector<std::shared_ptr<Edge>> Node::getEdges() {
    std::vector<std::shared_ptr<Edge>> nodeEdges;
    for (auto &port : inputPorts) {
        for (const auto &edge : port->getEdges()) {
            nodeEdges.push_back(edge);
        }
    }
    for (auto &port : outputPorts) {
        for (const auto &edge : port->getEdges()) {
            nodeEdges.push_back(edge);
        }
    }
    return nodeEdges;
}

std::shared_ptr<Layer> &Node::getLayer() { return layer; }

std::shared_ptr<Graph> &Node::getGraph() { return graph; }
void Node::setGraph(std::shared_ptr<Graph> &nextGraph) { graph = nextGraph; };

nlohmann::json Node::json() {
    nlohmann::json res;
    res["name"] = name;
    res["x"] = getPos().x;
    res["y"] = getPos().y;
    res["width"] = getSize().x;
    res["height"] = getSize().y;
    return res;
}
}  // namespace GuiBridge