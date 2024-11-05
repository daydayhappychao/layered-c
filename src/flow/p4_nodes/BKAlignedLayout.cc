#include "BKAlignedLayout.h"
#include <algorithm>
#include "../../Graph.h"
#include "../../Layer.h"

namespace GuiBridge {
BKAlignedLayout::BKAlignedLayout(std::shared_ptr<Graph> &layeredGraph, int nodeCount, VDirection vdir, HDirection hdir)
    : layeredGraph(layeredGraph), vdir(vdir), hdir(hdir) {
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

double BKAlignedLayout::calculateDelta(std::shared_ptr<Port> &src, std::shared_ptr<Port> &tgt) {
    double srcPos =
        y[src->getNode()->getId()] + innerShift[src->getNode()->getId()] + src->getPos().y + src->getAnchor().y;
    double tgtPos =
        y[tgt->getNode()->getId()] + innerShift[tgt->getNode()->getId()] + tgt->getPos().y + tgt->getAnchor().y;
    return tgtPos - srcPos;
}

}  // namespace GuiBridge