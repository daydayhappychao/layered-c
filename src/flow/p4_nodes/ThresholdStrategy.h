#ifndef THRESHOLD_STRATEGY_HPP
#define THRESHOLD_STRATEGY_HPP

#include <memory>
#include <queue>
#include <set>
#include <stack>

namespace GuiBridge {
class Node;
class Edge;
class BKAlignedLayout;
class NeighborhoodInformation;

class Postprocessable {
public:
    // SUPPRESS CHECKSTYLE NEXT 8 VisibilityModifier
    /** the node whose block can potentially be moved. */
    std::shared_ptr<Node> free;
    /** whether {@code free} is the root node of its block. */
    bool isRoot;
    /** whether {@code free} has edges. */
    bool hasEdges;
    /** the edge that was selected to be straightened. */
    std::shared_ptr<Edge> edge;

    Postprocessable(std::shared_ptr<Node> &free, bool isRoot);
};
class ThresholdStrategy {
public:
    virtual ~ThresholdStrategy() = default;
    void init(std::shared_ptr<BKAlignedLayout> &theBal, std::shared_ptr<NeighborhoodInformation> &theNi);
    void finishBlock(std::shared_ptr<Node> &n);
    virtual double calculateThreshold(double oldThresh, std::shared_ptr<Node> &blockRoot,
                                      std::shared_ptr<Node> &currentNode) = 0;
    virtual void postProcess() = 0;
    std::shared_ptr<Node> getOther(std::shared_ptr<Edge> &edge, std::shared_ptr<Node> &n);

protected:
    std::shared_ptr<BKAlignedLayout> bal;
    std::set<std::shared_ptr<Node>> blockFinished;
    std::queue<std::shared_ptr<Postprocessable>> postProcessablesQueue;
    std::stack<std::shared_ptr<Postprocessable>> postProcessablesStack;
    double THRESHOLD = std::numeric_limits<double>::max();
    double EPSILON = 0.0001;
    std::shared_ptr<NeighborhoodInformation> ni;
};

class SimpleThresholdStrategy : public ThresholdStrategy {
public:
    double calculateThreshold(double oldThresh, std::shared_ptr<Node> &blockRoot,
                              std::shared_ptr<Node> &currentNode) override;
    void postProcess() override;

private:
    std::shared_ptr<Postprocessable> pickEdge(std::shared_ptr<Postprocessable> &pp);
    double getBound(std::shared_ptr<Node> &blockNode, bool isRoot);
    bool process(std::shared_ptr<Postprocessable> &pp);
};
}  // namespace GuiBridge
#endif