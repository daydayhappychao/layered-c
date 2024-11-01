#ifndef PORT_LIST_SORTER_HPP
#define PORT_LIST_SORTER_HPP

#include <memory>
#include "../../opts/PortType.h"

namespace GuiBridge {
class Graph;
class Port;

class PortListSorter {
public:
    void process(const std::shared_ptr<Graph> &graph);

private:
    void reverseWestAndSouthSide(std::vector<std::shared_ptr<Port>> &ports);
    std::pair<int, int> findPortSideRange(const std::vector<std::shared_ptr<Port>> &ports, PortType type);
    void reverse(std::vector<std::shared_ptr<Port>> &ports, int lowIdx, int highIdx);
};
}  // namespace GuiBridge
#endif