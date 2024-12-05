#pragma once

#include <memory>
#include <vector>
#include "HyperEdgeSegment.h"
namespace GuiBridge {

class HyperEdgeSegmentDependency;
class HyperEdgeCycleDetector {
public:
    static std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> detectCycles(
        std::vector<std::shared_ptr<HyperEdgeSegment>> &segments, bool criticalOnly);

private:
    static void initialize(std::vector<std::shared_ptr<HyperEdgeSegment>> &segments,
                           std::vector<std::shared_ptr<HyperEdgeSegment>> &sources,
                           std::vector<std::shared_ptr<HyperEdgeSegment>> &sinks, bool criticalOnly);
    static void computeLinearOrderingMarks(std::vector<std::shared_ptr<HyperEdgeSegment>> &segments,
                                           std::vector<std::shared_ptr<HyperEdgeSegment>> &sources,
                                           std::vector<std::shared_ptr<HyperEdgeSegment>> &sinks, bool criticalOnly);

    static void updateNeighbors(std::shared_ptr<HyperEdgeSegment> &node,
                                std::vector<std::shared_ptr<HyperEdgeSegment>> &sources,
                                std::vector<std::shared_ptr<HyperEdgeSegment>> &sinks, bool criticalOnly);
};
}  // namespace GuiBridge