
#include <memory>
#include <optional>

#include "../Graph.h"
#include "../Port.h"
#include "../opts/PortType.h"
namespace GuiBridge {

std::shared_ptr<Port> provideCollectorPort(const std::shared_ptr<Graph> &graph, const std::shared_ptr<Node> &node,
                                           PortType type) {
    switch (type) {
        case PortType::INPUT:
            for (auto inport : node->getInputPorts()) {
                if (inport->getInternalCollect()) {
                    return inport;
                }
            }
            break;
        case PortType::OUTPUT:
            for (auto inport : node->getOutputPorts()) {
                if (inport->getInternalCollect()) {
                    return inport;
                }
            }
            break;
    }
    Port port("");
    port.setInternalCollect(true);

    std::shared_ptr<Port> ptr(&port);

    if (type == PortType::INPUT) {
        node->addInputPort(ptr);
    } else if (type == PortType::OUTPUT) {
        node->addOutputPort(ptr);
    }
    return ptr;
}
}  // namespace GuiBridge