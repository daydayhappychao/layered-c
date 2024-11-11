#include "LayerSweepTypeDecider.h"

namespace GuiBridge {
LayerSweepTypeDecider::LayerSweepTypeDecider(GraphInfoHolder &graphData) : graphData(graphData) {
    nodeInfo.resize(graphData.currentNodeOrder.size());
}

}  // namespace GuiBridge
