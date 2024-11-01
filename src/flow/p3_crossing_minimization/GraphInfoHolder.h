#ifndef GRAPH_INFO_HOLDER_HPP
#define GRAPH_INFO_HOLDER_HPP

#include <memory>
#include <vector>
#include "../../opts/CrossMinType.h"
#include "counting/Initializable.h"
namespace GuiBridge {
class Graph;
class Node;
class GraphInfoHolder : public Initializable {
public:
    explicit GraphInfoHolder(const std::shared_ptr<Graph> &graph, CrossMinType crossMinType);
    std::shared_ptr<Graph> graph;
    std::vector<std::vector<std::shared_ptr<Node>>> currentNodeOrder;
    
private:
};
}  // namespace GuiBridge
#endif