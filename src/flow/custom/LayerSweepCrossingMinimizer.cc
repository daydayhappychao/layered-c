#include "LayerSweepCrossingMinimizer.h"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "../../Graph.h"
#include "../../utils/VectorUtil.h"

namespace GuiBridge {

Neighbor::Neighbor(std::shared_ptr<Node> &node, std::shared_ptr<Graph> &graph) : node(node) {
    int layerIndex = 0;
    for (int i = 0; i < graph->getLayers().size(); i++) {
        if (vecInclude(graph->getLayers()[i]->getNodes(), node)) {
            layerIndex = i;
            break;
        }
    }
    if (layerIndex != 0) {
        int lastLayerIndex = layerIndex - 1;
        for (auto &incomingEdge : node->getIncomingEdges()) {
            auto other = incomingEdge->getSrc();
            // 计算重心时每个节点只算一次
            if (vecInclude(leftNeighborNodes, other.node)) {
                portNeighbor[incomingEdge->getDst().port].emplace_back(other);
                continue;
            }
            // 只处理正常线，回转线逻辑不安排在这里
            for (int i = 0; i < graph->getLayers()[lastLayerIndex]->getNodes().size(); i++) {
                if (graph->getLayers()[lastLayerIndex]->getNodes()[i] == other.node) {
                    leftNeighborNodes.emplace_back(other.node);
                    portNeighbor[incomingEdge->getDst().port].emplace_back(other);
                    barycenter += i;
                }
            }
        }
    }
    if (layerIndex != graph->getLayers().size() - 1) {
        int nextLayerIndex = layerIndex + 1;
        for (auto &outgoingEdge : node->getOutgoingEdges()) {
            auto other = outgoingEdge->getDst();
            // 只处理正常线，回转线逻辑不安排在这里
            if (vecInclude(graph->getLayers()[nextLayerIndex]->getNodes(), other.node)) {
                rightNeighborNodes.emplace_back(other.node);
                portNeighbor[outgoingEdge->getSrc().port].emplace_back(other);
            }
        }
    }
}

void LayerSweepCrossingMinimizer::process(std::shared_ptr<Graph> &graph) {
    /**
     * 1. 先确定 node 顺序
     * 2. 再确定 port 顺序
     */
    layerNodeIdPos.resize(graph->getLayers().size());
    layerLeftPortIdPos.resize(graph->getLayers().size());
    layerRightPortIdPos.resize(graph->getLayers().size());
    for (auto &node : graph->getNodes()) {
        auto neighbor = std::make_shared<Neighbor>(node, graph);
        neighborData[node] = neighbor;
    }
    sortLayerNode(graph);
    sortNodePort(graph);
}

void LayerSweepCrossingMinimizer::sortLayerNode(std::shared_ptr<Graph> &graph) {
    for (size_t i = 0; i < graph->getLayers().size(); i++) {
        if (i == 0) {
            for (auto &node : graph->getLayers()[0]->getNodes()) {
                layerNodeIdPos[0].emplace_back(node->_id);
            }
        } else {
            auto &lastLayerNodeIdPos = layerNodeIdPos[i - 1];
            std::vector<std::pair<int, int>> nodeBarycenters;
            for (auto &node : graph->getLayers()[i]->getNodes()) {
                int barycenter = 0;
                for (auto &incomingEdges : node->getIncomingEdges()) {
                    int neighborNodeIndex = vecIndexOf(lastLayerNodeIdPos, incomingEdges->getSrc().node->_id);
                    if (neighborNodeIndex != -1) {
                        barycenter += neighborNodeIndex;
                    }
                }
                nodeBarycenters.emplace_back(node->_id, barycenter);
                neighborData.at(node)->barycenter = barycenter;
            }
            std::sort(
                nodeBarycenters.begin(), nodeBarycenters.end(),
                [](const std::pair<int, int> &v1, const std::pair<int, int> &v2) { return v1.second < v2.second; });
            for (auto &barycenter : nodeBarycenters) {
                layerNodeIdPos[i].emplace_back(barycenter.first);
            }
        }
    }
    syncNodePos(graph);
}

void LayerSweepCrossingMinimizer::syncNodePos(std::shared_ptr<Graph> &graph) {
    for (int i = 0; i < graph->getLayers().size(); i++) {
        std::vector<std::shared_ptr<Node>> newNodeSort;
        for (auto id : layerNodeIdPos[i]) {
            newNodeSort.emplace_back(graph->getNodeById(id));
        }
        graph->getLayers()[i]->getNodes() = newNodeSort;
    }
}

void LayerSweepCrossingMinimizer::sortNodePort(std::shared_ptr<Graph> &graph) {
    for (int i = 0; i < graph->getLayers().size(); i++) {
        auto &layer = graph->getLayers()[i];

        for (auto &node : layer->getNodes()) {
            auto neighbor = neighborData.at(node);
            /**
             * outport 节点只需要关注他连接的 node，不需要关注具体连接的 port, port 的位置
             * 让 inport 去适配它就行
             */
            for (auto &outport : node->getOutputPorts()) {
                if (neighbor->portNeighbor.count(outport) == 0U) {
                    continue;
                }
                auto portNeighbors = neighbor->portNeighbor.at(outport);
                std::vector<std::shared_ptr<Node>> sameNodeFilter;
                for (auto &portNeighbor : portNeighbors) {
                    if (vecInclude(sameNodeFilter, portNeighbor.node)) {
                        continue;
                    }
                    sameNodeFilter.emplace_back(portNeighbor.node);
                    auto &nextLayer = graph->getLayers()[i + 1];
                    int targetBaryCenter = vecIndexOf(nextLayer->getNodes(), portNeighbor.node);
                    neighbor->portBarycenter[outport] += targetBaryCenter;
                }
            }
            for (auto &inport : node->getInputPorts()) {
                if (neighbor->portNeighbor.count(inport) == 0U) {
                    continue;
                }
                auto portNeighbors = neighbor->portNeighbor.at(inport);
                std::vector<std::shared_ptr<Node>> sameNodeFilter;
                int barcenter = 0;
                for (auto &neighbor : portNeighbors) {
                    if (!vecInclude(sameNodeFilter, neighbor.node)) {
                        sameNodeFilter.emplace_back(neighbor.node);
                        auto &lastLayer = graph->getLayers()[i - 1];

                        barcenter += vecIndexOf(lastLayer->getNodes(), neighbor.node) * 100000;
                    }
                    barcenter += neighborData[neighbor.node]->portBarycenter[neighbor.port];
                }
                neighbor->portBarycenter[inport] = barcenter;
            }
        }
    }
    for (auto &node : graph->getNodes()) {
        auto neighbor = neighborData.at(node);
        if (neighbor != nullptr) {
            std::sort(node->getProto()->inputPorts.begin(), node->getProto()->inputPorts.end(),
                      [neighbor](const std::shared_ptr<Port> &v1, const std::shared_ptr<Port> &v2) {
                          if (neighbor->portBarycenter.count(v1) == 1 && neighbor->portBarycenter.count(v2) == 1) {
                              return neighbor->portBarycenter.at(v1) < neighbor->portBarycenter.at(v2);
                          }
                          if (neighbor->portBarycenter.count(v1) == 1 || neighbor->portBarycenter.count(v2) == 1) {
                              return true;
                          }
                          return false;
                      });
            std::sort(node->getProto()->outputPorts.begin(), node->getProto()->outputPorts.end(),
                      [neighbor](const std::shared_ptr<Port> &v1, const std::shared_ptr<Port> &v2) {
                          if (neighbor->portBarycenter.count(v1) == 1 && neighbor->portBarycenter.count(v2) == 1) {
                              return neighbor->portBarycenter.at(v1) < neighbor->portBarycenter.at(v2);
                          }
                          if (neighbor->portBarycenter.count(v1) == 1 || neighbor->portBarycenter.count(v2) == 1) {
                              return true;
                          }
                          return false;
                      });
        }
    }
    graph->updateAllPortPos();
}

}  // namespace GuiBridge