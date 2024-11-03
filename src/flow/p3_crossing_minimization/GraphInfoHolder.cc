#include "./GraphInfoHolder.h"
#include <list>
#include <memory>
#include <vector>
#include "../../Graph.h"
#include "../intermediate/greedyswitch/GreedySwitchHeuristic.h"
#include "BarycenterHeuristic.h"
#include "ForsterConstraintResolver.h"
#include "ISweepPortDistributor.h"
#include "counting/AllCrossingsCounter.h"
#include "counting/Initializable.h"

namespace GuiBridge {
GraphInfoHolder::GraphInfoHolder(const std::shared_ptr<Graph> &graph, CrossMinType crossMinType) {
    this->graph = graph;
    this->currentNodeOrder = graph->toNodeArray();

    crossingsCounter = std::make_shared<AllCrossingsCounter>(currentNodeOrder);

    std::random_device rd;
    std::mt19937 random(rd());

    portDistributor = sweepPortDistributorCreate(crossMinType, random, currentNodeOrder);

    std::vector<std::shared_ptr<Initializable>> initializables{shared_from_this(), crossingsCounter, portDistributor};

    if (crossMinType == CrossMinType::BARYCENTER) {
        std::shared_ptr<Initializable> constraintResolver =
            std::make_shared<ForsterConstraintResolver>(currentNodeOrder);
        initializables.push_back(constraintResolver);
        crossMinimizer =
            std::make_shared<BarycenterHeuristic>(constraintResolver, random, portDistributor, currentNodeOrder);

    } else {
        crossMinimizer = std::make_shared<GreedySwitchHeuristic>(crossMinType, shared_from_this());
    }

    initializables.push_back(crossMinimizer);

    Initializable::init(initializables, currentNodeOrder);
}
}  // namespace GuiBridge