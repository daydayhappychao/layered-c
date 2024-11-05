#ifndef GREEDY_PORT_DISTRIBUTOR_H
#define GREEDY_PORT_DISTRIBUTOR_H

#include <memory>
#include <vector>
#include "ISweepPortDistributor.h"

namespace GuiBridge {
class Node;
class Port;
class CrossingCounter;
class Initializable;
class BetweenLayerEdgeTwoNodeCrossingsCounter;
class ISweepPortDistributor;
enum class PortType;

class GreedyPortDistributor : public ISweepPortDistributor {
public:
    bool distributePortsWhileSweeping(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex,
                                      bool isForwardSweep) override;

    void initAtNodeLevel(int l, int n, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override;
    void initAfterTraversal() override;

private:
    CrossingCounter *crossingsCounter;
    int nPorts;
    std::vector<int> portPos;
    BetweenLayerEdgeTwoNodeCrossingsCounter *hierarchicalCrossingsCounter;

    bool distributePortsInLayer(const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex,
                                bool isForwardSweep);
    void initForLayers(std::vector<std::shared_ptr<Node>> &leftLayer, std::vector<std::shared_ptr<Node>> &rightLayer);

    void initialize(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex, bool isForwardSweep);
};
}  // namespace GuiBridge

#endif  // GREEDY_PORT_DISTRIBUTOR_H