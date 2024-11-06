#include "BKNodePlacer.h"
#include <map>
#include <memory>
#include <vector>
#include "../../Graph.h"
#include "BKAlignedLayout.h"
#include "BKAligner.h"
#include "BKCompactor.h"
#include "NeighborhoodInformation.h"

namespace GuiBridge {
void BKNodePlacer::process(std::shared_ptr<Graph> &layeredGraph) {
    this->lGraph = layeredGraph;

    ni = NeighborhoodInformation::buildFor(layeredGraph);

    markConflicts(layeredGraph);

    std::shared_ptr<BKAlignedLayout> rightdown;
    std::shared_ptr<BKAlignedLayout> rightup;
    std::shared_ptr<BKAlignedLayout> leftdown;
    std::shared_ptr<BKAlignedLayout> leftup;

    leftdown = std::make_shared<BKAlignedLayout>(layeredGraph, ni->getNodeCount(), VDirection::DOWN, HDirection::LEFT);
    leftup = std::make_shared<BKAlignedLayout>(layeredGraph, ni->getNodeCount(), VDirection::UP, HDirection::LEFT);
    rightdown =
        std::make_shared<BKAlignedLayout>(layeredGraph, ni->getNodeCount(), VDirection::DOWN, HDirection::RIGHT);
    rightup = std::make_shared<BKAlignedLayout>(layeredGraph, ni->getNodeCount(), VDirection::UP, HDirection::RIGHT);
    std::vector<std::shared_ptr<BKAlignedLayout>> layouts{rightdown, rightup, leftdown, leftup};

    auto aligner = std::make_shared<BKAligner>(layeredGraph, ni);
    for (auto &bal : layouts) {
        aligner->verticalAlignment(bal, markedEdges);
        aligner->insideBlockShift(bal);
    }

    auto compacter = std::make_shared<BKCompactor>(layeredGraph, ni);
    for (auto &bal : layouts) {
        compacter->horizontalCompaction(bal);
    }

    std::shared_ptr<BKAlignedLayout> chosenLayout;

    if (produceBalancedLayout) {
        auto balanced = createBalancedLayout(layouts, ni->getNodeCount());
        if (checkOrderConstraint(layeredGraph, balanced)) {
            chosenLayout = balanced;
        }
    }

    if (chosenLayout == nullptr) {
        for (auto &bal : layouts) {
            if (checkOrderConstraint(layeredGraph, bal)) {
                if (chosenLayout != nullptr || chosenLayout->layoutSize() > bal->layoutSize()) {
                    chosenLayout = bal;
                }
            }
        }
    }

    if (chosenLayout == nullptr) {
        chosenLayout = layouts[0];
    }

    for (auto &layer : layeredGraph->getLayers()) {
        for (auto &node : layer->getNodes()) {
            node->getPos().setY(chosenLayout->y[node->getId()] + chosenLayout->innerShift[node->getId()]);
        }
    }

    for (auto &bal : layouts) {
        bal->cleanup();
    }

    ni->cleanup();
    markedEdges.clear();
}

void BKNodePlacer::markConflicts(std::shared_ptr<Graph> &layeredGraph) {
    auto numberOfLayers = layeredGraph->getLayers().size();

    if (numberOfLayers < MIN_LAYERS_FOR_CONFLICTS) {
        return;
    }

    std::vector<int> layerSize(numberOfLayers);

    int layerIndex = 0;
    for (auto &layer : layeredGraph->getLayers()) {
        layerSize[layerIndex++] = layer->getNodes().size();
    }

    auto layerIterator = std::next(layeredGraph->getLayers().begin(), 2);
    for (int i = 1; i < numberOfLayers - 1; i++) {
        auto currentLayer = *layerIterator;
        auto nodeIterator = currentLayer->getNodes().begin();

        int k_0 = 0;
        int l = 0;

        for (int l_1 = 0; l_1 < layerSize[i + 1]; l_1++) {
            auto v_l_i = *nodeIterator;

            if (l_1 == (layerSize[i + 1] - 1) || incidentToInnerSegment(v_l_i, i + 1, i)) {
                int k_1 = layerSize[i] - 1;
                if (incidentToInnerSegment(v_l_i, i + 1, i)) {
                    k_1 = ni->nodeIndex[ni->leftNeighbors[v_l_i->getId()][0].first->getId()];
                }

                while (l <= l_1) {
                    auto v_l = currentLayer->getNodes()[l];

                    if (!incidentToInnerSegment(v_l, i + 1, i)) {
                        for (const auto &upperNeighbor : ni->leftNeighbors[v_l->getId()]) {
                            int k = ni->nodeIndex[upperNeighbor.first->getId()];

                            if (k < k_0 || k > k_1) {
                                markedEdges.insert(upperNeighbor.second);
                            }
                        }
                    }

                    l++;
                }

                k_0 = k_1;
            }

            nodeIterator++;
        }

        layerIterator++;
    }
}

std::shared_ptr<BKAlignedLayout> BKNodePlacer::createBalancedLayout(
    const std::vector<std::shared_ptr<BKAlignedLayout>> &layouts, int nodeCount) {
    int noOfLayouts = layouts.size();
    // c++枚举不允许传 null，看代码这里的值似乎没用，瞎传2个又何妨
    auto balanced = std::make_shared<BKAlignedLayout>(lGraph, nodeCount, VDirection::DOWN, HDirection::LEFT);
    std::vector<double> width(noOfLayouts);
    std::vector<double> min(noOfLayouts);
    std::vector<double> max(noOfLayouts);
    int minWidthLayout = 0;

    for (int i = 0; i < noOfLayouts; i++) {
        min[i] = std::numeric_limits<int>::max();
        max[i] = std::numeric_limits<int>::min();
    }

    for (int i = 0; i < noOfLayouts; i++) {
        const auto &bal = layouts[i];
        width[i] = bal->layoutSize();
        if (width[minWidthLayout] > width[i]) {
            minWidthLayout = i;
        }

        for (auto &l : lGraph->getLayers()) {
            for (auto &n : l->getNodes()) {
                double nodePosY = bal->y[n->getId()] + bal->innerShift[n->getId()];
                min[i] = std::min(min[i], nodePosY);
                max[i] = std::max(max[i], nodePosY + n->getSize().y);
            }
        }
    }

    std::vector<double> shift(noOfLayouts);
    for (int i = 0; i < noOfLayouts; i++) {
        if (layouts[i]->vdir == VDirection::DOWN) {
            shift[i] = min[minWidthLayout] - min[i];
        } else {
            shift[i] = max[minWidthLayout] - max[i];
        }
    }

    std::vector<double> calculatedYs(noOfLayouts);
    for (auto &layer : lGraph->getLayers()) {
        for (auto &node : layer->getNodes()) {
            for (int i = 0; i < noOfLayouts; i++) {
                calculatedYs[i] = layouts[i]->y[node->getId()] + layouts[i]->innerShift[node->getId()] + shift[i];
            }

            std::sort(calculatedYs.begin(), calculatedYs.end());
            balanced->y[node->getId()] = (calculatedYs[1] + calculatedYs[2]) / 2.0;
            balanced->innerShift[node->getId()] = 0.0;
        }
    }

    return balanced;
}

bool BKNodePlacer::incidentToInnerSegment(std::shared_ptr<Node> &node, int layer1, int layer2) {
    if (node->isDummy) {
        for (auto &edge : node->getIncomingEdges()) {
            auto srcNode = edge->getSrc()->getNode();
            if (srcNode->isDummy && ni->layerIndex[edge->getSrc()->getNode()->getLayer()->id] == layer2 &&
                ni->layerIndex[node->getLayer()->id] == layer1) {
                return true;
            }
        }
    }
    return false;
}

std::shared_ptr<Edge> BKNodePlacer::getEdge(std::shared_ptr<Node> &source, std::shared_ptr<Node> &target) {
    for (auto &edge : source->getEdges()) {
        if (edge->getDst()->getNode() == target || (edge->getSrc()->getNode() == target)) {
            return edge;
        }
    }
    return nullptr;
}

std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> BKNodePlacer::getBlocks(
    std::shared_ptr<BKAlignedLayout> &bal) {
    std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> blocks;
    for (auto &layer : bal->layeredGraph->getLayers()) {
        for (auto &node : layer->getNodes()) {
            auto root = bal->root[node->getId()];
            auto it = blocks.find(root);
            if (it == blocks.end()) {
                std::vector<std::shared_ptr<Node>> blockContents;
                blocks[root] = blockContents;
            }
            blocks[root].push_back(node);
        }
    }
    return blocks;
}

std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> BKNodePlacer::getClasses(
    std::shared_ptr<BKAlignedLayout> &bal) {
    std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> classes;
    std::set<std::shared_ptr<Node>> roots(bal->root.begin(), bal->root.end());
    for (const auto &root : roots) {
        std::shared_ptr<Node> sink = bal->sink[root->getId()];
        auto it = classes.find(sink);
        if (it == classes.end()) {
            std::vector<std::shared_ptr<Node>> classContents;
            classes[sink] = classContents;
        }
        classes[sink].push_back(root);
    }
    return classes;
}

bool BKNodePlacer::checkOrderConstraint(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<BKAlignedLayout> &bal) {
    bool feasible = true;
    for (auto &layer : layeredGraph->getLayers()) {
        double pos = std::numeric_limits<double>::lowest();
        std::shared_ptr<Node> previous;
        for (auto &node : layer->getNodes()) {
            double top = bal->y[node->getId()] + bal->innerShift[node->getId()] - node->margin.top;
            double bottom =
                bal->y[node->getId()] + bal->innerShift[node->getId()] + node->getSize().y + node->margin.bottom;
            if (top > pos && bottom > pos) {
                previous = node;
                pos = bal->y[node->getId()] + bal->innerShift[node->getId()] + node->getSize().y + node->margin.bottom;
            } else {
                feasible = false;
                break;
            }
        }
        if (!feasible) {
            break;
        }
    }
    return feasible;
}
}  // namespace GuiBridge