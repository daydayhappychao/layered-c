#include "EdgeToPloyLine.h"
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <valarray>
#include <vector>
#include "../../Graph.h"
#include "../../utils/Constants.h"
#include "../../utils/VectorUtil.h"
#include "../../utils/randomUtil.h"

namespace GuiBridge {
void EdgeToPloyLine::process(std::shared_ptr<Graph> &graph) {
    layerGapPos.resize(graph->getLayers().size());
    for (int i = 0; i < graph->getLayers().size(); i++) {
        auto &layer = graph->getLayers()[i];
        for (auto &node : layer->getNodes()) {
            for (auto &port : node->getAllPorts()) {
                auto edges = node->getEdgesByPort(port);
                std::vector<std::shared_ptr<Edge>> positiveEdges;
                std::vector<std::shared_ptr<Edge>> reverseEdges;

                KVectorChain alreadyBendPoints;
                int nextX;

                // 正方向，直线，不需要 bendpoint
                if (edges.size() == 1 && (edges[0]->getSrcPoint().y == edges[0]->getDstPoint().y) &&
                    (edges[0]->getSrcPoint().x < edges[0]->getDstPoint().x)) {
                    continue;
                }

                for (auto &edge : edges) {
                    auto srcPoint = edge->getSrcPoint();
                    auto dstPoint = edge->getDstPoint();
                    if (srcPoint.x < dstPoint.x) {
                        positiveEdges.emplace_back(edge);

                    } else {
                        reverseEdges.emplace_back(edge);
                    }
                    if (!edge->getBendPoints().empty()) {
                        alreadyBendPoints = edge->getBendPoints();
                    }
                }

                if (!positiveEdges.empty() && alreadyBendPoints.empty()) {
                    nextX = layerGapPos[i].applyNewXLeft() + layer->getPos().x + layer->getSize().x;
                }

                for (auto &edge : positiveEdges) {
                    if (!edge->getBendPoints().empty()) {
                        continue;
                    }
                    auto srcPoint = edge->getSrcPoint();
                    auto dstPoint = edge->getDstPoint();
                    if (alreadyBendPoints.empty()) {
                        edge->getBendPoints().add(nextX, srcPoint.y);
                        edge->getBendPoints().add(nextX, dstPoint.y);
                    } else {
                        edge->getBendPoints().add(alreadyBendPoints[0].x, srcPoint.y);
                        edge->getBendPoints().add(alreadyBendPoints[0].x, dstPoint.y);
                    }
                }

                for (auto &edge : reverseEdges) {
                    if (!edge->getBendPoints().empty()) {
                        continue;
                    }
                    int leftX;
                    int rightX;
                    if (edge->getSrc().port == port) {
                        int leftIdx = vecIndexOf(graph->getLayers(), edge->getDst().node->getLayer()) - 1;
                        rightX = layerGapPos[i].applyNewXLeft() + layer->getPos().x + layer->getSize().x;
                        leftX = layerGapPos[leftIdx].applyNewXRight() + graph->getLayers()[leftIdx]->getPos().x +
                                graph->getLayers()[leftIdx]->getSize().x;
                    } else {
                        int leftIdx = i - 1;
                        int rightIdx = vecIndexOf(graph->getLayers(), edge->getSrc().node->getLayer());
                        leftX = layerGapPos[leftIdx].applyNewXRight() + graph->getLayers()[leftIdx]->getPos().x +
                                graph->getLayers()[leftIdx]->getSize().x;
                        rightX = layerGapPos[rightIdx].applyNewXLeft() + graph->getLayers()[rightIdx]->getPos().x +
                                 graph->getLayers()[rightIdx]->getSize().x;
                    }

                    auto startPoint = (edge->getSrcPoint());
                    startPoint.setX(leftX);
                    auto endPoint = (edge->getDstPoint());
                    endPoint.setX(rightX);
                    int y = LayerGapPosManage::applyMinCrossY(edge->getSrcPoint(), edge->getDstPoint(), graph);

                    edge->getBendPoints().add(rightX, edge->getSrcPoint().y);
                    edge->getBendPoints().add(rightX, y);
                    edge->getBendPoints().add(leftX, y);
                    edge->getBendPoints().add(leftX, edge->getDstPoint().y);
                }
            }
        }
    }

    // for (auto &edge : graph->getEdges()) {
    //     auto srcPoint = edge->getSrcPoint();
    //     auto dstPoint = edge->getDstPoint();
    //     // 第一重判断，是否正方向线
    //     if (srcPoint.x < dstPoint.x) {
    //         // 第二重判断，是否已经水平
    //         if (srcPoint.y == dstPoint.y) {
    //             continue;
    //         }

    //         float x = srcPoint.x + (dstPoint.x - srcPoint.x) / randomUtil->rangeFloat(2.0, 4.0);

    //         edge->getBendPoints().add(x, srcPoint.y);
    //         edge->getBendPoints().add(x, dstPoint.y);
    //     } else {
    //         edge->getBendPoints().add(srcPoint.x + 10, srcPoint.y);
    //         auto endPointY = std::max(edge->getSrc().node->getSize().y, edge->getDst().node->getSize().y) + 20;
    //         edge->getBendPoints().add(srcPoint.x + 10, endPointY);
    //         edge->getBendPoints().add(dstPoint.x - 10, endPointY);
    //         edge->getBendPoints().add(dstPoint.x - 10, dstPoint.y);
    //     }
    // }
}

bool LayerGapPosManage::isAvaliable(int p) {
    for (int i : leftPos) {
        if (std::abs(p - i) < LINE_GAP) {
            return false;
        }
    }
    for (int i : rightPos) {
        if (std::abs(p - i) < LINE_GAP) {
            return false;
        }
    }
    return true;
}

int LayerGapPosManage::applyNewXLeft() {
    int x = LINE_GAP;
    if (!leftPos.empty()) {
        x += leftPos.back();
    }
    leftPos.emplace_back(x);
    return x;
}

int LayerGapPosManage::applyNewXRight() {
    int x = LAYER_MARGIN - LINE_GAP;
    if (!rightPos.empty()) {
        x -= rightPos.back();
    }
    rightPos.emplace_back(x);
    return x;
}

int LayerGapPosManage::applyMinCrossY(const KVector &startPoint, const KVector &endPoint,
                                      std::shared_ptr<Graph> &graph) {
    double graphMinY = std::numeric_limits<double>::max();
    double graphMaxY = std::numeric_limits<double>::min();
    // node 位置信息，pos，size
    std::vector<std::pair<KVector, KVector>> nodesPos;
    // edge 位置信息，起点xy，终点xy
    std::vector<std::pair<KVector, KVector>> edgesPos;
    for (auto &node : graph->getNodes()) {
        nodesPos.emplace_back(node->getPos(), node->getSize());
        graphMaxY = std::max(node->getPos().y, graphMaxY);
        graphMinY = std::min(node->getPos().y, graphMinY);
    }
    for (auto &edge : graph->getEdges()) {
        // 还没处理的斜线
        if (edge->getBendPoints().empty() && (edge->getSrcPoint().x != edge->getDstPoint().x)) {
            continue;
        }
        auto lastPoint = edge->getSrcPoint();
        for (auto &bendPoint : edge->getBendPoints()) {
            edgesPos.emplace_back(lastPoint, bendPoint);
            lastPoint = bendPoint;
        }
        edgesPos.emplace_back(lastPoint, edge->getDstPoint());
    }

    int bestY;
    int bestCrossCount = std::numeric_limits<int>::max();

    for (int nextY = graphMinY - LINE_GAP; nextY < graphMaxY + LINE_GAP; nextY += LINE_GAP) {
        int crossCount;
        bool nodeIntersection = false;
        for (auto &nodePos : nodesPos) {
            if (nextY > nodePos.first.y - LINE_GAP && nextY < nodePos.first.y + nodePos.second.y + LINE_GAP) {
                nodeIntersection = true;
                nextY = nodePos.first.y + nodePos.second.y;
                break;
            }
        }

        for (auto &edge : edgesPos) {
            if (edge.first.y == edge.second.y && (std::abs(edge.first.y - nextY) < LINE_GAP)) {
                nodeIntersection = true;
                break;
            }
        }

        if (nodeIntersection) {
            continue;
        }

        std::pair<KVector, KVector> line1(startPoint, KVector(startPoint.x, nextY));
        std::pair<KVector, KVector> line2(KVector(startPoint.x, nextY), KVector(endPoint.x, nextY));
        std::pair<KVector, KVector> line3(KVector(endPoint.x, nextY), endPoint);

        for (auto &edgePos : edgesPos) {
            // y 轴平行线
            if (edgePos.first.x == edgePos.second.x) {
                if (line2.first.y > std::min(edgePos.first.y, edgePos.second.y) &&
                    line2.first.y < std::max(edgePos.first.y, edgePos.second.y) &&
                    edgePos.first.x > std::min(line2.first.x, line2.second.x) &&
                    edgePos.first.x < std::min(line2.first.x, line2.second.x)) {
                    crossCount++;
                }
            } else if (edgePos.first.y == edgePos.second.y) {
                if (line1.first.x > std::min(edgePos.first.x, edgePos.second.x) &&
                    line1.first.x < std::max(edgePos.first.x, edgePos.second.x) &&
                    edgePos.first.y > std::min(line1.first.y, line1.second.y) &&
                    edgePos.first.y < std::max(line1.first.y, line1.second.y)) {
                    crossCount++;
                }
                if (line3.first.x > std::min(edgePos.first.x, edgePos.second.x) &&
                    line3.first.x < std::max(edgePos.first.x, edgePos.second.x) &&
                    edgePos.first.y > std::min(line3.first.y, line3.second.y) &&
                    edgePos.first.y < std::max(line3.first.y, line3.second.y)) {
                    crossCount++;
                }
            }
        }
        if (crossCount < bestCrossCount) {
            bestY = nextY;
            bestCrossCount = crossCount;
        }
    }
    return bestY;
}

}  // namespace GuiBridge