#include "NodeProto.h"
#include "Port.h"
#include "opts/PortType.h"

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
    return port;
}
}  // namespace GuiBridge