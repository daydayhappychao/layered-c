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
            node->getPorts().clear();
            node->getPorts().insert(node->getPorts().end(), portOrders[i][j].begin(), portOrders[i][j].end());
        }
    }

    for (const auto &node : updatePortOrder) {
        std::sort(node->getPorts().begin(), node->getPorts().end(), PortListSorter::CMP_COMBINED);
        node->cachePortSides();
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

std::shared_ptr<Node> SweepCopy::assertCorrectPortSides(const std::shared_ptr<Node> &dummy) {
    assert(dummy->getType() == NodeType::NORTH_SOUTH_PORT);

    auto origin = dummy->getProperty(InternalProperties::IN_LAYER_LAYOUT_UNIT);
    const auto &dummyPorts = dummy->getPorts();
    auto dummyPort = dummyPorts[0];

    for (const auto &port : origin->getPorts()) {
        if (port == dummyPort->getProperty(InternalProperties::ORIGIN)) {
            // 需要根据条件切换端口的边
            if ((port->getSide() == PortSide::NORTH) && (dummy->id > origin->id)) {
                port->setSide(PortSide::SOUTH);
                if (port->isExplicitlySuppliedPortAnchor()) {
                    double portHeight = port->getSize().y;
                    double anchorY = port->getAnchor().y;
                    port->getAnchor().y = portHeight - anchorY;
                }
            } else if ((port->getSide() == PortSide::SOUTH) && (origin->id > dummy->id)) {
                port->setSide(PortSide::NORTH);
                if (port->isExplicitlySuppliedPortAnchor()) {
                    double portHeight = port->getSize().y;
                    double anchorY = port->getAnchor().y;
                    port->getAnchor().y = -(portHeight - anchorY);
                }
            }
            break;
        }
    }
    return origin;
}
}  // namespace GuiBridge
