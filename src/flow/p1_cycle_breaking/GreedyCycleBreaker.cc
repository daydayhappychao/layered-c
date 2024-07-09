#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <random>
#include <vector>

#include "../../Edge.h"
#include "../../Graph.h"
#include "../../Port.h"
namespace GuiBridge {

void greedy_cycle_breaker(const std::shared_ptr<Graph> &graph) {
    auto nodes = graph->getLayerlessNodes();

    std::vector<int> indeg;
    std::vector<int> outdeg;
    std::vector<int> mark;
    std::list<std::shared_ptr<Node>> sinks;
    std::list<std::shared_ptr<Node>> sources;

    int unprocessNodeCount = nodes.size();
    int index = 0;
    for (const auto &node : nodes) {
        node->setId(index);
        for (const auto &port : node->getInputPorts()) {
            for (const auto &edge : port->getEdges()) {
                // self-loops edge
                if (edge->getSrc()->getNode() == node) {
                    continue;
                }
                if (index + 1 > indeg.size()) {
                    indeg.resize(index + 1);
                }
                indeg[index] += 1;
            }
        }

        for (const auto &port : node->getOutputPorts()) {
            if (port == nullptr) {
                continue;
            }
            for (const auto &edge : port->getEdges()) {
                // self-loops edge
                if (edge == nullptr) {
                    std::cout << "xxxxxx" << std::endl;
                    continue;
                }
                auto dst = edge->getDst();
                auto originNode = dst->getNode();
                if (originNode == node) {
                    continue;
                }
                if (index + 1 > outdeg.size()) {
                    outdeg.resize(index + 1);
                }
                outdeg[index] += 1;
            }
        }

        auto outdegSize = outdeg.size();
        if (outdegSize > index && outdeg[index] == 0) {
            sinks.push_back(node);
        } else if (indeg.size() > index && indeg[index] == 0) {
            sources.push_back(node);
        }
        index++;
    }

    int nextRight = -1;
    int nextLeft = -1;

    std::list<std::shared_ptr<Node>> maxNodes;

    auto updateNeighbors = [&](const std::shared_ptr<Node> &node) {
        for (const auto &port : node->getAllPorts()) {
            for (const auto &edge : port->getEdges()) {
                auto connectedPort = edge->getSrc() == port ? edge->getDst() : edge->getSrc();
                auto endpoint = connectedPort->getNode();

                // self-loops
                if (node == endpoint) {
                    continue;
                }
                int index = endpoint->getId();
                if (mark[index] == 0) {
                    if (edge->getDst() == connectedPort) {
                        indeg[index] -= 1;
                        if (indeg[index] <= 0 && outdeg[index] > 0) {
                            sources.push_back(endpoint);
                        }
                    } else {
                        outdeg[index] -= 1;
                        if (outdeg[index] <= 0 && indeg[index] > 0) {
                            sinks.push_back(endpoint);
                        }
                    }
                }
            }
        }
    };

    // 1. 创建随机数生成器
    std::random_device rd;   // 非确定性随机数生成器
    std::mt19937 gen(rd());  // 使用 std::random_device 初始化 Mersenne Twister 引擎

    auto chooseNodeWithMaxOutflow = [&](std::list<std::shared_ptr<Node>> nodeList) {
        std::uniform_int_distribution<> dist(0, nodeList.size() - 1);
        int randomIndex = dist(gen);
        auto it = nodeList.begin();
        std::advance(it, randomIndex);
        return *it;
    };

    while (unprocessNodeCount > 0) {
        while (!sinks.empty()) {
            auto sink = sinks.front();
            sinks.pop_front();
            mark[sink->getId()] = nextRight--;
            updateNeighbors(sink);
            unprocessNodeCount--;
        }

        while (!sources.empty()) {
            auto source = sources.front();
            sources.pop_front();
            mark[source->getId()] = nextLeft++;
            updateNeighbors(source);
            unprocessNodeCount--;
        }

        if (unprocessNodeCount > 0) {
            int maxOutflow = std::numeric_limits<int>::min();

            for (const auto &node : nodes) {
                if (mark.size() < node->getId() + 1) {
                    mark.resize(node->getId() + 1);
                }
                if (mark[node->getId()] == 0) {
                    int outflow = outdeg[node->getId()] - indeg[node->getId()];
                    if (outflow >= maxOutflow) {
                        if (outflow > maxOutflow) {
                            maxNodes.clear();
                            maxOutflow = outflow;
                        }
                        maxNodes.push_back(node);
                    }
                }
            }

            assert(maxOutflow > std::numeric_limits<int>::min());

            std::shared_ptr<Node> maxNode = chooseNodeWithMaxOutflow(maxNodes);
            mark[maxNode->getId()] = nextLeft++;
            updateNeighbors(maxNode);
            unprocessNodeCount--;
        }
    }

    int shiftBase = nodes.size() + 1;
    for (index = 0; index < nodes.size(); index++) {
        if (mark[index] < 0) {
            mark[index] += shiftBase;
        }
    }

    for (const auto &node : nodes) {
        for (const auto &port : node->getOutputPorts()) {
            for (const auto &edge : port->getEdges()) {
                int targetIx = edge->getDst()->getNode()->getId();
                if (mark[node->getId()] > mark[targetIx]) {
                    edge->reverse(graph, true);
                }
            }
        }
    }
}
}  // namespace GuiBridge