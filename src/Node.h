

#ifndef NODE_HPP
#define NODE_HPP

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "NodeProto.h"
#include "Shape.h"
#include "math/Margin.h"
#include "nlohmann/json_fwd.hpp"
#include "opts/NodeSide.h"
#include "opts/PortType.h"
#include "utils/HiddenNodeConnections.h"
namespace GuiBridge {

class Graph;
class Layer;
class Edge;
class Port;
class EdgeTarget;
class HiddenNodeConnections;

class Node : public std::enable_shared_from_this<Node>, public Shape {
public:
    explicit Node(std::string name, std::shared_ptr<NodeProto> &proto, int _id);
    Node(std::string name, std::shared_ptr<NodeProto> &proto, int _id, NodeSide side);
    int getId() const;
    void setId(int id);
    // void addInputPort(const std::shared_ptr<Port> &port);
    // void addOutputPort(const std::shared_ptr<Port> &port);

    // ports 相关

    std::vector<std::shared_ptr<Port>> &getInputPorts();
    std::vector<std::shared_ptr<Port>> &getOutputPorts();
    std::vector<std::shared_ptr<Port>> getAllPorts();
    std::vector<std::shared_ptr<Port>> &getPortsByPortType(PortType type);
    // 寻找与该 port 连接的 port
    std::vector<EdgeTarget> getConnectedPorts(std::shared_ptr<Port> &port);

    // edge相关

    std::vector<std::shared_ptr<Edge>> getEdges();
    std::vector<std::shared_ptr<Edge>> getIncomingEdges();
    std::vector<std::shared_ptr<Edge>> getOutgoingEdges();
    std::vector<std::shared_ptr<Edge>> getEdgesByPort(const std::shared_ptr<Port> &port);
    void addEdge(std::shared_ptr<Port> &port, std::shared_ptr<Edge> &edge);
    // void addEdge(std::shared_ptr<Port> &fromPort, EdgeTarget &target);
    void removeEdge(std::shared_ptr<Port> &port, std::shared_ptr<Edge> &edge);

    std::shared_ptr<Layer> &getLayer();
    std::shared_ptr<Graph> &getGraph();
    void setGraph(std::shared_ptr<Graph> &nextGraph);
    NodeSide getSide();
    void setSide(NodeSide nextSide);
    HiddenNodeConnections::Connection getHiddenNodeConnections();
    void setHiddenNodeConnections(HiddenNodeConnections::Connection &connections);
    void setLayer(const std::shared_ptr<Layer> &layer);

    nlohmann::json json();

    // 用于 networksimplex 的属性
    int layerIndex;
    int internalId;
    bool treeNode;
    std::vector<std::shared_ptr<Edge>> unknownCutvalues;

    // 用于 dummyNode 的属性
    bool isDummy = false;
    std::shared_ptr<Edge> dummyOrigin;
    std::shared_ptr<Port> dummySource;
    std::shared_ptr<Port> dummyTarget;
    void setDummy(const std::shared_ptr<Edge> &dummyOrigin);
    void setDummySource(const std::shared_ptr<Port> &dummySource) { this->dummySource = dummySource; };
    void setDummyTarget(const std::shared_ptr<Port> &dummyTarget) { this->dummyTarget = dummyTarget; };

    Margin margin;
    std::string name;

    // 最初的id
    int _id;

private:
    int id;
    std::shared_ptr<NodeProto> proto;
    std::shared_ptr<Graph> graph;
    std::shared_ptr<Layer> layer = nullptr;
    std::map<std::shared_ptr<Port>, std::vector<std::shared_ptr<Edge>>> edges;

    NodeSide side;
    HiddenNodeConnections::Connection hiddenNodeConnections = HiddenNodeConnections::Connection::NONE;

    
};
}  // namespace GuiBridge
#endif  // NODE_HPP