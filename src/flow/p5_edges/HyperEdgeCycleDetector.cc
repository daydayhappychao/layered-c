#include "HyperEdgeCycleDetector.h"
#include <limits>
#include <list>
#include <memory>
#include <set>
#include <vector>
#include "../../utils/randomUtil.h"
#include "HyperEdgeSegment.h"
#include "HyperEdgeSegmentDependency.h"

namespace GuiBridge {
std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> HyperEdgeCycleDetector::detectCycles(
    std::vector<std::shared_ptr<HyperEdgeSegment>> &segments, bool criticalOnly) {
    std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> result;

    std::vector<std::shared_ptr<HyperEdgeSegment>> sources;
    std::vector<std::shared_ptr<HyperEdgeSegment>> sinks;
    initialize(segments, sources, sinks, criticalOnly);
    computeLinearOrderingMarks(segments, sources, sinks, criticalOnly);

    for (auto &source : segments) {
        for (auto &outDependency : source->getOutgoingSegmentDependencies()) {
            if (!criticalOnly || outDependency->getType() == DependencyType::CRITICAL) {
                if (source->mark > outDependency->getTarget()->mark) {
                    result.emplace_back(outDependency);
                }
            }
        }
    }
    return result;
};

// 计算与 CRITICAL 依赖相关的权重
int sumCriticalWeights(const std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &dependencies,
                       DependencyType type) {
    int sum = 0;
    for (const auto &dep : dependencies) {
        if (dep->getType() == type) {
            sum += dep->getWeight();
        }
    }
    return sum;
}
int sumCriticalWeights(const std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &dependencies) {
    int sum = 0;
    for (const auto &dep : dependencies) {
        sum += dep->getWeight();
    }
    return sum;
}

void HyperEdgeCycleDetector::initialize(std::vector<std::shared_ptr<HyperEdgeSegment>> &segments,
                                        std::vector<std::shared_ptr<HyperEdgeSegment>> &sources,
                                        std::vector<std::shared_ptr<HyperEdgeSegment>> &sinks, bool criticalOnly) {
    int nextMark = -1;
    for (auto &segment : segments) {
        segment->mark = nextMark--;

        int criticalInWeight = sumCriticalWeights(segment->getIncomingSegmentDependencies(), DependencyType::CRITICAL);
        int criticalOutWeight = sumCriticalWeights(segment->getOutgoingSegmentDependencies(), DependencyType::CRITICAL);

        int inWeight = criticalInWeight;
        int outWeight = criticalOutWeight;

        if (!criticalOnly) {
            inWeight = sumCriticalWeights(segment->getIncomingSegmentDependencies());
            outWeight = sumCriticalWeights(segment->getOutgoingSegmentDependencies());
        }

        segment->setInWeight(inWeight);
        segment->setCriticalInWeight(criticalInWeight);
        segment->setOutWeight(outWeight);
        segment->setCriticalOutWeight(criticalOutWeight);

        if (outWeight == 0) {
            sinks.emplace_back(segment);
        } else if (inWeight == 0) {
            sources.emplace_back(segment);
        }
    }
}

void HyperEdgeCycleDetector::computeLinearOrderingMarks(std::vector<std::shared_ptr<HyperEdgeSegment>> &segments,
                                                        std::vector<std::shared_ptr<HyperEdgeSegment>> &sources,
                                                        std::vector<std::shared_ptr<HyperEdgeSegment>> &sinks,
                                                        bool criticalOnly) {
    std::set<std::shared_ptr<HyperEdgeSegment>> unprocessed(segments.begin(), segments.end());
    std::vector<std::shared_ptr<HyperEdgeSegment>> maxSegments;

    int markBase = segments.size();
    int nextSinkMark = markBase - 1;
    int nextSourceMark = markBase + 1;

    while (!unprocessed.empty()) {
        while (!sinks.empty()) {
            auto sink = sinks.front();
            sinks.erase(sinks.begin());
            unprocessed.erase(sink);
            sink->mark = nextSinkMark--;
            updateNeighbors(sink, sources, sinks, criticalOnly);
        }

        while (!sources.empty()) {
            auto source = sources.front();
            sources.erase(sources.begin());
            unprocessed.erase(source);
            source->mark = nextSourceMark++;
            updateNeighbors(source, sources, sinks, criticalOnly);
        }

        int maxOutflow = std::numeric_limits<int>::min();
        for (const auto &segment : unprocessed) {
            if (!criticalOnly && segment->getCriticalOutWeight() > 0 && segment->getCriticalInWeight() <= 0) {
                maxSegments.clear();
                maxSegments.emplace_back(segment);
                break;
            }
            int outflow = segment->getOutWeight() - segment->getInWeight();
            if (outflow >= maxOutflow) {
                if (outflow > maxOutflow) {
                    maxSegments.clear();
                    maxOutflow = outflow;
                }
                maxSegments.emplace_back(segment);
            }
        }

        if (!maxSegments.empty()) {
            auto maxNode = maxSegments[randomUtil->rangeInt(0, maxSegments.size())];
            unprocessed.erase(maxNode);
            maxNode->mark = nextSourceMark++;
            updateNeighbors(maxNode, sources, sinks, criticalOnly);
            maxSegments.clear();
        }
    }

    int shiftBase = segments.size() + 1;
    for (auto &node : segments) {
        if (node->mark < markBase) {
            node->mark = node->mark + shiftBase;
        }
    }
}

void HyperEdgeCycleDetector::updateNeighbors(std::shared_ptr<HyperEdgeSegment> &node,
                                             std::vector<std::shared_ptr<HyperEdgeSegment>> &sources,
                                             std::vector<std::shared_ptr<HyperEdgeSegment>> &sinks, bool criticalOnly) {
    for (auto &dep : node->getOutgoingSegmentDependencies()) {
        if (!criticalOnly || dep->getType() == DependencyType::CRITICAL) {
            auto target = dep->getTarget();
            if (target->mark < 0 && dep->getWeight() > 0) {
                target->setInWeight(target->getInWeight() - dep->getWeight());
                if (dep->getType() == DependencyType::CRITICAL) {
                    target->setCriticalInWeight(target->getCriticalInWeight() - dep->getWeight());
                }

                if (target->getInWeight() <= 0 && target->getOutWeight() > 0) {
                    sources.emplace_back(target);
                }
            }
        }
    }
    for (auto &dep : node->getIncomingSegmentDependencies()) {
        if (!criticalOnly || dep->getType() == DependencyType::CRITICAL) {
            auto source = dep->getSource();
            if (source->mark < 0 && dep->getWeight() > 0) {
                source->setOutWeight(source->getOutWeight() - dep->getWeight());
                if (dep->getType() == DependencyType::CRITICAL) {
                    source->setCriticalOutWeight(source->getCriticalOutWeight() - dep->getWeight());
                }

                if (source->getOutWeight() <= 0 && source->getInWeight() > 0) {
                    sinks.emplace_back(source);
                }
            }
        }
    }
}
}  // namespace GuiBridge