#include "./OrthogonalRoutingGenerator.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <valarray>
#include <vector>
#include "../../Edge.h"
#include "../../Node.h"
#include "HyperEdgeCycleDetector.h"
#include "HyperEdgeSegment.h"
#include "HyperEdgeSegmentDependency.h"
#include "HyperEdgeSegmentSplitter.h"
#include "WestToEastRoutingStrategy.h"

namespace GuiBridge {
OrthogonalRoutingGenerator::OrthogonalRoutingGenerator(double edgeSpacing) : edgeSpacing(edgeSpacing) {
    conflictThreshold = CONFLICT_THRESHOLD_FACTOR * edgeSpacing;
    routingStrategy = std::make_shared<WestToEastRoutingStrategy>();
}

int OrthogonalRoutingGenerator::routeEdges(std::shared_ptr<Graph> &layeredGraph,
                                           std::vector<std::shared_ptr<Node>> &sourceLayerNodes, int sourceLayerIndex,
                                           std::vector<std::shared_ptr<Node>> &targetLayerNodes, double startPos) {
    PortToEdgeSegmentMap portToEdgeSegmentMap;
    std::vector<std::shared_ptr<HyperEdgeSegment>> edgeSegments;

    createHyperEdgeSegments(sourceLayerNodes, routingStrategy->getSourcePortSide(), edgeSegments, portToEdgeSegmentMap);
    createHyperEdgeSegments(targetLayerNodes, routingStrategy->getTargetPortSide(), edgeSegments, portToEdgeSegmentMap);

    criticalConflictThreshold = CRITICAL_CONFLICT_THRESHOLD_FACTOR * minimumHorizontalSegmentDistance(edgeSegments);

    int criticalDependencyCount = 0;

    if (!edgeSegments.empty()) {
        for (int firstIdx = 0; firstIdx < edgeSegments.size() - 1; firstIdx++) {
            auto firstSegment = edgeSegments.at(firstIdx);
            for (int secondIdx = firstIdx + 1; secondIdx < edgeSegments.size(); secondIdx++) {
                criticalDependencyCount += createDependencyIfNecessary(firstSegment, edgeSegments.at(secondIdx));
            }
        }
    }

    if (criticalDependencyCount >= 2) {
        breakCriticalCycles(edgeSegments);
    }

    breakNonCriticalCycles(edgeSegments);

    topologicalNumbering(edgeSegments);

    int rankCount = -1;
    for (auto &node : edgeSegments) {
        if (std::abs(node->getStartCoordinate() - node->getEndCoordinate()) < TOLERANCE) {
            continue;
        }
        rankCount = std::max(rankCount, node->getRoutingSlot());
        routingStrategy->calculateBendPoints(node, startPos, edgeSpacing);
    }

    routingStrategy->clearCreatedJunctionPoints();
    return rankCount + 1;
}

double OrthogonalRoutingGenerator::minimumHorizontalSegmentDistance(
    const std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments) {
    // 获取所有的 Incoming 和 Outgoing 坐标并求最小差值
    std::vector<double> incomingCoordinates;
    std::vector<double> outgoingCoordinates;

    for (const auto &segment : edgeSegments) {
        auto incoming = segment->getIncomingConnectionCoordinates();
        incomingCoordinates.insert(incomingCoordinates.end(), incoming.begin(), incoming.end());

        auto outgoing = segment->getOutgoingConnectionCoordinates();
        outgoingCoordinates.insert(outgoingCoordinates.end(), outgoing.begin(), outgoing.end());
    }
    double minIncomingDistance = minimumDifference(incomingCoordinates);
    double minOutgoingDistance = minimumDifference(outgoingCoordinates);
    return std::min(minIncomingDistance, minOutgoingDistance);
}
double OrthogonalRoutingGenerator::minimumDifference(const std::vector<double> &numberStream) {
    // 排序并去重
    std::vector<double> uniqueNumbers = numberStream;
    std::sort(uniqueNumbers.begin(), uniqueNumbers.end());
    uniqueNumbers.erase(std::unique(uniqueNumbers.begin(), uniqueNumbers.end()), uniqueNumbers.end());

    double minDifference = std::numeric_limits<double>::infinity();

    if (uniqueNumbers.size() >= 2) {
        double previousNumber = uniqueNumbers[0];

        // 遍历并计算相邻数字的最小差值
        for (size_t i = 1; i < uniqueNumbers.size(); ++i) {
            double currentNumber = uniqueNumbers[i];
            minDifference = std::min(minDifference, currentNumber - previousNumber);
            previousNumber = currentNumber;
        }
    }

    return minDifference;
}

void OrthogonalRoutingGenerator::createHyperEdgeSegments(std::vector<std::shared_ptr<Node>> &nodes, PortType portSide,
                                                         std::vector<std::shared_ptr<HyperEdgeSegment>> &hyperEdges,
                                                         PortToEdgeSegmentMap &portToHyperEdgeSegmentMap) {
    if (!nodes.empty()) {
        for (auto &node : nodes) {
            for (auto &port : node->getPortsByPortType(portSide)) {
                if (portToHyperEdgeSegmentMap.count(EdgeTarget(node, port)) == 0) {
                    auto hyperEdge = std::make_shared<HyperEdgeSegment>(routingStrategy);
                    hyperEdges.emplace_back(hyperEdge);
                    hyperEdge->addPortPositions(node, port, portToHyperEdgeSegmentMap);
                }
            }
        }
    }
}

int OrthogonalRoutingGenerator::createDependencyIfNecessary(std::shared_ptr<HyperEdgeSegment> &he1,
                                                            std::shared_ptr<HyperEdgeSegment> &he2) {
    if (std::abs(he1->getStartCoordinate() - he1->getEndCoordinate()) < TOLERANCE ||
        (std::abs(he2->getStartCoordinate() - he2->getEndCoordinate()) < TOLERANCE)) {
        return 0;
    }
    int conflicts1 = countConflicts(he1->getOutgoingConnectionCoordinates(), he2->getIncomingConnectionCoordinates());
    int conflicts2 = countConflicts(he2->getOutgoingConnectionCoordinates(), he1->getIncomingConnectionCoordinates());

    bool criticalConflictsDetected =
        conflicts1 == CRITICAL_CONFLICTS_DETECTED || conflicts2 == CRITICAL_CONFLICTS_DETECTED;
    int criticalDependencyCount = 0;

    if (criticalConflictsDetected) {
        if (conflicts1 == CRITICAL_CONFLICTS_DETECTED) {
            HyperEdgeSegmentDependency::createAndAddCritical(he2, he1);
            criticalDependencyCount++;
        }

        if (conflicts2 == CRITICAL_CONFLICTS_DETECTED) {
            HyperEdgeSegmentDependency::createAndAddCritical(he1, he2);
            criticalDependencyCount++;
        }
    } else {
        int crossings1 =
            countCrossings(he1->getOutgoingConnectionCoordinates(), he2->getStartCoordinate(), he2->getEndCoordinate());
        crossings1 +=
            countCrossings(he2->getIncomingConnectionCoordinates(), he1->getStartCoordinate(), he1->getEndCoordinate());
        int crossings2 =
            countCrossings(he2->getOutgoingConnectionCoordinates(), he1->getStartCoordinate(), he1->getEndCoordinate());
        crossings2 +=
            countCrossings(he1->getIncomingConnectionCoordinates(), he2->getStartCoordinate(), he2->getEndCoordinate());

        int depValue1 = CONFLICT_PENALTY * conflicts1 + CROSSING_PENALTY * crossings1;
        int depValue2 = CONFLICT_PENALTY * conflicts2 + CROSSING_PENALTY * crossings2;

        if (depValue1 < depValue2) {
            // hyperedge 1 wants to be left of hyperedge 2
            HyperEdgeSegmentDependency::createAndAddRegular(he1, he2, depValue2 - depValue1);
        } else if (depValue1 > depValue2) {
            // hyperedge 2 wants to be left of hyperedge 1
            HyperEdgeSegmentDependency::createAndAddRegular(he2, he1, depValue1 - depValue2);
        } else if (depValue1 > 0 && depValue2 > 0) {
            // create two dependencies with zero weight
            HyperEdgeSegmentDependency::createAndAddRegular(he1, he2, 0);
            HyperEdgeSegmentDependency::createAndAddRegular(he2, he1, 0);
        }
    }
    return criticalDependencyCount;
}

int OrthogonalRoutingGenerator::countConflicts(const std::vector<double> &posis1, const std::vector<double> &posis2) {
    int conflicts = 0;

    // 检查 posis1 和 posis2 是否为空
    if (!posis1.empty() && !posis2.empty()) {
        auto iter1 = posis1.begin();
        auto iter2 = posis2.begin();
        double pos1 = *iter1;
        double pos2 = *iter2;
        bool hasMore = true;

        // 遍历 posis1 和 posis2 中的元素
        do {
            if (pos1 > pos2 - criticalConflictThreshold && pos1 < pos2 + criticalConflictThreshold) {
                // 一旦发现一个关键冲突，立即返回 -1
                return -1;
            }
            if (pos1 > pos2 - conflictThreshold && pos1 < pos2 + conflictThreshold) {
                conflicts++;
            }

            if (pos1 <= pos2 && iter1 != posis1.end()) {
                pos1 = *++iter1;
            } else if (pos2 <= pos1 && iter2 != posis2.end()) {
                pos2 = *++iter2;
            } else {
                hasMore = false;
            }
        } while (hasMore);
    }

    return conflicts;
}

int OrthogonalRoutingGenerator::countCrossings(const std::vector<double> &posis, double start, double end) {
    int crossings = 0;
    for (double pos : posis) {
        if (pos > end) {
            break;
        }
        if (pos >= start) {
            crossings++;
        }
    }
    return crossings;
}

void OrthogonalRoutingGenerator::breakCriticalCycles(std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments) {
    auto cycleDependencies = HyperEdgeCycleDetector::detectCycles(edgeSegments, true);

    if (segmentSplitter == nullptr) {
        auto ptr = shared_from_this();
        segmentSplitter = std::make_shared<HyperEdgeSegmentSplitter>(ptr);
    }

    segmentSplitter->splitSegments(cycleDependencies, edgeSegments, criticalConflictThreshold);
}
void OrthogonalRoutingGenerator::breakNonCriticalCycles(std::vector<std::shared_ptr<HyperEdgeSegment>> &edgeSegments) {
    auto cycleDependencies = HyperEdgeCycleDetector::detectCycles(edgeSegments, false);

    for (auto &cycleDependency : cycleDependencies) {
        if (cycleDependency->getWeight() == 0) {
            // Simply remove this dependency. This assumes that only two-cycles will have dependency weight 0
            cycleDependency->remove();
        } else {
            cycleDependency->reverse();
        }
    }
};

void OrthogonalRoutingGenerator::topologicalNumbering(std::vector<std::shared_ptr<HyperEdgeSegment>> &segments) {
    std::list<std::shared_ptr<HyperEdgeSegment>> sources;
    std::list<std::shared_ptr<HyperEdgeSegment>> rightwardTargets;

    for (auto &node : segments) {
        node->setInWeight(node->getIncomingSegmentDependencies().size());
        node->setOutWeight(node->getOutgoingSegmentDependencies().size());

        if (node->getInWeight() == 0) {
            sources.emplace_back(node);
        }

        if (node->getOutWeight() == 0 && node->getIncomingConnectionCoordinates().empty()) {
            rightwardTargets.emplace_back(node);
        }
    }

    int maxRank = -1;

    while (!sources.empty()) {
        auto node = sources.front();
        sources.pop_front();
        for (auto &dep : node->getOutgoingSegmentDependencies()) {
            const auto &target = dep->getTarget();
            target->setRoutingSlot(std::max(target->getRoutingSlot(), node->getRoutingSlot() + 1));
            maxRank = std::max(maxRank, target->getRoutingSlot());

            target->setInWeight(target->getInWeight() - 1);
            if (target->getInWeight() == 0) {
                sources.emplace_back(target);
            }
        }
    }

    if (maxRank > -1) {
        for (auto &node : rightwardTargets) {
            node->setRoutingSlot(maxRank);
        }

        while (!rightwardTargets.empty()) {
            auto node = rightwardTargets.front();
            rightwardTargets.pop_front();
            for (auto &dep : node->getIncomingSegmentDependencies()) {
                const auto &source = dep->getSource();
                if (!source->getIncomingConnectionCoordinates().empty()) {
                    continue;
                }
                source->setRoutingSlot(std::min(source->getRoutingSlot(), node->getRoutingSlot() - 1));

                source->setOutWeight(source->getOutWeight() - 1);
                if (source->getOutWeight() == 0) {
                    rightwardTargets.emplace_back(source);
                }
            }
        }
    }
}

}  // namespace GuiBridge