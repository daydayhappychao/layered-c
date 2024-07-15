#ifndef COMPONENTS_PROCESSOR_HPP
#define COMPONENTS_PROCESSOR_HPP
#include <memory>
#include <set>
#include <vector>
#include "../opts/PortType.h"

namespace GuiBridge {
class Graph;
class Node;

using DfsDataType = std::pair<std::vector<std::shared_ptr<Node>>, std::set<PortType>>;
class ComponentsProcessor {
public:
    ComponentsProcessor();
    std::vector<std::shared_ptr<Graph>> split(const std::shared_ptr<Graph> &graph);
    std::optional<DfsDataType> dfs(const std::shared_ptr<Node> &node, std::optional<DfsDataType> &data);
    void combine(const std::vector<std::shared_ptr<Graph>> &components, const std::shared_ptr<Graph> &target);
};
}  // namespace GuiBridge
#endif  // COMPONENTS_PROCESSOR_HPP