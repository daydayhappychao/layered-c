

#include "./LongEdgeSplitter.h"
#include <memory>
#include "../../Graph.h"
namespace GuiBridge {
void LongEdgeSplitter::process(const std::shared_ptr<Graph> &graph) {
    auto &layers = graph->getLayers();
    for (int i = 0; i < layers.size() - 1; i++) {
        const auto &layer = layers[i];
        const auto &nextLayer = layers[i + 1];
        for (const auto &node : layer->getNodes()) {
            for (const auto &port : node->getOutputPorts()) {
                for (const auto &edge : node->getEdgesByPort(port)) {
                    const auto &targetPort = edge->getDst();
                    const auto &targetLayer = targetPort.node->getLayer();
                    if (targetLayer != layer && targetLayer != nextLayer) {
                        splitEdge(edge, createDummyNode(graph, nextLayer, edge));
                    }
                }
            }
        }
    }
}

std::shared_ptr<Node> LongEdgeSplitter::createDummyNode(const std::shared_ptr<Graph> &graph,
                                                        const std::shared_ptr<Layer> &targetLayer,
                                                        const std::shared_ptr<Edge> &edgeToSplit) {
    auto dummyNode = std::make_shared<Node>("dummy");
    dummyNode->setDummy(edgeToSplit);
    dummyNode->setLayer(targetLayer);
    graph->addNode(dummyNode);
    return dummyNode;
}

std::shared_ptr<Edge> LongEdgeSplitter::splitEdge(const std::shared_ptr<Edge> &edge,
                                                  const std::shared_ptr<Node> &dummyNode) {
    const auto &oldEdgeTarget = edge->getDst();
    auto dummyInputPort = std::make_shared<Port>("dummyInput");
    auto dummyOutPort = std::make_shared<Port>("dummyOut");
    // dummyNode->addInputPort(dummyInputPort);
    // dummyNode->addOutputPort(dummyOutPort);

    edge->setDst(dummyInputPort);

    const auto &dummyEdge = std::make_shared<Edge>(dummyOutPort, oldEdgeTarget);
    dummyNode->setDummySource(edge->getSrc());
    dummyNode->setDummyTarget(dummyEdge->getDst());
    return dummyEdge;
}

}  // namespace GuiBridge
