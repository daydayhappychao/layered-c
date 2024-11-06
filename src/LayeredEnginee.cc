#include "LayeredEnginee.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <utility>

#include "./flow/p1_cycle_breaking/GreedyCycleBreaker.h"
#include "./flow/p4_nodes/BKNodePlacer.h"
#include "./utils/ComponentsProcessor.h"
#include "Graph.h"
#include "flow/intermediate/LayerConstraintPostprocessor.h"
#include "flow/intermediate/LayerConstraintPreprocessor.h"
#include "flow/p2_layers/networkSimplex/NetworkSimplexLayerer.h"
#include "flow/p3_crossing_minimization/LayerSweepCrossingMinimizer.h"
#include "flow/p3_crossing_minimization/LongEdgeSplitter.h"
#include "opts/CrossMinType.h"

namespace GuiBridge {

using json = nlohmann::json;
ELKLayered::ELKLayered(std::shared_ptr<Graph> graph) : graph(std::move(graph)) {}

void ELKLayered::layered() {
    ComponentsProcessor componentsProcessor;

    auto components = componentsProcessor.split(this->graph);

    auto graph = components[0];

    // p1 cycle breaking
    greedy_cycle_breaker(graph);

    // p2 layer assignment
    LayerConstraintPreprocessor layerConstraintPreprocessor;
    layerConstraintPreprocessor.process(graph);
    NetworkSimplexLayerer networkSimplexLayerer;
    networkSimplexLayerer.process(graph);
    LayerConstraintPostprocessor layerConstraintPostprocessor;
    layerConstraintPostprocessor.process(graph);

    // p3 crossing minimization
    LongEdgeSplitter longEdgeSplitter;
    longEdgeSplitter.process(graph);
    // LayerSweepCrossingMinimizer layerSweepCrossingMinimizer(CrossMinType::BARYCENTER);
    // layerSweepCrossingMinimizer.process(graph);
    // LayerSweepCrossingMinimizer layerSweepCrossingMinimizer2(CrossMinType::TWO_SIDED_GREEDY_SWITCH);
    // layerSweepCrossingMinimizer2.process(graph);
    // p4 node placement
    BKNodePlacer bkNodePlacer;
    bkNodePlacer.process(graph);
    printf("哈哈");

    // p5 edge routing
}

void ELKLayered::printLayers() {
    for (const auto &[node, layerLevel] : layer) {
        std::cout << "Node " << node->getId() << " is at layer " << layerLevel << std::endl;
    }
}

void ELKLayered::printJson() {
    json res;

    auto nodes = graph->getNodes();
    auto edges = graph->getEdges();

    for (const auto &node : nodes) {
        auto nodeJson = node->json();
        for (const auto &port : node->getAllPorts()) {
            nodeJson["ports"].push_back(port->json());
        }
        res["nodes"].push_back(nodeJson);
    }

    for (const auto &edge : edges) {
        res["edges"].push_back(edge->json());
    }

    std::cout << res.dump() << std::endl;
}
}  // namespace GuiBridge