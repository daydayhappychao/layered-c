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
    void process(std::shared_ptr<Graph> &graph);
    std::shared_ptr<Node> createDummyNode(std::shared_ptr<Graph> &graph, std::shared_ptr<Layer> &targetLayer,
                                          std::shared_ptr<Edge> &edgeToSplit);
    void splitEdge(std::shared_ptr<Graph> &graph, std::shared_ptr<Edge> &edge, std::shared_ptr<Node> &dummyNode);
};
}  // namespace GuiBridge

#endif