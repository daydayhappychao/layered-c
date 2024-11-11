#include "NodeProto.h"
#include "Port.h"
#include "opts/PortType.h"

#include <cstddef>
#include <memory>
#include <utility>

namespace GuiBridge {
NodeProto::NodeProto(std::string name, double width, double height, int id) : name(std::move(name)), id(id) {
    this->size.setX(width);
    this->size.setY(height);
}

std::shared_ptr<Port> NodeProto::addPort(std::string portName, int id, PortType portType) {
    auto port = std::make_shared<Port>(portName, id, portType);
    port->nodeProto = shared_from_this();
    if (portType == PortType::INPUT) {
        inputPorts.emplace_back(port);
    } else {
        outputPorts.emplace_back(port);
    }
    updatePortPos();
    return port;
}

void NodeProto::updatePortPos() {
    auto inputGap = this->getSize().y / (inputPorts.size() + 1);
    for (size_t i = 0; i < inputPorts.size(); i++) {
        auto &inport = inputPorts[i];
        inport->getPos().setX(0.0);
        inport->getPos().setY(inputGap * (i + 1));
    }

    auto outputGap = this->getSize().y / (outputPorts.size() + 1);
    for (size_t i = 0; i < outputPorts.size(); i++) {
        auto &outport = outputPorts[i];
        outport->getPos().setX(this->getSize().x);
        outport->getPos().setY(outputGap * (i + 1));
    }
}
}  // namespace GuiBridge