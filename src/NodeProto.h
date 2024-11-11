#pragma once

#include <map>
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
    std::string name;
    int id;

    std::vector<std::shared_ptr<Port>> inputPorts;
    std::vector<std::shared_ptr<Port>> outputPorts;

    std::shared_ptr<Port> addPort(std::string portName, int id, PortType portType);

    // 更新 port 的位置，port 的 pos 属性是相对 node 的相对坐标
    void updatePortPos();

private:
};
}  // namespace GuiBridge
