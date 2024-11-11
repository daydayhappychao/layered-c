#include "LayerSizeAndGraphHeightCalculator.h"
#include <algorithm>
#include "../../Graph.h"

namespace GuiBridge {
void LayerSizeAndGraphHeightCalculator::process(std::shared_ptr<Graph> &graph) {
    double minY = std::numeric_limits<double>::infinity();
    double maxY = -std::numeric_limits<double>::infinity();

    bool foundNodes = false;

    for (auto &layer : graph->getLayers()) {
        auto &layerSize = layer->getSize();
        layerSize.setX(0.0);
        layerSize.setY(0.0);

        if (layer->getNodes().empty()) {
            continue;
        }
        foundNodes = true;

        for (auto &node : layer->getNodes()) {
            auto &nodeSize = node->getSize();
            auto &nodeMargin = node->margin;

            layerSize.setX(std::max(layerSize.x, nodeSize.x + nodeMargin.left + nodeMargin.right));
        }

        auto firstNode = layer->getNodes()[0];
        double top = firstNode->getPos().y - firstNode->margin.top;
        auto lastNode = layer->getNodes()[layer->getNodes().size() - 1];
        double bottom = lastNode->getPos().y + lastNode->getSize().y + lastNode->margin.bottom;
        layerSize.setY(bottom - top);

        minY = std::min(minY, top);
        maxY = std::max(maxY, bottom);
    }

    if (!foundNodes) {
        minY = 0;
        maxY = 0;
    }

    graph->getSize().setY(maxY - minY);
    // graph->getOffset().setY(graph->getOffset().y - minY);
}
}  // namespace GuiBridge