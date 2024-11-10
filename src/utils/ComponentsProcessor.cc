#include "./ComponentsProcessor.h"

#include <memory>
#include <optional>
#include <set>
#include <utility>
#include <vector>

#include "../Graph.h"

namespace GuiBridge {
ComponentsProcessor::ComponentsProcessor() = default;
;
std::optional<DfsDataType> ComponentsProcessor::dfs(const std::shared_ptr<Node> &node,
                                                    std::optional<DfsDataType> &data) {
    if (node->getId() == 0) {
        node->setId(1);

        if (!data.has_value()) {
            std::vector<std::shared_ptr<Node>> component;
            std::set<PortType> extPortSides;
            data = DfsDataType(component, extPortSides);
        }

        data->first.push_back(node);

        for (auto &port1 : node->getAllPorts()) {
            for (const auto &port2 : node->getConnectedPorts(port1)) {
                dfs(port2.node, data);
            }
        }
        return data;
    }
    return std::nullopt;
};

std::vector<std::shared_ptr<Graph>> ComponentsProcessor::split(const std::shared_ptr<Graph> &graph) {
    std::vector<std::shared_ptr<Graph>> result;

    for (const auto &node : graph->getLayerlessNodes()) {
        node->setId(0);
    }

    for (const auto &node : graph->getLayerlessNodes()) {
        std::optional<DfsDataType> nullData = std::nullopt;
        auto data = dfs(node, nullData);
        if (data.has_value()) {
            auto newGraph = std::make_shared<Graph>();

            for (const auto &n : data->first) {
                newGraph->addNode(n);
            }
            result.push_back(newGraph);
        }
    }
    return result;
};

void ComponentsProcessor::combine(const std::vector<std::shared_ptr<Graph>> &components,
                                  const std::shared_ptr<Graph> &target) {}

}  // namespace GuiBridge