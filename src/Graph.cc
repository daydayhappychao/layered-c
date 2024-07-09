#include "Graph.h"

#include <memory>
#include <vector>

#include "Node.h"
namespace GuiBridge {

void Graph::addNode(const std::shared_ptr<Node> &node) { nodes.push_back(node); }

void Graph::addEdge(const std::shared_ptr<Edge> &edge) { edges.push_back(edge); }

std::vector<std::shared_ptr<Node>> &Graph::getNodes() { return nodes; }

std::vector<std::shared_ptr<Edge>> &Graph::getEdges() { return edges; }

std::vector<std::shared_ptr<Node>> Graph::getLayerlessNodes() {
    std::vector<std::shared_ptr<Node>> layerlessNodes;
    for (const auto &node : nodes) {
        if (!node->getLayer().has_value()) {
            layerlessNodes.push_back(node);
        }
    }
    return layerlessNodes;
}
}  // namespace GuiBridge