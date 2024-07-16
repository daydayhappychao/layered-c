#include "./SimpleRowGraphPlacer.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include "../Graph.h"
#include "../math/KVector.h"

namespace GuiBridge {

void combine(std::vector<std::shared_ptr<Graph>> &components, std::shared_ptr<Graph> &target) {
    if (components.size() == 1) {
        std::shared_ptr<Graph> source = components[0];
        if (source != target) {
            moveGraph(target, source, 0, 0);
            target->getSize().set(source->getSize());
        }
        return;
    }
    if (components.empty()) {
        target->getSize().set(0, 0);
        return;
    }

    sortComponents(components);
    std::shared_ptr<Graph> firstComponent = components[0];
    double maxRowWidth = 0.0F;
    double totalArea = 0.0F;
    for (const auto &graph : components) {
        auto size = graph->getSize();
        maxRowWidth = std::max(maxRowWidth, size.x);
        totalArea += size.x * size.y;
    }
    maxRowWidth = std::max(maxRowWidth, sqrt(totalArea) * 1.3);
    double componentSpacing = 20.0F;
    placeComponents(components, target, maxRowWidth, componentSpacing);
    moveGraphs(target, components, 0, 0);
}

void moveGraph(std::shared_ptr<Graph> &destGraph, std::shared_ptr<Graph> &sourceGraph, double offsetX, double offsetY) {
    KVector graphOffset = sourceGraph->getOffset().add(offsetX, offsetY);

    for (const auto &node : sourceGraph->getLayerlessNodes()) {
        node->getPos().add(graphOffset);
        for (const auto &port : node->getOutputPorts()) {
            for (const auto &edge : port->getEdges()) {
                edge->getBendPoints().offset(graphOffset);
            }
        }
        destGraph->addNode(node);
    }
}

void moveGraphs(std::shared_ptr<Graph> &destGraph, std::vector<std::shared_ptr<Graph>> &sourceGraphs, double offsetX,
                double offsetY) {
    for (auto sourceGraph : sourceGraphs) {
        moveGraph(destGraph, sourceGraph, offsetX, offsetY);
    }
}

void sortComponents(std::vector<std::shared_ptr<Graph>> &components) {
    std::sort(components.begin(), components.end(),
              [](const std::shared_ptr<Graph> &a, const std::shared_ptr<Graph> &b) {
                  auto size1 = a->getSize().x * a->getSize().y;
                  auto size2 = b->getSize().x * b->getSize().y;
                  return size1 >= size2;
              });
}

void placeComponents(std::vector<std::shared_ptr<Graph>> &components, const std::shared_ptr<Graph> &target,
                     double maxRowWidth, double componentSpacing) {
    double xpos = 0;
    double ypos = 0;
    double highestBox = 0;
    double broadestRow = componentSpacing;
    for (auto graph : components) {
        auto size = graph->getSize();
        if (xpos + size.x > maxRowWidth) {
            xpos = 0;
            ypos += highestBox + componentSpacing;
            highestBox = 0;
        }
        auto offset = graph->getOffset();
        offsetGraph(graph, xpos + offset.x, ypos + offset.y);
        offset.reset();
        broadestRow = std::max(broadestRow, xpos + size.x);
        highestBox = std::max(highestBox, size.y);
        xpos += size.x + componentSpacing;
    }
}

void offsetGraph(std::shared_ptr<Graph> &graph, double offsetX, double offsetY) {
    auto graphOffset = KVector(offsetX, offsetY);
    for (const auto &node : graph->getLayerlessNodes()) {
        node->getPos().add(graphOffset);
        for (const auto &port : node->getOutputPorts()) {
            for (const auto &edge : port->getEdges()) {
                edge->getBendPoints().offset(graphOffset);
            }
        }
    }
}

}  // namespace GuiBridge
