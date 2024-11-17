#include "BKAlignedLayout.h"
#include <algorithm>
#include "../../Graph.h"
#include "../../Layer.h"
#include "NeighborhoodInformation.h"

namespace GuiBridge {
BKAlignedLayout::BKAlignedLayout(std::shared_ptr<Graph> &layeredGraph, int nodeCount, VDirection vdir, HDirection hdir)
    : vdir(vdir), hdir(hdir), layeredGraph(layeredGraph) {
    root.resize(nodeCount);
    blockSize.resize(nodeCount);
    align.resize(nodeCount);
    innerShift.resize(nodeCount);
    sink.resize(nodeCount);
    shift.resize(nodeCount);
    y.resize(nodeCount);
    su.resize(nodeCount, false);
    od.resize(nodeCount, true);
};

void BKAlignedLayout::cleanup() {
    root.clear();
    blockSize.clear();
    align.clear();
    innerShift.clear();
    sink.clear();
    shift.clear();
    y.clear();
}

double BKAlignedLayout::layoutSize() {
    double min = std::numeric_limits<double>::infinity();
    double max = -std::numeric_limits<double>::infinity();
    for (auto &layer : layeredGraph->getLayers()) {
        for (auto &n : layer->getNodes()) {
            double yMin = y[n->getId()];
            double yMax = yMin + blockSize[root[n->getId()]->getId()];
            min = std::min(min, yMin);
            max = std::max(max, yMax);
        }
    }
    return max - min;
}

double BKAlignedLayout::calculateDelta(EdgeTarget &src, EdgeTarget &tgt) {
    double srcPos =
        y[src.node->getId()] + innerShift[src.node->getId()] + src.port->getPos().y + src.port->getAnchor().y;
    double tgtPos =
        y[tgt.node->getId()] + innerShift[tgt.node->getId()] + tgt.port->getPos().y + tgt.port->getAnchor().y;
    return tgtPos - srcPos;
}

void BKAlignedLayout::shiftBlock(std::shared_ptr<Node> &rootNode, double delta) {
    auto current = rootNode;
    do {
        double newPos = y[current->getId()] + delta;
        y[current->getId()] = newPos;
        current = align[current->getId()];
    } while (current != rootNode);
}

double BKAlignedLayout::checkSpaceAbove(std::shared_ptr<Node> &blockRoot, double delta,
                                        std::shared_ptr<NeighborhoodInformation> &ni) {
    double availableSpace = delta;
    const auto &rootNode = blockRoot;
    auto current = rootNode;
    do {
        current = align[current->getId()];

        double minYCurrent = getMinY(current);

        auto neighbor = getUpperNeighbor(current, ni);

        if (neighbor != nullptr) {
            double maxYNeighbor = getMaxY(neighbor);

            /**
             * @todo
             *  这里的 100是我暂时瞎填的,包括下面的
             */
            availableSpace = std::min(availableSpace, minYCurrent - (maxYNeighbor + 10));
        }

    } while (rootNode != current);
    return availableSpace;
}

double BKAlignedLayout::checkSpaceBelow(std::shared_ptr<Node> &blockRoot, double delta,
                                        std::shared_ptr<NeighborhoodInformation> &ni) {
    double availableSpace = delta;
    const auto &rootNode = blockRoot;
    auto current = rootNode;
    do {
        current = align[current->getId()];
        double maxYCurrent = getMaxY(current);
        auto neighbor = getLowerNeighbor(current, ni);
        if (neighbor != nullptr) {
            double minYNeighbor = getMinY(neighbor);

            availableSpace = std::min(availableSpace, minYNeighbor - (maxYCurrent + 10));
        }
    } while (rootNode != current);
    return availableSpace;
}

double BKAlignedLayout::getMinY(std::shared_ptr<Node> &n) {
    auto rootNode = root[n->getId()];
    return y[rootNode->getId()] + innerShift[n->getId()] - n->margin.top;
}

double BKAlignedLayout::getMaxY(std::shared_ptr<Node> &n) {
    auto rootNode = root[n->getId()];
    return y[rootNode->getId()] + innerShift[n->getId()] + n->getSize().y + n->margin.bottom;
}

std::shared_ptr<Node> BKAlignedLayout::getLowerNeighbor(std::shared_ptr<Node> &n,
                                                        std::shared_ptr<NeighborhoodInformation> &ni) {
    auto l = n->getLayer();
    auto layerIndex = ni->getNodeIndex()[n->getId()];
    if (layerIndex < l->getNodes().size() - 1) {
        return l->getNodes()[layerIndex + 1];
    }
    return nullptr;
}

std::shared_ptr<Node> BKAlignedLayout::getUpperNeighbor(std::shared_ptr<Node> &n,
                                                        std::shared_ptr<NeighborhoodInformation> &ni) {
    auto l = n->getLayer();
    auto layerIndex = ni->getNodeIndex()[n->getId()];
    if (layerIndex > 0) {
        return l->getNodes()[layerIndex - 1];
    }
    return nullptr;
}

}  // namespace GuiBridge