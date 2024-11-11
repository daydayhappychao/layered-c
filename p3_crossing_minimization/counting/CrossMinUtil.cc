#include "CrossMinUtil.h"

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
};
};  // namespace GuiBridge