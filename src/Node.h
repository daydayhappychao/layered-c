

#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Edge.h"
#include "Port.h"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

class Node : public std::enable_shared_from_this<Node> {
public:
    explicit Node(std::string name);
    int getId() const;
    void setId(int id);
    void addInputPort(const std::shared_ptr<Port> &port);
    void addOutputPort(const std::shared_ptr<Port> &port);
    std::vector<std::shared_ptr<Port>> &getInputPorts();
    std::vector<std::shared_ptr<Port>> &getOutputPorts();
    std::vector<std::shared_ptr<Port>> getAllPorts();
    std::optional<int> getLayer();
    std::vector<std::shared_ptr<Edge>> getEdges();

    nlohmann::json json();

private:
    int id;
    std::string name;
    std::vector<std::shared_ptr<Port>> inputPorts;
    std::vector<std::shared_ptr<Port>> outputPorts;
    std::optional<int> layer;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};
}  // namespace GuiBridge
#endif  // NODE_HPP