#include "NeighborhoodInformation.h"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include "../../Graph.h"
#include "../../Layer.h"

namespace GuiBridge {
std::shared_ptr<NeighborhoodInformation> NeighborhoodInformation::buildFor(const std::shared_ptr<Graph> &graph) {
    auto ni = std::make_shared<NeighborhoodInformation>();

    ni->nodeCount = 0;
    for (auto &layer : graph->getLayers()) {
        ni->nodeCount += layer->getNodes().size();
    }
    int lId = 0;
    int lIndex = 0;
    ni->layerIndex.resize(graph->getLayers().size());
    int nId = 0;
    int nIndex = 0;
    ni->nodeIndex.resize(ni->nodeCount);

    for (auto &layer : graph->getLayers()) {
        layer->id = lId++;
        ni->layerIndex[layer->id] = lIndex++;
        nIndex = 0;
        for (auto &node : layer->getNodes()) {
            node->setId(nId++);
            ni->nodeIndex[node->getId()] = nIndex++;
        }
    }
    ni->leftNeighbors.resize(ni->nodeCount);
    determineAllLeftNeighbors(ni, graph);
    ni->rightNeighbors.resize(ni->nodeCount);
    determineAllRightNeighbors(ni, graph);
    return ni;
}

void NeighborhoodInformation::cleanup() {
    layerIndex.clear();
    nodeIndex.clear();
    leftNeighbors.clear();
    rightNeighbors.clear();
}

void NeighborhoodInformation::determineAllRightNeighbors(const std::shared_ptr<NeighborhoodInformation> &ni,
                                                         const std::shared_ptr<Graph> &graph) {
    for (auto &l : graph->getLayers()) {
        for (auto &n : l->getNodes()) {
            std::vector<Neighbor> result;
            int maxPriority = 0;
            for (auto &edge : n->getOutgoingEdges()) {
                if (edge->isInLayerEdge()) {
                    continue;
                }
                Neighbor ptr = std::make_pair(edge->getDst()->getNode(), edge);
                result.emplace_back(ptr);
            }
            std::sort(result.begin(), result.end(), [ni](Neighbor &o1, Neighbor &o2) {
                return ni->nodeIndex[o1.first->getId()] < ni->nodeIndex[o2.first->getId()];
            });
            ni->rightNeighbors[n->getId()] = result;
        }
    }
}

void NeighborhoodInformation::determineAllLeftNeighbors(const std::shared_ptr<NeighborhoodInformation> &ni,
                                                        const std::shared_ptr<Graph> &graph) {
    for (auto &l : graph->getLayers()) {
        for (auto &n : l->getNodes()) {
            std::vector<Neighbor> result;
            int maxPriority = 0;
            for (auto &edge : n->getIncomingEdges()) {
                if (edge->isInLayerEdge()) {
                    continue;
                }
                Neighbor ptr = std::make_pair(edge->getSrc()->getNode(), edge);
                result.emplace_back(ptr);
            }
            std::sort(result.begin(), result.end(), [ni](Neighbor &o1, Neighbor &o2) {
                return ni->nodeIndex[o1.first->getId()] < ni->nodeIndex[o2.first->getId()];
            });
            ni->leftNeighbors[n->getId()] = result;
        }
    }
}

}  // namespace GuiBridge