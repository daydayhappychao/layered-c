#include "./GraphInfoHolder.h"
#include "../../Graph.h"
namespace GuiBridge {
GraphInfoHolder::GraphInfoHolder(const std::shared_ptr<Graph> &graph, CrossMinType crossMinType) {
    this->graph = graph;
    this->currentNodeOrder = graph->toNodeArray();

    

}
}  // namespace GuiBridge