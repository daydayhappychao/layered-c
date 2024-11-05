#include "LayerConstraintPreprocessor.h"
#include <memory>
#include <vector>
#include "../../Graph.h"
#include "../../utils/HiddenNodeConnections.h"
namespace GuiBridge {
void LayerConstraintPreprocessor::process(std::shared_ptr<Graph> &graph) {
    std::vector<std::shared_ptr<Node>> hiddenNodes;
    auto &nodeList = graph->getLayerlessNodes();
    for (auto it = nodeList.begin(); it != nodeList.end();) {
        auto lNode = *it;
        // 固定在首列和末列的先隐藏
        if (isRelevantNode(lNode)) {
            hide(lNode);
            hiddenNodes.push_back(lNode);
            it = nodeList.erase(it);  // 删除元素并移动迭代器到下一个元素
        } else {
            ++it;  // 移动到下一个元素
        }
    }
    if (!hiddenNodes.empty()) {
        graph->setHiddenNodes(hiddenNodes);
    }
};

bool LayerConstraintPreprocessor::isRelevantNode(std::shared_ptr<Node> &node) {
    switch (node->getSide()) {
        case NodeSide::FIRST_SEPARATE:
        case NodeSide::LAST_SEPARATE:
            return true;
        default:
            return false;
    }
};

void LayerConstraintPreprocessor::hide(std::shared_ptr<Node> &node) {
    for (auto &edge : node->getEdges()) {
        hide(node, edge);
    }
};
void LayerConstraintPreprocessor::hide(std::shared_ptr<Node> &node, std::shared_ptr<Edge> &edge) {
    bool isOutgoing = edge->getSrc()->getNode() == node;
    OppositeType oppositeType = isOutgoing ? Dst : Src;
    std::shared_ptr<Port> oppositePort = edge->setOppositePort(oppositeType);
    auto oppositeNode = oppositePort->getNode();
    updateOppositeNodeLayerConstraints(node, oppositeNode);
};

void LayerConstraintPreprocessor::updateOppositeNodeLayerConstraints(std::shared_ptr<Node> &hiddenNode,
                                                                     std::shared_ptr<Node> &oppositeNode) {
    if (oppositeNode->getSide() == NodeSide::FIRST_SEPARATE || oppositeNode->getSide() == NodeSide::LAST_SEPARATE) {
        return;
    }
    auto connections = HiddenNodeConnections::combine(oppositeNode->getHiddenNodeConnections(), hiddenNode->getSide());
    oppositeNode->setHiddenNodeConnections(connections);

    if (!oppositeNode->getEdges().empty()) {
        return;
    }

    switch (connections) {
        case HiddenNodeConnections::Connection::FIRST_SEPARATE:
            oppositeNode->setSide(NodeSide::FIRST);
            break;
        case HiddenNodeConnections::Connection::LAST_SEPARATE:
            oppositeNode->setSide(NodeSide::LAST);
            break;
        default:
            break;
    }
};

}  // namespace GuiBridge