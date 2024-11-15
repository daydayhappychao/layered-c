#include "EdgeToPloyLine.h"
#include <algorithm>
#include "../../Graph.h"
#include "../../utils/randomUtil.h"

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

            float x = srcPoint.x + (dstPoint.x - srcPoint.x) / randomUtil->rangeFloat(2.0, 4.0);

            edge->getBendPoints().add(x, srcPoint.y);
            edge->getBendPoints().add(x, dstPoint.y);
        } else {
            edge->getBendPoints().add(srcPoint.x + 10, srcPoint.y);
            auto endPointY = std::max(edge->getSrc().node->getSize().y, edge->getDst().node->getSize().y) + 20;
            edge->getBendPoints().add(srcPoint.x + 10, endPointY);
            edge->getBendPoints().add(dstPoint.x - 10, endPointY);
            edge->getBendPoints().add(dstPoint.x - 10, dstPoint.y);
        }
    }
}
}  // namespace GuiBridge