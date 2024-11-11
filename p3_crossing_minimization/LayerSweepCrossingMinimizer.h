#ifndef LAYER_SWEEP_CROSSING_MINIMIZER
#define LAYER_SWEEP_CROSSING_MINIMIZER
#include <list>
#include <memory>
#include <random>
#include "../../opts/CrossMinType.h"
#include "GraphInfoHolder.h"
namespace GuiBridge {
class Graph;

class LayerSweepCrossingMinimizer {
public:
    explicit LayerSweepCrossingMinimizer(CrossMinType ct) : crossMinType(ct){};
    void process(const std::shared_ptr<Graph> &graph);
    void compareDifferentRandomizedLayouts();

private:
    CrossMinType crossMinType;
    std::mt19937 random;
    std::shared_ptr<GraphInfoHolder> graphInfoHolder;
    void initialize(const std::shared_ptr<Graph> &rootGraph);
    void compareDifferentRandomizedLayouts(std::shared_ptr<GraphInfoHolder> &gData);
    int minimizeCrossingsWithCounter(std::shared_ptr<GraphInfoHolder> &gData);
    void saveAllNodeOrdersOfChangedGraphs();
    double countCurrentNumberOfCrossingsNodePortOrder(std::shared_ptr<GraphInfoHolder> &currentGraph);
    void setCurrentlyBestNodeOrders();
    bool sweepReducingCrossings(std::shared_ptr<GraphInfoHolder> &graph, bool forward, bool firstSweep);
    void transferNodeAndPortOrdersToGraph();
    void minimizeCrossingsNoCounter(std::shared_ptr<GraphInfoHolder> &gData);

    bool isFirstTry = true;
    bool isSecondTry = false;
    bool wasNodeOrderChanged = false;
    int firstIndex(bool isForwardSweep, int length) { return isForwardSweep ? 0 : length - 1; }
    int firstFree(bool isForwardSweep, int length) { return isForwardSweep ? 1 : length - 2; }
    bool isNotEnd(int length, int freeLayerIndex, bool isForwardSweep) {
        return isForwardSweep ? freeLayerIndex < length : freeLayerIndex >= 0;
    }
    int next(bool isForwardSweep) { return isForwardSweep ? 1 : -1; }
};
}  // namespace GuiBridge
#endif