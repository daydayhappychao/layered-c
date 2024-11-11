#ifndef LAYER_TOTAL_PORT_DISTRIBUTOR_H
#define LAYER_TOTAL_PORT_DISTRIBUTOR_H

#include <stdexcept>
#include <vector>
#include "AbstractBarycenterPortDistributor.h"

namespace GuiBridge {
class Node;
enum class PortType;
class LayerTotalPortDistributor : public AbstractBarycenterPortDistributor {
public:
    explicit LayerTotalPortDistributor(int numLayers);

protected:
    float calculatePortRanks(std::shared_ptr<Node> &node, float rankSum, PortType type) override;

private:
    std::vector<float> portRanks;
};

}  // namespace GuiBridge

#endif  // LAYER_TOTAL_PORT_DISTRIBUTOR_H