#ifndef LAYER_SWEEP_CROSSING_MINIMIZER
#define LAYER_SWEEP_CROSSING_MINIMIZER
#include <memory>
namespace GuiBridge {
class Graph;

class LayerSweepCrossingMinimizer {
public:
    void process(const std::shared_ptr<Graph> &graph);
    void compareDifferentRandomizedLayouts();
};
}  // namespace GuiBridge
#endif