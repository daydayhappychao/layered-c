#ifndef NETWORK_SIMPLEX_LAYERER_HPP
#define NETWORK_SIMPLEX_LAYERER_HPP

#include <memory>
#include <vector>
namespace GuiBridge {
class Graph;
class Node;
class Port;
class Edge;
class NGraph;
class NetworkSimplexLayerer {
public:
    std::vector<std::vector<std::shared_ptr<Node>>> connectedComponents(std::vector<std::shared_ptr<Node>> &theNodes);
    void connectedComponentsDFS(std::shared_ptr<Node> node);
    void process(std::shared_ptr<Graph> &graph);
    std::shared_ptr<Port> getOpposite(std::shared_ptr<Port> port, std::shared_ptr<Edge> edge);

private:
    std::vector<std::shared_ptr<Node>> componentNodes;
    std::vector<bool> nodeVisited;
    NGraph initialize(std::vector<Node> &theNodes);
    void dispose();
};
}  // namespace GuiBridge

#endif