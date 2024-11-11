#ifndef FORSTER_CONSTRAINT_RESOLVER_HPP
#define FORSTER_CONSTRAINT_RESOLVER_HPP
#include <list>
#include <memory>
#include <vector>
#include "BarycenterHeuristic.h"
#include "counting/Initializable.h"
namespace GuiBridge {
class ForsterConstraintResolver;
class ConstraintGroup {
public:
    explicit ConstraintGroup(std::shared_ptr<Node> &node, std::shared_ptr<ForsterConstraintResolver> &origin);
    ConstraintGroup(std::shared_ptr<ConstraintGroup> &nodeGroup1, std::shared_ptr<ConstraintGroup> &nodeGroup2,
                    std::shared_ptr<ForsterConstraintResolver> &origin);
    void setBarycenter(double barycenter);
    double getBarycenter() const;
    std::list<std::shared_ptr<ConstraintGroup>> getOutgoingConstraints();
    void resetOutgoingConstraints();
    bool hasOutgoingConstraints() const;
    std::list<std::shared_ptr<ConstraintGroup>> getIncomingConstraints();
    void resetIncomingConstraints();
    bool hasIncomingConstraints() const;
    std::vector<std::shared_ptr<Node>> getNodes() const;
    std::shared_ptr<Node> getNode() const;

    int incomingConstraintsCount;

private:
    std::shared_ptr<ForsterConstraintResolver> &origin;
    double summedWeight;
    int degree;
    std::vector<std::shared_ptr<Node>> nodes;
    std::list<std::shared_ptr<ConstraintGroup>> outgoingConstraints;
    std::list<std::shared_ptr<ConstraintGroup>> incomingConstraints;
};

class ForsterConstraintResolver : public Initializable, public std::enable_shared_from_this<ForsterConstraintResolver> {
public:
    explicit ForsterConstraintResolver(std::vector<std::vector<std::shared_ptr<Node>>> &currentNodeOrder);

    void initAtLayerLevel(int l, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override;
    void initAtNodeLevel(int l, int n, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override;
    void initAtNodeLevel(std::shared_ptr<Node> &node, bool fullInit);
    void initAtPortLevel(int l, int n, int p, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override{};
    void initAtEdgeLevel(int l, int n, int p, int e, std::shared_ptr<Edge> &edge,
                         std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) override{};
    void initAfterTraversal() override{};

    std::vector<std::vector<std::shared_ptr<BarycenterState>>> getBarycenterStates();
    void processConstraints(std::vector<std::shared_ptr<Node>> &nodes);
    void processConstraints(std::vector<std::shared_ptr<Node>> &nodes, bool onlyBetweenNormalNodes);
    void buildConstraintsGraph(std::vector<std::shared_ptr<ConstraintGroup>> &groups, bool onlyBetweenNormalNodes);
    std::pair<std::shared_ptr<ConstraintGroup>, std::shared_ptr<ConstraintGroup>> findViolatedConstraint(
        std::vector<std::shared_ptr<ConstraintGroup>> &groups);
    void handleViolatedConstraint(std::shared_ptr<ConstraintGroup> firstNodeGroup,
                                  std::shared_ptr<ConstraintGroup> secondNodeGroup,
                                  std::vector<std::shared_ptr<ConstraintGroup>> &nodeGroups);
    std::shared_ptr<BarycenterState> stateOf(const std::shared_ptr<Node> &node);

private:
    bool constraintsBetweenNonDummies;
    std::vector<std::vector<std::shared_ptr<Node>>> layoutUnits;
    std::vector<std::vector<std::shared_ptr<BarycenterState>>> barycenterStates;
    std::vector<std::vector<std::shared_ptr<ConstraintGroup>>> constraintGroups;

    std::shared_ptr<ConstraintGroup> groupOf(const std::shared_ptr<Node> &node);
};
}  // namespace GuiBridge
#endif