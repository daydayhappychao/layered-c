#include "HyperEdgeSegmentSplitter.h"
#include <memory>
#include <unordered_set>
#include <vector>
#include "HyperEdgeSegment.h"
#include "HyperEdgeSegmentDependency.h"
#include "OrthogonalRoutingGenerator.h"

namespace GuiBridge {
FreeArea::FreeArea(double startPosition, double endPosition)
    : startPosition(startPosition), endPosition(endPosition), size(endPosition - startPosition){};
AreaRating::AreaRating(int dependencies, int crossings) : dependencies(dependencies), crossings(crossings){};

HyperEdgeSegmentSplitter::HyperEdgeSegmentSplitter(std::shared_ptr<OrthogonalRoutingGenerator> &routingGenerator)
    : routingGenerator(routingGenerator){};

void HyperEdgeSegmentSplitter::splitSegments(
    std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &dependenciesToResolve,
    std::vector<std::shared_ptr<HyperEdgeSegment>> &segments, double criticalConflictThreshold) {
    if (dependenciesToResolve.empty()) {
        return;
    }

    auto freeAreas = findFreeAreas(segments, criticalConflictThreshold);
    auto segmentsToSplit = decideWhichSegmentsToSplit(dependenciesToResolve);

    // Split the segments in order from smallest to largest. The smallest ones need to be split first since they
    // have fewer options for where to put their horizontal connecting segments.
    std::vector<std::shared_ptr<HyperEdgeSegment>> sortedSegments(segmentsToSplit.begin(), segmentsToSplit.end());

    // Sort the segments based on their length
    std::sort(sortedSegments.begin(), sortedSegments.end(),
              [](const std::shared_ptr<HyperEdgeSegment> &hes1, const std::shared_ptr<HyperEdgeSegment> &hes2) {
                  return hes1->getLength() < hes2->getLength();
              });

    // Iterate over the sorted segments and split them
    for (auto &segment : sortedSegments) {
        split(segment, segments, freeAreas, criticalConflictThreshold);
    }
};

std::vector<FreeArea> HyperEdgeSegmentSplitter::findFreeAreas(std::vector<std::shared_ptr<HyperEdgeSegment>> &segments,
                                                              double criticalConflictThreshold) {
    std::vector<FreeArea> freeAreas;
    // Collect all positions where hyperedge segments connect to ports, and sort them
    std::vector<double> coordinates;

    // Gather incoming and outgoing coordinates from segments
    for (const auto &segment : segments) {
        const auto &inCoords = segment->getIncomingConnectionCoordinates();
        const auto &outCoords = segment->getOutgoingConnectionCoordinates();
        coordinates.insert(coordinates.end(), inCoords.begin(), inCoords.end());
        coordinates.insert(coordinates.end(), outCoords.begin(), outCoords.end());
    }

    // Sort all coordinates
    std::sort(coordinates.begin(), coordinates.end());

    // Go through each pair of coordinates and create free areas for those that are at least twice the critical
    // threshold
    for (size_t i = 1; i < coordinates.size(); ++i) {
        if (coordinates[i] - coordinates[i - 1] >= 2 * criticalConflictThreshold) {
            freeAreas.emplace_back(coordinates[i - 1] + criticalConflictThreshold,
                                   coordinates[i] - criticalConflictThreshold);
        }
    }

    return freeAreas;
};

std::unordered_set<std::shared_ptr<HyperEdgeSegment>> HyperEdgeSegmentSplitter::decideWhichSegmentsToSplit(
    std::vector<std::shared_ptr<HyperEdgeSegmentDependency>> &dependencies) {
    std::unordered_set<std::shared_ptr<HyperEdgeSegment>> segmentsToSplit;

    for (auto &dependency : dependencies) {
        auto sourceSegment = dependency->getSource();
        auto targetSegment = dependency->getTarget();

        if (segmentsToSplit.find(sourceSegment) != segmentsToSplit.end() ||
            (segmentsToSplit.find(targetSegment) != segmentsToSplit.end())) {
            continue;
        }

        auto segmentToSplit = sourceSegment;
        auto segmentCausingSplit = targetSegment;

        if (sourceSegment->representsHyperedge() && !targetSegment->representsHyperedge()) {
            segmentToSplit = targetSegment;
            segmentCausingSplit = sourceSegment;
        }

        segmentsToSplit.insert(segmentToSplit);
        segmentToSplit->setSplitBy(segmentCausingSplit);
    }
    return segmentsToSplit;
};

void HyperEdgeSegmentSplitter::split(std::shared_ptr<HyperEdgeSegment> &segment,
                                     std::vector<std::shared_ptr<HyperEdgeSegment>> &segments,
                                     std::vector<FreeArea> &freeAreas, double criticalConflictThreshold) {
    auto splitPosition = computePositionToSplitAndUpdateFreeAreas(segment, freeAreas, criticalConflictThreshold);
    segments.emplace_back(segment->splitAt(splitPosition));
    updateDependencies(segment, segments);
};

void HyperEdgeSegmentSplitter::updateDependencies(std::shared_ptr<HyperEdgeSegment> &segment,
                                                  std::vector<std::shared_ptr<HyperEdgeSegment>> &segments) {
    auto splitCausingSegment = segment->getSplitBy();
    auto splitPartner = segment->getSplitPartner();

    HyperEdgeSegmentDependency::createAndAddCritical(segment, splitCausingSegment);
    HyperEdgeSegmentDependency::createAndAddCritical(splitCausingSegment, splitPartner);

    // Now we just need to re-introduce dependencies to other segments
    for (auto &otherSegment : segments) {
        // We already have our dependencies between our three segments involved in the conflict settled
        if (otherSegment != splitCausingSegment && otherSegment != segment && otherSegment != splitPartner) {
            routingGenerator->createDependencyIfNecessary(otherSegment, segment);
            routingGenerator->createDependencyIfNecessary(otherSegment, splitPartner);
        }
    }
};

double HyperEdgeSegmentSplitter::computePositionToSplitAndUpdateFreeAreas(std::shared_ptr<HyperEdgeSegment> &segment,
                                                                          std::vector<FreeArea> &freeAreas,
                                                                          double criticalConflictThreshold) {
    // We shall iterate over the available areas (which are sorted by ascending positions) and find the index of the
    // first and the last area we could use since they are in our segment's reach
    int firstPossibleAreaIndex = -1;
    int lastPossibleAreaIndex = -1;

    for (int i = 0; i < freeAreas.size(); i++) {
        auto currArea = freeAreas.at(i);

        if (currArea.startPosition > segment->getEndCoordinate()) {
            // We're past the possible areas, so stop
            break;
        }
        if (currArea.endPosition >= segment->getStartCoordinate()) {
            // We've found a possible area; it might be the first
            if (firstPossibleAreaIndex < 0) {
                firstPossibleAreaIndex = i;
            }

            lastPossibleAreaIndex = i;
        }
    }

    // Determine the position to split the segment at
    double splitPosition = center(segment);

    if (firstPossibleAreaIndex >= 0) {
        // There are areas we can use
        int bestAreaIndex = chooseBestAreaIndex(segment, freeAreas, firstPossibleAreaIndex, lastPossibleAreaIndex);

        // We'll use the best area's centre and update the area list
        splitPosition = center(freeAreas.at(bestAreaIndex));
        useArea(freeAreas, bestAreaIndex, criticalConflictThreshold);
    }

    return splitPosition;
};

int HyperEdgeSegmentSplitter::chooseBestAreaIndex(std::shared_ptr<HyperEdgeSegment> &segment,
                                                  std::vector<FreeArea> &freeAreas, int fromIndex, int toIndex) {
    int bestAreaIndex = fromIndex;

    if (fromIndex < toIndex) {
        // We have more areas to choose from, so rate them and find the best one. We need to simulate splitting the
        // segment so that we can count potential crossings
        auto splitSegments = segment->simulateSplit();
        auto splitSegment = splitSegments.first;
        auto splitPartner = splitSegments.second;

        FreeArea bestArea = freeAreas.at(bestAreaIndex);
        auto bestRating = rateArea(segment, splitSegment, splitPartner, bestArea);

        for (int i = fromIndex + 1; i <= toIndex; i++) {
            // Determine how good our current area is
            FreeArea currArea = freeAreas.at(i);
            AreaRating currRating = rateArea(segment, splitSegment, splitPartner, currArea);

            if (isBetter(currArea, currRating, bestArea, bestRating)) {
                bestArea = currArea;
                bestRating = currRating;
                bestAreaIndex = i;
            }
        }
    }
    return bestAreaIndex;
};

AreaRating HyperEdgeSegmentSplitter::rateArea(std::shared_ptr<HyperEdgeSegment> &segment,
                                              std::shared_ptr<HyperEdgeSegment> &splitSegment,
                                              std::shared_ptr<HyperEdgeSegment> &splitPartner, const FreeArea &area) {
    double areaCentre = center(area);

    splitSegment->getOutgoingConnectionCoordinates().clear();
    splitSegment->getOutgoingConnectionCoordinates().emplace_back(areaCentre);

    splitPartner->getIncomingConnectionCoordinates().clear();
    splitPartner->getIncomingConnectionCoordinates().emplace_back(areaCentre);

    // We need to count the dependencies and crossings that the split partners would cause with the original
    // segments incident dependencies
    AreaRating rating(0, 0);

    for (auto &dependency : segment->getIncomingSegmentDependencies()) {
        auto otherSegment = dependency->getSource();

        updateConsideringBothOrderings(rating, splitSegment, otherSegment);
        updateConsideringBothOrderings(rating, splitPartner, otherSegment);
    }

    for (auto &dependency : segment->getOutgoingSegmentDependencies()) {
        auto otherSegment = dependency->getTarget();

        updateConsideringBothOrderings(rating, splitSegment, otherSegment);
        updateConsideringBothOrderings(rating, splitPartner, otherSegment);
    }

    // There will be two additional dependencies: splitSegment --> splitBySegment --> splitPartner. The order
    // between the three will not change, so we only have to count crossings for this concrete order
    rating.dependencies += 2;

    auto splitByPtr = segment->getSplitBy();
    rating.crossings += countCrossingsForSingleOrdering(splitSegment, splitByPtr);
    rating.crossings += countCrossingsForSingleOrdering(splitByPtr, splitPartner);

    return rating;
};

void HyperEdgeSegmentSplitter::updateConsideringBothOrderings(AreaRating &rating, std::shared_ptr<HyperEdgeSegment> &s1,
                                                              std::shared_ptr<HyperEdgeSegment> &s2) {
    int crossingsS1LeftOfS2 = countCrossingsForSingleOrdering(s1, s2);
    int crossingsS2LeftOfS1 = countCrossingsForSingleOrdering(s2, s1);

    if (crossingsS1LeftOfS2 == crossingsS2LeftOfS1) {
        // If the crossings are the same, we're only interested if there are more than 0
        if (crossingsS1LeftOfS2 > 0) {
            // Both orderings generate the same number of crossings (more than 0), so we have a two-cycle
            rating.dependencies += 2;
            rating.crossings += crossingsS1LeftOfS2;
        }
    } else {
        // One order is better than the other, so there will be a single dependency
        rating.dependencies += 1;
        rating.crossings += std::min(crossingsS1LeftOfS2, crossingsS2LeftOfS1);
    }
};

int HyperEdgeSegmentSplitter::countCrossingsForSingleOrdering(std::shared_ptr<HyperEdgeSegment> &left,
                                                              std::shared_ptr<HyperEdgeSegment> &right) {
    return OrthogonalRoutingGenerator::countCrossings(left->getOutgoingConnectionCoordinates(),
                                                      right->getStartCoordinate(), right->getEndCoordinate()) +
           OrthogonalRoutingGenerator::countCrossings(right->getIncomingConnectionCoordinates(),
                                                      left->getStartCoordinate(), left->getEndCoordinate());
};

bool HyperEdgeSegmentSplitter::isBetter(const FreeArea &currArea, const AreaRating &currRating,
                                        const FreeArea &bestArea, const AreaRating &bestRating) {
    if (currRating.crossings < bestRating.crossings) {
        // First criterion: number of crossings
        return true;
    }
    if (currRating.crossings == bestRating.crossings) {
        if (currRating.dependencies < bestRating.dependencies) {
            // Second criterion: number of dependencies
            return true;
        }
        if (currRating.dependencies == bestRating.dependencies) {
            if (currArea.size > bestArea.size) {
                // Third criterion: size
                return true;
            }
        }
    }

    return false;
};

void HyperEdgeSegmentSplitter::useArea(std::vector<FreeArea> &freeAreas, int usedAreaIndex,
                                       double criticalConflictThreshold) {
    auto oldArea = freeAreas[usedAreaIndex];
    freeAreas.erase(freeAreas.begin() + usedAreaIndex);  // Remove the area at the given index

    // Check if the area is large enough to split
    if (oldArea.size / 2 >= criticalConflictThreshold) {
        // This area is large enough to split and still have enough space
        int insertIndex = usedAreaIndex;

        double oldAreaCentre = center(oldArea);

        // Create the first new area
        double newEnd1 = oldAreaCentre - criticalConflictThreshold;
        if (oldArea.startPosition <= oldAreaCentre - criticalConflictThreshold) {
            FreeArea newArea1(oldArea.startPosition, newEnd1);
            freeAreas.insert(freeAreas.begin() + insertIndex++, newArea1);
        }

        // Create the second new area
        double newStart2 = oldAreaCentre + criticalConflictThreshold;
        if (newStart2 <= oldArea.endPosition) {
            FreeArea newArea2(newStart2, oldArea.endPosition);
            freeAreas.insert(freeAreas.begin() + insertIndex, newArea2);
        }
    }
};

double HyperEdgeSegmentSplitter::center(const std::shared_ptr<HyperEdgeSegment> &s) {
    return center(s->getStartCoordinate(), s->getEndCoordinate());
};

double HyperEdgeSegmentSplitter::center(const FreeArea &a) { return center(a.startPosition, a.endPosition); };
double HyperEdgeSegmentSplitter::center(double p1, double p2) { return (p1 + p2) / 2; };
}  // namespace GuiBridge