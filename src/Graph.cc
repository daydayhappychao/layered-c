#include "Graph.h"

#include <memory>
#include <vector>

#include "Layer.h"
#include "Node.h"

namespace GuiBridge {

void Graph::addNode(const std::shared_ptr<Node> &node) {
    nodes.push_back(node);
    layerlessNodes.push_back(node);
    auto ptr = shared_from_this();
    node->setGraph(ptr);
}

void Graph::addEdge(const std::shared_ptr<Edge> &edge) {
    edges.push_back(edge);
    auto srcPort = edge->getSrc();
    auto dstPort = edge->getDst();
    srcPort->addEdge(edge);
    dstPort->addEdge(edge);
}

std::vector<std::shared_ptr<Node>> &Graph::getNodes() { return nodes; }

std::vector<std::shared_ptr<Edge>> &Graph::getEdges() { return edges; }

std::vector<std::shared_ptr<Node>> &Graph::getLayerlessNodes() { return layerlessNodes; }

std::vector<std::shared_ptr<Layer>> &Graph::getLayers() { return layers; }
void Graph::addLayers(const std::shared_ptr<Layer> &layer) { layers.emplace_back(layer); }

void Graph::setHiddenNodes(std::vector<std::shared_ptr<Node>> &nodes) { hiddenNodes = nodes; };

std::vector<std::shared_ptr<Node>> &Graph::getHiddenNodes() { return hiddenNodes; };

KVector Graph::getSize() const { return size; };
KVector Graph::getOffset() const { return offset; };

std::vector<std::vector<std::shared_ptr<Node>>> Graph::toNodeArray() {
    std::vector<std::vector<std::shared_ptr<Node>>> nodeArray(this->layers.size());
    for (int i = 0; i < layers.size(); i++) {
        auto nodes = layers[i]->getNodes();
        nodeArray[i] = nodes;
    }
    return nodeArray;
}

}  // namespace GuiBridge