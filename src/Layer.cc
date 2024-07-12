#include "Layer.h"
#include <algorithm>
#include "Graph.h"
namespace GuiBridge {
Layer::Layer(std::shared_ptr<Graph> graph) : owner(graph), size() {}

KVector &Layer::getSize() { return size; }

std::vector<std::shared_ptr<Node>> &Layer::getNodes() { return nodes; }

std::shared_ptr<Graph> Layer::getGraph() { return owner; }

int Layer::getIndex() const {
    auto &layers = owner->getLayers();
    auto it = std::find(layers.begin(), layers.end(), shared_from_this());
    if (it != layers.end()) {
        return std::distance(layers.begin(), it);
    }
    return -1;  // Or throw an exception if the layer is not found in the graph
}

std::vector<std::shared_ptr<Node>>::iterator Layer::begin() { return nodes.begin(); }

std::vector<std::shared_ptr<Node>>::iterator Layer::end() { return nodes.end(); }

std::vector<std::shared_ptr<Node>>::const_iterator Layer::begin() const { return nodes.begin(); }

std::vector<std::shared_ptr<Node>>::const_iterator Layer::end() const { return nodes.end(); }

std::string Layer::toString() const { return "L_" + std::to_string(getIndex()) + " " + std::to_string(nodes.size()); }
}  // namespace GuiBridge