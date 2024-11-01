#ifndef ICROSSING_MINIMIZATION_HEURISTIC_H
#define ICROSSING_MINIMIZATION_HEURISTIC_H

#include <memory>
#include <vector>
#include "counting/Initializable.h"
namespace GuiBridge {
class Node;
class ICrossingMinimizationHeuristic : public Initializable {
public:
    virtual bool alwaysImproves() = 0;

    virtual bool setFirstLayerOrder(std::vector<std::vector<std::shared_ptr<Node>>> &order, bool forwardSweep) = 0;
    virtual bool minimizeCrossings(std::vector<std::vector<std::shared_ptr<Node>>> &order, int freeLayerIndex,
                                   bool forwardSweep, bool isFirstSweep) = 0;
    virtual bool isDeterministic();
};
}  // namespace GuiBridge
#endif