#ifndef SIMPLE_ROW_GRAPH_PLACER
#define SIMPLE_ROW_GRAPH_PLACER

#include <memory>
#include <vector>
namespace GuiBridge {

class Graph;
void combine(std::vector<std::shared_ptr<Graph>> &components, std::shared_ptr<Graph> &target);
void moveGraph(std::shared_ptr<Graph> &destGraph, std::shared_ptr<Graph> &sourceGraph, double offsetX, double offsetY);
void moveGraphs(std::shared_ptr<Graph> &destGraph, std::vector<std::shared_ptr<Graph>> &sourceGraphs, double offsetX,
                double offsetY);

void sortComponents(std::vector<std::shared_ptr<Graph>> &components);
void placeComponents(std::vector<std::shared_ptr<Graph>> &components, std::shared_ptr<Graph> target, double maxRowWidth,
                     double componentSpacing);
void offsetGraph(std::shared_ptr<Graph> &graph, double offsetX, double offsetY);
}  // namespace GuiBridge

#endif