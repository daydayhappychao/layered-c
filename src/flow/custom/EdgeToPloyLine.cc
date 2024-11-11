#include "EdgeToPloyLine.h"
#include <algorithm>
#include "../../Graph.h"

namespace GuiBridge {
void EdgeToPloyLine::process(std::shared_ptr<Graph> &graph) {
    for (auto &edge : graph->getEdges()) {
        auto srcPoint = edge->getSrcPoint();
        auto dstPoint = edge->getDstPoint();
        // 第一重判断，是否正方向线
        if (srcPoint.x < dstPoint.x) {
            // 第二重判断，是否已经水平
            if (srcPoint.y == dstPoint.y) {
                continue;
            }
            edge->getBendPoints().add((srcPoint.x + dstPoint.x) / 2, srcPoint.y);
            edge->getBendPoints().add((srcPoint.x + dstPoint.x) / 2, dstPoint.y);
        } else {
            edge->getBendPoints().add(srcPoint.x + 10, srcPoint.y);
            auto endPointY = std::max(edge->getSrc().node->getSize().y, edge->getDst().node->getSize().y) + 20;
            edge->getBendPoints().add(srcPoint.x + 10, endPointY);
            edge->getBendPoints().add(dstPoint.x + 10, endPointY);
            edge->getBendPoints().add(dstPoint.x + 10, dstPoint.y);
        }
    }
}
}  // namespace GuiBridge