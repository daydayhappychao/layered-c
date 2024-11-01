#ifndef NODE_RELATIVE_PORT_DISTRIBUTOR_H
#define NODE_RELATIVE_PORT_DISTRIBUTOR_H

#include <stdexcept>
#include <vector>
#include "AbstractBarycenterPortDistributor.h"

namespace GuiBridge {
class Node;
enum class PortType;
class NodeRelativePortDistributor : public AbstractBarycenterPortDistributor {
public:
    explicit NodeRelativePortDistributor(int numLayers);

protected:
    float calculatePortRanks(std::shared_ptr<Node> &node, float rankSum, PortType type) override;

private:
    std::vector<float> portRanks;
};

}  // namespace GuiBridge

#endif  // NODE_RELATIVE_PORT_DISTRIBUTOR_H