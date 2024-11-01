#ifndef CROSS_MIN_UTIL_HPP
#define CROSS_MIN_UTIL_HPP

#include <algorithm>
#include <memory>
#include <vector>
#include "../../../Node.h"
#include "../../../Port.h"
#include "../../../opts/PortType.h"
namespace GuiBridge {
std::vector<std::shared_ptr<Port>> inNorthSouthEastWestOrder(std::shared_ptr<Node> &node, PortType type) {
    auto inputPortsRef = node->getInputPorts();
    auto outputPortsRef = node->getOutputPorts();
    std::vector<std::shared_ptr<Port>> inputPorts = inputPortsRef;
    std::vector<std::shared_ptr<Port>> outputPorts = outputPortsRef;
    switch (type) {
        case PortType::INPUT:
            std::reverse(inputPorts.begin(), inputPorts.end());
            return inputPorts;
        case PortType::OUTPUT:
            return outputPorts;
    }
}
};  // namespace GuiBridge

#endif