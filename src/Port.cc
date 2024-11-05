#include "Port.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "Edge.h"
#include "Graph.h"
#include "Node.h"
#include "nlohmann/json.hpp"
#include "opts/PortType.h"
#include "utils/VectorUtil.h"

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

void Port::removeEdge(std::shared_ptr<Edge> &edge) {
    auto it = edges.begin();
    for (; it != edges.end();) {
        if (it->lock() == edge) {
            it = edges.erase(it);
            return;
        }
    }
};

bool Port::getInternalCollect() { return internal_collect; }

void Port::setInternalCollect(bool b) { internal_collect = b; }

std::vector<std::shared_ptr<Port>> Port::getConnectedPorts() {
    std::vector<std::shared_ptr<Port>> res;

    auto ptr = shared_from_this();
    for (auto &edge : node.lock()->getGraph()->getEdges()) {
        if (edge->getSrc() == ptr) {
            res.push_back(edge->getDst());
        } else if (edge->getDst() == ptr) {
            res.push_back(edge->getSrc());
        }
    }
    return res;
}

std::string Port::getName() { return name; }

PortType Port::getType() {
    const auto &thisPtr = shared_from_this();
    auto &inputPorts = getNode()->getInputPorts();
    if (vecInclude(inputPorts, thisPtr)) {
        return PortType::INPUT;
    }
    return PortType::OUTPUT;
}

KVector Port::getAnchor() { return anchor; }

nlohmann::json Port::json() {
    nlohmann::json res;
    res["x"] = x;
    res["y"] = y;
    res["name"] = name;
    return res;
}
}  // namespace GuiBridge