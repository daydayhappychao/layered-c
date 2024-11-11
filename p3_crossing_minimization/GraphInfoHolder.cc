#include "./GraphInfoHolder.h"
#include <list>
#include <memory>
#include <vector>
#include "../../Graph.h"
#include "../intermediate/greedyswitch/GreedySwitchHeuristic.h"
#include "AbstractBarycenterPortDistributor.h"
#include "BarycenterHeuristic.h"
#include "ForsterConstraintResolver.h"
#include "ISweepPortDistributor.h"
#include "counting/AllCrossingsCounter.h"
#include "counting/Initializable.h"

namespace GuiBridge {
GraphInfoHolder::GraphInfoHolder(const std::shared_ptr<Graph> &graph, CrossMinType crossMinType)
    : graph(graph), crossMinType(crossMinType) {}

void GraphInfoHolder::init() {
    this->currentNodeOrder = graph->toNodeArray();

    crossingsCounter = std::make_shared<AllCrossingsCounter>(currentNodeOrder);

    std::random_device rd;
    std::mt19937 random(rd());

    portDistributor = sweepPortDistributorCreate(crossMinType, random, currentNodeOrder);

    auto thisPtr = shared_from_this();

    std::vector<std::shared_ptr<Initializable>> initializables{thisPtr, crossingsCounter, portDistributor};

    if (crossMinType == CrossMinType::BARYCENTER) {
        std::shared_ptr<ForsterConstraintResolver> constraintResolver =
            std::make_shared<ForsterConstraintResolver>(currentNodeOrder);
        initializables.push_back(constraintResolver);

        auto portDistributorPtr = std::static_pointer_cast<AbstractBarycenterPortDistributor>(portDistributor);
        crossMinimizer =
            std::make_shared<BarycenterHeuristic>(constraintResolver, random, portDistributorPtr, currentNodeOrder);

    } else {
        auto ptr = shared_from_this();
        crossMinimizer = std::make_shared<GreedySwitchHeuristic>(crossMinType, ptr);
    }

    initializables.push_back(crossMinimizer);

    Initializable::init(initializables, currentNodeOrder);
}
}  // namespace GuiBridge