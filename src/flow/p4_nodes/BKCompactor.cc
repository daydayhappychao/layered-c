#include "BKCompactor.h"
#include <memory>
#include "../../Graph.h"
#include "BKAlignedLayout.h"
#include "NeighborhoodInformation.h"
#include "ThresholdStrategy.h"

namespace GuiBridge {
void ClassNode::addEdge(std::shared_ptr<ClassNode> &target, double separation) {
    auto se = std::make_shared<ClassEdge>();
    se->target = target;
    se->separation = separation;
    target->indegree++;
    outgoing.push_back(se);
}

BKCompactor::BKCompactor(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<NeighborhoodInformation> &ni)
    : layeredGraph(layeredGraph), ni(ni) {
    threshStrategy = std::make_shared<SimpleThresholdStrategy>();
}

void BKCompactor::horizontalCompaction(std::shared_ptr<BKAlignedLayout> &bal) {
    for (const auto &layer : layeredGraph->getLayers()) {
        for (const auto &node : layer->getNodes()) {
            bal->sink[node->getId()] = node;
            if (bal->vdir == VDirection::UP) {
                bal->shift[node->getId()] = std::numeric_limits<double>::lowest();
            } else {
                bal->shift[node->getId()] = std::numeric_limits<double>::max();
            }
        }
    }
    sinkNodes.clear();

    std::vector<std::shared_ptr<Layer>> layers = layeredGraph->getLayers();
    if (bal->hdir == HDirection::LEFT) {
        std::reverse(layers.begin(), layers.end());
    }

    threshStrategy->init(bal, ni);
    for (double &i : bal->y) {
        i = 0.0;
    }

    for (const auto &layer : layers) {
        std::vector<std::shared_ptr<Node>> nodes = layer->getNodes();
        if (bal->vdir == VDirection::UP) {
            std::reverse(nodes.begin(), nodes.end());
        }

        for (auto &v : nodes) {
            if (v == bal->root[v->getId()]) {
                placeBlock(v, bal);
            }
        }
    }

    placeClasses(bal);

    for (const auto &layer : layers) {
        for (const auto &v : layer->getNodes()) {
            bal->y[v->getId()] = bal->y[bal->root[v->getId()]->getId()];
            if (v == bal->root[v->getId()]) {
                double sinkShift = bal->shift[bal->sink[v->getId()]->getId()];
                if ((bal->vdir == VDirection::UP && sinkShift > std::numeric_limits<double>::lowest()) ||
                    (bal->vdir == VDirection::DOWN && sinkShift < std::numeric_limits<double>::max())) {
                    bal->y[v->getId()] += sinkShift;
                }
            }
        }
    }

    threshStrategy->postProcess();
}

void BKCompactor::placeBlock(std::shared_ptr<Node> &root, std::shared_ptr<BKAlignedLayout> &bal) {
    if (bal->y[root->getId()] != 0.0) {
        return;
    }

    bool isInitialAssignment = true;
    bal->y[root->getId()] = 0.0;

    std::shared_ptr<Node> currentNode = root;
    double thresh =
        (bal->vdir == VDirection::DOWN) ? std::numeric_limits<double>::lowest() : std::numeric_limits<double>::max();
    do {
        int currentIndexInLayer = ni->nodeIndex[currentNode->getId()];
        int currentLayerSize = currentNode->getLayer()->getNodes().size();

        if ((bal->vdir == VDirection::DOWN && currentIndexInLayer > 0) ||
            (bal->vdir == VDirection::UP && currentIndexInLayer < currentLayerSize - 1)) {
            std::shared_ptr<Node> neighbor;
            std::shared_ptr<Node> neighborRoot;
            if (bal->vdir == VDirection::UP) {
                neighbor = currentNode->getLayer()->getNodes()[currentIndexInLayer + 1];
            } else {
                neighbor = currentNode->getLayer()->getNodes()[currentIndexInLayer - 1];
            }
            neighborRoot = bal->root[neighbor->getId()];

            placeBlock(neighborRoot, bal);

            thresh = threshStrategy->calculateThreshold(thresh, root, currentNode);

            if (bal->sink[root->getId()] == root) {
                bal->sink[root->getId()] = bal->sink[neighborRoot->getId()];
            }

            if (bal->sink[root->getId()] == bal->sink[neighborRoot->getId()]) {
                /**
                 * @todo
                 * 这里是瞎填的
                 */
                double spacing = 10;
                if (bal->vdir == VDirection::UP) {
                    double currentBlockPosition = bal->y[root->getId()];
                    double newPosition = bal->y[neighborRoot->getId()] + bal->innerShift[neighbor->getId()] -
                                         neighbor->margin.top - spacing - currentNode->margin.bottom -
                                         currentNode->getSize().y - bal->innerShift[currentNode->getId()];
                    if (isInitialAssignment) {
                        isInitialAssignment = false;
                        bal->y[root->getId()] = std::min(newPosition, thresh);
                    } else {
                        bal->y[root->getId()] = std::min(currentBlockPosition, std::min(newPosition, thresh));
                    }
                } else {
                    double currentBlockPosition = bal->y[root->getId()];
                    double newPosition = bal->y[neighborRoot->getId()] + bal->innerShift[neighbor->getId()] +
                                         neighbor->getSize().y + neighbor->margin.bottom + spacing +
                                         currentNode->margin.top - bal->innerShift[currentNode->getId()];
                    if (isInitialAssignment) {
                        isInitialAssignment = false;
                        bal->y[root->getId()] = std::max(newPosition, thresh);
                    } else {
                        bal->y[root->getId()] = std::max(currentBlockPosition, std::max(newPosition, thresh));
                    }
                }
            } else {
                /**
                 * @todo
                 * 这里是瞎填的
                 */
                double spacing = 10;
                std::shared_ptr<ClassNode> sinkNode = getOrCreateClassNode(bal->sink[root->getId()], bal);
                std::shared_ptr<ClassNode> neighborSink = getOrCreateClassNode(bal->sink[neighborRoot->getId()], bal);
                if (bal->vdir == VDirection::UP) {
                    double requiredSpace =
                        bal->y[root->getId()] + bal->innerShift[currentNode->getId()] + currentNode->getSize().y +
                        currentNode->margin.bottom + spacing -
                        (bal->y[neighborRoot->getId()] + bal->innerShift[neighbor->getId()] - neighbor->margin.top);
                    sinkNode->addEdge(neighborSink, requiredSpace);
                } else {
                    double requiredSpace = bal->y[root->getId()] + bal->innerShift[currentNode->getId()] -
                                           currentNode->margin.top - bal->y[neighborRoot->getId()] -
                                           bal->innerShift[neighbor->getId()] - neighbor->getSize().y -
                                           neighbor->margin.bottom - spacing;
                    sinkNode->addEdge(neighborSink, requiredSpace);
                }
            }
        } else {
            thresh = threshStrategy->calculateThreshold(thresh, root, currentNode);
        }

        currentNode = bal->align[currentNode->getId()];
    } while (currentNode != root);

    threshStrategy->finishBlock(root);
}

void BKCompactor::placeClasses(std::shared_ptr<BKAlignedLayout> &bal) {
    std::queue<std::shared_ptr<ClassNode>> sinks;
    for (const auto &it : sinkNodes) {
        if (it.second->indegree == 0) {
            sinks.push(it.second);
        }
    }

    while (!sinks.empty()) {
        std::shared_ptr<ClassNode> n = sinks.front();
        sinks.pop();

        n->classShift = 0.0;

        for (const auto &e : n->outgoing) {
            if (e->target->classShift == 0.0) {
                e->target->classShift = n->classShift + e->separation;
            } else if (bal->vdir == VDirection::DOWN) {
                e->target->classShift = std::min(e->target->classShift, n->classShift + e->separation);
            } else {
                e->target->classShift = std::max(e->target->classShift, n->classShift + e->separation);
            }
            e->target->indegree--;
            if (e->target->indegree == 0) {
                sinks.push(e->target);
            }
        }
    }

    for (const auto &it : sinkNodes) {
        bal->shift[it.first->getId()] = it.second->classShift;
    }
}

std::shared_ptr<ClassNode> BKCompactor::getOrCreateClassNode(std::shared_ptr<Node> &sinkNode,
                                                             std::shared_ptr<BKAlignedLayout> &bal) {
    auto it = sinkNodes.find(sinkNode);
    if (it == sinkNodes.end()) {
        std::shared_ptr<ClassNode> node = std::make_shared<ClassNode>();
        node->node = sinkNode;
        sinkNodes[sinkNode] = node;
        return node;
    }
    return it->second;
}
}  // namespace GuiBridge