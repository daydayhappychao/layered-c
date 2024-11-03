#include "SweepCopy.h"
#include "../../Graph.h"
#include "../../Node.h"

namespace GuiBridge {
SweepCopy::SweepCopy(const std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrderIn)
    : nodeOrder(deepCopy(nodeOrderIn)) {
    inputPortOrders.resize(nodeOrderIn.size());
    outputPortOrders.resize(nodeOrderIn.size());
    for (size_t i = 0; i < nodeOrderIn.size(); ++i) {
        for (const auto &node : nodeOrderIn[i]) {
            inputPortOrders[i].emplace_back(node->getInputPorts());
            outputPortOrders[i].emplace_back(node->getOutputPorts());
        }
    }
}

SweepCopy::SweepCopy(const SweepCopy &sc)
    : nodeOrder(deepCopy(sc.nodeOrder)), inputPortOrders(sc.inputPortOrders), outputPortOrders(sc.outputPortOrders) {}

std::vector<std::vector<std::shared_ptr<Node>>> SweepCopy::nodes() const { return nodeOrder; }

void SweepCopy::transferNodeAndPortOrdersToGraph(std::shared_ptr<Graph> &lGraph, bool setPortConstraints) {
    std::set<std::shared_ptr<Node>> updatePortOrder;

    const auto &layers = lGraph->getLayers();
    for (size_t i = 0; i < layers.size(); ++i) {
        const auto &nodes = layers[i]->getNodes();

        for (size_t j = 0; j < nodes.size(); ++j) {
            auto &node = nodeOrder[i][j];
            node->setId(j);
            layers[i]->getNodes()[j] = node;

            node->getInputPorts().clear();
            node->getInputPorts().insert(node->getInputPorts().end(), inputPortOrders[i][j].begin(),
                                         inputPortOrders[i][j].end());

            node->getOutputPorts().clear();
            node->getOutputPorts().insert(node->getOutputPorts().end(), outputPortOrders[i][j].begin(),
                                          outputPortOrders[i][j].end());
        }
    }
}

std::vector<std::vector<std::shared_ptr<Node>>> SweepCopy::deepCopy(
    const std::vector<std::vector<std::shared_ptr<Node>>> &currentlyBestNodeOrder) {
    if (currentlyBestNodeOrder.empty()) {
        return {};
    }

    std::vector<std::vector<std::shared_ptr<Node>>> result(currentlyBestNodeOrder.size());
    for (size_t i = 0; i < currentlyBestNodeOrder.size(); ++i) {
        for (const auto &node : currentlyBestNodeOrder[i]) {
            result[i].push_back(node);  // 假设这里已经使用智能指针
        }
    }
    return result;
}

}  // namespace GuiBridge
