#include "./ComponentsProcessor.h"

#include <optional>
#include <set>
#include <utility>
#include <vector>

#include "../Graph.h"

namespace GuiBridge {
std::optional<DfsDataType> ComponentsProcessor::dfs(const std::shared_ptr<Node> &node,
                                                    const std::optional<DfsDataType> &data) {
    std::optional<DfsDataType> mutableData;
    if (node->getId() == 0) {
        node->setId(1);
        if (!data.has_value()) {
            std::vector<std::shared_ptr<Node>> component;
            std::set<PortType> extPortSides;
            mutableData = DfsDataType(component, extPortSides);
        } else {
            mutableData = data;
        }

        mutableData->first.push_back(node);

        for (auto port1 : node->getAllPorts()) {
            for (auto port2 : port1->getConnectedPorts()) {
                dfs(port2->getNode(), mutableData);
            }
        }
    }
    return mutableData;
};

std::vector<Graph> ComponentsProcessor::split(const std::shared_ptr<Graph> &graph) {
    std::vector<Graph> result;

    for (auto node : graph->getLayerlessNodes()) {
        node->setId(0);
    }

    for (auto node : graph->getLayerlessNodes()) {
        auto data = dfs(node, std::nullopt);
        if (data.has_value()) {
            Graph newGraph;

            for (auto n : data->first) {
                newGraph.addNode(n);
            }
            result.push_back(newGraph);
        }
    }
    return result;
};

void ComponentsProcessor::combine(const std::vector<std::shared_ptr<Graph>> &components, const std::shared_ptr<Graph> &target) {
    
}

}  // namespace GuiBridge