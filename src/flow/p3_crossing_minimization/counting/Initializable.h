#ifndef IINITIALIZABLE_HPP
#define IINITIALIZABLE_HPP

#include <memory>
#include <vector>

namespace GuiBridge {

class Node;
class Edge;
class Initializable {
public:
    static void init(const std::vector<std::shared_ptr<Initializable>> &initializables,
                     std::vector<std::vector<std::shared_ptr<Node>>> &order);

    virtual void initAtLayerLevel(int l, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) = 0;
    virtual void initAtNodeLevel(int l, int n, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) = 0;
    virtual void initAtPortLevel(int l, int n, int p, std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) = 0;
    virtual void initAtEdgeLevel(int l, int n, int p, int e, std::shared_ptr<Edge> &edge,
                                 std::vector<std::vector<std::shared_ptr<Node>>> &nodeOrder) = 0;
    virtual void initAfterTraversal() = 0;
};
}  // namespace GuiBridge

#endif  // IINITIALIZABLE_H