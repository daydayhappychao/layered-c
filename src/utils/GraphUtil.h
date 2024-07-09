#ifndef GRAPH_UTIL_HPP
#define GRAPH_UTIL_HPP

#include <memory>

#include "../opts/PortType.h"
namespace GuiBridge {

class Port;
class Graph;
class Node;

std::shared_ptr<Port> provideCollectorPort(const std::shared_ptr<Graph> &graph,
                                           const std::shared_ptr<Node> &node, PortType type);
}  // namespace GuiBridge
#endif  // GRAPH_UTIL_HPP