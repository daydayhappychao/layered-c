#include "NodeRelativePortDistributor.h"
#include "../../Node.h"
#include "../../Port.h"
#include "../../opts/PortType.h"

namespace GuiBridge {

NodeRelativePortDistributor::NodeRelativePortDistributor(int numLayers)
    : AbstractBarycenterPortDistributor(numLayers), portRanks(numLayers, 0.0F) {}

float NodeRelativePortDistributor::calculatePortRanks(std::shared_ptr<Node> &node, float rankSum, PortType type) {
    switch (type) {
        case PortType::INPUT: {
            int inputCount = 0;
            int northInputCount = 0;

            for (const auto &port : node->getInputPorts()) {
                inputCount++;
            }

            float incr = 1.0F / (inputCount + 1);
            float northPos = rankSum + northInputCount * incr;
            float restPos = rankSum + 1 - incr;

            for (const auto &port : node->getInputPorts()) {
                portRanks[port->id] = restPos;
                restPos -= incr;
            }
            break;
        }

        case PortType::OUTPUT: {
            int outputCount = 0;
            for (const auto &port : node->getOutputPorts()) {
                outputCount++;
            }

            float incr = 1.0F / (outputCount + 1);
            float pos = rankSum + incr;

            for (const auto &port : node->getOutputPorts()) {
                portRanks[port->id] = pos;
                pos += incr;
            }
            break;
        }

        default:
            throw std::invalid_argument("Port type is undefined");
    }

    return 1.0F;  // The consumed rank is always 1
}

}  // namespace GuiBridge