#ifndef SIMPLE_ROW_GRAPH_PLACER
#define SIMPLE_ROW_GRAPH_PLACER

#include <memory>
#include <vector>
namespace GuiBridge {

class Graph;
void combine(const std::vector<std::shared_ptr<Graph>> &components, const std::shared_ptr<Graph> &target);
void moveGraph(const std::shared_ptr<Graph> &destGraph, const std::shared_ptr<Graph> &sourceGraph, double offsetX,
               double offsetY);
}  // namespace GuiBridge

#endif