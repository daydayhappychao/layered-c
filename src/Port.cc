#include "Port.h"

#include <string>
#include <utility>

#include "Edge.h"
#include "Node.h"
#include "nlohmann/json.hpp"

namespace GuiBridge {

Port::Port(std::string name) : name(std::move(name)) {}

std::shared_ptr<Node> Port::getNode() { return node.lock(); }
void Port::setNode(const std::shared_ptr<Node> &node) { this->node = node; }

void Port::addEdge(const std::shared_ptr<Edge> &edge) { edges.push_back(edge); }

std::vector<std::shared_ptr<Edge>> Port::getEdges() {
    std::vector<std::shared_ptr<Edge>> res;
    res.reserve(edges.size());
    for (const auto &edge : edges) {
        res.push_back(edge.lock());
    }
    return res;
}

bool Port::getInternalCollect() { return internal_collect; }

void Port::setInternalCollect(bool b) { internal_collect = b; }

std::vector<std::shared_ptr<Port>> Port::getConnectedPorts() {
    std::vector<std::shared_ptr<Port>> res;
    res.reserve(connectedPorts.size());
    for (const auto &p : connectedPorts) {
        res.push_back(p.lock());
    }
    return res;
}

void Port::addConnectedPort(const std::shared_ptr<Port> &port) { connectedPorts.push_back(port); }

std::string Port::getName() { return name; }

nlohmann::json Port::json() {
    nlohmann::json res;
    res["x"] = x;
    res["y"] = y;
    res["name"] = name;
    return res;
}
}  // namespace GuiBridge