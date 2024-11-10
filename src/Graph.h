#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <memory>
#include <vector>

#include "Edge.h"
#include "Layer.h"
#include "Node.h"
#include "Shape.h"
#include "math/KVector.h"
#include "nlohmann/json_fwd.hpp"

namespace GuiBridge {

class Graph : public std::enable_shared_from_this<Graph>, public Shape {
public:
    void addNode(const std::shared_ptr<Node> &node);
    void _addEdge(const std::shared_ptr<Edge> &edge);
    std::vector<std::shared_ptr<Node>> &getNodes();
    std::vector<std::shared_ptr<Edge>> &getEdges();
    std::vector<std::shared_ptr<Layer>> &getLayers();
    void addLayers(const std::shared_ptr<Layer> &layer);
    std::vector<std::shared_ptr<Node>> &getLayerlessNodes();
    std::vector<std::shared_ptr<Node>> &getHiddenNodes();
    void setHiddenNodes(std::vector<std::shared_ptr<Node>> &nodes);
    KVector getSize() const;
    KVector getOffset() const;

    std::vector<std::vector<std::shared_ptr<Node>>> toNodeArray();
    nlohmann::json json();

private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Node>> layerlessNodes;
    std::vector<std::shared_ptr<Node>> hiddenNodes;
    std::vector<std::shared_ptr<Edge>> edges;
    KVector size = KVector();
    KVector offset = KVector();
    // 层级数据，最后生效的数据应该都在里面，上面的 nodes,edges 都是临时存放数据用的
    std::vector<std::shared_ptr<Layer>> layers;
};
}  // namespace GuiBridge

#endif  // GRAPH_HPP
