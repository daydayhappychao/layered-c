#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <memory>
#include <vector>

#include "Edge.h"
#include "Node.h"
#include "Port.h"
namespace GuiBridge {

class Graph {
public:
    void addNode(const std::shared_ptr<Node>& node);
    void addEdge(const std::shared_ptr<Edge>& edge);
    std::vector<std::shared_ptr<Node>> &getNodes();
    std::vector<std::shared_ptr<Edge>> &getEdges();
    std::vector<std::shared_ptr<Node>> getLayerlessNodes();

private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;
};
}  // namespace GuiBridge

#endif  // GRAPH_HPP
