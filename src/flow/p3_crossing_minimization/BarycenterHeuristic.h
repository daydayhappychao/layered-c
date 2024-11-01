#ifndef BARY_CENTER_HEURISTIC_HPP
#define BARY_CENTER_HEURISTIC_HPP

#include <list>
#include <memory>
#include <random>
#include <vector>
#include "AbstractBarycenterPortDistributor.h"
#include "ICrossingMinimizationHeuristic.h"
namespace GuiBridge {
class Node;
class ForsterConstraintResolver;

class BarycenterState {
public:
    explicit BarycenterState(std::shared_ptr<Node> &node);
    std::shared_ptr<Node> &node;
    double summedWeight;
    int degree;
    double barycenter = -1.0;
    bool visited;
};

class BarycenterHeuristic : public ICrossingMinimizationHeuristic {
public:
    BarycenterHeuristic(ForsterConstraintResolver *constraintResolver, std::mt19937 &random,
                        AbstractBarycenterPortDistributor *portDistributor,
                        const std::vector<std::vector<std::shared_ptr<Node>>> &graph);

    void minimizeCrossings(std::list<std::shared_ptr<Node>> &layer, bool preOrdered, bool randomize, bool forward);
    bool minimizeCrossings(std::vector<std::vector<std::shared_ptr<Node>>> &order, int freeLayerIndex,
                           bool forwardSweep, bool isFirstSweep) override;
    bool setFirstLayerOrder(std::vector<std::vector<std::shared_ptr<Node>>> &order, bool isForwardSweep) override;
    void initAfterTraversal() override;
    void initAtLayerLevel(int l, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder);

private:
    void randomizeBarycenters(std::list<std::shared_ptr<Node>> &nodes);
    void fillInUnknownBarycenters(std::list<std::shared_ptr<Node>> &nodes, bool preOrdered);
    void calculateBarycenters(std::list<std::shared_ptr<Node>> &nodes, bool forward);
    void calculateBarycenter(std::shared_ptr<Node> &node, bool forward);
    std::shared_ptr<BarycenterState> stateOf(std::shared_ptr<Node> &node);
    bool isFirstLayer(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex, bool forwardSweep);
    int startIndex(bool dir, int length);
    int changeIndex(bool dir) { return dir ? 1 : -1; };
    PortType portTypeFor(bool direction) { return direction ? PortType::OUTPUT : PortType::INPUT; };

    std::vector<std::vector<std::shared_ptr<BarycenterState>>> barycenterState;  // 每个节点的重心状态
    std::vector<float> portRanks;                                                // 端口等级数组
    std::mt19937 &random;                                                        // 随机数生成器
    ForsterConstraintResolver *constraintResolver;                               // 约束解析器
    AbstractBarycenterPortDistributor *portDistributor;                          // 端口分配器
};

}  // namespace GuiBridge

#endif