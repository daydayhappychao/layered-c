#ifndef BKALIGNER_H
#define BKALIGNER_H

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace GuiBridge {
class NeighborhoodInformation;
class Graph;
class BKAlignedLayout;
class Edge;

class BKAligner {
public:
    BKAligner(std::shared_ptr<Graph> &layeredGraph, std::shared_ptr<NeighborhoodInformation> &ni);
    void verticalAlignment(BKAlignedLayout &bal, std::set<std::shared_ptr<Edge>> &markedEdges);
    void insideBlockShift(BKAlignedLayout &bal);

private:
    std::shared_ptr<Graph> layeredGraph;
    std::shared_ptr<NeighborhoodInformation> ni;
};
}  // namespace GuiBridge

#endif