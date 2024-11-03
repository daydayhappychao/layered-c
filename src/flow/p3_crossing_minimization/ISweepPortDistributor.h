#ifndef ISWEEP_PORT_DISTRIBUTOR_H
#define ISWEEP_PORT_DISTRIBUTOR_H

#include <memory>
#include <random>
#include <vector>
#include "../../Node.h"
#include "../../opts/CrossMinType.h"
#include "./counting/Initializable.h"

namespace GuiBridge {

class ISweepPortDistributor : public Initializable {
public:
    virtual bool distributePortsWhileSweeping(const std::vector<std::vector<std::shared_ptr<Node>>> &order,
                                              int freeLayerIndex, bool isForwardSweep) = 0;
};

std::shared_ptr<ISweepPortDistributor> sweepPortDistributorCreate(
    CrossMinType cmt, std::mt19937 &r, std::vector<std::vector<std::shared_ptr<Node>>> &currentOrder);

}  // namespace GuiBridge

#endif  // ISWEEP_PORT_DISTRIBUTOR_H