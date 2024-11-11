#include "LayerTotalPortDistributor.h"
#include "../../Node.h"
#include "../../Port.h"

namespace GuiBridge {

LayerTotalPortDistributor::LayerTotalPortDistributor(int numLayers)
    : AbstractBarycenterPortDistributor(numLayers), portRanks(numLayers, 0.0F) {}

float LayerTotalPortDistributor::calculatePortRanks(std::shared_ptr<Node> &node, float rankSum, PortType type) {
    switch (type) {
        case PortType::INPUT: {
            int inputCount = 0;
            int northInputCount = 0;

            for (const auto &port : node->getInputPorts()) {
                inputCount++;
            }

            float northPos = rankSum + northInputCount;
            float restPos = rankSum + inputCount;

            for (const auto &port : node->getInputPorts()) {
                portRanks[port->id] = restPos;
                restPos--;
            }

            return inputCount;  // The consumed rank corresponds to the number of input ports
        }

        case PortType::OUTPUT: {
            int pos = 0;
            for (const auto &port : node->getOutputPorts()) {
                pos++;
                portRanks[port->id] = rankSum + pos;
            }
            return pos;
        }

        default:
            throw std::invalid_argument("Port type is undefined");
    }
}

}  // namespace GuiBridge