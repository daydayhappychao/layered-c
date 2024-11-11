#ifndef GRAPH_INFO_HOLDER_HPP
#define GRAPH_INFO_HOLDER_HPP

#include <list>
#include <memory>
#include <vector>
#include "../../opts/CrossMinType.h"
#include "./counting/AllCrossingsCounter.h"
#include "ICrossingMinimizationHeuristic.h"
#include "ISweepPortDistributor.h"
#include "SweepCopy.h"
#include "counting/Initializable.h"
namespace GuiBridge {
class Graph;
class Node;
class GraphInfoHolder : public Initializable, public std::enable_shared_from_this<GraphInfoHolder> {
public:
    explicit GraphInfoHolder(const std::shared_ptr<Graph> &graph, CrossMinType crossMinType);
    void init();
    std::shared_ptr<Graph> graph;

    // node orders
    std::vector<std::vector<std::shared_ptr<Node>>> currentNodeOrder;
    std::shared_ptr<SweepCopy> currentlyBestNodeAndPortOrder;
    std::shared_ptr<SweepCopy> bestNodeAndPortOrder;
    std::vector<int> portPositions;

    bool useBottomUp;

    std::shared_ptr<ICrossingMinimizationHeuristic> crossMinimizer;
    std::shared_ptr<ISweepPortDistributor> portDistributor;
    std::shared_ptr<AllCrossingsCounter> crossingsCounter;
    int nPorts;

    bool dontSweepInto() { return useBottomUp; };
    void setCurrentlyBestNodeAndPortOrder(std::shared_ptr<SweepCopy> &currentlyBestNodeAndPortOrder) {
        this->currentlyBestNodeAndPortOrder = currentlyBestNodeAndPortOrder;
    };

    void setBestNodeNPortOrder(std::shared_ptr<SweepCopy> &bestNodeNPortOrder) {
        this->bestNodeAndPortOrder = bestNodeNPortOrder;
    };

    std::shared_ptr<SweepCopy> getBestSweep() {
        return crossMinimizer->isDeterministic() ? currentlyBestNodeAndPortOrder : bestNodeAndPortOrder;
    }

    bool crossMinDeterministic() { return crossMinimizer->isDeterministic(); };

    bool crossMinAlwaysImproves() { return crossMinimizer->alwaysImproves(); };

    void initAtNodeLevel(int l, int n, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override {}

    void initAtPortLevel(int l, int n, int p, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override {
        nPorts++;
    }
    void initAfterTraversal() override { portPositions.resize(nPorts); }

private:
    CrossMinType crossMinType;
};
}  // namespace GuiBridge
#endif