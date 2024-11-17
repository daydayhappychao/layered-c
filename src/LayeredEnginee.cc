#include "LayeredEnginee.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "./flow/custom/EdgeToPloyLine.h"
#include "./flow/p1_cycle_breaking/GreedyCycleBreaker.h"
#include "./flow/p4_nodes/BKNodePlacer.h"
#include "./utils/ComponentsProcessor.h"
#include "Graph.h"
#include "NodeProto.h"
#include "Port.h"
#include "flow/custom/LayerSweepCrossingMinimizer.h"
#include "flow/custom/LongEdgeSplitter.h"
#include "flow/intermediate/LayerConstraintPostprocessor.h"
#include "flow/intermediate/LayerConstraintPreprocessor.h"
#include "flow/intermediate/LayerSizeAndGraphHeightCalculator.h"
#include "flow/p2_layers/networkSimplex/NetworkSimplexLayerer.h"
#include "opts/NodeSide.h"
#include "opts/PortType.h"

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
        } else if (buf == "proto") {
            int id;
            std::string display_name;
            int width;
            int height;
            in >> id >> std::quoted(display_name) >> width >> height;

            graph->addNodeProto(display_name, width, height, id);
        } else if (buf == "port") {
            int id;
            int protoId;
            std::string name;
            std::string type;
            in >> id >> protoId >> std::quoted(name) >> std::quoted(type);
            PortType portType = type == "in" ? PortType::INPUT : PortType::OUTPUT;
            graph->addPort(name, id, portType, protoId);

        } else if (buf == "node") {
            std::size_t id;
            std::size_t proto;
            std::string name;
            std::string pos;
            in >> id >> proto >> name >> pos;
            if (pos == "default") {
                graph->addNode(id, proto, name);
            } else {
                NodeSide side = pos == "west" ? NodeSide::FIRST_SEPARATE : NodeSide::LAST_SEPARATE;
                graph->addNode(id, proto, name, side);
            }
        } else if (buf == "edge") {
            int out_node;
            int out_pin;
            int in_node;
            int in_pin;
            std::string name;
            in >> out_node >> out_pin >> in_node >> in_pin >> name;
            graph->addEdge(out_node, out_pin, in_node, in_pin);
        } else {
            assert(false);
        }
    }
}

void ELKLayered::layered() {
    ComponentsProcessor componentsProcessor;

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

    LayerSweepCrossingMinimizer layerSweepCrossingMinimizer;
    layerSweepCrossingMinimizer.process(graph);

    // p4 node placement
    BKNodePlacer bkNodePlacer;
    bkNodePlacer.process(graph);

    LayerSizeAndGraphHeightCalculator layerSizeAndGraphHeightCalculator;
    layerSizeAndGraphHeightCalculator.process(graph);

    // p5 edge routing
    for (auto &edge : graph->getEdges()) {
        edge->show();
    }
    EdgeToPloyLine edgeToPloyLine;
    edgeToPloyLine.process(graph);

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
        res["nodes"].emplace_back(nodeJson);
    }

    for (const auto &edge : edges) {
        auto edgeJson = edge->json();
        res["edges"].emplace_back(edgeJson);
    }

    std::ofstream outFile("data/output.json");
    if (outFile.is_open()) {
        outFile << res.dump();  // 写入内容
        outFile.close();        // 关闭文件
        std::cout << "写入文件成功！" << std::endl;
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }

    // std::cout << res.dump() << std::endl;
}
}  // namespace GuiBridge