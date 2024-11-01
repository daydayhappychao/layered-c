#ifndef LONG_EDGE_SPLITTER_HPP
#define LONG_EDGE_SPLITTER_HPP

#include <memory>
namespace GuiBridge {
class Graph;
class Node;
class Layer;
class Edge;
class LongEdgeSplitter {
public:
    void process(const std::shared_ptr<Graph> &graph);
    std::shared_ptr<Node> createDummyNode(const std::shared_ptr<Graph> &graph,
                                          const std::shared_ptr<Layer> &targetLayer,
                                          const std::shared_ptr<Edge> &edgeToSplit);
    std::shared_ptr<Edge> splitEdge(const std::shared_ptr<Edge> &edge, const std::shared_ptr<Node> &dummyNode);
};
}  // namespace GuiBridge

#endif