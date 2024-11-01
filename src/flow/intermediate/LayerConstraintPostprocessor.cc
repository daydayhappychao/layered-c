#include "./LayerConstraintPostprocessor.h"
#include <memory>
#include "../../Graph.h"
#include "../../Layer.h"

namespace GuiBridge {
void LayerConstraintPostprocessor::process(std::shared_ptr<Graph> &graph) {
    auto &layers = graph->getLayers();

    if (!layers.empty()) {
        auto firstLayer = layers[0];
        auto lastLayer = layers.back();

        moveFirstAndLastNodes(graph, firstLayer, lastLayer);
    }

    if (!graph->getHiddenNodes().empty()) {
        auto firstSeparateLayer = std::make_shared<Layer>(graph);
        auto lastSeparateLayer = std::make_shared<Layer>(graph);
        restoreHiddenNodes(graph, firstSeparateLayer, lastSeparateLayer);
        if (!firstSeparateLayer->getNodes().empty()) {
            layers.insert(layers.begin(), firstSeparateLayer);
        }

        if (!lastSeparateLayer->getNodes().empty()) {
            layers.emplace_back(lastSeparateLayer);
        }
    }
}

void LayerConstraintPostprocessor::moveFirstAndLastNodes(std::shared_ptr<Graph> &graph,
                                                         const std::shared_ptr<Layer> &firstLayer,
                                                         const std::shared_ptr<Layer> &lastLayer) {
    auto &layers = graph->getLayers();

    for (const auto &layer : layers) {
        auto nodes = layer->getNodes();

        for (const auto &node : nodes) {
            switch (node->getSide()) {
                case NodeSide::FIRST:
                    node->setLayer(firstLayer);
                    break;
                case NodeSide::LAST:
                    node->setLayer(lastLayer);
                    break;
                case NodeSide::FIRST_SEPARATE:
                case NodeSide::LAST_SEPARATE:
                case NodeSide::NONE:
                    break;
            }
        }
    }

    for (auto it = layers.begin(); it != layers.end();) {
        if (it->get()->getNodes().empty()) {
            it = layers.erase(it);  // Remove the empty layer and get the next iterator
        } else {
            ++it;  // Move to the next layer
        }
    }
}

void LayerConstraintPostprocessor::restoreHiddenNodes(std::shared_ptr<Graph> &graph,
                                                      const std::shared_ptr<Layer> &firstSeparateLayer,
                                                      const std::shared_ptr<Layer> &lastSeparateLayer) {
    for (const auto &node : graph->getHiddenNodes()) {
        switch (node->getSide()) {
            case NodeSide::FIRST_SEPARATE:
                node->setLayer(firstSeparateLayer);
                for (const auto &edges : node->getOutgoingEdges()) {
                    edges->revertOppositePort();
                }
                break;
            case NodeSide::LAST_SEPARATE:
                node->setLayer(lastSeparateLayer);
                for (const auto &edges : node->getIncomingEdges()) {
                    edges->revertOppositePort();
                }
                break;
            case NodeSide::FIRST:
            case NodeSide::LAST:
            case NodeSide::NONE:
                break;
        }
    }
}

}  // namespace GuiBridge