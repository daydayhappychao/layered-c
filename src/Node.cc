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

Node::Node(std::string name, std::shared_ptr<NodeProto> &proto)
    : name(std::move(name)), side(NodeSide::NONE), proto(proto) {}
Node::Node(std::string name, NodeSide side, std::shared_ptr<NodeProto> &proto)
    : name(std::move(name)), side(side), proto(proto) {}

int Node::getId() const { return id; }
void Node::setId(int id) { this->id = id; }

// void Node::addInputPort(const std::shared_ptr<Port> &port) {
//     if (port->getNode() != nullptr) {
//         throw std::runtime_error("port {} is already used in a node");
//     }
//     if (this->side == NodeSide::FIRST_SEPARATE) {
//         throw std::runtime_error("Cannot add input port to node with side FIRST_SEPARATE");
//     }
//     inputPorts.push_back(port);
//     auto ptr = shared_from_this();

//     port->setNode(ptr);
// }

// void Node::addOutputPort(const std::shared_ptr<Port> &port) {
//     if (port->getNode() != nullptr) {
//         throw std::runtime_error("port {} is already used in a node");
//     }
//     if (this->side == NodeSide::LAST_SEPARATE) {
//         throw std::runtime_error("Cannot add input port to node with side LAST_SEPARATE");
//     }
//     outputPorts.push_back(port);
//     auto ptr = shared_from_this();

//     port->setNode(ptr);
// }

std::vector<std::shared_ptr<Port>> &Node::getInputPorts() { return proto->inputPorts; }

std::vector<std::shared_ptr<Port>> &Node::getOutputPorts() { return proto->outputPorts; }

std::vector<std::shared_ptr<Port>> Node::getAllPorts() {
    std::vector<std::shared_ptr<Port>> allPorts;
    allPorts.insert(allPorts.end(), proto->outputPorts.begin(), proto->outputPorts.end());
    allPorts.insert(allPorts.end(), proto->inputPorts.begin(), proto->inputPorts.end());
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
    std::vector<std::shared_ptr<Edge>> allEdges;
    for (const auto &pair : edges) {
        allEdges.insert(allEdges.end(), pair.second.begin(), pair.second.end());
    }
    return allEdges;
}

std::vector<std::shared_ptr<Edge>> Node::getIncomingEdges() {
    std::vector<std::shared_ptr<Edge>> allEdges;
    for (const auto &pair : edges) {
        if (vecInclude(proto->inputPorts, pair.first)) {
            allEdges.insert(allEdges.end(), pair.second.begin(), pair.second.end());
        }
    }
    return allEdges;
};
std::vector<std::shared_ptr<Edge>> Node::getOutgoingEdges() {
    std::vector<std::shared_ptr<Edge>> allEdges;
    for (const auto &pair : edges) {
        if (vecInclude(proto->outputPorts, pair.first)) {
            allEdges.insert(allEdges.end(), pair.second.begin(), pair.second.end());
        }
    }
    return allEdges;
};

std::vector<std::shared_ptr<Edge>> Node::getEdgesByPort(const std::shared_ptr<Port> &port) {
    return edges.contains(port) ? edges.at(port) : std::vector<std::shared_ptr<Edge>>{};
};

void Node::addEdge(std::shared_ptr<Port> &port, std::shared_ptr<Edge> &edge) {
    /**
     * @todo
     * 这里加一个判断, 如果已存在则 throw error
     */
    edges[port].emplace_back(edge);
}

void Node::removeEdge(std::shared_ptr<Port> &port, std::shared_ptr<Edge> &edge) { vecRemove(edges[port], edge); }

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
    res["width"] = proto->getSize().x;
    res["height"] = proto->getSize().y;
    return res;
}
}  // namespace GuiBridge