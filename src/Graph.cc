#include "Graph.h"

#include <memory>
#include <vector>

#include "Layer.h"
#include "Node.h"

namespace GuiBridge {

void Graph::addNode(const std::shared_ptr<Node> &node) {
    nodes.push_back(node);
    auto ptr = shared_from_this();
    node->setGraph(ptr);
}

void Graph::addEdge(const std::shared_ptr<Edge> &edge) {
    edges.push_back(edge);
    auto srcPort = edge->getSrc();
    auto dstPort = edge->getDst();
    srcPort->addEdge(edge);
    dstPort->addEdge(edge);
    srcPort->addConnectedPort(dstPort);
    dstPort->addConnectedPort(srcPort);
}

std::vector<std::shared_ptr<Node>> &Graph::getNodes() { return nodes; }

std::vector<std::shared_ptr<Edge>> &Graph::getEdges() { return edges; }

std::vector<std::shared_ptr<Node>> Graph::getLayerlessNodes() {
    std::vector<std::shared_ptr<Node>> layerlessNodes;
    for (const auto &node : nodes) {
        if (node->getLayer() == nullptr) {
            layerlessNodes.push_back(node);
        }
    }
    return layerlessNodes;
}

std::vector<std::shared_ptr<Layer>> &Graph::getLayers() { return layers; }

KVector Graph::getSize() const { return size; };
KVector Graph::getOffset() const { return offset; };

}  // namespace GuiBridge