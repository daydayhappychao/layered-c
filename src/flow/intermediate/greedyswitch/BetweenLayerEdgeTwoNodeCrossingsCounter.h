#ifndef BETWEEN_LAYER_EDGE_TWO_NODE_CROSSINGS_COUNTER_H
#define BETWEEN_LAYER_EDGE_TWO_NODE_CROSSINGS_COUNTER_H

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include "../../../opts/PortType.h"

namespace GuiBridge {

class Edge;
class Node;
class Port;
class BetweenLayerEdgeTwoNodeCrossingsCounter;

class AdjacencyList {
public:
    AdjacencyList(std::shared_ptr<Node> &node, PortType side, BetweenLayerEdgeTwoNodeCrossingsCounter *that);

    void reset();
    int countAdjacenciesBelowNodeOfFirstPort();
    void removeFirst();
    bool isEmpty() const;
    int first();
    int size();

    std::string toString() const;

private:
    struct Adjacency {
        int position;
        int cardinality;
        int currentCardinality;

        Adjacency(int adjacentPortPosition, std::shared_ptr<Port> &port);

        void reset();
        bool operator<(const Adjacency &other) const;
    };

    std::shared_ptr<Node> node;
    std::vector<Adjacency *> adjacencyList;
    PortType side;
    int size_ = 0;
    int currentSize = 0;
    int currentIndex = 0;
    BetweenLayerEdgeTwoNodeCrossingsCounter *that;

    void getAdjacenciesSortedByPosition();
    void iterateTroughEdgesCollectingAdjacencies();
    std::vector<std::shared_ptr<Edge>> getEdgesConnectedTo(std::shared_ptr<Port> &port);

    bool isNotInLayer(const std::shared_ptr<Edge> &edge) const;
    void addAdjacencyOf(const std::shared_ptr<Edge> &edge);
    std::shared_ptr<Port> adjacentPortOf(const std::shared_ptr<Edge> &edge, PortType side);
    Adjacency *currentAdjacency();
    void incrementCurrentIndex();
};

class BetweenLayerEdgeTwoNodeCrossingsCounter {
public:
    BetweenLayerEdgeTwoNodeCrossingsCounter(std::vector<std::vector<std::shared_ptr<Node>>> &currentNodeOrder,
                                            int freeLayerIndex);

    void countEasternEdgeCrossings(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode);
    void countWesternEdgeCrossings(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode);
    void countBothSideCrossings(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode);

    int getUpperLowerCrossings();
    int getLowerUpperCrossings();

    std::map<std::shared_ptr<Port>, int> portPositions;

private:
    void resetCrossingCount();
    void addEasternCrossings(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode);
    void addWesternCrossings(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode);
    void countCrossingsByMergingAdjacencyLists();
    AdjacencyList *getAdjacencyFor(std::shared_ptr<Node> &node, PortType side,
                                   std::map<std::shared_ptr<Node>, AdjacencyList *> &adjacencies);

    bool freeLayerIsNotFirstLayer() const;
    bool freeLayerIsNotLastLayer() const;
    void setPortPositionsForNeighbouringLayers();
    void setPortPositionsForLayer(int layerIndex, PortType portSide);

    int upperLowerCrossings;
    int lowerUpperCrossings;
    AdjacencyList *upperAdjacencies;
    AdjacencyList *lowerAdjacencies;

    std::vector<std::vector<std::shared_ptr<Node>>> &currentNodeOrder;
    int freeLayerIndex;
    std::map<std::shared_ptr<Node>, AdjacencyList *> easternAdjacencies;
    std::map<std::shared_ptr<Node>, AdjacencyList *> westernAdjacencies;

    bool isBelow(int firstPort, int secondPort);
};
}  // namespace GuiBridge
#endif  // BETWEEN_LAYER_EDGE_TWO_NODE_CROSSINGS_COUNTER_H