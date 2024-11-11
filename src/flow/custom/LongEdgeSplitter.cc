

#include "./LongEdgeSplitter.h"
#include <memory>
#include "../../Edge.h"
#include "../../Graph.h"
namespace GuiBridge {
void LongEdgeSplitter::process(std::shared_ptr<Graph> &graph) {
    auto &layers = graph->getLayers();
    for (int i = 0; i < layers.size() - 1; i++) {
        auto &layer = layers[i];
        auto &nextLayer = layers[i + 1];
        for (auto &node : layer->getNodes()) {
            for (auto &port : node->getOutputPorts()) {
                for (auto &edge : node->getEdgesByPort(port)) {
                    const auto &targetPort = edge->getDst();
                    auto &targetLayer = targetPort.node->getLayer();
                    if (targetLayer != layer && targetLayer != nextLayer) {
                        auto dummyNode = createDummyNode(graph, nextLayer, edge);
                        splitEdge(graph, edge, dummyNode);
                    }
                }
            }
        }
    }
}

std::shared_ptr<Node> LongEdgeSplitter::createDummyNode(std::shared_ptr<Graph> &graph,
                                                        std::shared_ptr<Layer> &targetLayer,
                                                        std::shared_ptr<Edge> &edgeToSplit) {
    auto dummyNodeProto = graph->getDummyNodeProto();
    auto dummyNode = std::make_shared<Node>("a_dummy_node", dummyNodeProto, 99999999 - graph->getNodes().size());
    dummyNode->setDummy(edgeToSplit);
    dummyNode->setLayer(targetLayer);
    graph->addNode(dummyNode);
    return dummyNode;
}

void LongEdgeSplitter::splitEdge(std::shared_ptr<Graph> &graph, std::shared_ptr<Edge> &edge,
                                                  std::shared_ptr<Node> &dummyNode) {
    auto oldEdgeTarget = edge->getDst();

    auto inPort = graph->getPortById(-1);
    auto outPort = graph->getPortById(-2);
    auto nextDst = EdgeTarget(dummyNode, inPort);
    edge->setDst(nextDst);

    graph->addEdge(dummyNode, outPort, oldEdgeTarget.node, oldEdgeTarget.port);
}

}  // namespace GuiBridge
