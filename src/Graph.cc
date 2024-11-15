#include "Graph.h"

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Edge.h"
#include "Layer.h"
#include "Node.h"
#include "NodeProto.h"
#include "Port.h"
#include "nlohmann/json.hpp"
#include "opts/NodeSide.h"
#include "utils/VectorUtil.h"

namespace GuiBridge {

Graph::Graph() {
    addNodeProto("dummyTemp", 1, 1, -1);
    addPort("dummyInport", -1, PortType::INPUT, -1);
    addPort("dummyOutport", -2, PortType::OUTPUT, -1);
};

std::shared_ptr<NodeProto> Graph::getDummyNodeProto() { return getProtoById(-1); }

std::shared_ptr<NodeProto> Graph::getProtoById(int id) {
    auto protoPtr = vecFind(nodeProtos, [id](const std::shared_ptr<NodeProto> &proto) { return proto->id == id; });
    if (!protoPtr.has_value()) {
        throw std::runtime_error("对应的 NodeProto 不存在");
    }
    auto protoPtrValue = protoPtr.value();
    return protoPtrValue;
}

void Graph::addNodeProto(const std::shared_ptr<NodeProto> &nodeProto) { nodeProtos.emplace_back(nodeProto); }
void Graph::addNodeProto(std::string name, double width, double height, int id) {
    auto nodeProto = std::make_shared<NodeProto>(name, width, height, id);
    addNodeProto(nodeProto);
};

void Graph::addPort(std::string name, int id, PortType portType, int protoId) {
    auto protoPtr = getProtoById(protoId);
    auto portPtr = protoPtr->addPort(std::move(name), id, portType);
    ports.emplace_back(portPtr);
}

std::shared_ptr<Port> Graph::getPortById(int id) {
    auto portPtr = vecFind(ports, [id](const std::shared_ptr<Port> &port) { return port->_id == id; });
    if (!portPtr.has_value()) {
        throw std::runtime_error("对应的 Port 不存在");
    }
    auto portPtrValue = portPtr.value();
    return portPtrValue;
};

void Graph::updateAllPortPos() {
    for (auto &proto : nodeProtos) {
        proto->updatePortPos();
    }
}

void Graph::addNode(const std::shared_ptr<Node> &node) {
    nodes.push_back(node);
    layerlessNodes.emplace_back(node);
}
void Graph::addNode(int id, int protoId, std::string name) {
    auto protoPtr = getProtoById(protoId);
    auto nodePtr = std::make_shared<Node>(name, protoPtr, id);
    addNode(nodePtr);
}

void Graph::addNode(int id, int protoId, std::string name, NodeSide side) {
    auto protoPtr = getProtoById(protoId);
    auto nodePtr = std::make_shared<Node>(name, protoPtr, id, side);
    addNode(nodePtr);
}

std::shared_ptr<Node> Graph::getNodeById(int id) {
    auto node = vecFind(nodes, [id](const std::shared_ptr<Node> &node) { return node->_id == id; });
    if (!node.has_value()) {
        throw std::runtime_error("根据 id 查询的 node 不存在");
    }
    return node.value();
};

void Graph::addEdge(std::shared_ptr<Edge> &edge) {
    auto portPtr = edge->getSrc().port;
    edge->getSrc().node->addEdge(portPtr, edge);
    auto dstPortPtr = edge->getDst().port;
    edge->getDst().node->addEdge(dstPortPtr, edge);
    edges.emplace_back(edge);
};
void Graph::addEdge(std::shared_ptr<Node> &srcNode, std::shared_ptr<Port> &srcPort, std::shared_ptr<Node> &dstNode,
                    std::shared_ptr<Port> &dstPort) {
    auto edge = std::make_shared<Edge>(srcNode, srcPort, dstNode, dstPort);
    addEdge(edge);
};
void Graph::addEdge(int srcNodeId, int srcPortId, int dstNodeId, int dstPortId) {
    auto srcNode = vecFind(nodes, [srcNodeId](const std::shared_ptr<Node> &node) { return node->_id == srcNodeId; });
    auto srcPort = vecFind(ports, [srcPortId](const std::shared_ptr<Port> &port) { return port->_id == srcPortId; });
    auto dstNode = vecFind(nodes, [dstNodeId](const std::shared_ptr<Node> &node) { return node->_id == dstNodeId; });
    auto dstPort = vecFind(ports, [dstPortId](const std::shared_ptr<Port> &port) { return port->_id == dstPortId; });
    if (srcNode.has_value() && srcPort.has_value() && dstNode.has_value() && dstPort.has_value()) {
        addEdge(srcNode.value(), srcPort.value(), dstNode.value(), dstPort.value());
    } else {
        throw std::runtime_error("edge 数据不合法");
    }
};

std::vector<std::shared_ptr<Node>> &Graph::getNodes() { return nodes; }

std::vector<std::shared_ptr<Edge>> &Graph::getEdges() { return edges; }

std::vector<std::shared_ptr<Node>> &Graph::getLayerlessNodes() { return layerlessNodes; }

std::vector<std::shared_ptr<Layer>> &Graph::getLayers() { return layers; }
void Graph::addLayers(const std::shared_ptr<Layer> &layer) { layers.emplace_back(layer); }

void Graph::setHiddenNodes(std::vector<std::shared_ptr<Node>> &nodes) { hiddenNodes = nodes; };

std::vector<std::shared_ptr<Node>> &Graph::getHiddenNodes() { return hiddenNodes; };

std::vector<std::vector<std::shared_ptr<Node>>> Graph::toNodeArray() {
    std::vector<std::vector<std::shared_ptr<Node>>> nodeArray(this->layers.size());
    for (int i = 0; i < layers.size(); i++) {
        auto nodes = layers[i]->getNodes();
        nodeArray[i] = nodes;
    }
    return nodeArray;
}

nlohmann::json Graph::json() {
    nlohmann::json res;
    for (auto &n : nodes) {
        res['node'].emplace_back(n->json());
    }
    return res;
};

}  // namespace GuiBridge