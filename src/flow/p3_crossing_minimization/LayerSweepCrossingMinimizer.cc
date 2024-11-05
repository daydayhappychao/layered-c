#include "./LayerSweepCrossingMinimizer.h"
#include <memory>
#include "../../Graph.h"
#include "../../opts/CrossMinType.h"
#include "../../utils/Constants.h"
#include "../../utils/randomUtil.h"
#include "GraphInfoHolder.h"
#include "SweepCopy.h"

namespace GuiBridge {
void LayerSweepCrossingMinimizer::process(const std::shared_ptr<Graph> &graph) {
    initialize(graph);
    if (!graphInfoHolder->crossMinDeterministic()) {
        compareDifferentRandomizedLayouts(graphInfoHolder);
    } else if (graphInfoHolder->crossMinAlwaysImproves()) {
        minimizeCrossingsNoCounter(graphInfoHolder);
    } else {
        minimizeCrossingsWithCounter(graphInfoHolder);
    }

    transferNodeAndPortOrdersToGraph();
}

void LayerSweepCrossingMinimizer::initialize(const std::shared_ptr<Graph> &rootGraph) {
    auto gData = std::make_shared<GraphInfoHolder>(rootGraph, crossMinType);
    gData->init();
    graphInfoHolder = gData;
}

void LayerSweepCrossingMinimizer::compareDifferentRandomizedLayouts(std::shared_ptr<GraphInfoHolder> &gData) {
    wasNodeOrderChanged = false;
    int bestCrossings = std::numeric_limits<int>::max();
    for (int i = 0; i < THOROUGHNESS; i++) {
        int crossings = minimizeCrossingsWithCounter(gData);
        if (crossings < bestCrossings) {
            bestCrossings = crossings;
            saveAllNodeOrdersOfChangedGraphs();
            if (bestCrossings == 0) {
                break;
            }
        }
    }
}

int LayerSweepCrossingMinimizer::minimizeCrossingsWithCounter(std::shared_ptr<GraphInfoHolder> &gData) {
    bool isForwardSweep = randomUtil->randomBoolean();

    double initialCrossings = countCurrentNumberOfCrossingsNodePortOrder(gData);
    if (initialCrossings == 0) {
        return 0;
    }

    if (!isFirstTry && !isSecondTry) {
        gData->crossMinimizer->setFirstLayerOrder(gData->currentNodeOrder, isForwardSweep);
    } else {
        isForwardSweep = isFirstTry;
    }

    sweepReducingCrossings(gData, isForwardSweep, true);

    if (isSecondTry) {
        isSecondTry = false;
    }

    if (isFirstTry) {
        isFirstTry = false;
        isSecondTry = true;
    }

    double crossingsInGraph = countCurrentNumberOfCrossingsNodePortOrder(gData);
    double oldNumberOfCrossings;

    do {
        setCurrentlyBestNodeOrders();

        if (crossingsInGraph == 0) {
            return 0;
        }
        isForwardSweep = !isForwardSweep;
        oldNumberOfCrossings = crossingsInGraph;
        sweepReducingCrossings(gData, isForwardSweep, false);
        crossingsInGraph = countCurrentNumberOfCrossingsNodePortOrder(gData);
    } while (oldNumberOfCrossings > crossingsInGraph);
    return oldNumberOfCrossings;
}

void LayerSweepCrossingMinimizer::saveAllNodeOrdersOfChangedGraphs() {
    if (wasNodeOrderChanged) {
        auto ptr = std::make_shared<SweepCopy>(graphInfoHolder->currentlyBestNodeAndPortOrder);
        graphInfoHolder->setBestNodeNPortOrder(ptr);
    }
};

double LayerSweepCrossingMinimizer::countCurrentNumberOfCrossingsNodePortOrder(
    std::shared_ptr<GraphInfoHolder> &currentGraph) {
    double totalCrossings = 0;
    double modelOrderInfluence = 0;
    totalCrossings +=
        currentGraph->crossingsCounter->countAllCrossings(currentGraph->currentNodeOrder) + modelOrderInfluence;
    return totalCrossings;
};

void LayerSweepCrossingMinimizer::setCurrentlyBestNodeOrders() {
    if (wasNodeOrderChanged) {
        auto ptr = std::make_shared<SweepCopy>(graphInfoHolder->currentNodeOrder);

        graphInfoHolder->setCurrentlyBestNodeAndPortOrder(ptr);
    }
};

bool LayerSweepCrossingMinimizer::sweepReducingCrossings(std::shared_ptr<GraphInfoHolder> &graph, bool forward,
                                                         bool firstSweep) {
    auto &nodes = graph->currentNodeOrder;
    int length = nodes.size();

    bool improved = graph->portDistributor->distributePortsWhileSweeping(nodes, firstIndex(forward, length), forward);

    auto firstLayer = nodes[firstIndex(forward, length)];

    for (int i = firstFree(forward, length); isNotEnd(length, i, forward); i += next(forward)) {
        improved |=
            graph->crossMinimizer->minimizeCrossings(nodes, i, forward, firstSweep && !isFirstTry && !isSecondTry);
        improved |= graph->portDistributor->distributePortsWhileSweeping(nodes, i, forward);
    }
    wasNodeOrderChanged = true;
    return improved;
}

void LayerSweepCrossingMinimizer::transferNodeAndPortOrdersToGraph() {
    auto bestSweep = graphInfoHolder->getBestSweep();
    if (bestSweep) {
        bestSweep->transferNodeAndPortOrdersToGraph(graphInfoHolder->graph, true);
    }
}

void LayerSweepCrossingMinimizer::minimizeCrossingsNoCounter(std::shared_ptr<GraphInfoHolder> &gData) {
    bool isForwardSweep = randomUtil->randomBoolean();
    bool improved = true;

    while (improved) {
        improved = false;
        improved = gData->crossMinimizer->setFirstLayerOrder(gData->currentNodeOrder, isForwardSweep);
        improved |= sweepReducingCrossings(gData, isForwardSweep, false);
        isForwardSweep = !isForwardSweep;
    }
    setCurrentlyBestNodeOrders();
}
}  // namespace GuiBridge