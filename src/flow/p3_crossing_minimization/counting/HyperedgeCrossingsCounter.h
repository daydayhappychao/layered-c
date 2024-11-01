#ifndef HYPEREDGE_CROSSINGS_COUNTER_H
#define HYPEREDGE_CROSSINGS_COUNTER_H

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace GuiBridge {
class Edge;  // Forward declaration
class Node;  // Forward declaration
class Port;  // Forward declaration

class Hyperedge {
public:
    std::vector<std::shared_ptr<Edge>> edges;
    std::vector<std::shared_ptr<Port>> ports;
    int upperLeft = INT_MAX;
    int lowerLeft = INT_MIN;
    int upperRight = INT_MAX;
    int lowerRight = INT_MIN;

    bool operator<(const Hyperedge &other) const;
};

class HyperedgeCorner {
public:
    Hyperedge *hyperedge;
    int position;
    int oppositePosition;
    enum class Type { UPPER, LOWER } type;

    HyperedgeCorner(Hyperedge *he, int pos, int oppPos, Type t)
        : hyperedge(he), position(pos), oppositePosition(oppPos), type(t) {}

    bool operator<(const HyperedgeCorner &other) const;
};

class HyperedgeCrossingsCounter {
public:
    explicit HyperedgeCrossingsCounter(std::vector<int> &portPos);

    int countCrossings(const std::vector<std::shared_ptr<Node>> &leftLayer,
                       const std::vector<std::shared_ptr<Node>> &rightLayer);

private:
    std::vector<int> portPos;
};
}  // namespace GuiBridge

#endif  // HYPEREDGE_CROSSINGS_COUNTER_H