#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Edge.h"
#include "Layer.h"
#include "Node.h"
#include "NodeProto.h"
#include "Port.h"
#include "Shape.h"
#include "math/KVector.h"
#include "nlohmann/json_fwd.hpp"

namespace GuiBridge {

class Graph : public std::enable_shared_from_this<Graph>, public Shape {
public:
    // NodeProto

    void addNodeProto(const std::shared_ptr<NodeProto> &nodeProto);
    void addNodeProto(std::string name, double width, double height, int id);
    std::vector<std::shared_ptr<NodeProto>> &getNodeProtos() { return nodeProtos; };

    // Port

    void addPort(std::string name, int id, PortType portType, int protoId);
    std::vector<std::shared_ptr<Port>> &getPorts() { return ports; };

    // Node

    void addNode(const std::shared_ptr<Node> &node);
    void addNode(int id, int protoId, std::string name);

    // edge

    void addEdge(std::shared_ptr<Edge> &edge);
    void addEdge(std::shared_ptr<Node> &srcNode, std::shared_ptr<Port> &srcPort, std::shared_ptr<Node> &dstNode,
                 std::shared_ptr<Port> &dstPort);
    void addEdge(int srcNodeId, int srcPortId, int dstNodeId, int dstPortId);

    std::vector<std::shared_ptr<Node>> &getNodes();
    std::vector<std::shared_ptr<Edge>> &getEdges();
    std::vector<std::shared_ptr<Layer>> &getLayers();
    void addLayers(const std::shared_ptr<Layer> &layer);
    std::vector<std::shared_ptr<Node>> &getLayerlessNodes();
    std::vector<std::shared_ptr<Node>> &getHiddenNodes();
    void setHiddenNodes(std::vector<std::shared_ptr<Node>> &nodes);

    std::vector<std::vector<std::shared_ptr<Node>>> toNodeArray();
    nlohmann::json json();

private:
    std::vector<std::shared_ptr<NodeProto>> nodeProtos;
    std::vector<std::shared_ptr<Port>> ports;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Node>> layerlessNodes;
    std::vector<std::shared_ptr<Node>> hiddenNodes;
    std::vector<std::shared_ptr<Edge>> edges;
    // 层级数据，最后生效的数据应该都在里面，上面的 nodes,edges 都是临时存放数据用的
    std::vector<std::shared_ptr<Layer>> layers;

    std::shared_ptr<NodeProto> getProtoById(int id);
};
}  // namespace GuiBridge

#endif  // GRAPH_HPP
