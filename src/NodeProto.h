#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Shape.h"
#include "opts/PortType.h"
namespace GuiBridge {
class Port;
class NodeProto : public std::enable_shared_from_this<NodeProto>, public Shape {
public:
    NodeProto(std::string name, double width, double height, int id);

    std::vector<std::shared_ptr<Port>> inputPorts;
    std::vector<std::shared_ptr<Port>> outputPorts;
    std::string name;
    int id;

    void addPort(std::string portName, PortType portType);

private:
};
}  // namespace GuiBridge
