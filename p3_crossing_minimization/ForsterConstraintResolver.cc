#include "./ForsterConstraintResolver.h"
#include "../../Layer.h"
#include "../../Node.h"

namespace GuiBridge {
ConstraintGroup::ConstraintGroup(std::shared_ptr<Node> &node, std::shared_ptr<ForsterConstraintResolver> &origin)
    : origin(origin) {
    nodes.push_back(node);
}

ConstraintGroup::ConstraintGroup(std::shared_ptr<ConstraintGroup> &nodeGroup1,
                                 std::shared_ptr<ConstraintGroup> &nodeGroup2,
                                 std::shared_ptr<ForsterConstraintResolver> &origin)
    : origin(origin) {
    int length1 = nodeGroup1->nodes.size();
    int length2 = nodeGroup2->nodes.size();
    nodes.reserve(length1 + length2);
    for (const auto &n : nodeGroup1->nodes) {
        nodes.push_back(n);
    }
    for (const auto &n : nodeGroup2->nodes) {
        nodes.push_back(n);
    }

    if (!nodeGroup1->outgoingConstraints.empty()) {
        outgoingConstraints = nodeGroup1->outgoingConstraints;
        outgoingConstraints.remove(nodeGroup2);
        if (!nodeGroup2->outgoingConstraints.empty()) {
            for (const auto &candidate : nodeGroup2->outgoingConstraints) {
                if (candidate == nodeGroup1) {
                    continue;
                }
                if (std::find(outgoingConstraints.begin(), outgoingConstraints.end(), candidate) !=
                    outgoingConstraints.end()) {
                    candidate->incomingConstraintsCount--;
                } else {
                    outgoingConstraints.push_back(candidate);
                }
            }
        }
    } else if (!nodeGroup2->outgoingConstraints.empty()) {
        outgoingConstraints = nodeGroup2->outgoingConstraints;
        outgoingConstraints.remove(nodeGroup1);
    }

    summedWeight = nodeGroup1->summedWeight + nodeGroup2->summedWeight;
    degree = nodeGroup1->degree + nodeGroup2->degree;

    if (degree > 0) {
        setBarycenter(summedWeight / degree);
    } else if (nodeGroup1->getBarycenter() >= 0 && nodeGroup2->getBarycenter() >= 0) {
        setBarycenter((nodeGroup1->getBarycenter() + nodeGroup2->getBarycenter()) / 2);
    } else if (nodeGroup1->getBarycenter() >= 0) {
        setBarycenter(nodeGroup1->getBarycenter());
    } else if (nodeGroup2->getBarycenter() >= 0) {
        setBarycenter(nodeGroup2->getBarycenter());
    }
}

void ConstraintGroup::setBarycenter(double barycenter) {
    for (const auto &node : nodes) {
        this->origin->stateOf(node)->barycenter = barycenter;
    }
}

double ConstraintGroup::getBarycenter() const { return this->origin->stateOf(nodes[0])->barycenter; }

std::list<std::shared_ptr<ConstraintGroup>> ConstraintGroup::getOutgoingConstraints() { return outgoingConstraints; }

void ConstraintGroup::resetOutgoingConstraints() { outgoingConstraints.clear(); }

bool ConstraintGroup::hasOutgoingConstraints() const { return !outgoingConstraints.empty(); }

std::list<std::shared_ptr<ConstraintGroup>> ConstraintGroup::getIncomingConstraints() { return incomingConstraints; }

void ConstraintGroup::resetIncomingConstraints() { incomingConstraints.clear(); }

bool ConstraintGroup::hasIncomingConstraints() const { return !incomingConstraints.empty(); }

std::vector<std::shared_ptr<Node>> ConstraintGroup::getNodes() const { return nodes; }

std::shared_ptr<Node> ConstraintGroup::getNode() const {
    if (nodes.size() != 1) {
        throw std::runtime_error("ConstraintGroup should have exactly one node.");
    }
    return nodes[0];
}

ForsterConstraintResolver::ForsterConstraintResolver(
    std::vector<std::vector<std::shared_ptr<Node>>> &currentNodeOrder) {
    barycenterStates.resize(currentNodeOrder.size());
    constraintGroups.resize(currentNodeOrder.size());
}
void ForsterConstraintResolver::initAtLayerLevel(int l, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    barycenterStates[l].resize(nodeOrder[l].size());
    constraintGroups[l].resize(nodeOrder[l].size());
}
std::shared_ptr<BarycenterState> ForsterConstraintResolver::stateOf(const std::shared_ptr<Node> &node) {
    return barycenterStates[node->getLayer()->getIndex()][node->getId()];
}

void ForsterConstraintResolver::initAtNodeLevel(int l, int n,
                                                std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    initAtNodeLevel(nodeOrder[l][n], true);
}

void ForsterConstraintResolver::initAtNodeLevel(std::shared_ptr<Node> &node, bool fullInit) {
    int layerIndex = node->getLayer()->id;
    int nodeIndex = node->getId();

    auto ptr = shared_from_this();

    constraintGroups[layerIndex][nodeIndex] = std::make_shared<ConstraintGroup>(node, ptr);

    if (fullInit) {
        barycenterStates[layerIndex][nodeIndex] = std::make_shared<BarycenterState>(node);
    }
}

std::vector<std::vector<std::shared_ptr<BarycenterState>>> ForsterConstraintResolver::getBarycenterStates() {
    return barycenterStates;
}

void ForsterConstraintResolver::processConstraints(std::vector<std::shared_ptr<Node>> &nodes) {
    if (constraintsBetweenNonDummies) {
        processConstraints(nodes, true);
        for (auto &node : nodes) {
            initAtNodeLevel(node, false);
        }
    }

    processConstraints(nodes, false);
}

void ForsterConstraintResolver::processConstraints(std::vector<std::shared_ptr<Node>> &nodes,
                                                   bool onlyBetweenNormalNodes) {
    std::vector<std::shared_ptr<ConstraintGroup>> groups(nodes.size());
    for (const auto &node : nodes) {
        groups.push_back(constraintGroups[node->getLayer()->id][node->getId()]);
    }

    buildConstraintsGraph(groups, onlyBetweenNormalNodes);

    std::pair<std::shared_ptr<ConstraintGroup>, std::shared_ptr<ConstraintGroup>> violatedConstraint;
    while ((violatedConstraint = findViolatedConstraint(groups)) !=
           std::pair<std::shared_ptr<ConstraintGroup>, std::shared_ptr<ConstraintGroup>>(nullptr, nullptr)) {
        handleViolatedConstraint(violatedConstraint.first, violatedConstraint.second, groups);
    }

    nodes.clear();
    for (const auto &group : groups) {
        for (const auto &node : group->getNodes()) {
            nodes.push_back(node);
            this->stateOf(node)->barycenter = group->getBarycenter();
        }
    }
}

void ForsterConstraintResolver::buildConstraintsGraph(std::vector<std::shared_ptr<ConstraintGroup>> &groups,
                                                      bool onlyBetweenNormalNodes) {
    for (const auto &group : groups) {
        group->resetOutgoingConstraints();
        group->incomingConstraintsCount = 0;
    }

    std::shared_ptr<Node> lastNonDummyNode;
    for (const auto &group : groups) {
        std::shared_ptr<Node> node = group->getNode();

        if (!onlyBetweenNormalNodes) {
            if (lastNonDummyNode) {
                for (const auto &lastUnitNode : layoutUnits[lastNonDummyNode->getLayer()->id]) {
                    for (const auto &currentUnitNode : layoutUnits[node->getLayer()->id]) {
                        groupOf(lastUnitNode)->getOutgoingConstraints().push_back(groupOf(currentUnitNode));
                        groupOf(currentUnitNode)->incomingConstraintsCount++;
                    }
                }
            }

            lastNonDummyNode = node;
        }
    }
}

std::pair<std::shared_ptr<ConstraintGroup>, std::shared_ptr<ConstraintGroup>>
ForsterConstraintResolver::findViolatedConstraint(std::vector<std::shared_ptr<ConstraintGroup>> &groups) {
    std::vector<std::shared_ptr<ConstraintGroup>> activeGroups;

    double lastValue = std::numeric_limits<double>::lowest();
    for (const auto &group : groups) {
        lastValue = group->getBarycenter();
        group->resetIncomingConstraints();

        if (group->hasOutgoingConstraints() && group->incomingConstraintsCount == 0) {
            if (activeGroups.empty()) {
                activeGroups = std::vector<std::shared_ptr<ConstraintGroup>>();
            }
            activeGroups.push_back(group);
        }
    }

    if (!activeGroups.empty()) {
        while (!activeGroups.empty()) {
            std::shared_ptr<ConstraintGroup> group = activeGroups.front();
            activeGroups.erase(activeGroups.begin());

            if (group->hasIncomingConstraints()) {
                for (const auto &predecessor : group->getIncomingConstraints()) {
                    if (predecessor->getBarycenter() == group->getBarycenter()) {
                        if (std::find(groups.begin(), groups.end(), predecessor) >
                            std::find(groups.begin(), groups.end(), group)) {
                            return std::make_pair(predecessor, group);
                        }
                    } else if (predecessor->getBarycenter() > group->getBarycenter()) {
                        return std::make_pair(predecessor, group);
                    }
                }
            }

            for (auto &successor : group->getOutgoingConstraints()) {
                successor->getIncomingConstraints().push_back(group);

                if (successor->incomingConstraintsCount == successor->getIncomingConstraints().size()) {
                    activeGroups.push_back(successor);
                }
            }
        }
    }

    return std::make_pair(nullptr, nullptr);
}

void ForsterConstraintResolver::handleViolatedConstraint(std::shared_ptr<ConstraintGroup> firstNodeGroup,
                                                         std::shared_ptr<ConstraintGroup> secondNodeGroup,
                                                         std::vector<std::shared_ptr<ConstraintGroup>> &nodeGroups) {
    auto ptr = shared_from_this();
    std::shared_ptr<ConstraintGroup> newNodeGroup =
        std::make_shared<ConstraintGroup>(firstNodeGroup, secondNodeGroup, ptr);

    auto nodeGroupIterator = nodeGroups.begin();
    bool alreadyInserted = false;
    while (nodeGroupIterator != nodeGroups.end()) {
        std::shared_ptr<ConstraintGroup> nodeGroup = *nodeGroupIterator;

        if (nodeGroup == firstNodeGroup || nodeGroup == secondNodeGroup) {
            nodeGroupIterator = nodeGroups.erase(nodeGroupIterator);
            if (nodeGroupIterator == nodeGroups.end()) {
                break;
            }
        } else if (!alreadyInserted && nodeGroup->getBarycenter() > newNodeGroup->getBarycenter()) {
            nodeGroupIterator = nodeGroups.insert(nodeGroupIterator, newNodeGroup);
            alreadyInserted = true;
        } else if (nodeGroup->hasOutgoingConstraints()) {
            bool firstNodeGroupConstraint =
                std::find(nodeGroup->getOutgoingConstraints().begin(), nodeGroup->getOutgoingConstraints().end(),
                          firstNodeGroup) != nodeGroup->getOutgoingConstraints().end();
            bool secondNodeGroupConstraint =
                std::find(nodeGroup->getOutgoingConstraints().begin(), nodeGroup->getOutgoingConstraints().end(),
                          secondNodeGroup) != nodeGroup->getOutgoingConstraints().end();

            if (firstNodeGroupConstraint || secondNodeGroupConstraint) {
                nodeGroup->getOutgoingConstraints().push_back(newNodeGroup);
                newNodeGroup->incomingConstraintsCount++;
            }
        } else {
            ++nodeGroupIterator;
        }
    }

    if (!alreadyInserted) {
        nodeGroups.push_back(newNodeGroup);
    }
}

std::shared_ptr<ConstraintGroup> ForsterConstraintResolver::groupOf(const std::shared_ptr<Node> &node) {
    return constraintGroups[node->getLayer()->id][node->getId()];
}

}  // namespace GuiBridge