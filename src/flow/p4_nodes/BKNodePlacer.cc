#include "BKNodePlacer.h"
#include <memory>
#include <vector>
#include "BKAlignedLayout.h"
#include "NeighborhoodInformation.h"


namespace GuiBridge {
    void BKNodePlacer::process(std::shared_ptr<Graph> &layeredGraph) {
        this->lGraph = layeredGraph;


        auto ni = NeighborhoodInformation::buildFor(layeredGraph);

        markConflicts(layeredGraph);

         std::shared_ptr<BKAlignedLayout> rightdown;
         std::shared_ptr<BKAlignedLayout> rightup;
         std::shared_ptr<BKAlignedLayout> leftdown;
         std::shared_ptr<BKAlignedLayout> leftup;

         std::vector<std::shared_ptr<BKAlignedLayout>> layouts(4);

         leftdown = std::make_shared<BKAlignedLayout>(layeredGraph, ni->getNodeCount(), VDirection::DOWN, HDirection::LEFT);
         leftup = std::make_shared<BKAlignedLayout>(layeredGraph, ni->getNodeCount(), VDirection::UP, HDirection::LEFT);
         rightdown = std::make_shared<BKAlignedLayout>(layeredGraph, ni->getNodeCount(), VDirection::DOWN, HDirection::RIGHT);
         rightup = std::make_shared<BKAlignedLayout>(layeredGraph, ni->getNodeCount(), VDirection::UP, HDirection::RIGHT);
        layouts.push_back(rightdown);
        layouts.push_back(rightup);
        layouts.push_back(leftdown);
        layouts.push_back(leftup);

        
    }
}