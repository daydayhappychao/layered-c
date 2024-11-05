#ifndef LAYER_H
#define LAYER_H

#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include "./math/KVector.h"
namespace GuiBridge {
class Graph;
class Node;

class Layer : public std::enable_shared_from_this<Layer> {
private:
    std::shared_ptr<Graph> owner;
    KVector size;
    std::vector<std::shared_ptr<Node>> nodes;

public:
    explicit Layer(std::shared_ptr<Graph> graph);
    KVector &getSize();
    std::vector<std::shared_ptr<Node>> &getNodes();
    std::shared_ptr<Graph> getGraph();
    int getIndex() const;

    std::vector<std::shared_ptr<Node>>::iterator begin();
    std::vector<std::shared_ptr<Node>>::iterator end();
    std::vector<std::shared_ptr<Node>>::const_iterator begin() const;
    std::vector<std::shared_ptr<Node>>::const_iterator end() const;
    std::string toString() const;

    int id;
    int index;
};
}  // namespace GuiBridge
#endif  // LAYER_H
