#ifndef SWITCH_DECIDER_H
#define SWITCH_DECIDER_H

#include <memory>
#include <stdexcept>
#include <vector>
#include "../../../opts/PortType.h"

namespace GuiBridge {

class Node;
class CrossingMatrixFiller;
class CrossingCounter;
class GraphInfoHolder;

enum class CrossingCountSide { WEST, EAST };
class SwitchDecider {
public:
    SwitchDecider(int freeLayerIndex, const std::vector<std::vector<std::shared_ptr<Node>>> &graph,
                  std::shared_ptr<CrossingMatrixFiller> &crossingMatrixFiller, std::vector<int> &portPositions,
                  std::shared_ptr<GraphInfoHolder> &graphData, bool oneSided);

    void notifyOfSwitch(std::shared_ptr<Node> &upperNode, std::shared_ptr<Node> &lowerNode);
    bool doesSwitchReduceCrossings(int upperNodeIndex, int lowerNodeIndex) const;

private:
    std::vector<std::shared_ptr<Node>> freeLayer;
    std::shared_ptr<CrossingCounter> leftInLayerCounter;
    std::shared_ptr<CrossingCounter> rightInLayerCounter;
    std::shared_ptr<CrossingMatrixFiller> crossingMatrixFiller;
    std::shared_ptr<GraphInfoHolder> graphData;
    std::shared_ptr<CrossingCounter> parentCrossCounter;

    bool constraintsPreventSwitch(int nodeIndex, int lowerNodeIndex) const;
    bool haveSuccessorConstraints(const std::shared_ptr<Node> &upperNode, const std::shared_ptr<Node> &lowerNode) const;
    bool haveLayoutUnitConstraints(const std::shared_ptr<Node> &upperNode,
                                   const std::shared_ptr<Node> &lowerNode) const;
};
}  // namespace GuiBridge

#endif  // SWITCH_DECIDER_H