
#include <algorithm>
#include <deque>
#include <memory>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../../Graph.h"
#include "../../../utils/VectorUtil.h"

#include "./NetworkSimplex.h"

namespace GuiBridge {
NetworkSimplex::NetworkSimplex(std::shared_ptr<Graph> graphPtr, int limit)
    : graph(std::move(graphPtr)), iterationLimit(limit) {}
NetworkSimplex::~NetworkSimplex() = default;

void NetworkSimplex::execute() {
    if (graph->getLayerlessNodes().empty()) {
        return;
    }

    for (const auto &node : graph->getLayerlessNodes()) {
        node->layerIndex = 0;
    }

    // bool removeSubtrees = graph->getLayerlessNodes().size() >= REMOVE_SUBTREES_THRESH;

    // if (removeSubtrees) {
    //     this->removeSubtrees();
    // }

    initialize();
    feasibleTree();
    auto e = leaveEdge();
    int iter = 0;
    while (e != nullptr && iter < iterationLimit) {
        exchange(e, enterEdge(e));
        e = leaveEdge();
        iter++;
    }
    balance(normalize());
};

void NetworkSimplex::removeSubtrees() {
    std::queue<std::shared_ptr<Node>> leafs;
    for (const auto &node : graph->getLayerlessNodes()) {
        if (node->getEdges().size() == 1) {
            leafs.push(node);
        }
    }

    while (!leafs.empty()) {
        std::shared_ptr<Node> node = leafs.front();
        leafs.pop();
        if (node->getEdges().empty()) {
            continue;
        }
        std::shared_ptr<Edge> edge = node->getEdges()[0];
        bool isOutEdge = !node->getOutgoingEdges().empty();
        std::shared_ptr<Node> other = edge->getOther(node).node;

        // if (isOutEdge) {
        //     edge->setOppositePort(OppositeType::Dst);
        // } else {
        //     edge->setOppositePort(OppositeType::Src);
        // }

        edge->hidden();

        if (other->getEdges().size() == 1) {
            leafs.push(other);
        }

        subtreeNodesStack.emplace_back(node, edge);

        auto newEnd = std::remove(graph->getLayerlessNodes().begin(), graph->getLayerlessNodes().end(), node);
        graph->getLayerlessNodes().erase(newEnd, graph->getLayerlessNodes().end());
    }
}

void NetworkSimplex::initialize() {
    int numNodes = graph->getLayerlessNodes().size();

    for (const auto &node : graph->getLayerlessNodes()) {
        node->treeNode = false;
    }

    this->poID.resize(numNodes);
    this->lowestPoID.resize(numNodes);

    int index = 0;

    std::vector<std::shared_ptr<Edge>> theEdges;

    for (const auto &node : graph->getLayerlessNodes()) {
        node->internalId = index++;
        if (node->getIncomingEdges().empty()) {
            sources.push_back(node);
        }
        auto outgoingEdges = node->getOutgoingEdges();
        if (!outgoingEdges.empty()) {
            theEdges.insert(theEdges.end(), outgoingEdges.begin(), outgoingEdges.end());
        }
    }

    int counter = 0;

    for (const auto &edge : theEdges) {
        edge->internalId = counter++;
        edge->treeEdge = false;
    }

    int numEdges = theEdges.size();

    if (cutvalue.empty() || cutvalue.size() < numEdges) {
        cutvalue.resize(numEdges);
        edgeVisited.resize(numEdges);
    } else {
        std::fill(edgeVisited.begin(), edgeVisited.end(), false);
    }
    edges = theEdges;

    treeEdges.reserve(edges.size());
    postOrder = 1;
}

void NetworkSimplex::feasibleTree() {
    // 根据有向无环图的调用顺序完成一次 layer
    layeringTopologicalNumbering();

    if (!edges.empty()) {
        std::fill(edgeVisited.begin(), edgeVisited.end(), false);
        while (tightTreeDFS(graph->getLayerlessNodes().front()) < graph->getLayerlessNodes().size()) {
            auto e = minimalSlack();
            int slack = e->getDst().node->layerIndex - e->getSrc().node->layerIndex - e->delta;

            if (e->getDst().node->treeNode) {
                slack = -slack;
            }
            for (const auto &node : graph->getLayerlessNodes()) {
                if (node->treeNode) {
                    node->layerIndex += slack;
                }
            }
            std::fill(edgeVisited.begin(), edgeVisited.end(), false);
        }
        std::fill(edgeVisited.begin(), edgeVisited.end(), false);
        postorderTraversal(graph->getLayerlessNodes().front());
        cutvalues();
    }
}

void NetworkSimplex::layeringTopologicalNumbering() {
    std::vector<int> incident(graph->getLayerlessNodes().size(), 0);

    for (const auto &node : graph->getLayerlessNodes()) {
        incident[node->internalId] += node->getIncomingEdges().size();
    }

    std::queue<std::shared_ptr<Node>> roots;
    for (const auto &node : sources) {
        roots.push(node);
    }

    while (!roots.empty()) {
        auto node = roots.front();
        roots.pop();

        for (auto const &edge : node->getOutgoingEdges()) {
            auto target = edge->getDst().node;
            target->layerIndex = std::max(target->layerIndex, node->layerIndex + edge->delta);
            incident[target->internalId]--;
            if (incident[target->internalId] == 0) {
                roots.push(target);
            }
        }
    }
}

int NetworkSimplex::tightTreeDFS(const std::shared_ptr<Node> &node) {
    int nodeCount = 1;
    node->treeNode = true;
    std::shared_ptr<Node> opposite;

    for (const auto &edge : node->getEdges()) {
        if (!edgeVisited[edge->internalId]) {
            edgeVisited[edge->internalId] = true;
            opposite = edge->getOther(node).node;
            if (edge->treeEdge) {
                nodeCount += tightTreeDFS(opposite);
            } else if (!opposite->treeNode &&
                       edge->delta == edge->getDst().node->layerIndex - edge->getSrc().node->layerIndex) {
                edge->treeEdge = true;
                treeEdges.insert(edge);
                nodeCount += tightTreeDFS(opposite);
            }
        }
    }
    return nodeCount;
}

int NetworkSimplex::postorderTraversal(const std::shared_ptr<Node> &node) {
    int lowest = std::numeric_limits<int>::max();
    for (const auto &edge : node->getEdges()) {
        if (edge->treeEdge && !edgeVisited[edge->internalId]) {
            edgeVisited[edge->internalId] = true;
            lowest = std::min(lowest, postorderTraversal(edge->getOther(node).node));
        }
    }
    poID[node->internalId] = postOrder;
    lowestPoID[node->internalId] = std::min(lowest, postOrder++);
    return lowestPoID[node->internalId];
}

void NetworkSimplex::cutvalues() {
    std::vector<std::shared_ptr<Node>> leafs;
    int treeEdgeCount;
    for (const auto &node : graph->getLayerlessNodes()) {
        treeEdgeCount = 0;
        node->unknownCutvalues.clear();
        for (const auto &edge : node->getEdges()) {
            if (edge->treeEdge) {
                node->unknownCutvalues.push_back(edge);
                treeEdgeCount++;
            }
        }
        if (treeEdgeCount == 1) {
            leafs.push_back(node);
        }
    }

    std::shared_ptr<Edge> toDetermine;
    std::shared_ptr<Node> source;
    std::shared_ptr<Node> target;

    for (auto node : leafs) {
        while (node->unknownCutvalues.size() == 1) {
            toDetermine = node->unknownCutvalues.front();
            cutvalue[toDetermine->internalId] = toDetermine->weight;
            source = toDetermine->getSrc().node;
            target = toDetermine->getDst().node;
            for (const auto &edge : node->getEdges()) {
                if (edge != toDetermine) {
                    if (edge->treeEdge) {
                        if (source == edge->getSrc().node || target == edge->getDst().node) {
                            cutvalue[toDetermine->internalId] -= cutvalue[edge->internalId] - edge->weight;
                        } else {
                            cutvalue[toDetermine->internalId] += cutvalue[edge->internalId] - edge->weight;
                        }
                    } else {
                        if (node == source) {
                            if (edge->getSrc().node == node) {
                                cutvalue[toDetermine->internalId] += edge->weight;
                            } else {
                                cutvalue[toDetermine->internalId] -= edge->weight;
                            }
                        } else {
                            if (edge->getSrc().node == node) {
                                cutvalue[toDetermine->internalId] -= edge->weight;
                            } else {
                                cutvalue[toDetermine->internalId] += edge->weight;
                            }
                        }
                    }
                }
            }
            vecRemove(source->unknownCutvalues, toDetermine);
            vecRemove(target->unknownCutvalues, toDetermine);

            if (source == node) {
                node = toDetermine->getDst().node;
            } else {
                node = toDetermine->getSrc().node;
            }
        }
    }
}

std::shared_ptr<Edge> NetworkSimplex::leaveEdge() {
    for (const auto &edge : treeEdges) {
        if (edge->treeEdge && cutvalue[edge->internalId] < -1e-10) {
            return edge;
        }
    }
    return nullptr;
}

void NetworkSimplex::exchange(const std::shared_ptr<Edge> &leave, const std::shared_ptr<Edge> &enter) {
    if (!leave->treeEdge) {
        throw std::runtime_error("Given leave edge is no tree edge.");
    }
    if (enter->treeEdge) {
        throw std::runtime_error("Given enter edge is a tree edge already.");
    }

    leave->treeEdge = false;
    treeEdges.erase(leave);
    enter->treeEdge = true;
    treeEdges.insert(enter);

    int delta = enter->getDst().node->layerIndex - enter->getSrc().node->layerIndex - enter->delta;
    if (!isInHead(enter->getDst().node, leave)) {
        delta = -delta;
    }
    for (const auto &node : graph->getLayerlessNodes()) {
        if (!isInHead(node, leave)) {
            node->layerIndex += delta;
        }
    }

    postOrder = 1;
    std::fill(edgeVisited.begin(), edgeVisited.end(), false);
    postorderTraversal(graph->getLayerlessNodes().front());
    cutvalues();
}

bool NetworkSimplex::isInHead(const std::shared_ptr<Node> &node, const std::shared_ptr<Edge> &edge) {
    auto source = edge->getSrc().node;
    auto target = edge->getDst().node;

    if (lowestPoID[source->internalId] <= poID[node->internalId] &&
        poID[node->internalId] <= poID[source->internalId] &&
        lowestPoID[target->internalId] <= poID[node->internalId] && poID[node->internalId] <= poID[target->internalId]

    ) {
        return poID[source->internalId] >= poID[target->internalId];
    }

    return poID[source->internalId] < poID[target->internalId];
}

std::shared_ptr<Edge> NetworkSimplex::enterEdge(const std::shared_ptr<Edge> &leave) {
    std::shared_ptr<Edge> replace;
    int repSlack = std::numeric_limits<int>::max();
    int slack;
    std::shared_ptr<Node> source;
    std::shared_ptr<Node> target;
    for (const auto &edge : edges) {
        source = edge->getSrc().node;
        target = edge->getDst().node;
        if (isInHead(source, leave) && !isInHead(target, leave)) {
            slack = target->layerIndex - source->layerIndex - edge->delta;
            if (slack < repSlack) {
                repSlack = slack;
                replace = edge;
            }
        }
    }
    return replace;
}

std::vector<int> NetworkSimplex::normalize() {
    int highest = std::numeric_limits<int>::min();
    int lowest = std::numeric_limits<int>::max();
    for (const auto &node : graph->getLayerlessNodes()) {
        lowest = std::min(lowest, node->layerIndex);
        highest = std::max(highest, node->layerIndex);
    }
    std::vector<int> filling(highest - lowest + 1);
    for (const auto &node : graph->getLayerlessNodes()) {
        node->layerIndex -= lowest;
        filling[node->layerIndex]++;
    }

    return filling;
}

void NetworkSimplex::balance(std::vector<int> filling) {
    int newLayer;

    std::pair<int, int> range;

    for (const auto &node : graph->getLayerlessNodes()) {
        if (node->getIncomingEdges().size() == node->getOutgoingEdges().size()) {
            newLayer = node->layerIndex;
            range = minimalSpan(node);
            for (int i = node->layerIndex - range.first + 1; i < node->layerIndex + range.second; i++) {
                if (filling[i] < filling[newLayer]) {
                    newLayer = i;
                }
            }

            if (filling[newLayer] < filling[node->layerIndex]) {
                filling[node->layerIndex]--;
                filling[newLayer]++;
                node->layerIndex = newLayer;
            }
        }
    }
}

std::pair<int, int> NetworkSimplex::minimalSpan(const std::shared_ptr<Node> &node) {
    int minSpanOut = std::numeric_limits<int>::max();
    int minSpanIn = std::numeric_limits<int>::max();
    int currentSpan;

    for (const auto &edge : node->getEdges()) {
        currentSpan = edge->getDst().node->layerIndex - edge->getSrc().node->layerIndex;
        if (edge->getDst().node == node && currentSpan < minSpanIn) {
            minSpanIn = currentSpan;
        } else if (currentSpan < minSpanOut) {
            minSpanOut = currentSpan;
        }
    }

    if (minSpanIn == std::numeric_limits<int>::max()) {
        minSpanIn = -1;
    }
    if (minSpanOut == std::numeric_limits<int>::max()) {
        minSpanOut = -1;
    }

    auto resData = std::pair<int, int>(minSpanIn, minSpanOut);

    return resData;
}

std::shared_ptr<Edge> NetworkSimplex::minimalSlack() {
    int minSlack = std::numeric_limits<int>::max();
    std::shared_ptr<Edge> minSlackEdge;
    int curSlack;

    for (const auto &edge : edges) {
        if (edge->getSrc().node->treeNode ^ edge->getDst().node->treeNode) {
            curSlack = edge->getDst().node->layerIndex - edge->getSrc().node->layerIndex - edge->delta;
            if (curSlack < minSlack) {
                minSlack = curSlack;
                minSlackEdge = edge;
            }
        }
    }
    return minSlackEdge;
}

}  // namespace GuiBridge