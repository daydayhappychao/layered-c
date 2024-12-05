#include "HyperEdgeSegmentDependency.h"
#include <memory>
#include "HyperEdgeSegment.h"

namespace GuiBridge {
HyperEdgeSegmentDependency::HyperEdgeSegmentDependency(DependencyType type, std::shared_ptr<HyperEdgeSegment> &source,
                                                       std::shared_ptr<HyperEdgeSegment> &target, int weight)
    : type(type), weight(weight) {}

std::shared_ptr<HyperEdgeSegmentDependency> HyperEdgeSegmentDependency::createAndAddRegular(
    std::shared_ptr<HyperEdgeSegment> &source, std::shared_ptr<HyperEdgeSegment> &target, int weight) {
    auto newPtr = std::make_shared<HyperEdgeSegmentDependency>(DependencyType::REGULAR, source, target, weight);
    newPtr->setSource(source);
    newPtr->setTarget(target);
    return newPtr;
};

std::shared_ptr<HyperEdgeSegmentDependency> HyperEdgeSegmentDependency::createAndAddCritical(
    std::shared_ptr<HyperEdgeSegment> &source, std::shared_ptr<HyperEdgeSegment> &target) {
    auto newPtr = std::make_shared<HyperEdgeSegmentDependency>(DependencyType::CRITICAL, source, target,
                                                               CRITICAL_DEPENDENCY_WEIGHT);
    newPtr->setSource(source);
    newPtr->setTarget(target);
    return newPtr;
};
void HyperEdgeSegmentDependency::remove() {
    auto ptr = shared_from_this();

    if (source != nullptr) {
        auto it = std::find(source->getOutgoingSegmentDependencies().begin(),
                            source->getOutgoingSegmentDependencies().end(), ptr);
        source->getOutgoingSegmentDependencies().erase(it);
    }
    source = nullptr;
    if (target != nullptr) {
        auto it = std::find(target->getIncomingSegmentDependencies().begin(),
                            target->getIncomingSegmentDependencies().end(), shared_from_this());
        target->getIncomingSegmentDependencies().erase(it);
    }
    target = nullptr;
};

void HyperEdgeSegmentDependency::reverse() {
    auto oldSource = source;
    auto oldTarget = target;
    setSource(oldTarget);
    setTarget(oldSource);
};

DependencyType HyperEdgeSegmentDependency::getType() const { return type; };

std::shared_ptr<HyperEdgeSegment> HyperEdgeSegmentDependency::getSource() const { return source; };

void HyperEdgeSegmentDependency::setSource(std::shared_ptr<HyperEdgeSegment> &newSource) {
    auto ptr = shared_from_this();
    if (source != nullptr) {
        auto it = std::find(source->getOutgoingSegmentDependencies().begin(),
                            source->getOutgoingSegmentDependencies().end(), ptr);
        source->getOutgoingSegmentDependencies().erase(it);
    }
    source = newSource;
    source->getOutgoingSegmentDependencies().emplace_back(ptr);
};

std::shared_ptr<HyperEdgeSegment> HyperEdgeSegmentDependency::getTarget() const { return target; };

void HyperEdgeSegmentDependency::setTarget(std::shared_ptr<HyperEdgeSegment> &newTarget) {
    if (target != nullptr) {
        auto it = std::find(target->getIncomingSegmentDependencies().begin(),
                            target->getIncomingSegmentDependencies().end(), shared_from_this());
        target->getIncomingSegmentDependencies().erase(it);
    }

    target = newTarget;

    target->getIncomingSegmentDependencies().emplace_back(shared_from_this());
};

int HyperEdgeSegmentDependency::getWeight() const { return weight; };

}  // namespace GuiBridge