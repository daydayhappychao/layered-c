#pragma once
#include <memory>
#include <vector>
#include "../../opts/PortType.h"
#include "../../utils/Constants.h"

namespace GuiBridge {
class Graph;
class KVector;
class Layer;

class EdgeToPloyLine {
public:
    void process(std::shared_ptr<Graph> &graph);

private:
    float applyMinCrossY(const KVector &startPoint, const KVector &endPoint, std::shared_ptr<Graph> &graph);
    float applyMinCrossX(std::shared_ptr<Layer> &layer, PortType portType, float currentY, float targetY);
    // 记录 layer 之间已经被使用掉的 x 坐标
    float reverseEdgeGap = (EDGE_NODE_GAP + EDGE_EDGE_GAP) / 4;
};
}  // namespace GuiBridge