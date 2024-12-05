#ifndef LAYER_H
#define LAYER_H

#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include "Shape.h"
namespace GuiBridge {
class Graph;
class Node;

class Layer : public std::enable_shared_from_this<Layer>, public Shape {
private:
    std::shared_ptr<Graph> owner;
    std::vector<std::shared_ptr<Node>> nodes;
    float margin = 50.0F;

public:
    float getMargin();
    void setMargin(float v);
    explicit Layer(std::shared_ptr<Graph> &graph);
    std::vector<std::shared_ptr<Node>> &getNodes();
    std::shared_ptr<Graph> getGraph();
    int getIndex() const;
    std::shared_ptr<Layer> getLeftLayer();
    std::shared_ptr<Layer> getRightLayer();
    void adjustSize();
    void adjustLayerAndNodePosX();

    int id;
};
}  // namespace GuiBridge
#endif  // LAYER_H
