#include "ThresholdStrategy.h"
#include <memory>
#include "../../Graph.h"
#include "BKAlignedLayout.h"

namespace GuiBridge {
Postprocessable::Postprocessable(std::shared_ptr<Node> &free, bool isRoot) : free(free), isRoot(isRoot){};

void ThresholdStrategy::init(std::shared_ptr<BKAlignedLayout> &theBal,
                             std::shared_ptr<NeighborhoodInformation> &theNi) {
    this->bal = theBal;
    this->ni = theNi;
    blockFinished.clear();
};

void ThresholdStrategy::finishBlock(std::shared_ptr<Node> &n) { blockFinished.insert(n); }

std::shared_ptr<Node> ThresholdStrategy::getOther(std::shared_ptr<Edge> &edge, std::shared_ptr<Node> &n) {
    if (edge->getSrc().node == n) {
        return edge->getDst().node;
    }
    if (edge->getDst().node == n) {
        return edge->getSrc().node;
    }
    throw std::invalid_argument("Node " + n->name + " is neither source nor target of edge ");
}

double SimpleThresholdStrategy::calculateThreshold(double oldThresh, std::shared_ptr<Node> &blockRoot,
                                                   std::shared_ptr<Node> &currentNode) {
    bool isRoot = blockRoot == currentNode;
    bool isLast = bal->align[currentNode->getId()] == blockRoot;

    if (!(isRoot || isLast)) {
        return oldThresh;
    }

    double t = oldThresh;
    if (bal->hdir == HDirection::RIGHT) {
        if (isRoot) {
            t = getBound(blockRoot, true);
        }
        if (std::isinf(t) && isLast) {
            t = getBound(currentNode, false);
        }
    } else {  // LEFT
        if (isRoot) {
            t = getBound(blockRoot, true);
        }
        if (std::isinf(t) && isLast) {
            t = getBound(currentNode, false);
        }
    }

    return t;
};

std::shared_ptr<Postprocessable> SimpleThresholdStrategy::pickEdge(std::shared_ptr<Postprocessable> &pp) {
    std::vector<std::shared_ptr<Edge>> edges;
    if (pp->isRoot) {
        if (bal->hdir == HDirection::RIGHT) {
            edges = pp->free->getIncomingEdges();
        } else {
            edges = pp->free->getOutgoingEdges();
        }
    } else {
        if (bal->hdir == HDirection::LEFT) {
            edges = pp->free->getIncomingEdges();
        } else {
            edges = pp->free->getOutgoingEdges();
        }
    }

    bool hasEdges = false;
    for (auto &e : edges) {
        // 忽略层内边（除非块仅通过层内边连接）
        bool onlyDummies = bal->od[bal->root[pp->free->getId()]->getId()];
        if (!onlyDummies && e->isInLayerEdge()) {
            continue;
        }

        // 如果该块已经是直边的一部分，则不能再次移动它
        if (bal->su[bal->root[pp->free->getId()]->getId()] || bal->su[bal->root[pp->free->getId()]->getId()]) {
            continue;
        }

        hasEdges = true;

        // 如果另一个节点还没有位置，则忽略这条边
        if (blockFinished.find(bal->root[getOther(e, pp->free)->getId()]) != blockFinished.end()) {
            pp->hasEdges = true;
            pp->edge = e;
            return pp;
        }
    }

    // 没有选择边
    pp->hasEdges = hasEdges;
    pp->edge = nullptr;
    return pp;
}

double SimpleThresholdStrategy::getBound(std::shared_ptr<Node> &blockNode, bool isRoot) {
    double invalid = (bal->vdir == VDirection::UP) ? std::numeric_limits<double>::infinity()
                                                   : -std::numeric_limits<double>::infinity();

    auto ptr = std::make_shared<Postprocessable>(blockNode, isRoot);
    auto pick = pickEdge(ptr);

    // 如果存在边但我们找不到合适的边
    if (pick->edge == nullptr && pick->hasEdges) {
        postProcessablesQueue.push(pick);
        return invalid;
    }
    if (pick->edge != nullptr) {
        double threshold;
        auto left = pick->edge->getSrc();
        auto right = pick->edge->getDst();

        if (isRoot) {
            auto rootPort = (bal->hdir == HDirection::RIGHT) ? right : left;
            auto otherPort = (bal->hdir == HDirection::RIGHT) ? left : right;

            auto otherRoot = bal->root[otherPort.node->getId()];
            threshold = bal->y[otherRoot->getId()] + bal->innerShift[otherPort.node->getId()] +
                        otherPort.port->getPos().y + otherPort.port->getAnchor().y -
                        bal->innerShift[rootPort.node->getId()] - rootPort.port->getPos().y -
                        rootPort.port->getAnchor().y;
        } else {
            auto rootPort = (bal->hdir == HDirection::LEFT) ? right : left;
            auto otherPort = (bal->hdir == HDirection::LEFT) ? left : right;

            threshold = bal->y[bal->root[otherPort.node->getId()]->getId()] + bal->innerShift[otherPort.node->getId()] +
                        otherPort.port->getPos().y + otherPort.port->getAnchor().y -
                        bal->innerShift[rootPort.node->getId()] - rootPort.port->getPos().y -
                        rootPort.port->getAnchor().y;
        }

        // 为了使另一条边变直，我们不允许再移动这个块
        bal->su[bal->root[left.node->getId()]->getId()] = true;
        bal->su[bal->root[right.node->getId()]->getId()] = true;

        return threshold;
    }

    return invalid;
}

void SimpleThresholdStrategy::postProcess() {
    while (!postProcessablesQueue.empty()) {
        auto pp = postProcessablesQueue.front();
        postProcessablesQueue.pop();
        auto pick = pickEdge(pp);

        if (pick->edge == nullptr) {
            continue;
        }

        auto edge = pick->edge;

        // 忽略层内边
        bool onlyDummies = bal->od[bal->root[pp->free->getId()]->getId()];
        if (!onlyDummies && edge->isInLayerEdge()) {
            continue;
        }

        // 尝试拉直边...
        bool moved = process(pp);
        // 如果不可能，稍后在相反的迭代方向再次尝试
        if (!moved) {
            postProcessablesStack.push(pp);
        }
    }

    // 反向迭代顺序
    while (!postProcessablesStack.empty()) {
        process(postProcessablesStack.top());
        postProcessablesStack.pop();
    }
}
bool SimpleThresholdStrategy::process(std::shared_ptr<Postprocessable> &pp) {
    auto edge = pp->edge;
    EdgeTarget fix = edge->getSrc().node == pp->free ? edge->getDst() : edge->getSrc();
    EdgeTarget block = edge->getSrc().node == pp->free ? edge->getSrc() : edge->getDst();
    // t必须是不同块的根节点
    double delta = bal->calculateDelta(fix, block);

    auto nodePtr = block.node;
    if (delta > 0 && delta < THRESHOLD) {
        // 目标y大于源y --> 向上移动？
        double availableSpace = bal->checkSpaceAbove(nodePtr, delta, ni);
        bal->shiftBlock(nodePtr, -availableSpace);
        return availableSpace > 0;
    }
    if (delta < 0 && -delta < THRESHOLD) {
        // 方向向上，我们可能将一些块向上移动得太远，以至于边不能变直，所以检查是否可以再次向下移动
        double availableSpace = bal->checkSpaceBelow(nodePtr, -delta, ni);
        bal->shiftBlock(nodePtr, availableSpace);
        return availableSpace > 0;
    }

    return false;
};
}  // namespace GuiBridge