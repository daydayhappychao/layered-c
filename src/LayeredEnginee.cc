#include "LayeredEnginee.h"

#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <utility>

#include "./flow/p1_cycle_breaking/GreedyCycleBreaker.h"
#include "./utils/ComponentsProcessor.h"
#include "Graph.h"

namespace GuiBridge {

using json = nlohmann::json;
ELKLayered::ELKLayered(std::shared_ptr<Graph> graph) : graph(std::move(graph)) {}

void ELKLayered::layered() {
    ComponentsProcessor componentsProcessor;

    std::vector<Graph> components = componentsProcessor.split(graph);

    for (auto graph : components) {
        std::shared_ptr<Graph> graphPtr(&graph);

        // p1 cycle breaking
        greedy_cycle_breaker(graphPtr);

        // p2 layer assignment

        // p3 crossing minimization

        // p4 node placement

        // p5 edge routing
    }
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