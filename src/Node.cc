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
#include "math/KVector.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include "opts/NodeSide.h"
#include "opts/PortType.h"
#include "utils/VectorUtil.h"
namespace GuiBridge {

Node::Node(std::string name, std::shared_ptr<NodeProto> &proto, int _id)
    : name(std::move(name)), side(NodeSide::NONE), proto(proto), _id(_id) {}
Node::Node(std::string name, std::shared_ptr<NodeProto> &proto, int _id, NodeSide side)
    : name(std::move(name)), side(side), proto(proto), _id(_id) {}

int Node::getId() const { return id; }
void Node::setId(int id) { this->id = id; }

std::shared_ptr<NodeProto> Node::getProto() { return this->proto; };
KVector &Node::getSize() { return this->proto->getSize(); };

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

std::vector<EdgeTarget> Node::getConnectedPorts(std::shared_ptr<Port> &port) {
    auto edgeData = edges.at(port);
    std::vector<EdgeTarget> connectedPorts;
    auto thisPtr = shared_from_this();
    for (auto &edge : edgeData) {
        connectedPorts.emplace_back(edge->getOther(thisPtr));
    }
    return connectedPorts;
};

std::vector<std::shared_ptr<Edge>> Node::getEdges() {
    std::vector<std::shared_ptr<Edge>> allEdges;
    for (const auto &pair : edges) {
        allEdges.insert(allEdges.end(), pair.second.begin(), pair.second.end());
    }
    return allEdges;
}

std::vector<std::shared_ptr<Edge>> Node::getIncomingEdges() {
    std::vector<std::shared_ptr<Edge>> allEdges;

    for (auto &port : getInputPorts()) {
        auto edge = edges.at(port);
        allEdges.insert(allEdges.end(), edge.begin(), edge.end());
    }
    return allEdges;
};
std::vector<std::shared_ptr<Edge>> Node::getOutgoingEdges() {
    std::vector<std::shared_ptr<Edge>> allEdges;
    for (auto &port : getOutputPorts()) {
        auto edge = edges.at(port);
        allEdges.insert(allEdges.end(), edge.begin(), edge.end());
    }
    return allEdges;
};

std::vector<std::shared_ptr<Edge>> Node::getEdgesByPort(const std::shared_ptr<Port> &port) {
    return edges.find(port) != edges.end() ? edges.at(port) : std::vector<std::shared_ptr<Edge>>{};
};

void Node::addEdge(std::shared_ptr<Port> &port, std::shared_ptr<Edge> &edge) {
    /**
     * @todo
     * 这里加一个判断, 如果已存在则 throw error
     */
    edges[port].emplace_back(edge);
}

void Node::removeEdge(std::shared_ptr<Port> &port, std::shared_ptr<Edge> &edge) { 
    vecRemove(edges[port], edge); 
}

std::shared_ptr<Layer> &Node::getLayer() { return layer; }

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
        this->layer->adjustSize();
    }
    this->layer = layer;

    if (this->layer != nullptr) {
        this->layer->getNodes().emplace_back(ptr);
        this->layer->adjustSize();
    }
}

void Node::setDummy(const std::shared_ptr<Edge> &dummyOrigin) {
    this->isDummy = true;
    this->dummyOrigin = dummyOrigin;
}

KVector Node::getPortPos(std::shared_ptr<Port> &port) { return KVector(this->getPos()).add(port->getPos()); };

nlohmann::json Node::json() {
    nlohmann::json res;
    res["name"] = name;
    res["x"] = getPos().x;
    res["y"] = getPos().y;
    res["width"] = getSize().x;
    res["height"] = getSize().y;
    for (auto &port : this->getAllPorts()) {
        nlohmann::json portItem;
        portItem["name"] = port->name;
        portItem["x"] = getPortPos(port).x;
        portItem["y"] = getPortPos(port).y;
        res["ports"].emplace_back(portItem);
    }
    return res;
}
}  // namespace GuiBridge