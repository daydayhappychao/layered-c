#ifndef LAYER_SIZE_AND_GRAPH_HEIGHT_CALCULATOR_HPP
#define LAYER_SIZE_AND_GRAPH_HEIGHT_CALCULATOR_HPP

#include <memory>
namespace GuiBridge {
class Graph;
class LayerSizeAndGraphHeightCalculator {
public:
    void process(std::shared_ptr<Graph> &graph);
};
}  // namespace GuiBridge

#endif