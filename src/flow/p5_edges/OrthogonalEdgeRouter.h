#ifndef ORTHOGONAL_EDGE_ROUTER
#define ORTHOGONAL_EDGE_ROUTER

#include <memory>
#include <set>
#include <vector>

namespace GuiBridge {
class Graph;
class OrthogonalEdgeRouter {
public:
    void process(std::shared_ptr<Graph> &layeredGraph);
};

}  // namespace GuiBridge

#endif