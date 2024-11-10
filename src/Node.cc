#include "Node.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "./Layer.h"
#include "Edge.h"
#include "Port.h"
#include "Shape.h"
#include "nlohmann/json.hpp"
#include "opts/NodeSide.h"
#include "opts/PortType.h"
#include "utils/VectorUtil.h"
namespace GuiBridge {

Node::Node(std::string name) : name(std::move(name)), side(NodeSide::NONE) {
    this->getSize().setX(100);
    this->getSize().setY(50);
}
Node::Node(std::string name, NodeSide side) : name(std::move(name)), side(side) {
    this->getSize().setX(100);
    this->getSize().setY(50);
}

int Node::getId() const { return id; }
void Node::setId(int id) { this->id = id; }

void Node::addInputPort(const std::shared_ptr<Port> &port) {
    if (port->getNode() != nullptr) {
        throw std::runtime_error("port {} is already used in a node");
    }
    if (this->side == NodeSide::FIRST_SEPARATE) {
        throw std::runtime_error("Cannot add input port to node with side FIRST_SEPARATE");
    }
    inputPorts.push_back(port);
    auto ptr = shared_from_this();

    port->setNode(ptr);
}

void Node::addOutputPort(const std::shared_ptr<Port> &port) {
    if (port->getNode() != nullptr) {
        throw std::runtime_error("port {} is already used in a node");
    }
    if (this->side == NodeSide::LAST_SEPARATE) {
        throw std::runtime_error("Cannot add input port to node with side LAST_SEPARATE");
    }
    outputPorts.push_back(port);
    auto ptr = shared_from_this();

    port->setNode(ptr);
}

std::vector<std::shared_ptr<Port>> &Node::getInputPorts() { return inputPorts; }

std::vector<std::shared_ptr<Port>> &Node::getOutputPorts() { return outputPorts; }

std::vector<std::shared_ptr<Port>> Node::getAllPorts() {
    std::vector<std::shared_ptr<Port>> allPorts;
    allPorts.insert(allPorts.end(), outputPorts.begin(), outputPorts.end());
    allPorts.insert(allPorts.end(), inputPorts.begin(), inputPorts.end());
    return allPorts;
}

std::vector<std::shared_ptr<Port>> &Node::getPortsByPortType(PortType type) {
    switch (type) {
        case PortType::INPUT:
            return getInputPorts();
        case PortType::OUTPUT:
            return getOutputPorts();
    }
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

std::vector<std::shared_ptr<Edge>> Node::getIncomingEdges() {
    std::vector<std::shared_ptr<Edge>> nodeEdges;
    for (auto &port : inputPorts) {
        for (const auto &edge : port->getEdges()) {
            nodeEdges.push_back(edge);
        }
    }
    return nodeEdges;
};
std::vector<std::shared_ptr<Edge>> Node::getOutgoingEdges() {
    std::vector<std::shared_ptr<Edge>> nodeEdges;
    for (auto &port : outputPorts) {
        for (const auto &edge : port->getEdges()) {
            nodeEdges.push_back(edge);
        }
    }
    return nodeEdges;
};

std::shared_ptr<Layer> &Node::getLayer() { return layer; }

std::shared_ptr<Graph> &Node::getGraph() { return graph; }
void Node::setGraph(std::shared_ptr<Graph> &nextGraph) { graph = nextGraph; };

NodeSide Node::getSide() { return side; };
void Node::setSide(NodeSide nextSide) { side = nextSide; };

void Node::setHiddenNodeConnections(HiddenNodeConnections::Connection &connections) {
    hiddenNodeConnections = connections;
};
HiddenNodeConnections::Connection Node::getHiddenNodeConnections() { return hiddenNodeConnections; };

void Node::setLayer(const std::shared_ptr<Layer> &layer) {
    auto ptr = shared_from_this();
    if (this->layer != nullptr) {
        vecRemove(this->layer->getNodes(), ptr);
    }
    this->layer = layer;

    if (this->layer != nullptr) {
        this->layer->getNodes().emplace_back(ptr);
    }
}

void Node::setDummy(const std::shared_ptr<Edge> &dummyOrigin) {
    this->isDummy = true;
    this->dummyOrigin = dummyOrigin;
}

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