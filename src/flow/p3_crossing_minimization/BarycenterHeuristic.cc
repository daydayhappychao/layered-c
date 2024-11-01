#include "./BarycenterHeuristic.h"
#include <algorithm>
#include <list>
#include <memory>
#include <vector>
#include "../../Layer.h"
#include "../../Node.h"
#include "../../Port.h"
#include "./ForsterConstraintResolver.h"

namespace GuiBridge {
BarycenterState::BarycenterState(std::shared_ptr<Node> &node) : node(node) {}

BarycenterHeuristic::BarycenterHeuristic(ForsterConstraintResolver *constraintResolver, std::mt19937 &random,
                                         AbstractBarycenterPortDistributor *portDistributor,
                                         const std::vector<std::vector<std::shared_ptr<Node>>> &graph)
    : constraintResolver(constraintResolver), random(random), portDistributor(portDistributor) {}

void BarycenterHeuristic::minimizeCrossings(std::list<std::shared_ptr<Node>> &layer, bool preOrdered, bool randomize,
                                            bool forward) {
    if (randomize) {
        randomizeBarycenters(layer);
    } else {
        calculateBarycenters(layer, forward);
        fillInUnknownBarycenters(layer, preOrdered);
    }

    if (layer.size() > 1) {
        std::sort(layer.begin(), layer.end(), [this](auto &n1, auto &n2) {
            BarycenterState *s1 = stateOf(n1);
            BarycenterState *s2 = stateOf(n2);

            if (s1->barycenter != -1 && s2->barycenter != -1) {
                return s1->barycenter < s2->barycenter;  // 使用小于运算符进行比较
            }
            if (s1->barycenter != -1) {
                return true;  // s1 的重心存在，排在前面
            }
            if (s2->barycenter != -1) {
                return false;  // s2 的重心存在，排在前面
            }
            return false;  // 都不存在重心，保持顺序
        });
        std::vector<std::shared_ptr<Node>> layerArr(layer.begin(), layer.end());
        this->constraintResolver->processConstraints(layerArr);
    }
}

void BarycenterHeuristic::randomizeBarycenters(std::list<std::shared_ptr<Node>> &nodes) {
    for (auto &node : nodes) {
        // 仅为包含单个节点的节点组设置重心
        stateOf(node)->barycenter = random() / static_cast<float>(RAND_MAX);  // 随机生成重心值
        stateOf(node)->summedWeight = stateOf(node)->barycenter;
        stateOf(node)->degree = 1;
    }
}

void BarycenterHeuristic::fillInUnknownBarycenters(std::list<std::shared_ptr<Node>> &nodes, bool preOrdered) {
    if (preOrdered) {
        double lastValue = -1;
        auto nodesIterator = nodes.begin();

        while (nodesIterator != nodes.end()) {
            auto node = *nodesIterator;
            double value = stateOf(node)->barycenter;

            if (value == -1) {  // 重心未定义
                // 取前一个值的中心和下一个定义值的中心
                double nextValue = lastValue + 1;

                auto nextNodeIterator = nodesIterator;
                while (++nextNodeIterator != nodes.end()) {
                    double x = stateOf(*nextNodeIterator)->barycenter;
                    if (x != -1) {
                        nextValue = x;
                        break;
                    }
                }

                value = (lastValue + nextValue) / 2;
                stateOf(node)->barycenter = value;
                stateOf(node)->summedWeight = value;
                stateOf(node)->degree = 1;
            }

            lastValue = value;
            ++nodesIterator;
        }
    } else {
        // 没有之前的排序 - 为新节点确定随机位置
        double maxBary = 0;
        for (auto &node : nodes) {
            if (stateOf(node)->barycenter != -1) {
                maxBary = std::max(maxBary, stateOf(node)->barycenter);
            }
        }

        maxBary += 2;
        for (auto &node : nodes) {
            if (stateOf(node)->barycenter == -1) {
                double value = random() / static_cast<float>(RAND_MAX) * maxBary - 1;  // 生成随机值
                stateOf(node)->barycenter = value;
                stateOf(node)->summedWeight = value;
                stateOf(node)->degree = 1;
            }
        }
    }
}

// 计算给定节点组的重心
void BarycenterHeuristic::calculateBarycenters(std::list<std::shared_ptr<Node>> &nodes, bool forward) {
    // 将所有访问标志设置为 false
    for (auto &node : nodes) {
        stateOf(node)->visited = false;
    }

    for (auto &node : nodes) {
        // 计算节点组的新重心（可能为 null）
        calculateBarycenter(node, forward);
    }
}

// 计算给定单节点节点组的重心
void BarycenterHeuristic::calculateBarycenter(std::shared_ptr<Node> &node, bool forward) {
    // 检查节点组的重心是否已经计算
    if (stateOf(node)->visited) {
        return;
    }
    stateOf(node)->visited = true;

    stateOf(node)->degree = 0;
    stateOf(node)->summedWeight = 0.0F;
    stateOf(node)->barycenter = -1;

    auto &allPorts = forward ? node->getOutputPorts() : node->getInputPorts();
    for (const auto &freePort : allPorts) {
        auto portIterable = freePort->getConnectedPorts();
        for (auto &fixedPort : portIterable) {
            // 如果固定端口所属的节点是自由层的一部分，使用该节点的重心计算
            auto fixedNode = fixedPort->getNode();

            if (fixedNode->getLayer() == node->getLayer()) {
                // 忽略自环
                if (fixedNode != node) {
                    // 找到固定节点的节点组并计算其重心
                    calculateBarycenter(fixedNode, forward);

                    // 更新该节点组的值
                    stateOf(node)->degree += stateOf(fixedNode)->degree;
                    stateOf(node)->summedWeight += stateOf(fixedNode)->summedWeight;
                }
            } else {
                stateOf(node)->summedWeight += portRanks[fixedPort->id];
                stateOf(node)->degree++;
            }
        }
    }
    if (stateOf(node)->degree > 0) {
        stateOf(node)->summedWeight += random() / static_cast<float>(RAND_MAX) * 0.07F - 0.07F / 2;
        stateOf(node)->barycenter = stateOf(node)->summedWeight / stateOf(node)->degree;
    }
}

std::shared_ptr<BarycenterState> BarycenterHeuristic::stateOf(std::shared_ptr<Node> &node) {
    return barycenterState[node->getLayer()->id][node->getId()];
}

bool BarycenterHeuristic::minimizeCrossings(std::vector<std::vector<std::shared_ptr<Node>>> &order, int freeLayerIndex,
                                            bool forwardSweep, bool isFirstSweep) {
    if (!isFirstLayer(order, freeLayerIndex, forwardSweep)) {
        auto &fixedLayer = order[freeLayerIndex - changeIndex(forwardSweep)];
        portDistributor->calculatePortRanks(fixedLayer, portTypeFor(forwardSweep));
    }

    auto &firstNodeInLayer = order[freeLayerIndex][0];
    bool preOrdered = !isFirstSweep;

    std::list<std::shared_ptr<Node>> nodes(order[freeLayerIndex].begin(), order[freeLayerIndex].end());
    minimizeCrossings(nodes, preOrdered, false, forwardSweep);

    int index = 0;
    for (auto &nodeGroup : nodes) {
        order[freeLayerIndex][index++] = nodeGroup;
    }

    return false;  // 不一定能改进
}

bool BarycenterHeuristic::isFirstLayer(std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder, int currentIndex,
                                       bool forwardSweep) {
    return currentIndex == startIndex(forwardSweep, nodeOrder.size());
}

int BarycenterHeuristic::startIndex(bool dir, int length) { return dir ? 0 : std::max(0, length - 1); }

bool BarycenterHeuristic::setFirstLayerOrder(std::vector<std::vector<std::shared_ptr<Node>>> &order,
                                             bool isForwardSweep) {
    auto theStartIndex = startIndex(isForwardSweep, order.size());
    std::list<std::shared_ptr<Node>> nodes(order[theStartIndex].begin(), order[theStartIndex].end());
    minimizeCrossings(nodes, false, true, isForwardSweep);
    int index = 0;

    for (auto &nodeGroup : nodes) {
        order[theStartIndex][index++] = nodeGroup;
    }
    return false;
}

void BarycenterHeuristic::initAfterTraversal() { barycenterState = constraintResolver->getBarycenterStates(); }
void BarycenterHeuristic::initAtLayerLevel(int l, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) {
    nodeOrder[l][0]->getLayer()->id = l;
}
}  // namespace GuiBridge