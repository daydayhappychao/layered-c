#ifndef NETWORK_SIMPLEX_HPP
#define NETWORK_SIMPLEX_HPP

#include <deque>
#include <memory>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

namespace GuiBridge {
class Graph;
class Node;
class Edge;

class NetworkSimplex {
public:
    explicit NetworkSimplex(std::shared_ptr<Graph> graphPtr, int limit);
    ~NetworkSimplex();

    void execute();

    void removeSubtrees();

    void initialize();

    void feasibleTree();

    void layeringTopologicalNumbering();

    int tightTreeDFS(const std::shared_ptr<Node> &node);

    int postorderTraversal(const std::shared_ptr<Node> &node);

    void cutvalues();

    std::shared_ptr<Edge> leaveEdge();

    void exchange(const std::shared_ptr<Edge> &leave, const std::shared_ptr<Edge> &enter);

    bool isInHead(const std::shared_ptr<Node> &node, const std::shared_ptr<Edge> &edge);

    std::shared_ptr<Edge> enterEdge(const std::shared_ptr<Edge> &leave);

    std::vector<int> normalize();

    void balance(std::vector<int> filling);

    std::pair<int, int> minimalSpan(const std::shared_ptr<Node> &node);

    std::shared_ptr<Edge> minimalSlack();

private:
    std::shared_ptr<Graph> graph;

    std::vector<int> poID;

    std::vector<int> lowestPoID;
    std::vector<std::shared_ptr<Node>> sources;
    std::vector<double> cutvalue;
    std::vector<bool> edgeVisited;

    std::vector<std::shared_ptr<Edge>> edges;

    std::unordered_set<std::shared_ptr<Edge>> treeEdges;

    int postOrder;

    std::deque<std::pair<std::shared_ptr<Node>, std::shared_ptr<Edge>>> subtreeNodesStack;
    int iterationLimit = std::numeric_limits<int>::max();
};

}  // namespace GuiBridge

#endif