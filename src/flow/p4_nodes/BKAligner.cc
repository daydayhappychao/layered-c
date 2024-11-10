#include "BKAligner.h"
#include <algorithm>
#include <list>
#include <memory>
#include <vector>
#include "../../Graph.h"
#include "./BKAlignedLayout.h"
#include "BKNodePlacer.h"
#include "NeighborhoodInformation.h"

namespace GuiBridge {
BKAligner::BKAligner(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<NeighborhoodInformation> &ni)
    : layeredGraph(layeredGraph), ni(ni) {}

void BKAligner::verticalAlignment(std::shared_ptr<BKAlignedLayout> &bal, std::set<std::shared_ptr<Edge>> &markedEdges) {
    for (auto &layer : layeredGraph->getLayers()) {
        for (auto &node : layer->getNodes()) {
            bal->root[node->getId()] = node;
            bal->align[node->getId()] = node;
            bal->innerShift[node->getId()] = 0.0;
        }
    }
    auto _layers = layeredGraph->getLayers();
    auto layers = _layers;

    if (bal->hdir == HDirection::LEFT) {
        std::reverse(layers.begin(), layers.end());
    }
    for (auto &layer : layers) {
        int r = -1;
        auto nodes = layer->getNodes();
        if (bal->vdir == VDirection::UP) {
            r = std::numeric_limits<int>::max();
            std::reverse(nodes.begin(), nodes.end());
        }

        for (auto &vik : nodes) {
            std::vector<Neighbor> neighbors;
            if (bal->hdir == HDirection::LEFT) {
                neighbors = ni->rightNeighbors[vik->getId()];
            } else {
                neighbors = ni->leftNeighbors[vik->getId()];
            }
            if (!neighbors.empty()) {
                int d = neighbors.size();
                int low = static_cast<int>(std::floor(((d + 1.0) / 2.0))) - 1;
                int high = static_cast<int>(std::ceil(((d + 1.0) / 2.0))) - 1;

                if (bal->vdir == VDirection::UP) {
                    for (int m = high; m >= low; m--) {
                        if (bal->align[vik->getId()] == vik) {
                            auto &u_m_pair = neighbors[m];
                            auto &u_m = u_m_pair.first;
                            auto it = markedEdges.find(u_m_pair.second);
                            if (it == markedEdges.end() && r > ni->nodeIndex[u_m->getId()]) {
                                bal->align[u_m->getId()] = vik;
                                bal->root[vik->getId()] = bal->root[u_m->getId()];
                                bal->align[vik->getId()] = bal->root[vik->getId()];
                                bal->od[bal->root[vik->getId()]->getId()] =
                                    bal->od[bal->root[vik->getId()]->getId()] && vik->isDummy;
                                r = ni->nodeIndex[u_m->getId()];
                            }
                        }
                    }
                } else {
                    for (int m = low; m <= high; m++) {
                        if (bal->align[vik->getId()] == vik) {
                            auto &um_pair = neighbors[m];
                            auto &um = um_pair.first;
                            auto it = markedEdges.find(um_pair.second);

                            if (it == markedEdges.end() && r < ni->nodeIndex[um->getId()]) {
                                bal->align[um->getId()] = vik;
                                bal->root[vik->getId()] = bal->root[um->getId()];
                                bal->align[vik->getId()] = bal->root[vik->getId()];
                                bal->od[bal->root[vik->getId()]->getId()] =
                                    bal->od[bal->root[vik->getId()]->getId()] && vik->isDummy;
                                r = ni->nodeIndex[um->getId()];
                            }
                        }
                    }
                }
            }
        }
    }
}

void BKAligner::insideBlockShift(std::shared_ptr<BKAlignedLayout> &bal) {
    auto blocks = BKNodePlacer::getBlocks(bal);
    for (auto &block : blocks) {
        const auto &root = block.first;
        double spaceAbove = root->margin.top;
        double spaceBelow = root->getSize().y + root->margin.bottom;
        bal->innerShift[root->getId()] = 0.0;

        auto current = root;
        std::shared_ptr<Node> next;
        while ((next = bal->align[current->getId()]) != root) {
            auto edge = BKNodePlacer::getEdge(current, next);

            double portPosDiff = 0.0;
            if (bal->hdir == HDirection::LEFT) {
                portPosDiff = edge->getDst().port->getPos().y + edge->getDst().port->getAnchor().y -
                              edge->getSrc().port->getPos().y - edge->getSrc().port->getAnchor().y;
            } else {
                portPosDiff = edge->getSrc().port->getPos().y + edge->getSrc().port->getAnchor().y -
                              edge->getDst().port->getPos().y - edge->getDst().port->getAnchor().y;
            }

            double nextInnerShift = bal->innerShift[current->getId()] + portPosDiff;
            bal->innerShift[next->getId()] = nextInnerShift;

            spaceAbove = std::max(spaceAbove, next->margin.top - nextInnerShift);
            spaceBelow = std::max(spaceBelow, nextInnerShift + next->getSize().y + next->margin.bottom);

            current = next;
        }

        current = root;
        do {
            bal->innerShift[current->getId()] = bal->innerShift[current->getId()] + spaceAbove;
            current = bal->align[current->getId()];

        } while (current != root);

        bal->blockSize[root->getId()] = spaceAbove + spaceBelow;
    }
};

}  // namespace GuiBridge