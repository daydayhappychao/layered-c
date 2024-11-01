#ifndef LAYER_CONSTRAINT_POSTPROCESSOR_HPP
#define LAYER_CONSTRAINT_POSTPROCESSOR_HPP

#include <memory>
namespace GuiBridge {

class Graph;
class Node;
class Edge;
class Layer;

class LayerConstraintPostprocessor {
public:
    void process(std::shared_ptr<Graph> &graph);

    void moveFirstAndLastNodes(std::shared_ptr<Graph> &graph, const std::shared_ptr<Layer> &firstLayer,
                               const std::shared_ptr<Layer> &lastLayer);

    void restoreHiddenNodes(std::shared_ptr<Graph> &graph, const std::shared_ptr<Layer> &firstSeparateLayer,
                            const std::shared_ptr<Layer> &lastSeparateLayer);
};

}  // namespace GuiBridge

#endif