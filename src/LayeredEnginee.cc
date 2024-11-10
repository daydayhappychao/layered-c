#include "LayeredEnginee.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <utility>

#include "./flow/p1_cycle_breaking/GreedyCycleBreaker.h"
#include "./flow/p4_nodes/BKNodePlacer.h"
#include "./utils/ComponentsProcessor.h"
#include "Graph.h"
#include "Port.h"
#include "flow/intermediate/LayerConstraintPostprocessor.h"
#include "flow/intermediate/LayerConstraintPreprocessor.h"
#include "flow/intermediate/LayerSizeAndGraphHeightCalculator.h"
#include "flow/p2_layers/networkSimplex/NetworkSimplexLayerer.h"
#include "flow/p3_crossing_minimization/LayerSweepCrossingMinimizer.h"
#include "flow/p3_crossing_minimization/LongEdgeSplitter.h"
#include "opts/CrossMinType.h"

namespace GuiBridge {

using json = nlohmann::json;
ELKLayered::ELKLayered(std::shared_ptr<Graph> graph) : graph(std::move(graph)) {}
ELKLayered::ELKLayered(const std::filesystem::path &path) {
    graph = std::make_shared<Graph>();

    std::ifstream in{path};
    std::string buf;
    in.exceptions(std::ios::badbit);

    while (in >> buf) {
        // ignore comment lines
        if (buf == "#") {
            std::getline(in, buf);
        } else if (buf == "canvas") {
            in >> g.size.x() >> g.size.y();
            // LOG(info, "Canvas Si/ze: {}, {}", g.size.x(), g.size.y());
        } else if (buf == "proto") {
            std::size_t id;
            std::string display_name;
            Vector2f size;
            std::size_t in_pins, out_pins;
            in >> id >> std::quoted(display_name) >> size.x() >> size.y() >> in_pins >> out_pins;
            assert(g.prototypes.size() == id);
            g.prototypes.emplace_back(display_name, size, in_pins, out_pins);
            // LOG(info, "Node Prototype: #{} ({}) <{}, {}> {}:{}",
            //     id, display_name, size.x(), size.y(), in_pins, out_pins
            // );
        } else if (buf == "node") {
            std::size_t id;
            std::size_t proto;
            std::string name;
            in >> id >> proto >> std::quoted(name);
            assert(g.nodes.size() == id);
            assert(proto < g.prototypes.size());
            g.nodes.emplace_back(&g.prototypes[proto], name);
            // LOG(info, "Node: #{}({}) <- {}",
            //     id, name, g.prototypes[proto].name
            // );
        } else if (buf == "link") {
            std::size_t out_node, out_pin, in_node, in_pin;
            in >> out_node >> out_pin >> in_node >> in_pin;
            assert(out_node < g.nodes.size());
            assert(out_pin < g.nodes[out_node].prototype->out_ports.size());
            assert(in_node < g.nodes.size());
            assert(in_pin < g.nodes[in_node].prototype->in_ports.size());
            g.links.emplace_back(out_node, out_pin, in_node, in_pin);
            // LOG(info, "Link: {}:{}->{}:{}",
            //     out_node, out_pin, in_node, in_pin
            // );
        } else {
            assert(false);
        }
    }

    return g;
}

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

    LayerSizeAndGraphHeightCalculator layerSizeAndGraphHeightCalculator;
    layerSizeAndGraphHeightCalculator.process(graph);

    // p5 edge routing

    printJson();
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