#include "Node.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Edge.h"
#include "Port.h"
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

std::optional<int> Node::getLayer() { return layer; }

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

nlohmann::json Node::json() {
    nlohmann::json res;
    res["name"] = name;
    res["x"] = x;
    res["y"] = y;
    res["width"] = width;
    res["height"] = height;
    return res;
}
}  // namespace GuiBridge