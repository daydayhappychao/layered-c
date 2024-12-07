#include "Layer.h"
#include <algorithm>
#include <cstddef>
#include <utility>
#include "Graph.h"
#include "utils/Constants.h"
#include "utils/VectorUtil.h"
namespace GuiBridge {
Layer::Layer(std::shared_ptr<Graph> &graph) : owner(graph) {}

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

void Layer::adjustSize() {
    float maxWidth = 0.0F;
    for (auto &node : nodes) {
        if (node->getSize().x > maxWidth) {
            maxWidth = node->getSize().x;
        }
    }
    if (maxWidth != this->size.x) {
        this->size.setX(maxWidth);
        auto thisPtr = shared_from_this();
        auto it = std::find(owner->getLayers().begin(), owner->getLayers().end(), thisPtr);
        std::vector<std::shared_ptr<Layer>> needCalculateLayers(it, owner->getLayers().end());
        for (auto &layer : needCalculateLayers) {
            layer->adjustLayerAndNodePosX();
        }
    } else {
        this->adjustLayerAndNodePosX();
    }
}

void Layer::adjustLayerAndNodePosX() {
    auto thisPtr = shared_from_this();

    float layerX = 0.0F;

    for (std::size_t i = 0; i < owner->getLayers().size(); i++) {
        auto &layer = owner->getLayers()[i];
        auto &lastLayer = owner->getLayers()[i - 1];
        if (i != 0) {
            layerX += layer->getMargin();
        }
        if (thisPtr == layer) {
            break;
        }
        layerX += layer->size.x;
    }

    this->getPos().setX(layerX);

    for (auto &node : nodes) {
        float nodeX = layerX + (this->size.x - node->getSize().x) / 2;
        node->getPos().setX(nodeX);
    }
}

float Layer::getMargin() { return margin; }

void Layer::setMargin(float v) {
    margin = v;
    adjustLayerAndNodePosX();
};

std::shared_ptr<Layer> Layer::getRightLayer() {
    auto &layers = owner->getLayers();
    int currentIndex = vecIndexOf(layers, shared_from_this());
    if (layers.size() > currentIndex + 1) {
        return layers[currentIndex + 1];
    }
    return nullptr;
};
std::shared_ptr<Layer> Layer::getLeftLayer() {
    auto &layers = owner->getLayers();
    int currentIndex = vecIndexOf(layers, shared_from_this());
    if (currentIndex > 0) {
        return layers[currentIndex - 1];
    }
    return nullptr;
};

}  // namespace GuiBridge