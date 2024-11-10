// #include "OrthogonalEdgeRouter.h"
// #include <memory>
// #include "../../Graph.h"
// #include "OrthogonalRoutingGenerator.h"

// namespace GuiBridge {
// void OrthogonalEdgeRouter::process(std::shared_ptr<Graph> &layeredGraph) {
//     double nodeNodeSpacing = 20.0;
//     double edgeEdgeSpacing = 10.0;
//     double edgeNodeSpacing = 10.0;
//     auto routingGenerator = std::make_shared<OrthogonalRoutingGenerator>(edgeEdgeSpacing);

//     float xpos = 0.0;

//     int leftLayerIndex;
//     std::shared_ptr<Layer> leftLayer;
//     std::vector<std::shared_ptr<Node>> leftLayerNodes;

//     for (int i = 0; i < layeredGraph->getLayers().size(); i++) {
//         int rightLayerIndex = i + 1;
//         auto rightLayer =
//             rightLayerIndex < layeredGraph->getLayers().size() ? layeredGraph->getLayers()[rightLayerIndex] : nullptr;
//         std::vector<std::shared_ptr<Node>> rightLayerNodes;
//         if (rightLayer != nullptr) {
//             rightLayerNodes = rightLayer->getNodes();
//         }

//         if (leftLayer != nullptr) {
//             xpos += leftLayer->getSize().x;
//         }

//         double startPos = leftLayer == nullptr ? xpos : xpos + edgeNodeSpacing;
//         int slotsCount =
//             routingGenerator->routeEdges(layeredGraph, leftLayerNodes, leftLayerIndex, rightLayerNodes, startPos);

//         if (slotsCount > 0) {
//             double routingWidth = (slotsCount - 1) * edgeEdgeSpacing;
//             if (leftLayer != nullptr) {
//                 routingWidth += edgeNodeSpacing;
//             }
//             if (rightLayer != nullptr) {
//                 routingWidth += edgeNodeSpacing;
//             }
//             if (routingWidth < nodeNodeSpacing) {
//                 routingWidth = nodeNodeSpacing;
//             }
//             xpos += routingWidth;
//         } else {
//             xpos += nodeNodeSpacing;
//         }
//         leftLayer = rightLayer;
//         leftLayerNodes = rightLayerNodes;
//         leftLayerIndex = rightLayerIndex;
//     }
//     layeredGraph->getSize().setX(xpos);
// }
// }  // namespace GuiBridge