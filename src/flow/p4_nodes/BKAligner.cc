#include "BKAligner.h"
#include <algorithm>
#include <list>
#include <vector>
#include "../../Graph.h"
#include "./BKAlignedLayout.h"
#include "NeighborhoodInformation.h"

namespace GuiBridge {
BKAligner::BKAligner(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<NeighborhoodInformation> &ni)
    : layeredGraph(layeredGraph), ni(ni) {}

void BKAligner::verticalAlignment(BKAlignedLayout &bal, std::set<std::shared_ptr<Edge>> &markedEdges) {
    for (auto &layer : layeredGraph->getLayers()) {
        for (auto &node : layer->getNodes()) {
            bal.root[node->getId()] = node;
            bal.align[node->getId()] = node;
            bal.innerShift[node->getId()] = 0.0;
        }
    }
    auto _layers = layeredGraph->getLayers();
    auto layers = _layers;

    if (bal.hdir == HDirection::LEFT) {
        std::reverse(layers.begin(), layers.end());
    }
    for (auto &layer : layers) {
        int r = -1;
        auto nodes = layer->getNodes();
        if (bal.vdir == VDirection::UP) {
            r = std::numeric_limits<int>::max();
            std::reverse(nodes.begin(), nodes.end());
        }

        for (auto &vik : nodes) {
            std::vector<Neighbor> neighbors;
            if (bal.hdir == HDirection::LEFT) {
                neighbors = ni->rightNeighbors[vik->getId()];
            } else {
                neighbors = ni->leftNeighbors[vik->getId()];
            }
            if (neighbors.size() > 0) {
                int d = neighbors.size();
                int low = static_cast<int>(std::floor(((d + 1.0) / 2.0))) - 1;
                int high = static_cast<int>(std::ceil(((d + 1.0) / 2.0))) - 1;

                if (bal.vdir == VDirection::UP) {
                    for (int m = high; m >= low; m--) {
                        if (bal.align[vik->getId()] == vik) {
                            auto &u_m_pair = neighbors[m];
                            auto &u_m = u_m_pair.first;
                            auto it = markedEdges.find(u_m_pair.second);
                            if (it == markedEdges.end() && r > ni->nodeIndex[u_m->getId()]) {
                                bal.align[u_m->getId()] = vik;
                                bal.root[vik->getId()] = bal.root[u_m->getId()];
                                bal.align[vik->getId()] = bal.root[vik->getId()];
                                bal.od[bal.root[vik->getId()]->getId()] =
                                    bal.od[bal.root[vik->getId()]->getId()] && vik->isDummy;
                                r = ni->nodeIndex[u_m->getId()];
                            }
                        }
                    }
                } else {
                    for (int m = low; m <= high; m++) {
                        if (bal.align[vik->getId()] == vik) {
                            auto &um_pair = neighbors[m];
                            auto &um = um_pair.first;
                            auto it = markedEdges.find(um_pair.second);

                            if (it == markedEdges.end() && r < ni->nodeIndex[um->getId()]) {
                                bal.align[um->getId()] = vik;
                                bal.root[vik->getId()] = bal.root[um->getId()];
                                bal.align[vik->getId()] = bal.root[vik->getId()];
                                bal.od[bal.root[vik->getId()]->getId()] =
                                    bal.od[bal.root[vik->getId()]->getId()] && vik->isDummy;
                                r = ni->nodeIndex[um->getId()];
                            }
                        }
                    }
                }
            }
        }
    }
}

}  // namespace GuiBridge