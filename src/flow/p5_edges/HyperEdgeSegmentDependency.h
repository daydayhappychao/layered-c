#ifndef HYPER_EDGE_SEGMENT_DEPENDENCY_HPP
#define HYPER_EDGE_SEGMENT_DEPENDENCY_HPP

#include <memory>
#include <string>
namespace GuiBridge {
class HyperEdgeSegment;

enum class DependencyType { REGULAR, CRITICAL };
class HyperEdgeSegmentDependency {
public:
    static const int CRITICAL_DEPENDENCY_WEIGHT = 1;
    HyperEdgeSegmentDependency(DependencyType type, std::shared_ptr<HyperEdgeSegment> &source,
                               std::shared_ptr<HyperEdgeSegment> &target, int weight);
    static std::shared_ptr<HyperEdgeSegmentDependency> createAndAddRegular(std::shared_ptr<HyperEdgeSegment> &source,
                                                                           std::shared_ptr<HyperEdgeSegment> &target,
                                                                           int weight);
    static std::shared_ptr<HyperEdgeSegmentDependency> createAndAddCritical(std::shared_ptr<HyperEdgeSegment> &source,
                                                                            std::shared_ptr<HyperEdgeSegment> &target);
    void remove();
    void reverse();
    DependencyType getType() const;
    std::shared_ptr<HyperEdgeSegment> getSource() const;
    void setSource(std::shared_ptr<HyperEdgeSegment> &newSource);
    std::shared_ptr<HyperEdgeSegment> getTarget() const;
    void setTarget(std::shared_ptr<HyperEdgeSegment> &newTarget);
    int getWeight() const;
    std::string toString() const;

private:
    DependencyType type;
    std::shared_ptr<HyperEdgeSegment> source;
    std::shared_ptr<HyperEdgeSegment> target;
    int weight;
};
}  // namespace GuiBridge

#endif