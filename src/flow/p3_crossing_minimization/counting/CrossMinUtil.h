#ifndef CROSS_MIN_UTIL_HPP
#define CROSS_MIN_UTIL_HPP

#include <algorithm>
#include <memory>
#include <vector>
#include "../../../Node.h"
#include "../../../Port.h"
#include "../../../opts/PortType.h"
namespace GuiBridge {
std::vector<std::shared_ptr<Port>> inNorthSouthEastWestOrder(std::shared_ptr<Node> &node, PortType type);
};  // namespace GuiBridge

#endif