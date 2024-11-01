#include "./PortListSorter.h"
#include <algorithm>
#include <memory>
#include "../../Graph.h"
#include "../../Port.h"

namespace GuiBridge {
void PortListSorter::process(const std::shared_ptr<Graph> &graph) {
    for (const auto &layer : graph->getLayers()) {
        for (const auto &node : layer->getNodes()) {
            auto &inputPorts = node->getInputPorts();
            auto &outputPorts = node->getOutputPorts();
        }
    }
}
}  // namespace GuiBridge