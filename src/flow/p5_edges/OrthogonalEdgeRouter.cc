#include "OrthogonalEdgeRouter.h"
#include <memory>
#include "../../Graph.h"
#include "../../utils/Constants.h"
#include "OrthogonalRoutingGenerator.h"

namespace GuiBridge {
void OrthogonalEdgeRouter::process(std::shared_ptr<Graph> &layeredGraph) {
    float nodeNodeSpacing = NODE_NODE_GAP;
    float edgeEdgeSpacing = EDGE_EDGE_GAP;
    float edgeNodeSpacing = EDGE_NODE_GAP;
    auto routingGenerator = std::make_shared<OrthogonalRoutingGenerator>(edgeEdgeSpacing);

    float xpos = 0.0;

    int leftLayerIndex;
    std::shared_ptr<Layer> leftLayer;
    std::vector<std::shared_ptr<Node>> leftLayerNodes;

    for (int i = 0; i < layeredGraph->getLayers().size(); i++) {
        int rightLayerIndex = i + 1;
        auto rightLayer =
            rightLayerIndex < layeredGraph->getLayers().size() ? layeredGraph->getLayers()[rightLayerIndex] : nullptr;
        std::vector<std::shared_ptr<Node>> rightLayerNodes;
        if (rightLayer != nullptr) {
            rightLayerNodes = rightLayer->getNodes();
        }

        if (leftLayer != nullptr) {
            xpos += leftLayer->getSize().x;
        }

        float startPos = xpos + edgeNodeSpacing;
        int slotsCount =
            routingGenerator->routeEdges(layeredGraph, leftLayerNodes, leftLayerIndex, rightLayerNodes, startPos);

        if (slotsCount > 0) {
            float routingWidth = (slotsCount - 1) * edgeEdgeSpacing;
            if (leftLayer != nullptr) {
                routingWidth += edgeNodeSpacing;
            }
            if (rightLayer != nullptr) {
                routingWidth += edgeNodeSpacing;
            }
            if (routingWidth < nodeNodeSpacing) {
                routingWidth = nodeNodeSpacing;
            }
            if (rightLayer != nullptr) {
                rightLayer->setMargin(routingWidth);
                rightLayer->adjustSize();
            }
            xpos += routingWidth;
        } else {
            if (rightLayer != nullptr) {
                rightLayer->setMargin(nodeNodeSpacing);
                rightLayer->adjustSize();
            }
            xpos += nodeNodeSpacing;
        }
        leftLayer = rightLayer;
        leftLayerNodes = rightLayerNodes;
        leftLayerIndex = rightLayerIndex;
    }
    layeredGraph->getSize().setX(xpos);
}
}  // namespace GuiBridge