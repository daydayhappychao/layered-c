#include "./SimpleRowGraphPlacer.h"
#include <memory>
#include "../Graph.h"
#include "../math/KVector.h"

namespace GuiBridge {

void combine(const std::vector<std::shared_ptr<Graph>> &components, const std::shared_ptr<Graph> &target) {
    if (components.size() == 1) {
        std::shared_ptr<Graph> source = components[0];
        if (source != target) {
            moveGraph(target, source, 0, 0);
        }
    }
}

void moveGraph(const std::shared_ptr<Graph> &destGraph, const std::shared_ptr<Graph> &sourceGraph, double offsetX,
               double offsetY) {
    KVector graphOffset = sourceGraph->getOffset().add(offsetX, offsetY);

    for (auto node : sourceGraph->getLayerlessNodes()) {
        node->getPos().add(graphOffset);
        for (auto port : node->getOutputPorts()) {
            for (auto edge : port->getEdges()) {
                edge->getBendPoints().offset(graphOffset);
            }
        }
        destGraph->addNode(node);
        
    }
}
}  // namespace GuiBridge
