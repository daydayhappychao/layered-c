#ifndef LAYER_H
#define LAYER_H

#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include "./math/KVector.h"
#include "Shape.h"
namespace GuiBridge {
class Graph;
class Node;

class Layer : public std::enable_shared_from_this<Layer>, public Shape {
private:
    std::shared_ptr<Graph> owner;
    std::vector<std::shared_ptr<Node>> nodes;
    float getMargin();

public:
    explicit Layer(std::shared_ptr<Graph> &graph);
    std::vector<std::shared_ptr<Node>> &getNodes();
    std::shared_ptr<Graph> getGraph();
    int getIndex() const;
    void adjustSize();
    void adjustLayerAndNodePosX();

    int id;
};
}  // namespace GuiBridge
#endif  // LAYER_H
