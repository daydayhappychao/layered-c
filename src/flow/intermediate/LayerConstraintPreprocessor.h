#ifndef LAYER_CONSTRAINT_PREPROCESSOR_HPP
#define LAYER_CONSTRAINT_PREPROCESSOR_HPP

#include <memory>
namespace GuiBridge {

class Graph;
class Node;
class Edge;

class LayerConstraintPreprocessor {
public:
    void process(std::shared_ptr<Graph> &graph);
    bool isRelevantNode(std::shared_ptr<Node> &node);
    void hide(std::shared_ptr<Node> &node);
    void hide(std::shared_ptr<Node> &node, std::shared_ptr<Edge> &edge);
    void updateOppositeNodeLayerConstraints(std::shared_ptr<Node> &hiddenNode, std::shared_ptr<Node> &oppositeNode);
};

}  // namespace GuiBridge

#endif