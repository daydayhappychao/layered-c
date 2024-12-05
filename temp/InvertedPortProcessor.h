#ifndef INVERTED_PORT_PROCESSOR
#define INVERTED_PORT_PROCESSOR

#include <memory>
#include <vector>

namespace GuiBridge {
class Graph;
class Edge;
class Port;
class Node;

class InvertedPortProcessor {
public:
    void process(std::shared_ptr<Graph> &layeredGraph);

private:
    void createEastPortSideDummies(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<Port> &eastwardPort,
                                   std::shared_ptr<Edge> &edge, std::vector<std::shared_ptr<Node>> &layerNodeList);
    void createWestPortSideDummies(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<Port> &westwardPort,
                                   std::shared_ptr<Edge> &edge, std::vector<std::shared_ptr<Node>> &layerNodeList);
    void setLongEdgeSourceAndTarget(std::shared_ptr<Node> &longEdgeDummy, std::shared_ptr<Port> &dummyInputPort,
                                    std::shared_ptr<Port> &dummyOutputPort, std::shared_ptr<Port> &oddPort);
};
}  // namespace GuiBridge

#endif