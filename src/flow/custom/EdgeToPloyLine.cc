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
#include "../../opts/PortType.h"

namespace GuiBridge {
void EdgeToPloyLine::process(std::shared_ptr<Graph> &graph) {
    std::vector<std::shared_ptr<Edge>> reverseEdges;
    for (int i = 0; i < graph->getLayers().size(); i++) {
        auto &layer = graph->getLayers()[i];
        for (auto &node : layer->getNodes()) {
            for (auto &port : node->getAllPorts()) {
                auto edges = node->getEdgesByPort(port);
                std::vector<std::shared_ptr<Edge>> positiveEdges;

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
            }
        }
        for (auto &edge : reverseEdges) {
            if (!edge->getBendPoints().empty()) {
                continue;
            }
            int leftX;
            int rightX;
            if (edge->getSrc().node->getLayer() == layer) {
                float y = applyMinCrossY(edge->getSrcPoint(), edge->getDstPoint(), graph);
                rightX = applyMinCrossX(edge->getSrc().node->getLayer(), PortType::OUTPUT, edge->getSrcPoint().y, y);
                auto leftLayer = edge->getDst().node->getLayer()->getLeftLayer();
                leftX = applyMinCrossX(leftLayer, PortType::INPUT, edge->getDstPoint().y, y);

                auto startPoint = (edge->getSrcPoint());
                startPoint.setX(leftX);
                auto endPoint = (edge->getDstPoint());
                endPoint.setX(rightX);

                edge->getBendPoints().add(rightX, edge->getSrcPoint().y);
                edge->getBendPoints().add(rightX, y);
                edge->getBendPoints().add(leftX, y);
                edge->getBendPoints().add(leftX, edge->getDstPoint().y);
            }
        }
    }
}

float EdgeToPloyLine::applyMinCrossY(const KVector &startPoint, const KVector &endPoint,
                                     std::shared_ptr<Graph> &graph) {
    double graphMinY = std::numeric_limits<double>::max();
    double graphMaxY = std::numeric_limits<double>::min();
    // node 位置信息，pos，size
    std::vector<std::pair<KVector, KVector>> nodesPos;
    // edge 位置信息，起点xy，终点xy
    std::vector<std::pair<KVector, KVector>> edgesPos;
    for (auto &node : graph->getNodes()) {
        nodesPos.emplace_back(node->getPos(), node->getSize());
        graphMaxY = std::max(node->getPos().y + node->getSize().y, graphMaxY);
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

    float bestY = graphMinY;
    int bestCrossCount = std::numeric_limits<int>::max();

    for (float nextY = graphMinY - reverseEdgeGap * 2; nextY < graphMaxY + reverseEdgeGap * 2;
         nextY += reverseEdgeGap) {
        int crossCount;
        bool nodeIntersection = false;
        for (auto &nodePos : nodesPos) {
            if ((nextY > (nodePos.first.y - reverseEdgeGap)) &&
                (nextY < (nodePos.first.y + nodePos.second.y + reverseEdgeGap))) {
                nodeIntersection = true;
                nextY = nodePos.first.y + nodePos.second.y + reverseEdgeGap;
                break;
            }
        }
        if (nodeIntersection) {
            continue;
        }
        for (auto &edge : edgesPos) {
            if (edge.first.y == edge.second.y && (std::abs(edge.first.y - nextY) < reverseEdgeGap)) {
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

float EdgeToPloyLine::applyMinCrossX(std::shared_ptr<Layer> &layer, PortType portType, float currentY, float targetY) {
    std::vector<std::pair<KVector, KVector>> xLines;
    std::vector<std::pair<KVector, KVector>> yLines;
    auto nextLayer = layer->getRightLayer();
    float gap = reverseEdgeGap / 2;

    if (nextLayer == nullptr) {
        return layer->getPos().x + layer->getSize().x + gap;
    }

    for (auto &node : layer->getNodes()) {
        for (auto &edge : node->getEdges()) {
            if (edge->getSrc().node->getLayer() == layer && edge->getDst().node->getLayer() == nextLayer) {
                auto startPoint = edge->getSrcPoint();
                for (auto &bendPoint : edge->getBendPoints()) {
                    if (startPoint.y == bendPoint.y) {
                        xLines.emplace_back(startPoint, bendPoint);
                    } else if (startPoint.x == bendPoint.x) {
                        yLines.emplace_back(startPoint, bendPoint);
                    }
                    startPoint = bendPoint;
                }
                if (startPoint.y == edge->getDstPoint().y) {
                    xLines.emplace_back(startPoint, edge->getDstPoint());
                } else if (startPoint.x == edge->getDstPoint().x) {
                    yLines.emplace_back(startPoint, edge->getDstPoint());
                }
            }
        }
    }
    std::sort(yLines.begin(), yLines.end(), [](auto &v1, auto &v2) { return v1.first.x < v2.first.x; });

    float minX = std::numeric_limits<float>::min();
    int minCross = std::numeric_limits<int>::max();

    for (float pos = layer->getPos().x + layer->getSize().x + gap; pos < nextLayer->getPos().x - gap; pos += gap) {
        bool nodeIntersection = false;

        for (auto &yLine : yLines) {
            if (std::abs(pos - yLine.first.x) < gap) {
                pos = yLine.first.x + gap;
                nodeIntersection = true;
                break;
            }
        }
        if (nodeIntersection) {
            continue;
        }

        int nextCross = 0;
        for (auto &xLine : xLines) {
            if (xLine.first.x < pos && xLine.second.x > pos) {
                nextCross++;
            }
        }
        if (nextCross <= minCross) {
            minCross = nextCross;
            minX = pos;
        }
    }
    return minX;
};

}  // namespace GuiBridge