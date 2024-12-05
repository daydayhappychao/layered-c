#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

namespace GuiBridge {
class OrthogonalRoutingGenerator;
class HyperEdgeSegmentDependency;
class HyperEdgeSegment;

struct FreeArea {
    double startPosition;
    double endPosition;
    double size;
    FreeArea(double startPosition, double endPosition);
};

struct AreaRating {
    int dependencies;
    int crossings;
    AreaRating(int dependencies, int crossings);
};

class HyperEdgeSegmentSplitter {
public:
    explicit HyperEdgeSegmentSplitter(std::shared_ptr<OrthogonalRoutingGenerator> &routingGenerator);
    void splitSegments(std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &dependenciesToResolve,
                       std::vector<std::shared_ptr<HyperEdgeSegment>> &segments, double criticalConflictThreshold);

private:
    std::shared_ptr<OrthogonalRoutingGenerator> routingGenerator;
    std::vector<FreeArea> findFreeAreas(std::vector<std::shared_ptr<HyperEdgeSegment>> &segments,
                                        double criticalConflictThreshold);
    std::unordered_set<std::shared_ptr<HyperEdgeSegment>> decideWhichSegmentsToSplit(
        std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &dependencies);
    void split(std::shared_ptr<HyperEdgeSegment> &segment, std::vector<std::shared_ptr<HyperEdgeSegment>> &segments,
               std::vector<FreeArea> &freeAreas, double criticalConflictThreshold);
    void updateDependencies(std::shared_ptr<HyperEdgeSegment> &segment,
                            std::vector<std::shared_ptr<HyperEdgeSegment>> &segments);
    double computePositionToSplitAndUpdateFreeAreas(std::shared_ptr<HyperEdgeSegment> &segment,
                                                    std::vector<FreeArea> &freeAreas, double criticalConflictThreshold);
    int chooseBestAreaIndex(std::shared_ptr<HyperEdgeSegment> &segment, std::vector<FreeArea> &freeAreas, int fromIndex,
                            int toIndex);
    AreaRating rateArea(std::shared_ptr<HyperEdgeSegment> &segment, std::shared_ptr<HyperEdgeSegment> &splitSegment,
                        std::shared_ptr<HyperEdgeSegment> &splitPartner, const FreeArea &area);
    void updateConsideringBothOrderings(AreaRating &rating, std::shared_ptr<HyperEdgeSegment> &s1,
                                        std::shared_ptr<HyperEdgeSegment> &s2);
    int countCrossingsForSingleOrdering(std::shared_ptr<HyperEdgeSegment> &left,
                                        std::shared_ptr<HyperEdgeSegment> &right);
    bool isBetter(const FreeArea &currArea, const AreaRating &currRating, const FreeArea &bestArea,
                  const AreaRating &bestRating);
    void useArea(std::vector<FreeArea> &freeAreas, int usedAreaIndex, double criticalConflictThreshold);
    static double center(const std::shared_ptr<HyperEdgeSegment> &s);
    static double center(const FreeArea &a);
    static double center(double p1, double p2);
};
}  // namespace GuiBridge
