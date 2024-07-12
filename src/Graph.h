#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <memory>
#include <vector>

#include "Edge.h"
#include "Layer.h"
#include "Node.h"
#include "math/KVector.h"
namespace GuiBridge {

class Graph {
public:
    void addNode(const std::shared_ptr<Node> &node);
    void addEdge(const std::shared_ptr<Edge> &edge);
    std::vector<std::shared_ptr<Node>> &getNodes();
    std::vector<std::shared_ptr<Edge>> &getEdges();
    std::vector<std::shared_ptr<Layer>> &getLayers();
    std::vector<std::shared_ptr<Node>> getLayerlessNodes();
    KVector getSize() const;
    KVector getOffset() const;

private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;
    KVector size = KVector();
    KVector offset = KVector();
    std::vector<std::shared_ptr<Layer>> layers;
};
}  // namespace GuiBridge

#endif  // GRAPH_HPP
