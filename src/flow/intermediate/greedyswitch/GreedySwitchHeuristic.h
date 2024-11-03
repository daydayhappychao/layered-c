#ifndef GREEDY_SWITCH_HEURISTIC_H
#define GREEDY_SWITCH_HEURISTIC_H

#include <memory>
#include <vector>
#include "../../../Node.h"
#include "../../../opts/CrossMinType.h"
#include "../../p3_crossing_minimization/GraphInfoHolder.h"
#include "../../p3_crossing_minimization/ICrossingMinimizationHeuristic.h"
#include "SwitchDecider.h"

namespace GuiBridge {
class GreedySwitchHeuristic : public ICrossingMinimizationHeuristic {
public:
    GreedySwitchHeuristic(CrossMinType greedyType, std::shared_ptr<GraphInfoHolder> &graphData);

    bool minimizeCrossings(std::vector<std::vector<std::shared_ptr<Node>>> &order, int freeLayerIndex,
                           bool forwardSweep, bool isFirstSweep) override;
    bool setFirstLayerOrder(std::vector<std::vector<std::shared_ptr<Node>>> &order, bool forwardSweep) override;
    bool alwaysImproves() override;
    bool isDeterministic() override;
    void initAtPortLevel(int l, int n, int p, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override;
    void initAtLayerLevel(int l,  std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override;
    void initAfterTraversal() override;

private:
    void setUp(std::vector<std::vector<std::shared_ptr<Node>>> &order, int freeLayerIndex, bool forwardSweep);
    std::unique_ptr<SwitchDecider> getNewSwitchDecider(int freeLayerIndex, CrossingCountSide side);
    bool continueSwitchingUntilNoImprovementInLayer(int freeLayerIndex);
    bool sweepDownwardInLayer(int layerIndex);
    bool switchIfImproves(int layerIndex, int upperNodeIndex, int lowerNodeIndex);
    void exchangeNodes(int indexOne, int indexTwo, int layerIndex);
    int startIndex(bool isForwardSweep, int length);

    CrossMinType greedySwitchType;
    std::vector<std::vector<std::shared_ptr<Node>>> currentNodeOrder;
    std::unique_ptr<SwitchDecider> switchDecider;
    std::vector<int> portPositions;
    std::shared_ptr<GraphInfoHolder> graphData;
    int nPorts = 0;
};
}  // namespace GuiBridge

#endif  // GREEDY_SWITCH_HEURISTIC_H