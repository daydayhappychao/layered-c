

#include "NetworkSimplexLayerer.h"
#include <cmath>
#include <memory>
#include <vector>
#include "../../../Graph.h"
#include "../../../utils/Constants.h"
#include "NetworkSimplex.h"

namespace GuiBridge {

void NetworkSimplexLayerer::process(std::shared_ptr<Graph> &graph) {
    int thoroughness = THOROUGHNESS * 4;
    auto theNodes = graph->getLayerlessNodes();
    if (theNodes.empty()) {
        return;
    }

    std::vector<int> previousLayeringNodeCounts;

    int iterLimit = thoroughness * sqrt(theNodes.size());

    NetworkSimplex networkSimplex(graph, iterLimit);

    networkSimplex.execute();

    const auto &layers = graph->getLayers();
    for (const auto &node : graph->getLayerlessNodes()) {
        while (layers.size() <= node->layerIndex) {
            graph->addLayers(std::make_shared<Layer>(graph));
        }
        node->setLayer(layers[node->layerIndex]);
    }
}

}  // namespace GuiBridge